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
#include "feature_address_test_data.h"

#include <string.h>

#include "src/common/eebus_malloc.h"
#include "src/common/num_ptr.h"
#include "src/common/string_util.h"

std::ostream& operator<<(std::ostream& os, const FeatureAddressType& addr) {
  os << "{.device = ";
  if (addr.device != nullptr) {
    os << addr.device;
  } else {
    os << nullptr;
  }

  os << ", .entity = {";
  if (addr.entity != nullptr) {
    for (size_t i = 0; i < addr.entity_size; ++i) {
      if (i != 0) {
        os << ", ";
      }

      if (addr.entity[i] != nullptr) {
        os << *addr.entity[i];
      } else {
        os << nullptr;
      }
    }
  }

  os << "}";
  if (addr.feature != nullptr) {
    os << ", .feature = " << *addr.feature;
  } else {
    os << nullptr;
  }

  return os << "}";
}

std::ostream& operator<<(std::ostream& os, const FeatureAddressType* addr) {
  if (addr == nullptr) {
    return os << nullptr;
  }

  return os << reinterpret_cast<const void*>(addr) << " (" << *addr << ")";
}

std::ostream& operator<<(std::ostream& os, const FeatureAddressTestData& addr) {
  os << "{.device = " << addr.device;
  os << ", .entity = {";
  for (size_t i = 0; i < addr.entity.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }

    os << addr.entity[i];
  }

  os << "}";
  return os << ", .feature = " << addr.feature << "}";
}

FeatureAddressType* TestDataToFeatureAddress(const FeatureAddressTestData* addr_data) {
  if (addr_data == nullptr) {
    return nullptr;
  }

  FeatureAddressType* const addr = (FeatureAddressType*)EEBUS_MALLOC(sizeof(FeatureAddressType));
  memset(addr, 0, sizeof(FeatureAddressType));
  addr->device = StringCopy((const char*)addr_data->device);

  const size_t entity_size = addr_data->entity.size();
  if (entity_size != 0) {
    const uint32_t** entity_ids = (const uint32_t**)EEBUS_MALLOC(sizeof(uint32_t*) * entity_size);
    addr->entity                = entity_ids;
    for (size_t i = 0; i < addr_data->entity.size(); ++i) {
      entity_ids[i] = Uint32Create(addr_data->entity[i]);
    }
  }

  addr->entity_size = entity_size;
  addr->feature     = (addr_data->feature.get() != nullptr) ? Uint32Create(*addr_data->feature) : nullptr;

  return addr;
}
