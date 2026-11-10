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
 * @brief String Pointer class is aimed to simplify the checks
 * where the obtained value is accessible via pointer (const char* or std::string_view*),
 * so whenever comparing with the expected one following cases can be evaluated:
 * 1) Both obtained pointer and expected pointer are null — ok;
 * 2) Both obtained pointer and expected pointer are nonnull and
 * pointed strings match — ok;
 * 3) All other cases — nok.
 */

#ifndef TESTS_UNIT_TESTS_STRING_PTR_H
#define TESTS_UNIT_TESTS_STRING_PTR_H

#include <iostream>

#include "tests/unit_tests/value_ptr.h"

class StringPtr {
 private:
  ValuePtr<std::string_view> _ps;

 public:
  StringPtr() : _ps(nullptr) {}

  StringPtr(const StringPtr& p) : _ps(p._ps.get()) {}

  StringPtr(StringPtr&& p) = delete;
  StringPtr(const char* s) {
    if (s != nullptr) {
      std::string_view sv(s);
      _ps.reset(&sv);
    }
  }

  bool operator==(const char* s) const {
    return (s == nullptr) ? (_ps.get() == nullptr) : (*_ps == s);
  }

  bool operator==(const StringPtr& p) const {
    return _ps == p._ps;
  }

  StringPtr& operator=(const StringPtr& p) = delete;
  StringPtr& operator=(StringPtr&& p)      = delete;

  friend std::ostream& operator<<(std::ostream& os, const StringPtr& p);
  operator const char*() const {
    return (_ps.get() != nullptr) ? (*_ps.get()).data() : nullptr;
  }
};

#endif  // TESTS_UNIT_TESTS_STRING_PTR_H
