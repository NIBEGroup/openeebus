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
#include "tests/src/common/eebus_data/eebus_list_wrapper.h"

#include <stdint.h>

#include <gtest/gtest.h>

#include <memory>

TEST(EebusDataListTests, DataAppendTest) {
  // Arrange
  EebusListWrapper<int32_t> list;
  std::unique_ptr<int32_t> data = std::make_unique<int32_t>(42);

  // Act
  EebusError result = list.Append(data.get());

  // Assert
  EXPECT_EQ(result, kEebusErrorOk);
  ASSERT_EQ(list.Size(), 1);
  EXPECT_NE(list.Get(), nullptr);
  EXPECT_EQ(list[0], data.get());
}

TEST(EebusDataListTests, DataAppendNullTest) {
  // Arrange
  EebusListWrapper<int32_t> list;

  // Act
  EebusError result = list.Append(nullptr);

  // Assert
  EXPECT_EQ(result, kEebusErrorInputArgumentNull);
}

TEST(EebusDataListTests, DataRemoveBeginTest) {
  // Arrange
  EebusListWrapper<int32_t> list;

  std::unique_ptr<int32_t> data1 = std::make_unique<int32_t>(37);
  std::unique_ptr<int32_t> data2 = std::make_unique<int32_t>(43);
  std::unique_ptr<int32_t> data3 = std::make_unique<int32_t>(91);

  list.Append(data1.get());
  list.Append(data2.get());
  list.Append(data3.get());

  // Act
  EebusError result = list.Remove(data1.get());

  // Assert
  EXPECT_EQ(result, kEebusErrorOk);
  ASSERT_EQ(list.Size(), 2);
  EXPECT_NE(list.Get(), nullptr);
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[0]), data2.get());
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[1]), data3.get());
}

TEST(EebusDataListTests, DataRemoveMiddleTest) {
  // Arrange
  EebusListWrapper<int32_t> list;

  std::unique_ptr<int32_t> data1 = std::make_unique<int32_t>(37);
  std::unique_ptr<int32_t> data2 = std::make_unique<int32_t>(43);
  std::unique_ptr<int32_t> data3 = std::make_unique<int32_t>(55);
  std::unique_ptr<int32_t> data4 = std::make_unique<int32_t>(91);

  list.Append(data1.get());
  list.Append(data2.get());
  list.Append(data3.get());
  list.Append(data4.get());

  // Act
  EebusError result = list.Remove(data2.get());

  // Assert
  EXPECT_EQ(result, kEebusErrorOk);
  ASSERT_EQ(list.Size(), 3);
  EXPECT_NE(list.Get(), nullptr);
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[0]), data1.get());
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[1]), data3.get());
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[2]), data4.get());
}

TEST(EebusDataListTests, DataRemoveEndTest) {
  // Arrange
  EebusListWrapper<int32_t> list;

  std::unique_ptr<int32_t> data1 = std::make_unique<int32_t>(37);
  std::unique_ptr<int32_t> data2 = std::make_unique<int32_t>(43);
  std::unique_ptr<int32_t> data3 = std::make_unique<int32_t>(55);
  std::unique_ptr<int32_t> data4 = std::make_unique<int32_t>(91);
  std::unique_ptr<int32_t> data5 = std::make_unique<int32_t>(105);

  list.Append(data1.get());
  list.Append(data2.get());
  list.Append(data3.get());
  list.Append(data4.get());
  list.Append(data5.get());

  // Act
  EebusError result = list.Remove(data5.get());

  // Assert
  EXPECT_EQ(result, kEebusErrorOk);
  ASSERT_EQ(list.Size(), 4);
  EXPECT_NE(list.Get(), nullptr);
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[0]), data1.get());
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[1]), data2.get());
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[2]), data3.get());
  EXPECT_EQ(reinterpret_cast<int32_t*>(list[3]), data4.get());
}

TEST(EebusDataListTests, DataRemoveNullTest) {
  // Arrange
  EebusListWrapper<int32_t> list;

  // Act
  EebusError result = list.Remove(nullptr);

  // Assert
  EXPECT_EQ(result, kEebusErrorInputArgumentNull);
}
