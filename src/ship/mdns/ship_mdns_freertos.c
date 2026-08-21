/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * @brief FreeRTOS specific Mdns implementation
 *
 * Supported netifs: WiFi station (WIFI_STA_DEF) and Ethernet
 * (ETH_DEF — covers W5500 SPI, RMII Ethernet, and similar drivers).
 */

#include "mdns.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "src/common/array_util.h"
#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/ship/api/mdns_entry.h"
#include "src/ship/api/ship_mdns_interface.h"
#include "src/ship/mdns/mdns_debug.h"
#include "src/ship/mdns/ship_mdns.h"

/** mDNS debug printf(), enabled whith MDNS_DEBUG = 1 */
#if MDNS_DEBUG
#define MDNS_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define MDNS_DEBUG_PRINTF(fmt, ...)
#endif  // MDNS_DEBUG

static const char* kShipServiceType     = "_ship";
static const char* kShipServiceProtocol = "_tcp";
static const char* kShipServicePath     = "/ship/";
static const char* kShipServiceTxtVer   = "1";

static const uint32_t kMdnsQueryTimeoutMs = 5000;
static const size_t kMdnsQueryMaxResults  = 40;

typedef struct Mdns Mdns;

struct Mdns {
  /** Implements the Mdns Interface */
  ShipMdnsObject obj;

  OnMdnsEntriesFoundCallback on_entries_found_cb;
  void* context;

  bool cancel;
  const char* ski;
  EebusDeviceInfo* device_info;
  const char* service_name;
  int port;
  bool autoaccept;
  Vector* found_entries;
  EebusThreadObject* thread;
  SemaphoreHandle_t semaphore;
  bool service_registered;
};

#define MDNS(obj) ((Mdns*)(obj))

static void Destruct(ShipMdnsObject* self);
static EebusError Start(ShipMdnsObject* self);
static void Stop(ShipMdnsObject* self);
static EebusError RegisterService(ShipMdnsObject* self);
static void DeregisterService(ShipMdnsObject* self);
static void SetAutoaccept(ShipMdnsObject* self, bool autoaccept);
static void MdnsNotifyFoundEntries(Mdns* mdns);

static const ShipMdnsInterface mdns_methods = {
    .destruct           = Destruct,
    .start              = Start,
    .stop               = Stop,
    .register_service   = RegisterService,
    .deregister_service = DeregisterService,
    .set_autoaccept     = SetAutoaccept,
};

static EebusError MdnsConstruct(
    Mdns* self,
    const char* ski,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    OnMdnsEntriesFoundCallback cb,
    void* ctx
);

