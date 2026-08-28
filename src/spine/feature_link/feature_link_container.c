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
 * @brief Feature Link Container implementation
 */

#include "src/spine/api/feature_link_container.h"

#include "src/spine/api/feature_link.h"

void FeatureLinkContainerConstruct(FeatureLinkContainer* self) {
    VectorConstruct(&self->links);
}

void FeatureLinkContainerDestruct(FeatureLinkContainer* self) {
    for (size_t i = 0; i < VectorGetSize(&self->links); ++i) {
        FeatureLink* const link = (FeatureLink*)VectorGetElement(&self->links, i);
        FeatureLinkDelete(link);
    }

    VectorDestruct(&self->links);
}

void FeatureLinkContainerAdd(
    FeatureLinkContainer* self,
    uint64_t id,
    FeatureLocalObject* server_feature,
    FeatureRemoteObject* client_feature
) {
    VectorPushBack(&self->links, FeatureLinkCreate(id, server_feature, client_feature));
}

FeatureLink* FeatureLinkContainerFind(
    const FeatureLinkContainer* self,
    const FeatureAddressType* server_address,
    const FeatureAddressType* client_address
) {
    for (size_t i = 0; i < VectorGetSize(&self->links); ++i) {
        FeatureLink* const link = (FeatureLink*)VectorGetElement(&self->links, i);

        if (FeatureAddressCompare(server_address, FeatureLinkGetServerAddr(link))
            && FeatureAddressCompare(client_address, FeatureLinkGetClientAddr(link))) {
            return link;
        }
    }

    return NULL;
}

void FeatureLinkContainerRemove(FeatureLinkContainer* self, FeatureLink* link) {
    VectorRemove(&self->links, link);
    FeatureLinkDelete(link);
}

bool FeatureLinkContainerHasServer(FeatureLinkContainer* self, const FeatureAddressType* server_address) {
    for (size_t i = 0; i < VectorGetSize(&self->links); ++i) {
        const FeatureLink* const link = (const FeatureLink*)VectorGetElement(&self->links, i);

        if (FeatureAddressCompare(server_address, FeatureLinkGetServerAddr(link))) {
            return true;
        }
    }

    return false;
}

size_t
FeatureLinkContainerGetRemoteDeviceMatchNum(const FeatureLinkContainer* self, const DeviceRemoteObject* remote_device) {
    size_t n = 0;
    for (size_t i = 0; i < VectorGetSize(&self->links); ++i) {
        const FeatureLink* const se = VectorGetElement(&self->links, i);
        if (FeatureLinkRemoteDeviceMatch(se, remote_device)) {
            ++n;
        }
    }

    return n;
}
