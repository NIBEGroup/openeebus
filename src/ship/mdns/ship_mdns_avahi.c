#include <arpa/inet.h>
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-client/publish.h>
#include <avahi-common/error.h>
#include <avahi-common/malloc.h>
#include <avahi-common/simple-watch.h>
#include <sys/time.h>
#include <time.h>

#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/vector.h"
#include "src/ship/api/mdns_entry.h"
#include "src/ship/api/ship_mdns_interface.h"
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
};

#define MDNS(obj) ((Mdns*)(obj))

static void Destruct(ShipMdnsObject* self);
static EebusError Start(ShipMdnsObject* self);
static void Stop(ShipMdnsObject* self);
static EebusError RegisterService(ShipMdnsObject* self);
static void DeregisterService(ShipMdnsObject* self);
static void SetAutoaccept(ShipMdnsObject* self, bool autoaccept);

static const ShipMdnsInterface mdns_methods = {
    .destruct           = Destruct,
    .start              = Start,
    .stop               = Stop,
    .register_service   = RegisterService,
    .deregister_service = DeregisterService,
    .set_autoaccept     = SetAutoaccept,
};

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

static void Destruct(ShipMdnsObject* self) {
    SHIP_MDNS_STOP(self);

    Mdns* const mdns = MDNS(self);

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
    MDNS_DEBUG_PRINTF(
        "%s can be reached at %s:%u (interface %u), flags: %d\n",
        name,
        host_name,
        port,
        interface,
        flags
    );
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
            MDNS_DEBUG_PRINTF(
                "could not resolve %s: error: %s\n",
                name,
                avahi_strerror(avahi_client_errno(mdns->client))
            );
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
    result = avahi_string_list_add_pair(result, "id", mdns->service_name);
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

static void MdnsClientCallback(AvahiClient* c, AvahiClientState state, void* userdata) {
    Mdns* const mdns = (Mdns*)userdata;
    mdns->client     = c;

    MDNS_DEBUG_PRINTF("client state = %d\n", state);
    switch (state) {
        case AVAHI_CLIENT_S_RUNNING:
            MdnsRegisterService(mdns);
            MdnsBrowseServices(mdns);
            MdnsScheduleNotifyTimeout(mdns);
            break;

        case AVAHI_CLIENT_S_COLLISION:
        case AVAHI_CLIENT_S_REGISTERING:
            if (mdns->service_group != NULL) {
                avahi_entry_group_reset(mdns->service_group);
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
        if (mdns->service_browser != NULL) {
            avahi_service_browser_free(mdns->service_browser);
            mdns->service_browser = NULL;
        }
        if (mdns->service_group != NULL) {
            avahi_entry_group_free(mdns->service_group);
            mdns->service_group = NULL;
        }
        if (mdns->poll != NULL) {
            if (mdns->notify_timeout != NULL) {
                const AvahiPoll* api = avahi_simple_poll_get(mdns->poll);
                api->timeout_free(mdns->notify_timeout);
                mdns->notify_timeout = NULL;
            }
            avahi_simple_poll_free(mdns->poll);
            mdns->poll = NULL;
        }
        if (mdns->client != NULL) {
            avahi_client_free(mdns->client);
            mdns->client = NULL;
        }
        mdns->poll = avahi_simple_poll_new();
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

        avahi_simple_poll_loop(mdns->poll);
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
