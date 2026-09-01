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
 * @brief Feature Link is used as an element of Feature Link Container
 * (which can keep a collection of subscription entries or binding entries)
 */

#ifndef SRC_EEBUS_SRC_SPINE_API_FEATURE_LINK_H_
#define SRC_EEBUS_SRC_SPINE_API_FEATURE_LINK_H_

#include <stdint.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/feature_remote_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct FeatureLink FeatureLink;

struct FeatureLink {
    uint64_t id;
    FeatureLocalObject* server_feature;
    FeatureRemoteObject* client_feature;
};

FeatureLink* FeatureLinkCreate(uint64_t id, FeatureLocalObject* server_feature, FeatureRemoteObject* client_feature);
static inline void FeatureLinkDelete(FeatureLink* self) {
    EEBUS_FREE(self);
};
bool FeatureLinkRemoteDeviceMatch(const FeatureLink* self, const DeviceRemoteObject* remote_device);
bool FeatureLinkRemoteEntityMatch(const FeatureLink* self, const EntityRemoteObject* remote_entity);

static inline const FeatureAddressType* FeatureLinkGetClientAddr(const FeatureLink* self) {
    return FEATURE_GET_ADDRESS(FEATURE_OBJECT(self->client_feature));
}

static inline const FeatureAddressType* FeatureLinkGetServerAddr(const FeatureLink* self) {
    return FEATURE_GET_ADDRESS(FEATURE_OBJECT(self->server_feature));
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_SPINE_API_FEATURE_LINK_H_
