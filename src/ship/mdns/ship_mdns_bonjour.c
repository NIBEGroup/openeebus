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
 * @brief Mdns implementation
 */

// Useful information about dns-sd.
//
// See example code at:
// https://github.com/xbmc/mDNSResponder/blob/master/Clients/dns-sd.c
//
// Running "dns-sd -B _services._dns-sd._udp" will return a list of all available
// service types that are currently being advertised (the list is per interface,
// so there will be some redundancy). If this is done on a Mac with no active
// network connection, the list will of course only contain services running on that machine.
//
// Using that list, information about the individual services types can be requested
// by running e.g. "dns-sd -B _home-sharing._tcp" (which lists iTunes Home Sharing instances),
// and then, given an instance name, "dns-sd -L "Wes Campaigne’s Library" _home-sharing._tcp"
// will provide an information for a particular instance.
//
// Example of dns-sd output:
//-------------------------------------------------------------------------------------------//
// % dns-sd -B _ship._tcp
// Browsing for _ship._tcp
// DATE: ---Fri 19 Apr 2024---
// 18:41:25.166  ...STARTING...
// Timestamp     A/R    Flags  if Domain               Service Type         Instance Name
// 18:41:25.167  Add        2  13 local.               _ship._tcp.          Demo-EVSE-234567890
//
// % dns-sd -L "Demo-EVSE-234567890" _ship._tcp
// Lookup Demo-EVSE-234567890._ship._tcp.local
// DATE: ---Fri 19 Apr 2024---
// 18:41:44.184  ...STARTING...
// 18:41:44.185  Demo-EVSE-234567890._ship._tcp.local. can be reached at
// DESKTOP-IAKQS71.local.:4769 (interface 13)
// txtvers=1 path=/ship/ id=Demo-EVSE-234567890 ski=41c98b1bbe5fc7657ce311981951f12d304ab419
// brand=Demo model=EVSE type=ChargingStation register=false
//-------------------------------------------------------------------------------------------//
//
// From example above it is obvious that using dns-sd library the lookup can be done in
// two stages:
// 1. Call DNSServiceBrowse() with type "_ship._tcp"
// 2. Call DNSServiceResolve() using name obtained in [1] and same type "_ship._tcp"
// Each step shall be followed by event handling with timeout (see MdnsProcessResults() function
// from https://github.com/xbmc/mDNSResponder/blob/master/Clients/dns-sd.c)
//
// Note that when using libwebsockets, "DESKTOP-IAKQS71.local.:4769" shall be used without
// a dot after "local"!
// An example of working uri is: "wss://DESKTOP-IAKQS71.local:4769"
//
// To announce the mDNS entry use:
// dns-sd -R NIBE-06920619238006 _ship._tcp. local 7711 path=/ship/
//     ski=41c98b1bbe5fc7657ce311981951f12d304ab419
//     txtvers=1 id=NIBE-06920619238006 register=false model=nibe-n type=ControlBox brand=NIBE

#include <errno.h>
#ifdef _WIN32
// clang-format off
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Iphlpapi.h>
#include <process.h>
// clang-format on
#else
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#endif
#include <dns_sd.h>
#include <string.h>
#include <time.h>

#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/vector.h"
#include "src/ship/api/mdns_entry.h"
#include "src/ship/api/ship_mdns_interface.h"
#include "src/ship/api/ship_pairing_entry.h"
#include "src/ship/mdns/mdns_debug.h"

/** mDNS debug printf(), enabled whith MDNS_DEBUG = 1 */
#if MDNS_DEBUG
#define MDNS_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define MDNS_DEBUG_PRINTF(fmt, ...)
#endif  // MDNS_DEBUG

/** Delay before retrying to re-establish a dead mDNS session */
static const uint32_t kMdnsRecoveryDelaySeconds = 5;

static const char* kShipServiceType   = "_ship._tcp";
static const char* kShipPairingServiceType = SHIP_PAIRING_SERVICE_TYPE;

// Section 5.3 recommends a TTL of two minutes for shippairing records.
// DNSServiceRegister() does not expose the TTL of the records it publishes, so
// the responder's own default applies. The recommendation only affects how
// quickly a stale announcement ages out of a cache; a withdrawn request is
// removed by the goodbye that deregistering it sends.

/**
 * Section 5.3: the port of a shippairing SRV record is unused. It must never be
 * connected to and nothing is ever sent to it, but a port has to be published.
 */
static const uint16_t kShipPairingUnusedPort = 1;
static const char* kShipServicePath   = "/ship/";
static const char* kShipServiceTxtVer = "1";

static const struct timeval select_timeout = {
    .tv_sec  = 0,
    .tv_usec = 100000,
};

typedef struct Mdns Mdns;

typedef struct ActiveResolveEntry ActiveResolveEntry;
/** Tracks a single pending DNSServiceResolve call */
struct ActiveResolveEntry {
  /** The DNSServiceRef for this resolve */
  DNSServiceRef service_ref;
  /** The MdnsEntry being resolved, NULL for a shippairing resolve */
  MdnsEntry* entry;
  /**
   * @brief The ShipPairingEntry being resolved, NULL for a SHIP resolve
   *
   * A shippairing instance has to be resolved like any other, because a browse
   * reports only a name and the TXT record arrives with the resolve. Its host
   * and port are then discarded: section 5.3 forbids using them.
   */
  ShipPairingEntry* pairing_entry;
  /** The owning Mdns instance */
  Mdns* owner;
  /** Whether the resolve is done */
  bool done;
};

struct Mdns {
  /** Implements the Mdns Interface */
  ShipMdnsObject obj;
  const char* ski;
  EebusDeviceInfo* device_info;
  const char* service_name;
  int port;
  bool autoaccept;

