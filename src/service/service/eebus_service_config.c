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
 * @brief EEBUS Service Configuration functions implementation
 */

#include "src/service/api/eebus_service_config.h"

#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

static const int32_t kDefaultPort = 4711;

/**
 * @brief Generates a standard identifier used for mDNS ID and SHIP ID
 * Format: brand-model-serial_number
 * @param cfg EEBUS Service Configuration to generate the identifier with
 */
static const char* GenerateIdentifier(const EebusServiceConfig* cfg);

const char* GenerateIdentifier(const EebusServiceConfig* cfg) {
    return StringFmtSprintf("%s-%s", cfg->device_brand, cfg->device_serial_num);
}

EebusError EebusServiceConfigConstruct(
    EebusServiceConfig* cfg,
    const char* vendor_code,
    const char* device_brand,
    const char* device_model,
    const char* serial_number,
    const char* device_type,
    int32_t port
) {
    memset(cfg, 0, sizeof(*cfg));

    if (StringIsEmpty(vendor_code) || StringIsEmpty(device_brand) || StringIsEmpty(device_model)
        || StringIsEmpty(serial_number) || StringIsEmpty(device_type)) {
        return kEebusErrorInputArgument;
    }

    cfg->vendor_code = StringCopy(vendor_code);
    if (cfg->vendor_code == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    cfg->device_brand = StringCopy(device_brand);
    if (cfg->device_brand == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    cfg->device_model = StringCopy(device_model);
    if (cfg->device_model == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    cfg->device_serial_num = StringCopy(serial_number);
    if (cfg->device_serial_num == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    cfg->alternate_ship_id           = NULL;
    cfg->alternate_mdns_service_name = NULL;

    cfg->device_type = StringCopy(device_type);
    if (cfg->device_type == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    // Assign default feature set
    cfg->feature_set = kNetworkManagementFeatureSetTypeSmart;

    cfg->port = (port != 0) ? port : kDefaultPort;

    cfg->register_auto_accept = false;
    cfg->generated_id         = GenerateIdentifier(cfg);
    if (cfg->generated_id == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

EebusServiceConfig* EebusServiceConfigCreate(
    const char* vendor_code,
    const char* device_brand,
    const char* device_model,
    const char* serial_number,
    const char* device_type,
    int32_t port
) {
    EebusServiceConfig* const cfg = (EebusServiceConfig*)EEBUS_MALLOC(sizeof(EebusServiceConfig));
    if (cfg == NULL) {
        return NULL;
    }

    const EebusError status
        = EebusServiceConfigConstruct(cfg, vendor_code, device_brand, device_model, serial_number, device_type, port);

    if (status != kEebusErrorOk) {
        EebusServiceConfigDelete(cfg);
        return NULL;
    }

    return cfg;
}

void EebusServiceConfigDestruct(EebusServiceConfig* cfg) {
    StringDelete((char*)cfg->vendor_code);
    cfg->vendor_code = NULL;

    StringDelete((char*)cfg->device_brand);
    cfg->device_brand = NULL;

    StringDelete((char*)cfg->device_model);
    cfg->device_model = NULL;

    StringDelete((char*)cfg->device_serial_num);
    cfg->device_serial_num = NULL;

    StringDelete((char*)cfg->alternate_ship_id);
    cfg->alternate_ship_id = NULL;

    StringDelete((char*)cfg->alternate_mdns_service_name);
    cfg->alternate_mdns_service_name = NULL;

    StringDelete((char*)cfg->device_type);
    cfg->device_type = NULL;

    StringDelete((char*)cfg->generated_id);
    cfg->generated_id = NULL;
}

void EebusServiceConfigDelete(EebusServiceConfig* cfg) {
    if (cfg != NULL) {
        EebusServiceConfigDestruct(cfg);
        EEBUS_FREE(cfg);
    }
}
