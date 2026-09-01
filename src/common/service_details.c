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
 * @brief EEBUS Service Details functions implementation
 */

#include "service_details.h"

#include <stddef.h>

#include "src/common/eebus_errors.h"
#include "src/common/string_util.h"

static EebusError ServiceDetailsConstruct(
    ServiceDetails* service_details,
    const char* ski,
    const char* ship_id,
    const char* device_type,
    bool auto_accept
);
static void ServiceDetailsDestruct(ServiceDetails* service_details);

EebusError ServiceDetailsConstruct(
    ServiceDetails* service_details,
    const char* ski,
    const char* ship_id,
    const char* device_type,
    bool auto_accept
) {
    if (StringIsEmpty(ski) || StringIsEmpty(ship_id) || StringIsEmpty(device_type)) {
        return kEebusErrorInputArgument;
    }

    service_details->ski         = NULL;
    service_details->ipv4        = NULL;
    service_details->ship_id     = NULL;
    service_details->device_type = NULL;
    service_details->auto_accept = auto_accept;
    service_details->is_trusted  = false;
    service_details->state       = kConnectionStateNone;

    service_details->ski = StringCopy(ski);
    if (service_details->ski == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    service_details->ship_id = StringCopy(ship_id);
    if (service_details->ship_id == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    service_details->device_type = StringCopy(device_type);
    if (service_details->device_type == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

ServiceDetails* ServiceDetailsCreate(const char* ski, const char* ship_id, const char* device_type, bool auto_accept) {
    ServiceDetails* service_details = (ServiceDetails*)EEBUS_MALLOC(sizeof(ServiceDetails));
    if (service_details == NULL) {
        return NULL;
    }

    if (ServiceDetailsConstruct(service_details, ski, ship_id, device_type, auto_accept) != kEebusErrorOk) {
        ServiceDetailsDelete(service_details);
        return NULL;
    }

    return service_details;
}

ServiceDetails* ServiceDetailsCopy(const ServiceDetails* src) {
    if (src == NULL) {
        return NULL;
    }

    ServiceDetails* const service_details_copy
        = ServiceDetailsCreate(src->ski, src->ship_id, src->device_type, src->auto_accept);
    if (service_details_copy == NULL) {
        return NULL;
    }

    service_details_copy->ipv4 = StringCopy(src->ipv4);
    if (service_details_copy->ipv4 == NULL) {
        ServiceDetailsDelete(service_details_copy);
        return NULL;
    }

    service_details_copy->auto_accept = src->auto_accept;
    service_details_copy->is_trusted  = src->is_trusted;
    service_details_copy->state       = src->state;
    return service_details_copy;
}

void ServiceDetailsDestruct(ServiceDetails* service_details) {
    StringDelete((char*)service_details->ski);
    service_details->ski = NULL;

    StringDelete((char*)service_details->ipv4);
    service_details->ipv4 = NULL;

    StringDelete((char*)service_details->ship_id);
    service_details->ship_id = NULL;

    StringDelete((char*)service_details->device_type);
    service_details->device_type = NULL;
}

void ServiceDetailsDelete(ServiceDetails* service_details) {
    if (service_details != NULL) {
        ServiceDetailsDestruct(service_details);
        EEBUS_FREE(service_details);
    }
}