  OnMdnsEntriesFoundCallback on_entries_found_cb;
  void* context;

  EebusThreadObject* thread;
  DNSServiceRef dns_service_browser_ref;
  DNSServiceRef dns_service_register_ref;
  Vector* active_resolves;
  Vector* found_entries;

  /** @brief SHIP Pairing Service, browsed and announced alongside SHIP itself */
  OnShipPairingEntriesFoundCallback on_pairing_entries_found_cb;
  void* pairing_context;
  DNSServiceRef dns_service_pairing_browser_ref;
  DNSServiceRef dns_service_pairing_register_ref;
  Vector* pairing_found_entries;

  bool cancel;

  /** Set when the mDNS sessions must be recovered */
  bool needs_recovery;
  /** Earliest monotonic time (seconds) at which a session recovery attempt may run */
  uint32_t next_recovery_time_seconds;
};

#define MDNS(obj) ((Mdns*)(obj))

#define TRY_SET_TXT_RECORD_VALUE(record, key, value)                                                 \
  {                                                                                                  \
    const DNSServiceErrorType error = TXTRecordSetValue(record, key, (uint8_t)strlen(value), value); \
    if (error != kDNSServiceErr_NoError) {                                                           \
      MDNS_DEBUG_PRINTF("TXTRecordSetValue(%s) returned error %d\n", key, error);                    \
      return error;                                                                                  \
    }                                                                                                \
  }

static void Destruct(ShipMdnsObject* self);
static EebusError Start(ShipMdnsObject* self);
static void Stop(ShipMdnsObject* self);
static EebusError RegisterService(ShipMdnsObject* self);
static void DeregisterService(ShipMdnsObject* self);
static void SetAutoaccept(ShipMdnsObject* self, bool autoaccept);
static EebusError StartPairingBrowse(ShipMdnsObject* self, OnShipPairingEntriesFoundCallback cb, void* ctx);
static void StopPairingBrowse(ShipMdnsObject* self);
static EebusError RegisterPairingService(ShipMdnsObject* self, const ShipPairingEntry* entry);
static void DeregisterPairingService(ShipMdnsObject* self);

static const ShipMdnsInterface mdns_methods = {
    .destruct                   = Destruct,
    .start                      = Start,
    .stop                       = Stop,
    .register_service           = RegisterService,
    .deregister_service         = DeregisterService,
    .set_autoaccept             = SetAutoaccept,
    .start_pairing_browse       = StartPairingBrowse,
    .stop_pairing_browse        = StopPairingBrowse,
    .register_pairing_service   = RegisterPairingService,
    .deregister_pairing_service = DeregisterPairingService,
};

static void MdnsConstruct(
    Mdns* self,
    const char* ski,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    OnMdnsEntriesFoundCallback cb,
    void* ctx
);
static ActiveResolveEntry* MdnsActiveResolveEntryCreate(Mdns* owner, MdnsEntry* entry);
static void MdnsActiveResolveEntryDestroy(ActiveResolveEntry* resolve);
static void MdnsActiveResolveEntryDeallocator(void* resolve);
static bool MdnsHasMatchingEntry(const Mdns* mdns, const MdnsEntry* candidate);
static bool MdnsProcessActiveResolveEntry(ActiveResolveEntry* resolve, const fd_set* readfds);
static void MdnsProcessActiveResolves(Mdns* self, const fd_set* readfds);
static void MdnsRemoveEntryByName(Vector* entries, const char* name);
static inline uint16_t OpaquePortToUint16(uint16_t opaque_port);
static void MdnsResolveServiceCallback(
    DNSServiceRef service_ref,
    const DNSServiceFlags flags,
    uint32_t iface,
    DNSServiceErrorType err,
    const char* name,
    const char* host,
    uint16_t opaque_port,
    uint16_t txt_record_size,
    const unsigned char* txt_record,
    void* ctx
);
static void MdnsBrowseServicesCallback(
    DNSServiceRef service_ref,
    DNSServiceFlags flags,
    uint32_t iface,
    DNSServiceErrorType err,
    const char* name,
    const char* type,
    const char* domain,
    void* ctx
);
static void MdnsBrowseServices(Mdns* self);
static int MdnsPrepareSockFd(DNSServiceRef service_ref, fd_set* readfds, int* max_fd);
static void MdnsDispatchReadyFds(Mdns* mdns, const fd_set* readfds, int browse_fd, int register_fd);
static void MdnsNotifyFoundEntries(Mdns* mdns);
static void* MdnsBrowserLoop(void* parameters);
static void MdnsRegisterServiceCallback(
    DNSServiceRef ref,
    DNSServiceFlags flags,
    DNSServiceErrorType error,
    const char* name,
    const char* type,
    const char* domain,
    void* ctx
);
static DNSServiceErrorType MdnsCreateTextRecord(TXTRecordRef* txt_record, const Mdns* mdns);
static void MdnsBrowserReset(Mdns* self);
static void MdnsRecoverSessions(Mdns* self, uint32_t now_seconds);
static uint32_t MdnsGetRandomInterval(uint32_t min_seconds, uint32_t max_seconds);
static void MdnsPairingBrowseServices(Mdns* self);
static void MdnsNotifyFoundPairingEntries(Mdns* mdns);
static void MdnsPairingBrowseServicesCallback(
    DNSServiceRef service_ref,
    DNSServiceFlags flags,
    uint32_t iface,
    DNSServiceErrorType err,
    const char* name,
    const char* type,
    const char* domain,
    void* ctx
);
static void MdnsPairingResolveServiceCallback(
    DNSServiceRef service_ref,
    DNSServiceFlags flags,
    uint32_t iface,
    DNSServiceErrorType err,
    const char* name,
    const char* host,
    uint16_t opaque_port,
    uint16_t txt_record_size,
    const unsigned char* txt_record,
    void* ctx
);

