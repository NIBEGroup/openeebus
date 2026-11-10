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
#include "src/common/string_lut.h"

#include <gtest/gtest.h>

#include "src/common/num_ptr.h"
#include "tests/unit_tests/memory_leak.inc"
#include "tests/unit_tests/value_ptr.h"

static void DeleteNumber(void* n) {
  Uint32Delete(reinterpret_cast<uint32_t*>(n));
}

TEST(StringLutTest, StringLutTest) {
  StringLut lut;
  static uint32_t v1 = 1;
  StringLutInit(&lut);
  StringLutInsert(&lut, "value1", &v1, NULL);
  StringLutInsert(&lut, "valuei", Uint32Create(10), DeleteNumber);
  StringLutInsert(&lut, "valuej", Uint32Create(20), DeleteNumber);

  EXPECT_EQ(nullptr, StringLutFind(&lut, "value10"));
  const uint32_t* valuei = reinterpret_cast<uint32_t*>(StringLutFind(&lut, "valuei"));
  EXPECT_EQ(ValuePtrCreate<uint32_t>(10), valuei);
  const uint32_t* valuej = reinterpret_cast<uint32_t*>(StringLutFind(&lut, "valuej"));
  EXPECT_EQ(ValuePtrCreate<uint32_t>(20), valuej);
  const uint32_t* valuen = reinterpret_cast<uint32_t*>(StringLutFind(&lut, "valuen"));
  EXPECT_EQ(nullptr, valuen);

  StringLutRemove(&lut, "valuei");
  StringLutInsert(&lut, "valuen", Uint32Create(100), DeleteNumber);
  valuei = reinterpret_cast<uint32_t*>(StringLutFind(&lut, "valuei"));
  EXPECT_EQ(nullptr, valuei);
  valuei = reinterpret_cast<uint32_t*>(StringLutFind(&lut, "valuej"));
  EXPECT_EQ(ValuePtrCreate<uint32_t>(20), valuei);
  valuen = reinterpret_cast<uint32_t*>(StringLutFind(&lut, "valuen"));
  EXPECT_EQ(ValuePtrCreate<uint32_t>(100), valuen);

  StringLutRelease(&lut);
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}