EebusError MdnsConstruct(
    Mdns* self,
    const char* ski,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    OnMdnsEntriesFoundCallback cb,
    void* ctx
) {
  // Override "virtual functions table"
  SHIP_MDNS_INTERFACE(self) = &mdns_methods;

  self->cancel              = false;
  self->ski                 = StringCopy(ski);
  self->on_entries_found_cb = cb;
  self->context             = ctx;
  self->device_info         = EebusDeviceInfoCopy(device_info);
  self->service_name        = StringCopy(service_name);
  self->port                = port;
  self->autoaccept          = false;
  self->found_entries       = VectorCreateWithDeallocator(MdnsEntryDeallocator);
  self->thread              = NULL;
  self->semaphore           = xSemaphoreCreateBinary();
  self->service_registered  = false;
  if (self->semaphore == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

ShipMdnsObject* ShipMdnsCreate(
    const char* ski,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    OnMdnsEntriesFoundCallback cb,
    void* ctx
) {
  Mdns* const mdns = (Mdns*)EEBUS_MALLOC(sizeof(Mdns));
  if (mdns == NULL) {
    return NULL;
  }

  const EebusError err = MdnsConstruct(mdns, ski, device_info, service_name, port, cb, ctx);

  if (err != kEebusErrorOk) {
    MdnsDelete(SHIP_MDNS_OBJECT(mdns));
    return NULL;
  }

  return SHIP_MDNS_OBJECT(mdns);
}

void Destruct(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  SHIP_MDNS_STOP(self);

  if (mdns->found_entries != NULL) {
    VectorFreeElements(mdns->found_entries);
    VectorDestruct(mdns->found_entries);
    EEBUS_FREE(mdns->found_entries);
    mdns->found_entries = NULL;
  }

  if (mdns->semaphore != NULL) {
    vSemaphoreDelete(mdns->semaphore);
    mdns->semaphore = NULL;
  }

  EebusDeviceInfoDelete(mdns->device_info);
  mdns->device_info = NULL;

  StringDelete((char*)mdns->ski);
  mdns->ski = NULL;

  StringDelete((char*)mdns->service_name);
  mdns->service_name = NULL;
}

MdnsEntry* MdnsEntryCreateWithMdnsResult(mdns_result_t* result) {
  if (result == NULL) {
    return NULL;
  }

  MdnsEntry* entry = MdnsEntryCreate(result->instance_name, result->hostname, 0);
  if (entry == NULL) {
    return NULL;
  }

  const char* const host_local = StringFmtSprintf("%s.local.", result->hostname);
  MdnsEntrySetHost(entry, host_local);
  StringDelete((char*)host_local);
  MdnsEntrySetPort(entry, result->port);

  for (size_t i = 0; i < result->txt_count; i++) {
    const char* key   = result->txt[i].key;
    const char* value = result->txt[i].value;

    const EebusError err = MdnsEntrySetValue(entry, key, strlen(key), value, strlen(value));

    if (err != kEebusErrorOk) {
      MDNS_DEBUG_PRINTF("MdnsEntrySetValue() failed: %d, key = %s, value = %s\n", err, key, value);
    }
  }

  return entry;
}

static void MdnsNotifyFoundEntries(Mdns* mdns) {
  const size_t found_count = VectorGetSize(mdns->found_entries);
  Vector* const copy       = VectorCreateWithDeallocator(MdnsEntryDeallocator);

  for (size_t i = 0; i < found_count; ++i) {
    MdnsEntry* const entry = (MdnsEntry*)VectorGetElement(mdns->found_entries, i);
    if (entry != NULL) {
      VectorPushBack(copy, MdnsEntryCopy(entry));
    }
  }

  mdns->on_entries_found_cb(copy, mdns->context);
}

void MdnsProcessSearchResult(Mdns* mdns, mdns_result_t* results) {
  if (results == NULL) {
    MDNS_DEBUG_PRINTF("mDNS query returned no results\n");
    return;
  }

  mdns_result_t* r = results;
  while (r) {
    MdnsEntry* const entry = MdnsEntryCreateWithMdnsResult(r);
    if (entry == NULL) {
      MDNS_DEBUG_PRINTF("Failed to create mDNS entry\n");
      return;
    } else {
      if (MdnsEntryIsValid(entry) && (strcmp(entry->ski, mdns->ski) != 0)) {
        MDNS_DEBUG_PRINTF("Added entry: %s, ski: %s\n", entry->name, entry->ski);
        VectorPushBack(mdns->found_entries, entry);
      } else {
        MDNS_DEBUG_PRINTF("Ignored entry: %s, ski: %s\n", entry->name, entry->ski);
        MdnsEntryDelete(entry);
      }
    }

    r = r->next;
  }

  mdns_query_results_free(results);
  MdnsNotifyFoundEntries(mdns);
}

uint32_t GetUpdateIntervalMs(void) {
  uint8_t update_interval
      = kMdnsBrowseIntervalMinSeconds + rand() % (kMdnsBrowseIntervalMaxSeconds - kMdnsBrowseIntervalMinSeconds);

  return update_interval * 1000;
}

void* MdnsBrowserLoop(void* parameters) {
  Mdns* const mdns = (Mdns*)parameters;

  while (!mdns->cancel) {
    VectorFreeElements(mdns->found_entries);

    mdns_result_t* results = NULL;
    const esp_err_t err
        = mdns_query_ptr(kShipServiceType, kShipServiceProtocol, kMdnsQueryTimeoutMs, kMdnsQueryMaxResults, &results);

    if (err == ESP_OK) {
      MdnsProcessSearchResult(mdns, results);
    } else {
      MDNS_DEBUG_PRINTF("mdns_query_ptr() failed: %d\n", err);
    }

    if (!mdns->cancel) {
      const TickType_t timeout = pdMS_TO_TICKS(GetUpdateIntervalMs());
      xSemaphoreTake(mdns->semaphore, timeout);
    }
  }

  return NULL;
}

EebusError RegisterService(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  const char* register_str = mdns->autoaccept ? "true" : "false";

  // Structure with TXT records
  mdns_txt_item_t service_txt_data[] = {
      { "txtvers",       kShipServiceTxtVer},
      {      "id",       mdns->service_name},
      {    "path",         kShipServicePath},
      {     "ski",                mdns->ski},
      {"register",             register_str},
      {   "brand", mdns->device_info->brand},
      {    "type",  mdns->device_info->type},
      {   "model", mdns->device_info->model},
  };

  // Initialize service
  const esp_err_t err = mdns_service_add(
      mdns->service_name,
      kShipServiceType,
      kShipServiceProtocol,
      mdns->port,
      service_txt_data,
      ARRAY_SIZE(service_txt_data)
  );

  if (err != ESP_OK) {
    MDNS_DEBUG_PRINTF("mdns_service_add() failed: %d\n", err);
    return kEebusErrorInit;
  }

  mdns->service_registered = true;
  return kEebusErrorOk;
}

static esp_netif_t* MdnsGetEspNetif(Mdns* self) {
  UNUSED(self);

  // Locate the active netif. Try WiFi STA first (original assumption),
  // then fall back to the default Ethernet interface (ETH_DEF) which is
  // what the W5500 SPI Ethernet driver registers. If neither key matches
  // (custom netif keys), walk all registered netifs for the first UP one.
  static const char* const kNetifKeys[] = {
      "WIFI_STA_DEF",  // WiFi station
      "ETH_DEF",       // SPI / RMII Ethernet (W5500 / LAN87xx / ...)
  };

  esp_netif_t* netif = NULL;
  for (size_t i = 0; i < ARRAY_SIZE(kNetifKeys); i++) {
    netif = esp_netif_get_handle_from_ifkey(kNetifKeys[i]);
    if (netif != NULL) {
      return netif;
    }
  }

  // Last resort: walk the netif list for any UP interface.
  netif = NULL;
  while ((netif = esp_netif_next_unsafe(netif)) != NULL) {
    if (esp_netif_is_netif_up(netif)) {
      return netif;
    }
  }

  return netif;
}

EebusError Start(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  mdns->cancel = false;

  // mdns_init() may return ESP_ERR_INVALID_STATE when the host framework has
  // already initialized mDNS — that is fine, reuse it.
  esp_err_t err = mdns_init();
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
    MDNS_DEBUG_PRINTF("mdns_init() failed: %d\n", err);
    return kEebusErrorInit;
  }

  esp_netif_t* netif = MdnsGetEspNetif(mdns);
  if (netif == NULL) {
    MDNS_DEBUG_PRINTF("No suitable netif found (WIFI_STA_DEF, ETH_DEF, any UP)\n");
    return kEebusErrorInit;
  }

  const char* host_name = NULL;

  err = esp_netif_get_hostname(netif, &host_name);
  if ((err != ESP_OK) || (StringIsEmpty(host_name))) {
    MDNS_DEBUG_PRINTF("esp_netif_get_hostname() failed\n");
    return kEebusErrorInit;
  }

  mdns_hostname_set(host_name);

  EebusError ret = RegisterService(self);
  if (ret != kEebusErrorOk) {
    MDNS_DEBUG_PRINTF("RegisterService() returned error %d\n", ret);
    return ret;
  }

  mdns->thread = EebusThreadCreate(MdnsBrowserLoop, mdns, 4096);
  if (mdns->thread == NULL) {
    MDNS_DEBUG_PRINTF("EebusThreadCreate() failed\n");
    DeregisterService(self);
    return kEebusErrorThread;
  }

  return kEebusErrorOk;
}

void DeregisterService(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  mdns->cancel = true;

  if (mdns->thread != NULL) {
    xSemaphoreGive(mdns->semaphore);
    EEBUS_THREAD_JOIN(mdns->thread);
    EebusThreadDelete(mdns->thread);
    mdns->thread = NULL;
  }

  if (mdns->service_registered) {
    const esp_err_t err
        = mdns_service_remove_for_host(mdns->service_name, kShipServiceType, kShipServiceProtocol, NULL);
    if (err != ESP_OK && err != ESP_ERR_NOT_FOUND) {
      MDNS_DEBUG_PRINTF("mdns_service_remove_for_host() failed: %d\n", err);
    }
    mdns->service_registered = false;
  }
}

void Stop(ShipMdnsObject* self) {
  DeregisterService(self);
}

void SetAutoaccept(ShipMdnsObject* self, bool autoaccept) {
  Mdns* const mdns = MDNS(self);
  mdns->autoaccept = autoaccept;
}