void MdnsConstruct(
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

  self->ski                 = ski;
  self->device_info         = EebusDeviceInfoCopy(device_info);
  self->service_name        = service_name;
  self->port                = port;
  self->autoaccept          = false;
  self->on_entries_found_cb = cb;
  self->context             = ctx;

  self->thread                   = NULL;
  self->dns_service_browser_ref  = NULL;
  self->dns_service_register_ref = NULL;
  self->found_entries            = VectorCreateWithDeallocator(MdnsEntryDeallocator);
  self->active_resolves          = VectorCreateWithDeallocator(MdnsActiveResolveEntryDeallocator);

  self->on_pairing_entries_found_cb      = NULL;
  self->pairing_context                  = NULL;
  self->dns_service_pairing_browser_ref  = NULL;
  self->dns_service_pairing_register_ref = NULL;
  self->pairing_found_entries            = VectorCreateWithDeallocator(ShipPairingEntryDeallocator);

  self->cancel                     = false;
  self->needs_recovery             = false;
  self->next_recovery_time_seconds = 0;

  // Seed random number generator
  srand((int)time(NULL));
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

  MdnsConstruct(mdns, ski, device_info, service_name, port, cb, ctx);

  return SHIP_MDNS_OBJECT(mdns);
}

static ActiveResolveEntry* MdnsActiveResolveEntryCreate(Mdns* owner, MdnsEntry* entry) {
  ActiveResolveEntry* const resolve = (ActiveResolveEntry*)EEBUS_MALLOC(sizeof(ActiveResolveEntry));
  if (resolve == NULL) {
    return NULL;
  }

  resolve->service_ref   = NULL;
  resolve->entry         = entry;
  resolve->pairing_entry = NULL;
  resolve->owner         = owner;
  resolve->done          = false;

  return resolve;
}

static void MdnsActiveResolveEntryDestroy(ActiveResolveEntry* resolve) {
  if (resolve == NULL) {
    return;
  }

  if (resolve->service_ref != NULL) {
    DNSServiceRefDeallocate(resolve->service_ref);
    resolve->service_ref = NULL;
  }

  if (resolve->entry != NULL) {
    MdnsEntryDelete(resolve->entry);
    resolve->entry = NULL;
  }

  if (resolve->pairing_entry != NULL) {
    ShipPairingEntryDelete(resolve->pairing_entry);
    resolve->pairing_entry = NULL;
  }

  EEBUS_FREE(resolve);
}

static void MdnsActiveResolveEntryDeallocator(void* resolve) {
  MdnsActiveResolveEntryDestroy((ActiveResolveEntry*)resolve);
}

static bool MdnsHasMatchingEntry(const Mdns* mdns, const MdnsEntry* candidate) {
  if ((mdns == NULL) || (mdns->found_entries == NULL) || (candidate == NULL)) {
    return false;
  }

  const char* const candidate_ski = MdnsEntryGetSki(candidate);
  if ((candidate_ski == NULL) || (candidate_ski[0] == '\0')) {
    return false;
  }

  const size_t found_count = VectorGetSize(mdns->found_entries);
  for (size_t i = 0; i < found_count; ++i) {
    MdnsEntry* const existing = (MdnsEntry*)VectorGetElement(mdns->found_entries, i);
    if (existing == NULL) {
      continue;
    }

    const char* const existing_ski = MdnsEntryGetSki(existing);
    if ((existing_ski != NULL) && (existing_ski[0] != '\0') && (strcmp(candidate_ski, existing_ski) == 0)) {
      return true;
    }
  }

  return false;
}

static void MdnsBrowserReset(Mdns* mdns) {
  if (mdns->dns_service_browser_ref != NULL) {
    DNSServiceRefDeallocate(mdns->dns_service_browser_ref);
    mdns->dns_service_browser_ref = NULL;
  }

  if (mdns->dns_service_pairing_browser_ref != NULL) {
    DNSServiceRefDeallocate(mdns->dns_service_pairing_browser_ref);
    mdns->dns_service_pairing_browser_ref = NULL;
  }

  if (mdns->pairing_found_entries != NULL) {
    VectorFreeElements(mdns->pairing_found_entries);
    VectorClear(mdns->pairing_found_entries);
  }

  if (mdns->found_entries != NULL) {
    VectorFreeElements(mdns->found_entries);
    VectorClear(mdns->found_entries);
  }

  if (mdns->active_resolves != NULL) {
    VectorFreeElements(mdns->active_resolves);
    VectorClear(mdns->active_resolves);
  }
}

static void Destruct(ShipMdnsObject* self) {
  SHIP_MDNS_STOP(self);

  Mdns* const mdns = MDNS(self);
  MdnsBrowserReset(mdns);

  if (mdns->dns_service_register_ref != NULL) {
    DNSServiceRefDeallocate(mdns->dns_service_register_ref);
    mdns->dns_service_register_ref = NULL;
  }

  if (mdns->found_entries != NULL) {
    VectorFreeElements(mdns->found_entries);
    VectorDestruct(mdns->found_entries);
    EEBUS_FREE(mdns->found_entries);
    mdns->found_entries = NULL;
  }

  if (mdns->active_resolves != NULL) {
    VectorFreeElements(mdns->active_resolves);
    VectorDestruct(mdns->active_resolves);
    EEBUS_FREE(mdns->active_resolves);
    mdns->active_resolves = NULL;
  }

  DeregisterPairingService(self);

  if (mdns->pairing_found_entries != NULL) {
    VectorFreeElements(mdns->pairing_found_entries);
    VectorDestruct(mdns->pairing_found_entries);
    EEBUS_FREE(mdns->pairing_found_entries);
    mdns->pairing_found_entries = NULL;
  }

  EebusDeviceInfoDelete(mdns->device_info);
  mdns->device_info = NULL;
}

