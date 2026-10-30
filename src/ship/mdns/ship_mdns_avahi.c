#include <arpa/inet.h>
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-client/publish.h>
#include <avahi-common/domain.h>
#include <avahi-common/error.h>
#include <avahi-common/malloc.h>
#include <avahi-common/simple-watch.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_mutex/eebus_mutex.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/vector.h"
#include "src/ship/api/mdns_entry.h"
#include "src/ship/api/ship_mdns_interface.h"
#include "src/ship/api/ship_pairing_entry.h"
#include "src/ship/mdns/mdns_debug.h"
#include "src/ship/ship_connection/types.h"

/** mDNS debug printf(), enabled whith MDNS_DEBUG = 1 */
#if MDNS_DEBUG
#define MDNS_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define MDNS_DEBUG_PRINTF(fmt, ...)
#endif  // MDNS_DEBUG

static const char* kShipServiceType   = "_ship._tcp";
static const char* kShipServicePath   = "/ship/";
static const char* kShipServiceTxtVer = "1";

static const char* kShipPairingServiceType = SHIP_PAIRING_SERVICE_TYPE;

// Section 5.3 recommends a TTL of two minutes for shippairing records. The
// Avahi client API does not expose the TTL of the records a service publishes,
// so the daemon's default applies. A withdrawn request is still removed at once
// by the goodbye that freeing its entry group sends.

/**
 * Section 5.3: the port of a shippairing SRV record is unused. It must never be
 * connected to and nothing is ever sent to it, but a port has to be published.
 */
static const uint16_t kShipPairingUnusedPort = 1;

typedef struct Mdns Mdns;

typedef struct ActiveResolveEntry ActiveResolveEntry;
/** Tracks a single pending AvahiServiceResolver */
struct ActiveResolveEntry {
  /** The AvahiServiceResolver for this resolve */
  AvahiServiceResolver* service_resolver;
  /** The MdnsEntry being resolved */
  MdnsEntry* entry;
  /** The owning Mdns instance */
  Mdns* owner;
};

typedef struct PairingLookup PairingLookup;
/**
 * Tracks the TXT record query of one shippairing instance
 *
 * The raw TXT record is queried instead of resolving the service: Avahi hands
 * the TXT strings of a resolve over in reverse order, but section 5.4 requires
 * txtvers to be the first key. The host and port a resolve would add must not
 * be used anyway (section 5.3).
 */
struct PairingLookup {
  AvahiRecordBrowser* record_browser;
  ShipPairingEntry* entry;
  Mdns* owner;
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

  Vector* active_resolves;
  Vector* found_entries;

  EebusThreadObject* thread;

  AvahiSimplePoll* poll;
  AvahiClient* client;
  AvahiServiceBrowser* service_browser;
  AvahiEntryGroup* service_group;
  AvahiTimeout* notify_timeout;
  bool cancel;

  /**
   * @brief SHIP Pairing Service state requested by the callers
   *
   * Callers run on other threads, and the browse callback may stop the browse
   * from the mDNS thread itself. Avahi objects are only ever touched by the
   * mDNS thread, so a caller just records what it wants here and wakes that
   * thread up, which applies it between two poll iterations
   * (MdnsApplyPairingRequests). Also guards replacing the poll object, which
   * the wakeup needs.
   */
  EebusMutexObject* pairing_mutex;
  OnShipPairingEntriesFoundCallback on_pairing_entries_found_cb;
  void* pairing_context;
  ShipPairingEntry* pairing_announcement;
  bool pairing_announcement_changed;

  /** @brief Owned by the mDNS thread */
  AvahiServiceBrowser* pairing_browser;
  AvahiEntryGroup* pairing_group;
  Vector* pairing_lookups;
  Vector* pairing_found_entries;
};

#define MDNS(obj) ((Mdns*)(obj))

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
static bool SupportsPairing(ShipMdnsObject* self);

static const ShipMdnsInterface mdns_methods = {
    .destruct           = Destruct,
    .start              = Start,
    .stop               = Stop,
    .register_service   = RegisterService,
    .deregister_service = DeregisterService,
    .set_autoaccept     = SetAutoaccept,

    .start_pairing_browse       = StartPairingBrowse,
    .stop_pairing_browse        = StopPairingBrowse,
    .register_pairing_service   = RegisterPairingService,
    .deregister_pairing_service = DeregisterPairingService,
    .supports_pairing           = SupportsPairing,
};

static void MdnsFreeClientObjects(Mdns* self);
static void MdnsNotifyFoundPairingEntries(Mdns* self);

static ActiveResolveEntry* MdnsActiveResolveEntryCreate(Mdns* owner, MdnsEntry* entry) {
  ActiveResolveEntry* const resolve = (ActiveResolveEntry*)EEBUS_MALLOC(sizeof(ActiveResolveEntry));
  if (resolve == NULL) {
    return NULL;
  }

  resolve->service_resolver = NULL;
  resolve->entry            = entry;
  resolve->owner            = owner;

  return resolve;
}

static void MdnsActiveResolveEntryDestroy(ActiveResolveEntry* resolve) {
  if (resolve == NULL) {
    return;
  }

  if (resolve->service_resolver != NULL) {
    avahi_service_resolver_free(resolve->service_resolver);
    resolve->service_resolver = NULL;
  }

  if (resolve->entry != NULL) {
    MdnsEntryDelete(resolve->entry);
    resolve->entry = NULL;
  }

  EEBUS_FREE(resolve);
}

