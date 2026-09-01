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
 * @brief Feature Link implementation
 */

#include "src/spine/api/feature_link.h"

#include <string.h>

#include "src/spine/model/binding_management_types.h"

static void FeatureLinkConstruct(
    FeatureLink* self,
    uint64_t id,
    FeatureLocalObject* server_feature,
    FeatureRemoteObject* client_feature
);

void FeatureLinkConstruct(
    FeatureLink* self,
    uint64_t id,
    FeatureLocalObject* server_feature,
    FeatureRemoteObject* client_feature
) {
    self->id             = id;
    self->server_feature = server_feature;
    self->client_feature = client_feature;
}

FeatureLink* FeatureLinkCreate(uint64_t id, FeatureLocalObject* server_feature, FeatureRemoteObject* client_feature) {
    FeatureLink* const link = (FeatureLink*)EEBUS_MALLOC(sizeof(FeatureLink));

    if (link != NULL) {
        FeatureLinkConstruct(link, id, server_feature, client_feature);
    }

    return link;
}

bool FeatureLinkRemoteDeviceMatch(const FeatureLink* self, const DeviceRemoteObject* remote_device) {
    const DeviceRemoteObject* const dr = FEATURE_REMOTE_GET_DEVICE(self->client_feature);
    return strcmp(DEVICE_REMOTE_GET_SKI(dr), DEVICE_REMOTE_GET_SKI(remote_device)) == 0;
}

bool FeatureLinkRemoteEntityMatch(const FeatureLink* self, const EntityRemoteObject* remote_entity) {
    const EntityAddressType* const remote_entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(remote_entity));

    const FeatureAddressType* const client_addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(self->client_feature));

    EntityAddressType clinet_entity_addr = {
        .device      = client_addr->device,
        .entity      = client_addr->entity,
        .entity_size = client_addr->entity_size,
    };

    return EntityAddressCompare(remote_entity_addr, &clinet_entity_addr);
}
