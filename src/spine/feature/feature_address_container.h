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
 * @brief Feature Address Container keeps a collection of FeatureAddressType
 * objects. When adding a FeatureAddressType instance a copy of data is created and stored
 * internally
 */

#ifndef SRC_EEBUS_SRC_SPINE_FEATURE_FEATURE_ADDRESS_CONTAINER_H_
#define SRC_EEBUS_SRC_SPINE_FEATURE_FEATURE_ADDRESS_CONTAINER_H_

#include "src/common/vector.h"
#include "src/spine/model/feature_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct FeatureAddressContainer FeatureAddressContainer;

struct FeatureAddressContainer {
  Vector addresses;
};

void FeatureAddressContainerConstruct(FeatureAddressContainer* self);
void FeatureAddressContainerDestruct(FeatureAddressContainer* self);
void FeatureAddressContainerAdd(FeatureAddressContainer* self, const FeatureAddressType* addr);
void FeatureAddressContainerRemove(FeatureAddressContainer* self, const FeatureAddressType* addr);
void FeatureAddressContainerRemoveForDevice(FeatureAddressContainer* self, const char* device);
const FeatureAddressType*
FeatureAddressContainerFind(const FeatureAddressContainer* self, const FeatureAddressType* addr);
size_t FeatureAddressContainerGetSize(const FeatureAddressContainer* self);
const FeatureAddressType* FeatureAddressContainerGetElement(const FeatureAddressContainer* self, size_t idx);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_SPINE_FEATURE_FEATURE_ADDRESS_CONTAINER_H_