static bool MdnsProcessActiveResolveEntry(ActiveResolveEntry* resolve, const fd_set* readfds) {
  if (resolve == NULL) {
    return false;
  }

  if (resolve->service_ref == NULL) {
    MDNS_DEBUG_PRINTF("NULL service_ref in active resolve entry\n");
    return true;
  }

  if (resolve->done) {
    return true;
  }

  const int fd = DNSServiceRefSockFD(resolve->service_ref);
  if (fd < 0) {
    MDNS_DEBUG_PRINTF("Invalid resolve fd\n");
    resolve->done = true;
    return resolve->done;
  }

  if (FD_ISSET(fd, readfds)) {
    DNSServiceErrorType err = DNSServiceProcessResult(resolve->service_ref);
    if (err != kDNSServiceErr_NoError) {
      MDNS_DEBUG_PRINTF("DNSServiceProcessResult resolve error %d\n", err);
      resolve->done = true;
    }
  }

  return resolve->done;
}

static void MdnsProcessActiveResolves(Mdns* self, const fd_set* readfds) {
  size_t i = 0;

  while (i < VectorGetSize(self->active_resolves)) {
    ActiveResolveEntry* const resolve = (ActiveResolveEntry*)VectorGetElement(self->active_resolves, i);

    if (MdnsProcessActiveResolveEntry(resolve, readfds)) {
      VectorRemove(self->active_resolves, resolve);
      MdnsActiveResolveEntryDestroy(resolve);
    } else {
      ++i;
    }
  }
}

static void MdnsRemoveEntryByName(Vector* entries, const char* name) {
  const size_t size          = VectorGetSize(entries);
  MdnsEntry* entry_to_remove = NULL;

  for (size_t i = 0; i < size; ++i) {
    MdnsEntry* entry = (MdnsEntry*)VectorGetElement(entries, i);
    if ((entry != NULL) && (strcmp(entry->name, name) == 0)) {
      entry_to_remove = entry;
      break;
    }
  }

  if (entry_to_remove != NULL) {
    VectorRemove(entries, entry_to_remove);
    MdnsEntryDelete(entry_to_remove);
  }
}

static uint16_t OpaquePortToUint16(uint16_t opaque_port) {
  union {
    uint16_t s;
    uint8_t b[2];
  } port = {opaque_port};

  return ((uint16_t)port.b[0]) << 8 | port.b[1];
}

static void MdnsResolveServiceCallback(
    DNSServiceRef service_ref,
    const DNSServiceFlags flags,
    uint32_t iface,
    DNSServiceErrorType err,
    const char* name,
    const char* host,
    uint16_t opaque_port,
    uint16_t txt_record_size,
    const unsigned char* txt_record,
    void* ctx
) {
  UNUSED(service_ref);
  UNUSED(iface);
  UNUSED(name);

  ActiveResolveEntry* const resolve = (ActiveResolveEntry*)ctx;
  if ((resolve == NULL) || (resolve->owner == NULL)) {
    return;
  }

  if (err != kDNSServiceErr_NoError) {
    if (err == kDNSServiceErr_NoSuchRecord) {
      MDNS_DEBUG_PRINTF(" no Such Record\n");
    } else {
      MDNS_DEBUG_PRINTF(" error code: %d\n", err);
    }

    resolve->done = true;
    return;
  }

  const uint16_t port = OpaquePortToUint16(opaque_port);

  MDNS_DEBUG_PRINTF("%s can be reached at %s:%u (interface %u), flags: %X\n", name, host, port, iface, flags);

  Mdns* const mdns = resolve->owner;

  if (flags & kDNSServiceFlagsMoreComing) {
    MDNS_DEBUG_PRINTF("%s(), more coming...\n", __func__);
    return;
  }

  MdnsEntry* const entry = resolve->entry;
  if (entry == NULL) {
    MDNS_DEBUG_PRINTF("%s(), NULL mDNS entry\n", __func__);
    resolve->done = true;
    return;
  }

  MdnsEntrySetHost(entry, host);
  MdnsEntrySetPort(entry, port);
  MDNS_TXT_RECORD_PRINT(txt_record, txt_record_size);
  MdnsEntryParseTxtRecord(entry, (const char*)txt_record, txt_record_size);
  resolve->done = true;

  const bool is_valid     = MdnsEntryIsValid(entry);
  const bool is_own_entry = is_valid && (strcmp(entry->ski, mdns->ski) == 0);

  if (is_valid && !is_own_entry) {
    if (MdnsHasMatchingEntry(mdns, entry)) {
      MDNS_DEBUG_PRINTF("Ignoring duplicate entry: %s\n", entry->name);
      return;
    }

    // Transfer ownership of entry to found_entries vector
    VectorPushBack(mdns->found_entries, entry);
    resolve->entry = NULL;
    MdnsNotifyFoundEntries(mdns);
  } else {
    MDNS_DEBUG_PRINTF("Ignoring invalid or own entry\n");
  }
}

