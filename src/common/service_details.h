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
 * @brief Transformed api/servicedetails.go
 *
 * Generic service details about the local or any remote service
 */

#ifndef SRC_SHIP_API_SERVICE_DETAILS_H_
#define SRC_SHIP_API_SERVICE_DETAILS_H_

#include <stdbool.h>

#include "src/common/string_util.h"
#include "src/ship/api/connection_state.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct ServiceDetails ServiceDetails;

struct ServiceDetails {
    /**
     * @brief This is the SKI of the service, has to be persisted
     */
    const char* ski;
    /**
     * @brief This is the IPv4 address of the device running the service
     * This is optional only needed when this runs with
     * zeroconf as mDNS and the remote device is using the latest
     * avahi version and thus zeroconf can sometimes not detect
     * the IPv4 address and not initiate a connection
     */
    const char* ipv4;
    /**
     * @brief The SHIP identifier of the service, has to be persisted
     */
    const char* ship_id;
    /**
     * @brief The EEBUS device type of the device model
     */
    const char* device_type;
    /**
     * @brief Flags if the service auto auto accepts other services
     */
    bool auto_accept;
    /**
     * @brief Flags if the service is trusted and should be reconnected to
     * Should be enabled after the connection process resulted
     * ConnectionStateDetail == ConnectionStateTrusted the first time
     */
    bool is_trusted;
    /**
     * @brief Current connection state details
     */
    ConnectionState state;
};

/**
 * @brief Dynamically allocates and initialises a Service Details instance
 * (ipv4 set to NULL, autoaccept and trusted flag set to false, connection state
 * set to kConnectionStateNone)
 * @param service_details Service Details instance to be initialised
 * @param ski SKI to initialise the Service Details with
 * @param ship_id SHIP Id to initialise the Service Details with
 * @param device_type Device Type to initialise the Service Details with
 * @param auto_accept Auto Accept flag to initialise the Service Details with
 * @return New Service Details instance on success, NULL on error
 */
ServiceDetails* ServiceDetailsCreate(const char* ski, const char* ship_id, const char* device_type, bool auto_accept);

/**
 * @brief Create a copy of Service Details instance
 * @param src Service Details instance to copy from
 * @return Service Details instance created on success, NULL on error
 */
ServiceDetails* ServiceDetailsCopy(const ServiceDetails* src);

/**
 * @brief Destruct the Service Details instance and deallocate memory
 * @param service_details Service Details instance to be released
 */
void ServiceDetailsDelete(ServiceDetails* service_details);

/**
 * @brief Get the Service Details SKI
 * @param service_details Service Details instance to get the SKI
 * @return Service Details SKI
 */
static inline const char* ServiceDetailsGetSki(const ServiceDetails* service_details) {
    return service_details->ski;
}

/**
 * @brief Get the Service Details IPv4
 * @param service_details Service Details instance to get the IPv4
 * @return Service Details IPv4
 */
static inline const char* ServiceDetailsGetIpv4(ServiceDetails* service_details) {
    return service_details->ipv4;
}

/**
 * @brief Set the Service Details IPv4. Creates a string copy
 * @param service_details Service Details instance to set the IPv4
 * @param ipv4 IPv4 to be set
 */
static inline void ServiceDetailsSetIpv4(ServiceDetails* service_details, const char* ipv4) {
    StringDelete((char*)service_details->ipv4);
    service_details->ipv4 = StringCopy(ipv4);
}

/**
 * @brief Get the Service Details SHIP Id
 * @param service_details Service Details instance to get the SHIP Id
 * @return Service Details SHIP Id
 */
static inline const char* ServiceDetailsGetShipId(ServiceDetails* service_details) {
    return service_details->ship_id;
}

/**
 * @brief Get the Service Details Device Type
 * @param service_details Service Details instance to get the Device Type
 * @return Service Details Device Type
 */
static inline const char* ServiceDetailsGetDeviceType(ServiceDetails* service_details) {
    return service_details->device_type;
}

/**
 * @brief Get the Service Details Auto Accept flag
 * @param service_details Service Details instance to get the Auto Accept flag
 * @return Service Details Auto Accept flag
 */
static inline bool ServiceDetailsGetAutoAccept(ServiceDetails* service_details) {
    return service_details->auto_accept;
}

/**
 * @brief Set the Service Details Auto Accept flag
 * @param service_details Service Details instance to set the Auto Accept flag
 * @param auto_accept Auto Accept flag to be set
 */
static inline void ServiceDetailsSetAutoAccept(ServiceDetails* service_details, bool auto_accept) {
    service_details->auto_accept = auto_accept;
}

/**
 * @brief Get the Service Details Is Trusted flag
 * @param service_details Service Details instance to get the Is Trusted flag
 * @return Service Details Is Trusted flag
 */
static inline bool ServiceDetailsGetIsTrusted(ServiceDetails* service_details) {
    return service_details->is_trusted;
}

/**
 * @brief Set the Service Details Is Trusted flag
 * @param service_details Service Details instance to set the Is Trusted flag
 * @param is_strusted Is Trusted flag to be set
 */
static inline void ServiceDetailsSetIsTrusted(ServiceDetails* service_details, bool is_strusted) {
    service_details->is_trusted = is_strusted;
}

/**
 * @brief Get the Service Details Connection State
 * @param service_details Service Details instance to get the Connection State
 * @return Service Details Connection State
 */
static inline bool ServiceDetailsGetConnectionState(ServiceDetails* service_details) {
    return service_details->state;
}

/**
 * @brief Set the Service Details Connection State
 * @param service_details Service Details instance to set the Connection State
 * @param is_strusted Connection State to be set
 */
static inline void ServiceDetailsSetConnectionState(ServiceDetails* service_details, ConnectionState state) {
    service_details->state = state;
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SERVICE_DETAILS_H_
