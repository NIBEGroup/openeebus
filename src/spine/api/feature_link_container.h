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
 * @brief Feature Link Container can keep a collection of feature subscription entries or binding entries
 */

#ifndef SRC_EEBUS_SRC_SPINE_API_FEATURE_LINK_CONTAINER_H_
#define SRC_EEBUS_SRC_SPINE_API_FEATURE_LINK_CONTAINER_H_

#include "src/common/vector.h"
#include "src/spine/api/feature_link.h"

typedef struct FeatureLinkContainer FeatureLinkContainer;

struct FeatureLinkContainer {
    Vector links;
};

void FeatureLinkContainerConstruct(FeatureLinkContainer* self);
void FeatureLinkContainerDestruct(FeatureLinkContainer* self);

static inline size_t FeatureLinkContainerGetSize(const FeatureLinkContainer* self) {
    return VectorGetSize(&self->links);
}

static inline FeatureLink* FeatureLinkContainerGetElement(const FeatureLinkContainer* self, size_t idx) {
    return (FeatureLink*)VectorGetElement(&self->links, idx);
};

void FeatureLinkContainerAdd(
    FeatureLinkContainer* self,
    uint64_t id,
    FeatureLocalObject* server_feature,
    FeatureRemoteObject* client_feature
);
FeatureLink* FeatureLinkContainerFind(
    const FeatureLinkContainer* self,
    const FeatureAddressType* server_address,
    const FeatureAddressType* client_address
);
void FeatureLinkContainerRemove(FeatureLinkContainer* self, FeatureLink* link);
bool FeatureLinkContainerHasServer(FeatureLinkContainer* self, const FeatureAddressType* server_address);
size_t
FeatureLinkContainerGetRemoteDeviceMatchNum(const FeatureLinkContainer* self, const DeviceRemoteObject* remote_device);

#endif  // SRC_EEBUS_SRC_SPINE_API_FEATURE_LINK_CONTAINER_H_