static void MdnsActiveResolveEntryDeallocator(void* resolve) {
  MdnsActiveResolveEntryDestroy((ActiveResolveEntry*)resolve);
}

static PairingLookup* MdnsPairingLookupCreate(Mdns* owner, ShipPairingEntry* entry) {
  PairingLookup* const lookup = (PairingLookup*)EEBUS_MALLOC(sizeof(PairingLookup));
  if (lookup == NULL) {
    return NULL;
  }

  lookup->record_browser = NULL;
  lookup->entry          = entry;
  lookup->owner          = owner;

  return lookup;
}

static void MdnsPairingLookupDestroy(PairingLookup* lookup) {
  if (lookup == NULL) {
    return;
  }

  if (lookup->record_browser != NULL) {
    avahi_record_browser_free(lookup->record_browser);
    lookup->record_browser = NULL;
  }

  ShipPairingEntryDelete(lookup->entry);
  lookup->entry = NULL;

  EEBUS_FREE(lookup);
}

static void MdnsPairingLookupDeallocator(void* lookup) {
  MdnsPairingLookupDestroy((PairingLookup*)lookup);
}

static void MdnsVectorDelete(Vector* vector) {
  if (vector != NULL) {
    VectorFreeElements(vector);
    VectorDestruct(vector);
    EEBUS_FREE(vector);
  }
}

