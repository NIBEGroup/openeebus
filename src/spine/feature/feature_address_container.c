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
 * @brief Feature Address Container implementation
 */

#include "src/spine/feature/feature_address_container.h"

void FeatureAddressContainerConstruct(FeatureAddressContainer* self) {
  VectorConstruct(&self->addresses);
}

void FeatureAddressContainerDestruct(FeatureAddressContainer* self) {
  for (size_t i = 0; i < VectorGetSize(&self->addresses); ++i) {
    FeatureAddressType* const addr = VectorGetElement(&self->addresses, i);
    FeatureAddressDelete(addr);
  }

  VectorDestruct(&self->addresses);
}

void FeatureAddressContainerAdd(FeatureAddressContainer* self, const FeatureAddressType* addr) {
  if (FeatureAddressContainerFind(self, addr) != NULL) {
    // Address already exists in the container, do not add it again
    return;
  }

  FeatureAddressType* const addr_copy = FeatureAddressCopy(addr);
  VectorPushBack(&self->addresses, addr_copy);
}

void FeatureAddressContainerRemove(FeatureAddressContainer* self, const FeatureAddressType* addr) {
  const FeatureAddressType* addr_el = FeatureAddressContainerFind(self, addr);
  if (addr_el != NULL) {
    VectorRemove(&self->addresses, (void*)addr_el);
    FeatureAddressDelete((FeatureAddressType*)addr_el);
  }
}

void FeatureAddressContainerRemoveForDevice(FeatureAddressContainer* self, const char* device) {
  if (device == NULL) {
    return;
  }

  size_t i = VectorGetSize(&self->addresses);

  while (i > 0) {
    --i;
    const FeatureAddressType* const addr = VectorGetElement(&self->addresses, i);

    if (FeatureAddressMatchDevice(addr, device)) {
      VectorRemove(&self->addresses, (void*)addr);
      FeatureAddressDelete((FeatureAddressType*)addr);
    }
  }
}

size_t FeatureAddressContainerGetSize(const FeatureAddressContainer* self) {
  return VectorGetSize(&self->addresses);
}

const FeatureAddressType*
FeatureAddressContainerFind(const FeatureAddressContainer* self, const FeatureAddressType* addr) {
  for (size_t i = 0; i < VectorGetSize(&self->addresses); ++i) {
    const FeatureAddressType* const addr_el = VectorGetElement(&self->addresses, i);
    if (FeatureAddressCompare(addr_el, addr)) {
      return addr_el;
    }
  }

  return NULL;
}

const FeatureAddressType* FeatureAddressContainerGetElement(const FeatureAddressContainer* self, size_t idx) {
  return VectorGetElement(&self->addresses, idx);
}