static void MdnsBrowseServicesCallback(
    DNSServiceRef service_ref,
    DNSServiceFlags flags,
    uint32_t iface,
    DNSServiceErrorType err,
    const char* name,
    const char* type,
    const char* domain,
    void* ctx
) {
  UNUSED(service_ref);
  UNUSED(type);

  Mdns* const mdns = (Mdns*)ctx;

  if (err != kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("Bonjour browser error occurred: %d\n", err);
    if (mdns != NULL) {
      mdns->needs_recovery = true;
    }

    return;
  }

#if DEBUG_MDNS
  const char* const action = (flags & kDNSServiceFlagsAdd) ? "ADD" : "RMV";
  const char* const more   = (flags & kDNSServiceFlagsMoreComing) ? " (MORE)" : "";

  MDNS_DEBUG_PRINTF("%s %30s.%s%s on interface %d%s\n", action, name, type, domain, (int)iface, more);
#endif

  if (!(flags & kDNSServiceFlagsAdd)) {
    MDNS_DEBUG_PRINTF("Removed service: %s.%s%s\n", name, type, domain);
    MdnsRemoveEntryByName(mdns->found_entries, name);
    MdnsNotifyFoundEntries(mdns);
    return;
  }

  if (mdns == NULL) {
    MDNS_DEBUG_PRINTF("mDNS browse callback with NULL context\n");
    return;
  }

  // Ignore own service
  if (strcmp(name, mdns->service_name) == 0) {
    MDNS_DEBUG_PRINTF("Ignoring own service: %s\n", name);
    return;
  }

  MdnsEntry* const entry = MdnsEntryCreate(name, domain, iface);
  if (entry == NULL) {
    return;
  }

  ActiveResolveEntry* const resolve = MdnsActiveResolveEntryCreate(mdns, entry);
  if (resolve == NULL) {
    MdnsEntryDelete(entry);
    return;
  }

  DNSServiceRef service_resolve_ref = NULL;
  const DNSServiceErrorType res_err = DNSServiceResolve(
      &service_resolve_ref,
      0,
      iface,
      name,
      kShipServiceType,
      domain,
      MdnsResolveServiceCallback,
      resolve
  );

  if (res_err != kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("DNSServiceResolve() returned error %d\n", res_err);
    if (service_resolve_ref != NULL) {
      DNSServiceRefDeallocate(service_resolve_ref);
      service_resolve_ref = NULL;
    }

    MdnsActiveResolveEntryDestroy(resolve);
    return;
  }

  if (service_resolve_ref == NULL) {
    MDNS_DEBUG_PRINTF("DNSServiceResolve() failed to create a service ref\n");
    MdnsActiveResolveEntryDestroy(resolve);
    return;
  }

  resolve->service_ref = service_resolve_ref;
  VectorPushBack(mdns->active_resolves, resolve);
}

static void MdnsBrowseServices(Mdns* self) {
  if (self->dns_service_browser_ref != NULL) {
    DNSServiceRefDeallocate(self->dns_service_browser_ref);
    self->dns_service_browser_ref = NULL;
  }

  const DNSServiceErrorType err = DNSServiceBrowse(
      &self->dns_service_browser_ref,
      0,                             // No flags
      kDNSServiceInterfaceIndexAny,  // Browse on all network interfaces
      kShipServiceType,              // Browse for SHIP TCP services
      NULL,                          // Browse on the default domain (e.g. local.)
      MdnsBrowseServicesCallback,    // Callback function when Bonjour events occur
      self
  );  // Pass mDNS instance to callback

  if (err != kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("DNSServiceBrowse() returned error %d\n", err);
    return;
  }

  if (self->dns_service_browser_ref == NULL) {
    MDNS_DEBUG_PRINTF("DNSServiceBrowse() created no service ref\n");
    return;
  }
}

static int MdnsPrepareSockFd(DNSServiceRef service_ref, fd_set* readfds, int* max_fd) {
  if (service_ref == NULL) {
    return -1;
  }

  const int fd = DNSServiceRefSockFD(service_ref);
  if (fd < 0) {
    return -1;
  }

  FD_SET(fd, readfds);
  if (fd > *max_fd) {
    *max_fd = fd;
  }

  return fd;
}

static void MdnsDispatchReadyFds(Mdns* mdns, const fd_set* readfds, int browse_fd, int register_fd) {
  if ((browse_fd >= 0) && FD_ISSET(browse_fd, readfds)) {
    DNSServiceErrorType err = DNSServiceProcessResult(mdns->dns_service_browser_ref);
    if (err != kDNSServiceErr_NoError) {
      MDNS_DEBUG_PRINTF("dns_service_browser_ref processing error %d\n", err);
      mdns->needs_recovery = true;
    }
  }

  if ((register_fd >= 0) && FD_ISSET(register_fd, readfds)) {
    DNSServiceErrorType err = DNSServiceProcessResult(mdns->dns_service_register_ref);
    if (err != kDNSServiceErr_NoError) {
      MDNS_DEBUG_PRINTF("dns_service_register_ref processing error %d\n", err);
      mdns->needs_recovery = true;
    }
  }

  MdnsProcessActiveResolves(mdns, readfds);
}

static void MdnsNotifyFoundEntries(Mdns* mdns) {
  const size_t found_count = VectorGetSize(mdns->found_entries);

  MDNS_DEBUG_PRINTF("Number of found entries: %zu\n", found_count);

  Vector* const copy = VectorCreateWithDeallocator(MdnsEntryDeallocator);

  for (size_t i = 0; i < found_count; ++i) {
    MdnsEntry* const entry = (MdnsEntry*)VectorGetElement(mdns->found_entries, i);
    if (entry != NULL) {
      VectorPushBack(copy, MdnsEntryCopy(entry));
    }
  }

  mdns->on_entries_found_cb(copy, mdns->context);
}

static uint32_t MdnsGetRandomInterval(uint32_t min_seconds, uint32_t max_seconds) {
  const uint32_t update_interval
      = min_seconds + rand() % (max_seconds - min_seconds + 1);  // Random interval between min_seconds and max_seconds
  return update_interval;
}

static uint32_t MdnsGetCurrentTimeSeconds() {
#ifdef _WIN32
  time_t now = time(NULL);
  return (uint32_t)now;
#else
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (uint32_t)now.tv_sec;
#endif  // _WIN32
}

