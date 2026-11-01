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
 * @brief C++ equivalent of C structure Address
 * with aim to simplify tabulating unit test expected values
 */

#ifndef TESTS_UNIT_TESTS_COMMON_JSON_ADDRESS_CPP_H_
#define TESTS_UNIT_TESTS_COMMON_JSON_ADDRESS_CPP_H_

#include <iostream>

#include "tests/unit_tests/common/eebus_data/person.h"
#include "tests/unit_tests/string_ptr.h"

class AddressTestData {
 private:
  StringPtr street_ = nullptr;
  StringPtr city_   = nullptr;

 public:
  AddressTestData() {}

  AddressTestData(const char* street, const char* city) : street_(street), city_(city) {}

  AddressTestData(const Address& address) : street_(address.street), city_(address.city) {}

  bool operator==(const Address& address) const {
    return (street_ == address.street) && (city_ == address.city);
  }

  bool operator==(const AddressTestData& address) const {
    return (street_ == address.street_) && (city_ == address.city_);
  }

  friend std::ostream& operator<<(std::ostream& os, const AddressTestData& address);
  const StringPtr& get_street() const {
    return street_;
  }
  const StringPtr& get_city() const {
    return city_;
  }
};

std::ostream& operator<<(std::ostream& os, const Address& address);
std::ostream& operator<<(std::ostream& os, const Address* address);
std::ostream& operator<<(std::ostream& os, const AddressTestData& address);

#endif  // TESTS_UNIT_TESTS_COMMON_JSON_ADDRESS_CPP_H_
