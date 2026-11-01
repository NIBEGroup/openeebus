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
 * @brief C++ equivalent of C structure FeatureAddressType
 * with aim to simplify tabulating unit test expected values
 */

#ifndef TESTS_UNIT_TESTS_SPINE_MODEL_FEATURE_ADDRESS_TEST_DATA_H
#define TESTS_UNIT_TESTS_SPINE_MODEL_FEATURE_ADDRESS_TEST_DATA_H

#include <stdint.h>

#include <iostream>
#include <vector>

#include "src/spine/model/feature_types.h"
#include "tests/unit_tests/string_ptr.h"
#include "tests/unit_tests/value_ptr.h"

class FeatureAddressTestData {
 private:
  StringPtr device             = nullptr;
  std::vector<uint32_t> entity = {};
  ValuePtr<uint32_t> feature   = 0;

 public:
  FeatureAddressTestData() {}

  FeatureAddressTestData(const char* device, const std::vector<uint32_t>& entity, const ValuePtr<uint32_t>& feature)
      : device(device), entity(entity), feature(feature) {}

  FeatureAddressTestData(const FeatureAddressType& addr) : device(addr.device), entity({}), feature(addr.feature) {
    if (addr.entity != nullptr) {
      for (size_t i = 0; i < addr.entity_size; ++i) {
        entity.push_back(*addr.entity[i]);
      }
    }
  }

  bool operator==(const FeatureAddressType& addr) const {
    bool ok = (device == addr.device);
    if (entity.size() != 0) {
      ok = ok && (addr.entity != nullptr);
    } else {
      ok = ok && (addr.entity == nullptr);
    }

    ok = (ok && entity.size() == addr.entity_size);

    if (ok) {
      const size_t n = std::min(entity.size(), addr.entity_size);
      for (size_t i = 0; i < n; ++i) {
        if ((addr.entity[i] == nullptr) || (*addr.entity[i] != entity[i])) {
          ok = false;
          break;
        }
      }
    }

    ok = ok && (feature == addr.feature);

    return ok;
  }

  bool operator==(const FeatureAddressTestData& addr) const {
    return (device == addr.device) && (entity == addr.entity) && (feature == addr.feature);
  }

  friend std::ostream& operator<<(std::ostream& os, const FeatureAddressTestData& addr);
  friend FeatureAddressType* TestDataToFeatureAddress(const FeatureAddressTestData* addr_data);
  const StringPtr& get_device() const {
    return device;
  }
  const std::vector<uint32_t>& get_entity() const {
    return entity;
  }
  const ValuePtr<uint32_t>& get_feature() const {
    return feature;
  }
};

std::ostream& operator<<(std::ostream& os, const FeatureAddressType& addr);
std::ostream& operator<<(std::ostream& os, const FeatureAddressType* addr);
std::ostream& operator<<(std::ostream& os, const FeatureAddressTestData& addr);
FeatureAddressType* TestDataToFeatureAddress(const FeatureAddressTestData* addr);

#define FEATURE_ADDRESS_TEST_DATA(d, e, f) \
  ValuePtrCreate<FeatureAddressTestData>((d), std::vector<uint32_t>(e), ValuePtrCreate<uint32_t>(f))

#endif  // TESTS_UNIT_TESTS_SPINE_MODEL_FEATURE_ADDRESS_TEST_DATA_H