static void MdnsRecoverSessions(Mdns* mdns, uint32_t now_seconds) {
  if (mdns->needs_recovery == false) {
    return;
  }

  // Back off between attempts
  if (now_seconds < mdns->next_recovery_time_seconds) {
    return;
  }

  MDNS_DEBUG_PRINTF("Recovering mDNS sessions\n");

  bool recovered = (RegisterService(SHIP_MDNS_OBJECT(mdns)) == kEebusErrorOk);

  MdnsBrowserReset(mdns);
  MdnsBrowseServices(mdns);
  recovered = recovered && (mdns->dns_service_browser_ref != NULL);

  // The reset tore the shippairing browse down with the rest. The loop opens it
  // again if something is still waiting for what it finds.

  mdns->needs_recovery             = !recovered;
  mdns->next_recovery_time_seconds = now_seconds + kMdnsRecoveryDelaySeconds;
}

EebusError StartPairingBrowse(ShipMdnsObject* self, OnShipPairingEntriesFoundCallback cb, void* ctx) {
  Mdns* const mdns = MDNS(self);

  mdns->on_pairing_entries_found_cb = cb;
  mdns->pairing_context             = ctx;

  // The browse itself is opened by the loop, which owns every DNSServiceRef.
  // Opening it here would touch a ref another thread is selecting on.
  return kEebusErrorOk;
}

void StopPairingBrowse(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  mdns->on_pairing_entries_found_cb = NULL;
  mdns->pairing_context             = NULL;
}

