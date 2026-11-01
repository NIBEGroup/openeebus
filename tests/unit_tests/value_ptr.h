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
 * @brief Value Pointer class is aimed to simplify the checks
 * where the obtained value is accessible via pointer,
 * so whenever comparing with the expected one following cases can be evaluated:
 * 1) Both obtained pointer and expected pointer are null — ok;
 * 2) Both obtained pointer and expected pointer are nonnull and
 * pointed values match — ok;
 * 3) All other cases — nok.
 *
 * Note: unlike smart pointers from std, ValuePtr(const ValueType* p) creates
 * a copy of object p passed as argument.
 */

#ifndef TESTS_UNIT_TESTS_VALUE_PTR_H
#define TESTS_UNIT_TESTS_VALUE_PTR_H

#include <iostream>
#include <memory>

template <typename ValueType>
class ValuePtr {
 private:
  std::unique_ptr<ValueType> _pv;

 public:
  ValuePtr() {
    _pv = nullptr;
  }
  ValuePtr(const ValueType* p) : _pv((p != nullptr) ? std::make_unique<ValueType>(*p) : nullptr) {}
  ValuePtr(const ValuePtr& p) : _pv((p._pv != nullptr) ? std::make_unique<ValueType>(*p._pv) : nullptr) {}
  ValuePtr(ValuePtr&& p) : _pv((p._pv != nullptr) ? std::move(p._pv) : nullptr) {}

  ValuePtr& operator=(const ValuePtr& p) {
    _pv = (p._pv != nullptr) ? std::make_unique<ValueType>(*p._pv) : nullptr;
    return *this;
  }

  ValuePtr& operator=(ValuePtr&& p) {
    _pv = (p._pv != nullptr) ? std::move(p._pv) : nullptr;
    return *this;
  }

  void reset(const ValueType* p) {
    _pv = (p != nullptr) ? std::make_unique<ValueType>(*p) : nullptr;
  }

  template <typename Tp>
  bool operator==(const Tp* p) const {
    if ((_pv == nullptr) && (p == nullptr)) {
      // Both null — match
      return true;
    }

    return ((_pv != nullptr) && (p != nullptr) && (*_pv == *p));
  }

  // Check if both nonnull and pointed strings match
  bool operator==(const ValuePtr<ValueType>& p) const {
    return *this == p.get();
  }

  ValueType operator*() const {
    return *_pv;
  }
  const ValueType* get() const {
    return _pv.get();
  }
};

template <typename ValueType, typename... ArgTypes>
ValuePtr<ValueType> ValuePtrCreate(ArgTypes&&... args) {
  ValueType v(std::forward<ArgTypes>(args)...);
  return ValuePtr<ValueType>(&v);
}

template <typename DataType>
std::ostream& operator<<(std::ostream& os, const ValuePtr<DataType> p) {
  if (p == nullptr) {
    return os << nullptr;
  }

  return os << reinterpret_cast<const void*>(p.get()) << " (" << *p << ")";
}

#endif  // TESTS_UNIT_TESTS_VALUE_PTR_H
