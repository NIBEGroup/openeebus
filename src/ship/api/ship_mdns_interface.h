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
 * @brief SHIP mDNS interface declarations
 */

#ifndef SRC_SHIP_API_SHIP_MDNS_INTERFACE_H_
#define SRC_SHIP_API_SHIP_MDNS_INTERFACE_H_

#include <stdbool.h>

#include "src/common/vector.h"
#include "src/ship/api/mdns_entry.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum MdnsBrowseInterval {
    kMdnsBrowseIntervalMinSeconds = 10,
    kMdnsBrowseIntervalMaxSeconds = 20,
};

typedef enum MdnsBrowseInterval MdnsBrowseInterval;

/**
 * @brief On MDNS entries found callback
 *
 * Callback is used instead of MdnsSearchInterface
 * @note The user of the callback is responsible for freeing found_entries vector and its elements
 */
typedef void (*OnMdnsEntriesFoundCallback)(Vector* found_entries, void* context);

/**
 * @brief SHIP mDNS Interface
 * (SHIP mDNS "virtual functions table" declaration)
 */
typedef struct ShipMdnsInterface ShipMdnsInterface;

/**
 * @brief SHIP mDNS Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct ShipMdnsObject ShipMdnsObject;

/**
 * @brief ShipMdns Interface Structure
 */
struct ShipMdnsInterface {
    void (*destruct)(ShipMdnsObject* self);
    EebusError (*start)(ShipMdnsObject* self);
    void (*stop)(ShipMdnsObject* self);
    EebusError (*register_service)(ShipMdnsObject* self);
    void (*deregister_service)(ShipMdnsObject* self);
    void (*set_autoaccept)(ShipMdnsObject* self, bool autoaccept);
};

/**
 * @brief SHIP mDNS Object Structure
 */
struct ShipMdnsObject {
    const ShipMdnsInterface* interface_;
};

/**
 * @brief SHIP mDNS pointer typecast
 */
#define SHIP_MDNS_OBJECT(obj) ((ShipMdnsObject*)(obj))

/**
 * @brief SHIP mDNS Interface class pointer typecast
 */
#define SHIP_MDNS_INTERFACE(obj) (SHIP_MDNS_OBJECT(obj)->interface_)

/**
 * @brief SHIP mDNS Destruct caller definition
 */
#define SHIP_MDNS_DESTRUCT(obj) (SHIP_MDNS_INTERFACE(obj)->destruct(obj))

/**
 * @brief SHIP mDNS Start caller definition
 */
#define SHIP_MDNS_START(obj) (SHIP_MDNS_INTERFACE(obj)->start(obj))

/**
 * @brief SHIP mDNS Stop caller definition
 */
#define SHIP_MDNS_STOP(obj) (SHIP_MDNS_INTERFACE(obj)->stop(obj))

/**
 * @brief SHIP mDNS Register Service caller definition
 */
#define SHIP_MDNS_REGISTER_SERVICE(obj) (SHIP_MDNS_INTERFACE(obj)->register_service(obj))

/**
 * @brief SHIP mDNS Deregister Service caller definition
 */
#define SHIP_MDNS_DEREGISTER_SERVICE(obj) (SHIP_MDNS_INTERFACE(obj)->deregister_service(obj))

/**
 * @brief SHIP mDNS Set Autoaccept caller definition
 */
#define SHIP_MDNS_SET_AUTOACCEPT(obj, autoaccept) (SHIP_MDNS_INTERFACE(obj)->set_autoaccept(obj, autoaccept))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_MDNS_INTERFACE_H_