EebusError RegisterPairingService(ShipMdnsObject* self, const ShipPairingEntry* entry) {
  Mdns* const mdns = MDNS(self);

  if (entry == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  // Section 5.5: a corrected request replaces the previous announcement, which
  // has to be withdrawn first so that a goodbye is sent for it.
  DeregisterPairingService(self);

  TXTRecordRef txt_record;
  TXTRecordCreate(&txt_record, 0, NULL);

  for (size_t i = 0; i < ShipPairingEntryGetTxtPairCount(); ++i) {
    const char* const key   = ShipPairingEntryGetTxtKey(i);
    const char* const value = ShipPairingEntryGetTxtValue(entry, i);
    if ((key == NULL) || (value == NULL)) {
      MDNS_DEBUG_PRINTF("shippairing TXT record is incomplete, not announcing\n");
      TXTRecordDeallocate(&txt_record);
      return kEebusErrorInput;
    }

    const DNSServiceErrorType txt_error = TXTRecordSetValue(&txt_record, key, (uint8_t)strlen(value), value);
    if (txt_error != kDNSServiceErr_NoError) {
      MDNS_DEBUG_PRINTF("TXTRecordSetValue(%s) returned error %d\n", key, txt_error);
      TXTRecordDeallocate(&txt_record);
      return kEebusErrorInput;
    }
  }

  const DNSServiceErrorType error = DNSServiceRegister(
      &mdns->dns_service_pairing_register_ref,
      0,
      0,
      ShipPairingEntryGetName(entry),
      kShipPairingServiceType,
      NULL,
      NULL,
      htons(kShipPairingUnusedPort),
      TXTRecordGetLength(&txt_record),
      TXTRecordGetBytesPtr(&txt_record),
      MdnsRegisterServiceCallback,
      mdns
  );

  TXTRecordDeallocate(&txt_record);

  if (error != kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("DNSServiceRegister() for shippairing returned error %d\n", error);
    mdns->dns_service_pairing_register_ref = NULL;
    return kEebusErrorCommunication;
  }

  return kEebusErrorOk;
}

void DeregisterPairingService(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  if (mdns->dns_service_pairing_register_ref != NULL) {
    // Deallocating the ref is what sends the goodbye (section 5.5).
    DNSServiceRefDeallocate(mdns->dns_service_pairing_register_ref);
    mdns->dns_service_pairing_register_ref = NULL;
  }
}

void MdnsPairingBrowseServices(Mdns* self) {
  const DNSServiceErrorType error = DNSServiceBrowse(
      &self->dns_service_pairing_browser_ref,
      0,
      kDNSServiceInterfaceIndexAny,
      kShipPairingServiceType,
      NULL,
      MdnsPairingBrowseServicesCallback,
      self
  );

  if (error != kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("DNSServiceBrowse() for shippairing returned error %d\n", error);
    self->dns_service_pairing_browser_ref = NULL;
  }
}

void MdnsPairingBrowseServicesCallback(
    DNSServiceRef service_ref,
    DNSServiceFlags flags,
    uint32_t iface,
    DNSServiceErrorType err,
    const char* name,
    const char* type,
    const char* domain,
    void* ctx
) {
  UNUSED(service_ref);
  UNUSED(type);

  Mdns* const mdns = (Mdns*)ctx;

  if ((mdns == NULL) || (err != kDNSServiceErr_NoError)) {
    MDNS_DEBUG_PRINTF("shippairing browser error occurred: %d\n", err);
    if (mdns != NULL) {
      mdns->needs_recovery = true;
    }

    return;
  }

  if (!(flags & kDNSServiceFlagsAdd)) {
    // A goodbye. Nothing is trusted on the strength of an announcement still
    // being there, so there is nothing to undo.
    MDNS_DEBUG_PRINTF("Removed shippairing service: %s%s\n", name, domain);
    return;
  }

  // Stopped while this was in flight: resolving it would be work for an answer
  // nobody is waiting for.
  if (mdns->on_pairing_entries_found_cb == NULL) {
    return;
  }

  ShipPairingEntry* const entry = ShipPairingEntryCreate(name, domain, iface);
  if (entry == NULL) {
    return;
  }

  // A browse reports a name; the TXT record the request lives in arrives with
  // the resolve.
  ActiveResolveEntry* const resolve = (ActiveResolveEntry*)EEBUS_MALLOC(sizeof(ActiveResolveEntry));
  if (resolve == NULL) {
    ShipPairingEntryDelete(entry);
    return;
  }

  resolve->service_ref   = NULL;
  resolve->entry         = NULL;
  resolve->pairing_entry = entry;
  resolve->owner         = mdns;
  resolve->done          = false;

  const DNSServiceErrorType error = DNSServiceResolve(
      &resolve->service_ref,
      0,
      iface,
      name,
      kShipPairingServiceType,
      domain,
      MdnsPairingResolveServiceCallback,
      resolve
  );

  if (error != kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("DNSServiceResolve() for shippairing returned error %d\n", error);
    MdnsActiveResolveEntryDestroy(resolve);
    return;
  }

  VectorPushBack(mdns->active_resolves, resolve);
}

void MdnsPairingResolveServiceCallback(
    DNSServiceRef service_ref,
    DNSServiceFlags flags,
    uint32_t iface,
    DNSServiceErrorType err,
    const char* name,
    const char* host,
    uint16_t opaque_port,
    uint16_t txt_record_size,
    const unsigned char* txt_record,
    void* ctx
) {
  UNUSED(service_ref);
  UNUSED(flags);
  UNUSED(iface);
  UNUSED(name);
  // Section 5.3: the host name and port of a shippairing instance are not to be
  // used, so they are read off the wire and dropped.
  UNUSED(host);
  UNUSED(opaque_port);

  ActiveResolveEntry* const resolve = (ActiveResolveEntry*)ctx;
  if (resolve == NULL) {
    return;
  }

  Mdns* const mdns              = resolve->owner;
  ShipPairingEntry* const entry = resolve->pairing_entry;

  resolve->done = true;

  if ((err != kDNSServiceErr_NoError) || (mdns == NULL) || (entry == NULL)) {
    return;
  }

  MDNS_TXT_RECORD_PRINT(txt_record, txt_record_size);

  if (ShipPairingEntryParseTxtRecord(entry, (const char*)txt_record, txt_record_size) != kEebusErrorOk) {
    MDNS_DEBUG_PRINTF("Ignoring a malformed shippairing TXT record\n");
    return;
  }

  // Whether the request is valid, addressed here and genuine is decided by the
  // evaluator, which is the only thing holding the secret. Everything found is
  // handed over.
  VectorPushBack(mdns->pairing_found_entries, entry);
  resolve->pairing_entry = NULL;
}

void MdnsNotifyFoundPairingEntries(Mdns* mdns) {
  if ((mdns->on_pairing_entries_found_cb == NULL) || (VectorGetSize(mdns->pairing_found_entries) == 0)) {
    return;
  }

  Vector* const entries = mdns->pairing_found_entries;

  // Ownership passes to the callback, as it does for the SHIP browse.
  mdns->pairing_found_entries = VectorCreateWithDeallocator(ShipPairingEntryDeallocator);

  mdns->on_pairing_entries_found_cb(entries, mdns->pairing_context);
}

static void* MdnsBrowserLoop(void* parameters) {
  Mdns* const mdns = (Mdns*)parameters;

  uint32_t next_notify_time_seconds = 0;

  MdnsBrowseServices(mdns);
  if (mdns->dns_service_browser_ref == NULL) {
    MDNS_DEBUG_PRINTF("No browse ref to process, will retry\n");
    mdns->needs_recovery = true;
  }

  while (!mdns->cancel) {
    const uint32_t now_seconds = MdnsGetCurrentTimeSeconds();

    MdnsRecoverSessions(mdns, now_seconds);

    // Browsed from the same thread as SHIP itself, so that both share one set
    // of service refs and one select(). Neither specification asks for
    // discovery to be prompt, and a second browsing thread would have to be
    // kept in step with this one for no gain.
    //
    // Opened only while something is waiting for what it finds, so a node that
    // could not accept a request does not ask for any.
    if ((mdns->on_pairing_entries_found_cb != NULL) && (mdns->dns_service_pairing_browser_ref == NULL)) {
      MdnsPairingBrowseServices(mdns);
    } else if ((mdns->on_pairing_entries_found_cb == NULL) && (mdns->dns_service_pairing_browser_ref != NULL)) {
      DNSServiceRefDeallocate(mdns->dns_service_pairing_browser_ref);
      mdns->dns_service_pairing_browser_ref = NULL;
      VectorFreeElements(mdns->pairing_found_entries);
      VectorClear(mdns->pairing_found_entries);
    }

    fd_set readfds;
    FD_ZERO(&readfds);

    int maxfd               = -1;
    int browse_fd           = MdnsPrepareSockFd(mdns->dns_service_browser_ref, &readfds, &maxfd);
    int register_fd         = MdnsPrepareSockFd(mdns->dns_service_register_ref, &readfds, &maxfd);
    int pairing_browse_fd   = MdnsPrepareSockFd(mdns->dns_service_pairing_browser_ref, &readfds, &maxfd);
    int pairing_register_fd = MdnsPrepareSockFd(mdns->dns_service_pairing_register_ref, &readfds, &maxfd);

    for (size_t i = 0; i < VectorGetSize(mdns->active_resolves); ++i) {
      ActiveResolveEntry* const resolve = (ActiveResolveEntry*)VectorGetElement(mdns->active_resolves, i);
      if ((resolve != NULL) && (resolve->service_ref != NULL)) {
        MdnsPrepareSockFd(resolve->service_ref, &readfds, &maxfd);
      }
    }

    struct timeval tv = select_timeout;
    const int n       = select(maxfd + 1, &readfds, NULL, NULL, &tv);

    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      break;
    }

    if (n > 0) {
      MdnsDispatchReadyFds(mdns, &readfds, browse_fd, register_fd);

      if ((pairing_browse_fd >= 0) && FD_ISSET(pairing_browse_fd, &readfds)) {
        DNSServiceProcessResult(mdns->dns_service_pairing_browser_ref);
      }

      if ((pairing_register_fd >= 0) && FD_ISSET(pairing_register_fd, &readfds)) {
        DNSServiceProcessResult(mdns->dns_service_pairing_register_ref);
      }
    }

    if (now_seconds >= next_notify_time_seconds) {
      MdnsNotifyFoundEntries(mdns);
      MdnsNotifyFoundPairingEntries(mdns);
      next_notify_time_seconds
          = now_seconds + MdnsGetRandomInterval(kMdnsBrowseIntervalMinSeconds, kMdnsBrowseIntervalMaxSeconds);
    }
  }

  MdnsBrowserReset(mdns);

  return NULL;
}

