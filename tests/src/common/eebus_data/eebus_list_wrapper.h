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
#ifndef TESTS_SRC_COMMON_EEBUS_DATA_EEBUS_LIST_WRAPPER_H_
#define TESTS_SRC_COMMON_EEBUS_DATA_EEBUS_LIST_WRAPPER_H_

#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/eebus_malloc.h"

#include <stddef.h>

#include <vector>

template <class T>
class EebusListWrapper {
 public:
  EebusListWrapper()                                   = default;
  EebusListWrapper(const EebusListWrapper&)            = delete;
  EebusListWrapper& operator=(const EebusListWrapper&) = delete;

  EebusListWrapper(EebusListWrapper&& other) noexcept : array(other.array), size(other.size) {
    other.array = nullptr;
    other.size  = 0;
  }

  EebusListWrapper& operator=(EebusListWrapper&& other) noexcept {
    if (this != &other) {
      EEBUS_FREE(array);
      array       = other.array;
      size        = other.size;
      other.array = nullptr;
      other.size  = 0;
    }

    return *this;
  }
  ~EebusListWrapper() {
    EEBUS_FREE(array);
  }

  EebusError Append(T* element) {
    return EebusDataListDataAppend(&array, &size, reinterpret_cast<void*>(element));
  }

  EebusError AppendList(T* const* elements, size_t count) {
    if (elements == nullptr) {
      return EebusDataListDataAppendList(&array, &size, nullptr, count);
    }

    std::vector<const void*> elems;
    elems.reserve(count);
    for (size_t index = 0; index < count; ++index) {
      elems.push_back(elements[index]);
    }
    return EebusDataListDataAppendList(&array, &size, elems.data(), count);
  }

  EebusError Remove(T* element) {
    return EebusDataListDataRemove(&array, &size, reinterpret_cast<void*>(element));
  }

  T* operator[](size_t index) {
    if (index >= size) {
      return nullptr;
    }

    return reinterpret_cast<T*>(array[index]);
  }

  T** Get() {
    return reinterpret_cast<T**>(array);
  }

  size_t Size() const {
    return size;
  }

 private:
  void** array{nullptr};
  size_t size{0};
};

#endif  // TESTS_SRC_COMMON_EEBUS_DATA_EEBUS_LIST_WRAPPER_H_
