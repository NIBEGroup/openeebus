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
 * @brief EEBUS Service Configuration declaration
 *
 * Transformed from eebus-go/api/cfg.go
 */

#ifndef SRC_COMMON_CONFIGURATION_H_
#define SRC_COMMON_CONFIGURATION_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/common/array_util.h"
#include "src/common/eebus_errors.h"
#include "src/common/string_util.h"
#include "src/ship/api/ship_mdns_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/spine/model/device_types.h"
#include "src/spine/model/entity_types.h"
#include "src/spine/model/feature_types.h"
#include "src/spine/model/network_management_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EebusServiceConfig EebusServiceConfig;

// defines requires meta information about this service
struct EebusServiceConfig {
    /**
     * The vendors IANA PEN, optional but highly recommended.
     * If not set, brand will be used instead
     * Used for the Device Address: SPINE - Protocol Specification 7.1.1.2
     */
    const char* vendor_code;

    /**
     * The deviceBrand of the device, required
     * Used for the Device Address: SPINE - Protocol Specification 7.1.1.2
     * Used for mDNS txt record: SHIP - Specification 7.3.2
     */
    const char* device_brand;

    /**
     * The device model, required
     * Used for the Device Address: SPINE - Protocol Specification 7.1.1.2
     * Used for mDNS txt record: SHIP - Specification 7.3.2
     */
    const char* device_model;

    /**
     * Serial number of the device, required
     * Used for the Device Address: SPINE - Protocol Specification 7.1.1.2
     */
    const char* device_serial_num;

    /**
     * An alternate mDNS service identifier
     * Optional, if not set will be generated using "Brand-Model-SerialNumber"
     * Used for mDNS service and SHIP identifier: SHIP - Specification 7.2
     */
    const char* alternate_ship_id;

    /**
     * An alternate mDNS service name
     * Optional, if not set will be identical to alternateIdentifier or generated using "Brand-Model-SerialNumber"
     */
    const char* alternate_mdns_service_name;

    /**
     * SPINE device type of the device model, required
     * Used for SPINE device type
     * Used for mDNS txt record: SHIP - Specification 7.3.2
     */
    const char* device_type;

    /**
     * SPINE device network feature set type, optional
     * SPINE Protocol Specification 6
     */
    NetworkManagementFeatureSetType feature_set;

    /**
     * The port address of the websocket server, required
     */
    int32_t port;

    /**
     * Whether remote devices should be automatically accepted
     * If enabled will automatically search for other services with
     * the same setting and automatically connect to them.
     * Has to be set on configuring the service!
     * TODO: if disabled, user verification needs to be implemented and supported
     * the spec defines that this should have a timeout and be activate
     * e.g via a physical button
     */
    bool register_auto_accept;

    /**
     * Generated identifier. Format: brand-model-serial_number.
     * Can be used for both SHIP Id and mDNS service name if corresponding alternate
     * name has not been specified (see alternate_ship_id and alternate_mdns_service_name).
     */
    const char* generated_id;
};

/**
 * @brief Dynamically allocate and initialize an EebusServiceConfig instance
 * @param vendor_code Vendor code
 * @param device_brand Device brand
 * @param device_model Device model
 * @param serial_number Device serial number
 * @param device_type Device type
 * @param port Port (pass 0 to switch to default port)
 * @return EebusServiceConfig instance created on success, NULL on failure
 */
EebusServiceConfig* EebusServiceConfigCreate(
    const char* vendor_code,
    const char* device_brand,
    const char* device_model,
    const char* serial_number,
    const char* device_type,
    int32_t port
);

/**
 * @brief Deinitialize an EebusServiceConfig instance and release allocated memory
 * @param cfg EebusServiceConfig instance to be deleted
 */
void EebusServiceConfigDelete(EebusServiceConfig* cfg);

/**
 * @brief Dellocate the EebusServiceConfig fields
 * @param cfg EebusServiceConfig instance to be released
 */
void ConfigurationRelease(EebusServiceConfig* cfg);

static inline const char* EebusServiceConfigGetVendorCode(const EebusServiceConfig* cfg) {
    return cfg->vendor_code;
}

static inline const char* EebusServiceConfigGetDeviceBrand(const EebusServiceConfig* cfg) {
    return cfg->device_brand;
}

static inline const char* EebusServiceConfigGetDeviceModel(const EebusServiceConfig* cfg) {
    return cfg->device_model;
}

static inline const char* EebusServiceConfigGetDeviceSerialNumber(const EebusServiceConfig* cfg) {
    return cfg->device_serial_num;
}

static inline void EebusServiceConfigSetAlternateIdentifier(EebusServiceConfig* cfg, const char* identifier) {
    StringDelete((char*)cfg->alternate_ship_id);
    cfg->alternate_ship_id = StringCopy(identifier);
}

static inline void EebusServiceConfigSetAlternateMdnsServiceName(EebusServiceConfig* cfg, const char* name) {
    StringDelete((char*)cfg->alternate_mdns_service_name);
    cfg->alternate_mdns_service_name = StringCopy(name);
}

static inline const char* EebusServiceConfigGetDeviceType(const EebusServiceConfig* cfg) {
    return cfg->device_type;
}

static inline NetworkManagementFeatureSetType EebusServiceConfigGetFeatureSet(const EebusServiceConfig* cfg) {
    return cfg->feature_set;
}

static inline void EebusServiceConfigSetRegisterAutoAccept(EebusServiceConfig* cfg, bool auto_accept) {
    cfg->register_auto_accept = auto_accept;
}

/**
 * @brief Get the SHIP ID
 * Return the first valid identifier found in order:
 * 1) alternate_ship_id;
 * 2) generated_id;
 * @param cfg EebusServiceConfig to get the SHIP Id with
 */
static inline const char* EebusServiceConfigGetShipId(const EebusServiceConfig* cfg) {
    return (!StringIsEmpty(cfg->alternate_ship_id)) ? cfg->alternate_ship_id : cfg->generated_id;
}

/**
 * @brief Get the mDNS service name
 * Return the first valid identifier found in order:
 * 1) alternate_mdns_service_name;
 * 2) generated_id;
 * @param cfg EebusServiceConfig to get the mDNS service name with
 */
static inline const char* EebusServiceConfigGetMdnsServiceName(const EebusServiceConfig* cfg) {
    return (!StringIsEmpty(cfg->alternate_mdns_service_name)) ? cfg->alternate_mdns_service_name : cfg->generated_id;
}

static inline int32_t EebusServiceConfigGetPort(const EebusServiceConfig* cfg) {
    return cfg->port;
}

static inline bool EebusServiceConfigGetRegisterAutoAccept(const EebusServiceConfig* cfg) {
    return cfg->register_auto_accept;
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_CONFIGURATION_H_