static void MdnsRegisterServiceCallback(
    DNSServiceRef ref,
    DNSServiceFlags flags,
    DNSServiceErrorType error,
    const char* name,
    const char* type,
    const char* domain,
    void* ctx
) {
  UNUSED(ref);
  UNUSED(flags);
  UNUSED(name);
  UNUSED(type);
  UNUSED(domain);

  Mdns* const mdns = (Mdns*)ctx;

  if (error == kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("Service registered: %s.%s.%s\n", name, type, domain);
  } else {
    MDNS_DEBUG_PRINTF("Error registering service: %d\n", error);
    if (mdns != NULL) {
      mdns->needs_recovery = true;
    }
  }
}

static DNSServiceErrorType MdnsCreateTextRecord(TXTRecordRef* txt_record, const Mdns* mdns) {
  TXTRecordCreate(txt_record, 0, NULL);

  TRY_SET_TXT_RECORD_VALUE(txt_record, "txtvers", kShipServiceTxtVer);
  TRY_SET_TXT_RECORD_VALUE(txt_record, "id", mdns->service_name);
  TRY_SET_TXT_RECORD_VALUE(txt_record, "path", kShipServicePath);
  TRY_SET_TXT_RECORD_VALUE(txt_record, "ski", mdns->ski);

  const char* register_str = mdns->autoaccept ? "true" : "false";
  TRY_SET_TXT_RECORD_VALUE(txt_record, "register", register_str);
  TRY_SET_TXT_RECORD_VALUE(txt_record, "brand", mdns->device_info->brand);
  TRY_SET_TXT_RECORD_VALUE(txt_record, "type", mdns->device_info->type);
  TRY_SET_TXT_RECORD_VALUE(txt_record, "model", mdns->device_info->model);

  return kDNSServiceErr_NoError;
}

static EebusError RegisterService(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  DNSServiceErrorType dns_service_err = kDNSServiceErr_NoError;

  TXTRecordRef txt_record;
  dns_service_err = MdnsCreateTextRecord(&txt_record, mdns);
  if (dns_service_err != kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("MdnsCreateTextRecord() returned error %d\n", dns_service_err);
    return kEebusErrorInit;
  }

  if (mdns->dns_service_register_ref != NULL) {
    DNSServiceRefDeallocate(mdns->dns_service_register_ref);
    mdns->dns_service_register_ref = NULL;
  }

  dns_service_err = DNSServiceRegister(
      &mdns->dns_service_register_ref,
      0,                                  // No flags
      kDNSServiceInterfaceIndexAny,       // Register on all network interfaces
      mdns->service_name,                 // Service name
      kShipServiceType,                   // Service type
      NULL,                               // default domain (e.g. local.)
      NULL,                               // default host name
      htons(mdns->port),                  // Port number
      TXTRecordGetLength(&txt_record),    // TXT record length
      TXTRecordGetBytesPtr(&txt_record),  // TXT record
      MdnsRegisterServiceCallback,        // Callback function
      mdns                                // Callback context (owning Mdns instance)
  );

  TXTRecordDeallocate(&txt_record);
  if (dns_service_err != kDNSServiceErr_NoError) {
    MDNS_DEBUG_PRINTF("DNSServiceRegister() returned error %d\n", dns_service_err);
    return kEebusErrorActivate;
  }

  if (mdns->dns_service_register_ref == NULL) {
    MDNS_DEBUG_PRINTF("DNSServiceRegister() failed to create a service reference\n");
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

static EebusError Start(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  EebusError ret = RegisterService(self);
  if (ret != kEebusErrorOk) {
    MDNS_DEBUG_PRINTF("RegisterService() returned error %d\n", ret);
    return ret;
  }

  if (mdns->on_entries_found_cb == NULL) {
    MDNS_DEBUG_PRINTF("No callback function set\n");
    DeregisterService(self);
    return kEebusErrorInit;
  }

  mdns->thread = EebusThreadCreate(MdnsBrowserLoop, mdns, 4096);
  if (mdns->thread == NULL) {
    MDNS_DEBUG_PRINTF("EebusThreadCreate() failed\n");
    DeregisterService(self);
    return kEebusErrorThread;
  }

  return kEebusErrorOk;
}

static void DeregisterService(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  if (mdns->dns_service_register_ref != NULL) {
    DNSServiceRefDeallocate(mdns->dns_service_register_ref);
    mdns->dns_service_register_ref = NULL;
  }
}

static void Stop(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  mdns->cancel = true;

  if (mdns->thread != NULL) {
    EEBUS_THREAD_JOIN(mdns->thread);
    EebusThreadDelete(mdns->thread);
    mdns->thread = NULL;
  }

  DeregisterService(self);
}

static void SetAutoaccept(ShipMdnsObject* self, bool autoaccept) {
  Mdns* const mdns = MDNS(self);
  mdns->autoaccept = autoaccept;
}
