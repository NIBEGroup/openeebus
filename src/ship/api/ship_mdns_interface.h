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

#include "src/common/eebus_errors.h"
#include "src/common/vector.h"
#include "src/ship/api/mdns_entry.h"
#include "src/ship/api/ship_pairing_entry.h"

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
 * @brief On shippairing entries found callback
 *
 * @note The user of the callback is responsible for freeing found_entries vector and its elements
 */
typedef void (*OnShipPairingEntriesFoundCallback)(Vector* found_entries, void* context);

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

  /**
   * @defgroup ShipMdnsPairing SHIP Pairing Service
   *
   * Discovering and announcing "_shippairing._tcp" service instances
   * (SHIP Pairing Service TS 1.0.0, chapter 5).
   *
   * These are appended to the end of the table and may be NULL. A backend that
   * predates them, or that has no use for them, leaves them unset and keeps
   * working exactly as before; the helpers below report
   * kEebusErrorNotSupported rather than calling through a null pointer. Nothing
   * may be inserted before them, and no signature above them may change, or
   * every backend implemented outside this repository breaks.
   * @{
   */

  /**
   * @brief Starts looking for shippairing service instances
   *
   * @param cb Called with the instances found, as the SHIP browse reports its own
   * @param ctx Passed back to @p cb
   */
  EebusError (*start_pairing_browse)(ShipMdnsObject* self, OnShipPairingEntriesFoundCallback cb, void* ctx);

  /**
   * @brief Stops looking for shippairing service instances
   */
  void (*stop_pairing_browse)(ShipMdnsObject* self);

  /**
   * @brief Announces a shippairing service instance
   *
   * The instance name and the TXT record are taken from @p entry. The port in
   * the SRV record is unused and must never be connected to (section 5.3).
   *
   * @param entry Request to announce
   */
  EebusError (*register_pairing_service)(ShipMdnsObject* self, const ShipPairingEntry* entry);

  /**
   * @brief Withdraws the announced shippairing service instance
   */
  void (*deregister_pairing_service)(ShipMdnsObject* self);
  /** @} */
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

/**
 * @defgroup ShipMdnsPairingCallers SHIP Pairing Service callers
 *
 * Caller definitions rather than plain macros, because each has to check whether the backend implements the method
 * before calling it.
 * @{
 */

/**
 * @brief SHIP mDNS Start Pairing Browse caller definition
 * @return kEebusErrorNotSupported if the backend does not implement it
 */
#define SHIP_MDNS_START_PAIRING_BROWSE(obj, cb, ctx)                  \
  ((SHIP_MDNS_INTERFACE(obj)->start_pairing_browse != NULL)           \
       ? SHIP_MDNS_INTERFACE(obj)->start_pairing_browse(obj, cb, ctx) \
       : kEebusErrorNotSupported)

/**
 * @brief SHIP mDNS Stop Pairing Browse caller definition
 */
#define SHIP_MDNS_STOP_PAIRING_BROWSE(obj)                                                                      \
  ((SHIP_MDNS_INTERFACE(obj)->stop_pairing_browse != NULL) ? SHIP_MDNS_INTERFACE(obj)->stop_pairing_browse(obj) \
                                                           : (void)0)

/**
 * @brief SHIP mDNS Register Pairing Service caller definition
 * @return kEebusErrorNotSupported if the backend does not implement it
 */
#define SHIP_MDNS_REGISTER_PAIRING_SERVICE(obj, entry)                  \
  ((SHIP_MDNS_INTERFACE(obj)->register_pairing_service != NULL)         \
       ? SHIP_MDNS_INTERFACE(obj)->register_pairing_service(obj, entry) \
       : kEebusErrorNotSupported)

/**
 * @brief SHIP mDNS Deregister Pairing Service caller definition
 */
#define SHIP_MDNS_DEREGISTER_PAIRING_SERVICE(obj)                  \
  ((SHIP_MDNS_INTERFACE(obj)->deregister_pairing_service != NULL)  \
       ? SHIP_MDNS_INTERFACE(obj)->deregister_pairing_service(obj) \
       : (void)0)

/**
 * @brief SHIP mDNS Supports Pairing caller definition
 *
 * Reports whether the backend implements SHIP Pairing Service discovery.
 */
#define SHIP_MDNS_SUPPORTS_PAIRING(obj) (SHIP_MDNS_INTERFACE(obj)->start_pairing_browse != NULL)
/** @} */

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_MDNS_INTERFACE_H_