static bool MdnsHasMatchingEntry(const Mdns* self, const MdnsEntry* candidate) {
  if ((self == NULL) || (self->found_entries == NULL) || (candidate == NULL)) {
    return false;
  }

  const char* const candidate_ski = MdnsEntryGetSki(candidate);
  if ((candidate_ski == NULL) || (candidate_ski[0] == '\0')) {
    return false;
  }

  const size_t found_count = VectorGetSize(self->found_entries);
  for (size_t i = 0; i < found_count; ++i) {
    MdnsEntry* const existing = (MdnsEntry*)VectorGetElement(self->found_entries, i);
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

static void MdnsNotifyFoundEntries(const Mdns* self) {
  const size_t found_count = VectorGetSize(self->found_entries);

  MDNS_DEBUG_PRINTF("Number of found entries: %zu\n", found_count);

  Vector* const copy = VectorCreateWithDeallocator(MdnsEntryDeallocator);

  for (size_t i = 0; i < found_count; ++i) {
    MdnsEntry* const entry = (MdnsEntry*)VectorGetElement(self->found_entries, i);
    if (entry != NULL) {
      VectorPushBack(copy, MdnsEntryCopy(entry));
    }
  }

  self->on_entries_found_cb(copy, self->context);
}

static struct timeval MdnsGetCurrentTime(void) {
  struct timespec now;
  struct timeval tv;

  clock_gettime(CLOCK_REALTIME, &now);
  tv.tv_sec  = now.tv_sec;
  tv.tv_usec = now.tv_nsec / 1000;
  return tv;
}

static uint32_t GetUpdateIntervalS(void) {
  uint8_t update_interval
      = kMdnsBrowseIntervalMinSeconds + rand() % (kMdnsBrowseIntervalMaxSeconds - kMdnsBrowseIntervalMinSeconds);

  return update_interval;
}

static void MdnsNotifyTimeoutCallback(AvahiTimeout* t, void* userdata) {
  Mdns* const mdns = (Mdns*)userdata;

  MdnsNotifyFoundEntries(mdns);
  MdnsNotifyFoundPairingEntries(mdns);

  struct timeval tv = MdnsGetCurrentTime();
  tv.tv_sec += GetUpdateIntervalS();
  const AvahiPoll* api = avahi_simple_poll_get(mdns->poll);
  api->timeout_update(t, &tv);
}

static void MdnsScheduleNotifyTimeout(Mdns* mdns) {
  if (mdns->notify_timeout != NULL) {
    return;
  }
  struct timeval tv = MdnsGetCurrentTime();
  tv.tv_sec += GetUpdateIntervalS();
  const AvahiPoll* api = avahi_simple_poll_get(mdns->poll);
  mdns->notify_timeout = api->timeout_new(api, &tv, MdnsNotifyTimeoutCallback, mdns);
}

static void MdnsRemoveEntryByName(Mdns* self, const char* name) {
  const size_t size          = VectorGetSize(self->found_entries);
  MdnsEntry* entry_to_remove = NULL;

  for (size_t i = 0; i < size; ++i) {
    MdnsEntry* entry = (MdnsEntry*)VectorGetElement(self->found_entries, i);
    if ((entry != NULL) && (strcmp(entry->name, name) == 0)) {
      entry_to_remove = entry;
      break;
    }
  }

  if (entry_to_remove != NULL) {
    VectorRemove(self->found_entries, entry_to_remove);
    MdnsEntryDelete(entry_to_remove);
    MdnsNotifyFoundEntries(self);
  }
}

static void MdnsConstruct(
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

  self->found_entries   = VectorCreateWithDeallocator(MdnsEntryDeallocator);
  self->active_resolves = VectorCreateWithDeallocator(MdnsActiveResolveEntryDeallocator);

  self->poll            = NULL;
  self->client          = NULL;
  self->service_browser = NULL;
  self->service_group   = NULL;
  self->notify_timeout  = NULL;
  self->cancel          = false;

  self->pairing_mutex                = EebusMutexCreate();
  self->on_pairing_entries_found_cb  = NULL;
  self->pairing_context              = NULL;
  self->pairing_announcement         = NULL;
  self->pairing_announcement_changed = false;
  self->pairing_browser              = NULL;
  self->pairing_group                = NULL;
  self->pairing_lookups              = VectorCreateWithDeallocator(MdnsPairingLookupDeallocator);
  self->pairing_found_entries        = VectorCreateWithDeallocator(ShipPairingEntryDeallocator);

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

  if ((mdns->pairing_mutex == NULL) || (mdns->pairing_lookups == NULL) || (mdns->pairing_found_entries == NULL)) {
    EebusMutexDelete(mdns->pairing_mutex);
    MdnsVectorDelete(mdns->pairing_lookups);
    MdnsVectorDelete(mdns->pairing_found_entries);
    MdnsVectorDelete(mdns->found_entries);
    MdnsVectorDelete(mdns->active_resolves);
    EebusDeviceInfoDelete(mdns->device_info);
    EEBUS_FREE(mdns);
    return NULL;
  }

  return SHIP_MDNS_OBJECT(mdns);
}

static void Destruct(ShipMdnsObject* self) {
  SHIP_MDNS_STOP(self);

  Mdns* const mdns = MDNS(self);

  // Avahi objects belong to the client and have to be freed before it is.
  MdnsFreeClientObjects(mdns);

  MdnsVectorDelete(mdns->found_entries);
  mdns->found_entries = NULL;
  MdnsVectorDelete(mdns->active_resolves);
  mdns->active_resolves = NULL;
  MdnsVectorDelete(mdns->pairing_lookups);
  mdns->pairing_lookups = NULL;
  MdnsVectorDelete(mdns->pairing_found_entries);
  mdns->pairing_found_entries = NULL;

  ShipPairingEntryDelete(mdns->pairing_announcement);
  mdns->pairing_announcement = NULL;
  EebusMutexDelete(mdns->pairing_mutex);
  mdns->pairing_mutex = NULL;

  if (mdns->client != NULL) {
    avahi_client_free(mdns->client);
    mdns->client = NULL;
  }

  if (mdns->poll != NULL) {
    avahi_simple_poll_free(mdns->poll);
    mdns->poll = NULL;
  }

  EebusDeviceInfoDelete(mdns->device_info);
  mdns->device_info = NULL;
}

static void MdnsResolverFound(
    Mdns* self,
    ActiveResolveEntry* resolve,
    MdnsEntry* entry,
    AvahiIfIndex interface,
    const char* name,
    const char* host_name,
    uint16_t port,
    AvahiStringList* txt,
    AvahiLookupResultFlags flags
) {
  UNUSED(interface);
  UNUSED(name);
  UNUSED(flags);
  MDNS_DEBUG_PRINTF("%s can be reached at %s:%u (interface %u), flags: %d\n", name, host_name, port, interface, flags);
  MdnsEntrySetHost(entry, host_name);
  MdnsEntrySetPort(entry, port);

  AvahiStringList* it = txt;
  while (it != NULL) {
    char* key           = NULL;
    char* value         = NULL;
    size_t value_length = 0;
    avahi_string_list_get_pair(it, &key, &value, &value_length);

    if (key != NULL && value != NULL) {
      MdnsEntrySetValue(entry, key, strlen(key), value, value_length);
      avahi_free(key);
      avahi_free(value);
    }
    it = avahi_string_list_get_next(it);
  }

  const bool is_valid     = MdnsEntryIsValid(entry);
  const bool is_own_entry = is_valid && (strcmp(entry->ski, self->ski) == 0);

  if (is_valid && !is_own_entry) {
    if (MdnsHasMatchingEntry(self, entry)) {
      MDNS_DEBUG_PRINTF("Ignoring duplicate entry: %s\n", entry->name);
      return;
    }

    // Transfer ownership of entry to found_entries vector
    VectorPushBack(self->found_entries, entry);
    resolve->entry = NULL;
    MdnsNotifyFoundEntries(self);
  } else {
    MDNS_DEBUG_PRINTF("Ignoring invalid or own entry\n");
  }
}

static void MdnsResolverCallback(
    AvahiServiceResolver* r,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char* name,
    const char* type,
    const char* domain,
    const char* host_name,
    const AvahiAddress* address,
    uint16_t port,
    AvahiStringList* txt,
    AvahiLookupResultFlags flags,
    void* userdata
) {
  UNUSED(r);
  UNUSED(protocol);
  UNUSED(type);
  UNUSED(domain);
  UNUSED(address);
  ActiveResolveEntry* const resolve = (ActiveResolveEntry*)userdata;

  if (resolve == NULL) {
    return;
  }

  Mdns* const mdns       = resolve->owner;
  MdnsEntry* const entry = resolve->entry;

  if (entry == NULL) {
    MDNS_DEBUG_PRINTF("%s(), NULL mDNS entry\n", __func__);
  } else if (mdns == NULL) {
    MDNS_DEBUG_PRINTF("%s(), NULL mDNS owner\n", __func__);
  } else {
    if (event == AVAHI_RESOLVER_FAILURE) {
      MDNS_DEBUG_PRINTF("could not resolve %s: error: %s\n", name, avahi_strerror(avahi_client_errno(mdns->client)));
    } else if (event == AVAHI_RESOLVER_FOUND) {
      MdnsResolverFound(mdns, resolve, entry, interface, name, host_name, port, txt, flags);
    } else {
      MDNS_DEBUG_PRINTF("Unknown resolver event: %d\n", event);
    }
  }

  VectorRemove(mdns->active_resolves, resolve);
  MdnsActiveResolveEntryDestroy(resolve);
}

static void MdnsBrowserNew(
    Mdns* self,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    const char* name,
    const char* type,
    const char* domain,
    AvahiLookupResultFlags flags
) {
  MDNS_DEBUG_PRINTF("%s %30s.%s%s on interface %d\n", "ADD", name, type, domain, interface);

  if (flags & AVAHI_LOOKUP_RESULT_OUR_OWN) {
    MDNS_DEBUG_PRINTF("Ignoring own service: %s\n", name);
    return;
  }

  MdnsEntry* const entry = MdnsEntryCreate(name, domain, interface);
  if (entry == NULL) {
    return;
  }

  ActiveResolveEntry* const resolve = MdnsActiveResolveEntryCreate(self, entry);
  if (resolve == NULL) {
    MdnsEntryDelete(entry);
    return;
  }

  resolve->service_resolver = avahi_service_resolver_new(
      self->client,
      interface,
      protocol,
      name,
      type,
      domain,
      AVAHI_PROTO_UNSPEC,
      0,
      MdnsResolverCallback,
      resolve
  );
  if (resolve->service_resolver == NULL) {
    MDNS_DEBUG_PRINTF(
        "Failed to create service resolver for %s.%s%s: %s\n",
        name,
        type,
        domain,
        avahi_strerror(avahi_client_errno(self->client))
    );
    MdnsActiveResolveEntryDestroy(resolve);
    return;
  }

  VectorPushBack(self->active_resolves, resolve);
}

static void MdnsBrowserCallback(
    AvahiServiceBrowser* b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char* name,
    const char* type,
    const char* domain,
    AvahiLookupResultFlags flags,
    void* userdata
) {
  UNUSED(b);
  Mdns* const mdns = (Mdns*)userdata;

  switch (event) {
    case AVAHI_BROWSER_NEW: MdnsBrowserNew(mdns, interface, protocol, name, type, domain, flags); break;

    case AVAHI_BROWSER_REMOVE:
      MDNS_DEBUG_PRINTF("Removed service: %s.%s%s\n", name, type, domain);
      MdnsRemoveEntryByName(mdns, name);
      break;

    case AVAHI_BROWSER_FAILURE:
      MDNS_DEBUG_PRINTF("Bonjour browser error occurred: %s\n", avahi_strerror(avahi_client_errno(mdns->client)));
      avahi_simple_poll_quit(mdns->poll);
      break;

    case AVAHI_BROWSER_ALL_FOR_NOW:
    case AVAHI_BROWSER_CACHE_EXHAUSTED: break;
  }
}

static void MdnsBrowseServices(Mdns* self) {
  self->service_browser = avahi_service_browser_new(
      self->client,
      AVAHI_IF_UNSPEC,
      AVAHI_PROTO_UNSPEC,
      "_ship._tcp",
      NULL,
      0,
      MdnsBrowserCallback,
      self
  );
  if (self->service_browser == NULL) {
    MDNS_DEBUG_PRINTF("Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(self->client)));
    avahi_simple_poll_quit(self->poll);
  }
}

static void MdnsEntryGroupCallback(AvahiEntryGroup* g, AvahiEntryGroupState state, void* userdata) {
  (void)g;
  Mdns* const mdns = (Mdns*)userdata;

  switch (state) {
    case AVAHI_ENTRY_GROUP_ESTABLISHED: MDNS_DEBUG_PRINTF("Service registered: %s\n", mdns->service_name); break;

    case AVAHI_ENTRY_GROUP_COLLISION:
      MDNS_DEBUG_PRINTF("Service name collision: %s\n", mdns->service_name);
      avahi_simple_poll_quit(mdns->poll);
      break;

    case AVAHI_ENTRY_GROUP_FAILURE:
      MDNS_DEBUG_PRINTF("Error registering service: %s\n", avahi_strerror(avahi_client_errno(mdns->client)));
      avahi_simple_poll_quit(mdns->poll);
      break;

    default: break;
  }
}

static AvahiStringList* MdnsCreateTextRecord(const Mdns* mdns) {
  AvahiStringList* result  = NULL;
  const char* register_str = mdns->autoaccept ? "true" : "false";

  result = avahi_string_list_add_pair(result, "txtvers", kShipServiceTxtVer);
  result = avahi_string_list_add_pair(result, "id", mdns->device_info->ship_id);
  result = avahi_string_list_add_pair(result, "path", kShipServicePath);
  result = avahi_string_list_add_pair(result, "ski", mdns->ski);
  result = avahi_string_list_add_pair(result, "register", register_str);
  result = avahi_string_list_add_pair(result, "brand", mdns->device_info->brand);
  result = avahi_string_list_add_pair(result, "type", mdns->device_info->type);
  result = avahi_string_list_add_pair(result, "model", mdns->device_info->model);
  return result;
}

static void MdnsRegisterService(Mdns* self) {
  int ret;

  if (self->service_group == NULL) {
    MDNS_DEBUG_PRINTF("Creating entry group for service: %s\n", self->service_name);
    self->service_group = avahi_entry_group_new(self->client, MdnsEntryGroupCallback, self);
    if (self->service_group == NULL) {
      MDNS_DEBUG_PRINTF("Failed to create entry group: %s\n", avahi_strerror(avahi_client_errno(self->client)));
      avahi_simple_poll_quit(self->poll);
      return;
    }
  }

  if (!avahi_entry_group_is_empty(self->service_group)) {
    return;
  }

  AvahiStringList* const txt_record = MdnsCreateTextRecord(self);

  ret = avahi_entry_group_add_service_strlst(
      self->service_group,
      AVAHI_IF_UNSPEC,
      AVAHI_PROTO_UNSPEC,
      0,
      self->service_name,
      kShipServiceType,
      NULL,  // domain
      NULL,  // host
      self->port,
      txt_record
  );
  avahi_string_list_free(txt_record);

  if (ret < 0) {
    MDNS_DEBUG_PRINTF("Failed to add service: %s\n", avahi_strerror(ret));
    avahi_simple_poll_quit(self->poll);
    return;
  }

  ret = avahi_entry_group_commit(self->service_group);

  if (ret < 0) {
    MDNS_DEBUG_PRINTF("Failed to commit entry group: %s\n", avahi_strerror(ret));
    avahi_simple_poll_quit(self->poll);
  }
}

static bool MdnsIsPairingInstanceKnown(const Mdns* self, const char* name) {
  for (size_t i = 0; i < VectorGetSize(self->pairing_lookups); ++i) {
    const PairingLookup* const lookup = (const PairingLookup*)VectorGetElement(self->pairing_lookups, i);
    if ((lookup != NULL) && (strcmp(ShipPairingEntryGetName(lookup->entry), name) == 0)) {
      return true;
    }
  }

  for (size_t i = 0; i < VectorGetSize(self->pairing_found_entries); ++i) {
    const ShipPairingEntry* const entry = (const ShipPairingEntry*)VectorGetElement(self->pairing_found_entries, i);
    if ((entry != NULL) && (strcmp(ShipPairingEntryGetName(entry), name) == 0)) {
      return true;
    }
  }

  return false;
}

static void MdnsRemovePairingLookup(Mdns* self, PairingLookup* lookup) {
  VectorRemove(self->pairing_lookups, lookup);
  MdnsPairingLookupDestroy(lookup);
}

static void MdnsPairingRecordBrowserCallback(
    AvahiRecordBrowser* b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char* name,
    uint16_t clazz,
    uint16_t type,
    const void* rdata,
    size_t size,
    AvahiLookupResultFlags flags,
    void* userdata
) {
  UNUSED(b);
  UNUSED(interface);
  UNUSED(protocol);
  UNUSED(name);
  UNUSED(clazz);
  UNUSED(type);
  UNUSED(flags);

  PairingLookup* const lookup = (PairingLookup*)userdata;
  Mdns* const mdns            = lookup->owner;

  if (event == AVAHI_BROWSER_NEW) {
    MDNS_TXT_RECORD_PRINT(rdata, size);

    if ((size > UINT16_MAX)
        || (ShipPairingEntryParseTxtRecord(lookup->entry, (const char*)rdata, (uint16_t)size) != kEebusErrorOk)) {
      MDNS_DEBUG_PRINTF("Ignoring a malformed shippairing TXT record\n");
    } else {
      // Whether the request is valid, addressed here and genuine is decided by
      // the evaluator, which is the only thing holding the secret. Everything
      // found is handed over.
      VectorPushBack(mdns->pairing_found_entries, lookup->entry);
      lookup->entry = NULL;
    }
  } else if (event == AVAHI_BROWSER_FAILURE) {
    MDNS_DEBUG_PRINTF("shippairing TXT query failed: %s\n", avahi_strerror(avahi_client_errno(mdns->client)));
  } else {
    // ALL_FOR_NOW and CACHE_EXHAUSTED: the record may still arrive.
    return;
  }

  MdnsRemovePairingLookup(mdns, lookup);
}

static void MdnsPairingBrowserNew(
    Mdns* self,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    const char* name,
    const char* type,
    const char* domain
) {
  // The same instance is reported once per interface and protocol.
  if (MdnsIsPairingInstanceKnown(self, name)) {
    return;
  }

  char full_name[AVAHI_DOMAIN_NAME_MAX];
  if (avahi_service_name_join(full_name, sizeof(full_name), name, type, domain) < 0) {
    MDNS_DEBUG_PRINTF("Invalid shippairing instance name: %s\n", name);
    return;
  }

  ShipPairingEntry* const entry = ShipPairingEntryCreate(name, domain, (uint32_t)interface);
  if (entry == NULL) {
    return;
  }

  PairingLookup* const lookup = MdnsPairingLookupCreate(self, entry);
  if (lookup == NULL) {
    ShipPairingEntryDelete(entry);
    return;
  }

  lookup->record_browser = avahi_record_browser_new(
      self->client,
      interface,
      protocol,
      full_name,
      AVAHI_DNS_CLASS_IN,
      AVAHI_DNS_TYPE_TXT,
      0,
      MdnsPairingRecordBrowserCallback,
      lookup
  );
  if (lookup->record_browser == NULL) {
    MDNS_DEBUG_PRINTF("Failed to query shippairing TXT record: %s\n", avahi_strerror(avahi_client_errno(self->client)));
    MdnsPairingLookupDestroy(lookup);
    return;
  }

  VectorPushBack(self->pairing_lookups, lookup);
}

static void MdnsPairingBrowserRemove(Mdns* self, const char* name) {
  // A goodbye. Nothing is trusted on the strength of an announcement still
  // being there, so only a query that has not been answered yet is dropped.
  for (size_t i = 0; i < VectorGetSize(self->pairing_lookups); ++i) {
    PairingLookup* const lookup = (PairingLookup*)VectorGetElement(self->pairing_lookups, i);
    if ((lookup != NULL) && (strcmp(ShipPairingEntryGetName(lookup->entry), name) == 0)) {
      MdnsRemovePairingLookup(self, lookup);
      return;
    }
  }
}

static void MdnsPairingBrowserCallback(
    AvahiServiceBrowser* b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char* name,
    const char* type,
    const char* domain,
    AvahiLookupResultFlags flags,
    void* userdata
) {
  UNUSED(b);
  UNUSED(flags);
  Mdns* const mdns = (Mdns*)userdata;

  // Requests announced by this host are not filtered out: the evaluator
  // rejects any that is not addressed to this node.
  switch (event) {
    case AVAHI_BROWSER_NEW: MdnsPairingBrowserNew(mdns, interface, protocol, name, type, domain); break;

    case AVAHI_BROWSER_REMOVE:
      MDNS_DEBUG_PRINTF("Removed shippairing service: %s.%s%s\n", name, type, domain);
      MdnsPairingBrowserRemove(mdns, name);
      break;

    case AVAHI_BROWSER_FAILURE:
      MDNS_DEBUG_PRINTF("shippairing browser error occurred: %s\n", avahi_strerror(avahi_client_errno(mdns->client)));
      break;

    case AVAHI_BROWSER_ALL_FOR_NOW:
    case AVAHI_BROWSER_CACHE_EXHAUSTED: break;
  }
}

static void MdnsStopPairingBrowser(Mdns* self) {
  if (self->pairing_browser != NULL) {
    avahi_service_browser_free(self->pairing_browser);
    self->pairing_browser = NULL;
  }

  VectorFreeElements(self->pairing_lookups);
  VectorClear(self->pairing_lookups);
  VectorFreeElements(self->pairing_found_entries);
  VectorClear(self->pairing_found_entries);
}

static void MdnsPairingEntryGroupCallback(AvahiEntryGroup* g, AvahiEntryGroupState state, void* userdata) {
  UNUSED(g);
  Mdns* const mdns = (Mdns*)userdata;
  UNUSED(mdns);

  // Failures are only reported: the SHIP service must not be restarted because
  // of an announcement, and a corrected request is a new one (section 5.5).
  switch (state) {
    case AVAHI_ENTRY_GROUP_ESTABLISHED: MDNS_DEBUG_PRINTF("shippairing request announced\n"); break;

    case AVAHI_ENTRY_GROUP_COLLISION: MDNS_DEBUG_PRINTF("shippairing instance name collision\n"); break;

    case AVAHI_ENTRY_GROUP_FAILURE:
      MDNS_DEBUG_PRINTF("Error announcing shippairing request: %s\n", avahi_strerror(avahi_client_errno(mdns->client)));
      break;

    default: break;
  }
}

static AvahiStringList* MdnsCreatePairingTextRecord(const ShipPairingEntry* entry) {
  // Avahi publishes the strings in the order they were added, and section 5.4
  // requires that order (txtvers first).
  AvahiStringList* result = NULL;

  for (size_t i = 0; i < ShipPairingEntryGetTxtPairCount(); ++i) {
    result = avahi_string_list_add_pair(result, ShipPairingEntryGetTxtKey(i), ShipPairingEntryGetTxtValue(entry, i));
  }

  return result;
}

static void MdnsAnnouncePairingRequest(Mdns* self, const ShipPairingEntry* entry) {
  self->pairing_group = avahi_entry_group_new(self->client, MdnsPairingEntryGroupCallback, self);
  if (self->pairing_group == NULL) {
    MDNS_DEBUG_PRINTF(
        "Failed to create shippairing entry group: %s\n",
        avahi_strerror(avahi_client_errno(self->client))
    );
    return;
  }

  AvahiStringList* const txt_record = MdnsCreatePairingTextRecord(entry);

  int ret = avahi_entry_group_add_service_strlst(
      self->pairing_group,
      AVAHI_IF_UNSPEC,
      AVAHI_PROTO_UNSPEC,
      0,
      ShipPairingEntryGetName(entry),
      kShipPairingServiceType,
      NULL,  // domain
      NULL,  // host
      kShipPairingUnusedPort,
      txt_record
  );
  avahi_string_list_free(txt_record);

  if (ret >= 0) {
    ret = avahi_entry_group_commit(self->pairing_group);
  }

  if (ret < 0) {
    MDNS_DEBUG_PRINTF("Failed to announce shippairing request: %s\n", avahi_strerror(ret));
  }
}

/**
 * @brief Brings the Avahi objects in line with what the callers requested
 *
 * Runs on the mDNS thread only, between two poll iterations and when the client
 * (re)connects to the daemon, which also restores a browse or an announcement
 * that a daemon restart took down.
 */
static void MdnsApplyPairingRequests(Mdns* self) {
  if ((self->client == NULL) || (avahi_client_get_state(self->client) != AVAHI_CLIENT_S_RUNNING)) {
    return;
  }

  EEBUS_MUTEX_LOCK(self->pairing_mutex);

  if ((self->on_pairing_entries_found_cb != NULL) && (self->pairing_browser == NULL)) {
    self->pairing_browser = avahi_service_browser_new(
        self->client,
        AVAHI_IF_UNSPEC,
        AVAHI_PROTO_UNSPEC,
        kShipPairingServiceType,
        NULL,
        0,
        MdnsPairingBrowserCallback,
        self
    );
    if (self->pairing_browser == NULL) {
      MDNS_DEBUG_PRINTF("Failed to browse shippairing: %s\n", avahi_strerror(avahi_client_errno(self->client)));
    }
  } else if ((self->on_pairing_entries_found_cb == NULL) && (self->pairing_browser != NULL)) {
    MdnsStopPairingBrowser(self);
  }

  // Section 5.5: a replaced request is withdrawn first, freeing its group is
  // what sends the goodbye.
  const bool announced = (self->pairing_group != NULL);
  if (self->pairing_announcement_changed || (announced && (self->pairing_announcement == NULL))) {
    if (self->pairing_group != NULL) {
      avahi_entry_group_free(self->pairing_group);
      self->pairing_group = NULL;
    }
  }

  if ((self->pairing_announcement != NULL) && (self->pairing_group == NULL)) {
    MdnsAnnouncePairingRequest(self, self->pairing_announcement);
  }

  self->pairing_announcement_changed = false;

  EEBUS_MUTEX_UNLOCK(self->pairing_mutex);
}

static void MdnsWakeUp(Mdns* self) {
  if (self->poll != NULL) {
    avahi_simple_poll_wakeup(self->poll);
  }
}

static bool MdnsGetPairingCallback(Mdns* self, OnShipPairingEntriesFoundCallback* cb, void** ctx) {
  EEBUS_MUTEX_LOCK(self->pairing_mutex);
  *cb  = self->on_pairing_entries_found_cb;
  *ctx = self->pairing_context;
  EEBUS_MUTEX_UNLOCK(self->pairing_mutex);

  return *cb != NULL;
}

static void MdnsNotifyFoundPairingEntries(Mdns* self) {
  OnShipPairingEntriesFoundCallback cb = NULL;
  void* ctx                            = NULL;
  if (!MdnsGetPairingCallback(self, &cb, &ctx) || (VectorGetSize(self->pairing_found_entries) == 0)) {
    return;
  }

  Vector* const entries = self->pairing_found_entries;

  // Unlike the SHIP browse, which reports every service still there, each
  // request is handed over once: the evaluator only has to see it once, and a
  // request seen again would only be re-authenticated to be rejected as a
  // replay. A browse started again reports every instance still announced.
  // The callback may stop the browse, which is why no lock is held while
  // calling it.
  self->pairing_found_entries = VectorCreateWithDeallocator(ShipPairingEntryDeallocator);

  cb(entries, ctx);
}

static void MdnsFreeClientObjects(Mdns* self) {
  VectorFreeElements(self->active_resolves);
  VectorClear(self->active_resolves);

  MdnsStopPairingBrowser(self);

  if (self->pairing_group != NULL) {
    avahi_entry_group_free(self->pairing_group);
    self->pairing_group = NULL;
  }

  if (self->service_browser != NULL) {
    avahi_service_browser_free(self->service_browser);
    self->service_browser = NULL;
  }

  if (self->service_group != NULL) {
    avahi_entry_group_free(self->service_group);
    self->service_group = NULL;
  }
}

static void MdnsClientCallback(AvahiClient* c, AvahiClientState state, void* userdata) {
  Mdns* const mdns = (Mdns*)userdata;
  mdns->client     = c;

  MDNS_DEBUG_PRINTF("client state = %d\n", state);
  switch (state) {
    case AVAHI_CLIENT_S_RUNNING:
      MdnsRegisterService(mdns);
      MdnsBrowseServices(mdns);
      MdnsScheduleNotifyTimeout(mdns);
      MdnsApplyPairingRequests(mdns);
      break;

    case AVAHI_CLIENT_S_COLLISION:
    case AVAHI_CLIENT_S_REGISTERING:
      if (mdns->service_group != NULL) {
        avahi_entry_group_reset(mdns->service_group);
      }
      // Announced again once the client is running.
      if (mdns->pairing_group != NULL) {
        avahi_entry_group_free(mdns->pairing_group);
        mdns->pairing_group = NULL;
      }
      break;

    case AVAHI_CLIENT_FAILURE:
      MDNS_DEBUG_PRINTF("client failure = %s\n", avahi_strerror(avahi_client_errno(c)));
      avahi_simple_poll_quit(mdns->poll);
      break;

    case AVAHI_CLIENT_CONNECTING: break;
  }
}

static void* MdnsThreadLoop(void* parameters) {
  Mdns* const mdns = (Mdns*)parameters;

  while (!mdns->cancel) {
    MdnsFreeClientObjects(mdns);
    if (mdns->client != NULL) {
      avahi_client_free(mdns->client);
      mdns->client = NULL;
    }

    EEBUS_MUTEX_LOCK(mdns->pairing_mutex);
    if (mdns->poll != NULL) {
      if (mdns->notify_timeout != NULL) {
        const AvahiPoll* api = avahi_simple_poll_get(mdns->poll);
        api->timeout_free(mdns->notify_timeout);
        mdns->notify_timeout = NULL;
      }
      avahi_simple_poll_free(mdns->poll);
      mdns->poll = NULL;
    }
    mdns->poll = avahi_simple_poll_new();
    EEBUS_MUTEX_UNLOCK(mdns->pairing_mutex);

    if (mdns->poll == NULL) {
      MDNS_DEBUG_PRINTF("Failed to create simple poll object\n");
      continue;
    }
    mdns->client
        = avahi_client_new(avahi_simple_poll_get(mdns->poll), AVAHI_CLIENT_NO_FAIL, MdnsClientCallback, mdns, NULL);
    if (mdns->client == NULL) {
      MDNS_DEBUG_PRINTF("Failed to create Avahi client\n");
      continue;
    }

    // avahi_simple_poll_loop(), with the pairing requests applied after every
    // iteration: that is where no Avahi callback is running.
    for (;;) {
      const int ret = avahi_simple_poll_iterate(mdns->poll, -1);
      if ((ret != 0) && ((ret > 0) || (errno != EINTR))) {
        break;
      }

      MdnsApplyPairingRequests(mdns);
    }
  }

  return NULL;
}

static EebusError Start(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  if (mdns->on_entries_found_cb == NULL) {
    MDNS_DEBUG_PRINTF("No callback function set\n");
    return kEebusErrorInit;
  }

  mdns->thread = EebusThreadCreate(MdnsThreadLoop, mdns, 4096);
  if (mdns->thread == NULL) {
    MDNS_DEBUG_PRINTF("EebusThreadCreate() failed\n");
    return kEebusErrorThread;
  }

  return kEebusErrorOk;
}

static void Stop(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  mdns->cancel = true;
  avahi_simple_poll_quit(mdns->poll);

  if (mdns->thread != NULL) {
    EEBUS_THREAD_JOIN(mdns->thread);
    EebusThreadDelete(mdns->thread);
    mdns->thread = NULL;
  }
}

static EebusError RegisterService(ShipMdnsObject* self) {
  UNUSED(self);
  return kEebusErrorOk;
}

static void DeregisterService(ShipMdnsObject* self) {
  UNUSED(self);
}

static void SetAutoaccept(ShipMdnsObject* self, bool autoaccept) {
  Mdns* const mdns = MDNS(self);
  mdns->autoaccept = autoaccept;
}

static EebusError StartPairingBrowse(ShipMdnsObject* self, OnShipPairingEntriesFoundCallback cb, void* ctx) {
  Mdns* const mdns = MDNS(self);

  if (cb == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  EEBUS_MUTEX_LOCK(mdns->pairing_mutex);
  mdns->on_pairing_entries_found_cb = cb;
  mdns->pairing_context             = ctx;
  MdnsWakeUp(mdns);
  EEBUS_MUTEX_UNLOCK(mdns->pairing_mutex);

  return kEebusErrorOk;
}

static void StopPairingBrowse(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  EEBUS_MUTEX_LOCK(mdns->pairing_mutex);
  mdns->on_pairing_entries_found_cb = NULL;
  mdns->pairing_context             = NULL;
  MdnsWakeUp(mdns);
  EEBUS_MUTEX_UNLOCK(mdns->pairing_mutex);
}

static EebusError RegisterPairingService(ShipMdnsObject* self, const ShipPairingEntry* entry) {
  Mdns* const mdns = MDNS(self);

  if (entry == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  // Checked here because the announcement itself is made later, on the mDNS
  // thread, where an error could no longer be returned.
  for (size_t i = 0; i < ShipPairingEntryGetTxtPairCount(); ++i) {
    if ((ShipPairingEntryGetTxtKey(i) == NULL) || (ShipPairingEntryGetTxtValue(entry, i) == NULL)) {
      MDNS_DEBUG_PRINTF("shippairing TXT record is incomplete, not announcing\n");
      return kEebusErrorInput;
    }
  }

  ShipPairingEntry* const announcement = ShipPairingEntryCopy(entry);
  if (announcement == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  EEBUS_MUTEX_LOCK(mdns->pairing_mutex);
  ShipPairingEntryDelete(mdns->pairing_announcement);
  mdns->pairing_announcement         = announcement;
  mdns->pairing_announcement_changed = true;
  MdnsWakeUp(mdns);
  EEBUS_MUTEX_UNLOCK(mdns->pairing_mutex);

  return kEebusErrorOk;
}

static void DeregisterPairingService(ShipMdnsObject* self) {
  Mdns* const mdns = MDNS(self);

  EEBUS_MUTEX_LOCK(mdns->pairing_mutex);
  ShipPairingEntryDelete(mdns->pairing_announcement);
  mdns->pairing_announcement = NULL;
  MdnsWakeUp(mdns);
  EEBUS_MUTEX_UNLOCK(mdns->pairing_mutex);
}

static bool SupportsPairing(ShipMdnsObject* self) {
  UNUSED(self);
  return true;
}
