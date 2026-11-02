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
#include "address_test_data.h"

#include <iostream>

#include "tests/unit_tests/common/eebus_data/person.h"

std::ostream& operator<<(std::ostream& os, const Address& address) {
  os << "{.street = " << StringPtr(address.street);
  os << ", .city = " << StringPtr(address.city) << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Address* address) {
  if (address == nullptr) {
    return os << nullptr;
  }

  return os << reinterpret_cast<const void*>(address) << " (" << *address << ")";
}

std::ostream& operator<<(std::ostream& os, const AddressTestData& address) {
  return os << "{.street = " << address.street_ << ", .city = " << address.city_ << "}";
}
