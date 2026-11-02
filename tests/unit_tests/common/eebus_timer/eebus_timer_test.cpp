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
#include "src/common/eebus_timer/eebus_timer.h"

#include <gtest/gtest.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#define sleep(x) Sleep((x * 1000))
#endif

#include <string_view>

#include "src/common/eebus_arguments.h"
#include "tests/unit_tests/memory_leak.inc"

static void EebusTimerTimeoutCallbackStub(void* eebus_timer) {
  UNUSED(eebus_timer);
  return;
}

struct EebusTimerTestInput {
  std::string_view description;
  uint32_t timeout;
  uint32_t sleep;
  uint32_t time;
  uint32_t tolerance;
  EebusTimerState timer_state_1;
  EebusTimerState timer_state_2;
  EebusTimerState timer_state_3;
};

std::ostream& operator<<(std::ostream& os, EebusTimerTestInput test_input) {
  return os << test_input.description;
}

class EebusTimerTests : public ::testing::TestWithParam<EebusTimerTestInput> {
 public:
  void SetUp() override;
  void TearDown() override;

 protected:
  EebusTimerObject* eebus_timer;
};

class EebusTimerTestSuite : public testing::Test {};

void EebusTimerTests::SetUp() {
  eebus_timer = EebusTimerCreate(EebusTimerTimeoutCallbackStub, eebus_timer);
  ASSERT_EQ(EEBUS_TIMER_GET_TIMER_STATE(eebus_timer), kEebusTimerStateIdle);
}

void EebusTimerTests::TearDown() {
  EebusTimerDelete(eebus_timer);
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

TEST_P(EebusTimerTests, EebusTimerTimeoutTest) {
  // Arrange: Create timer and get test parameters
  uint32_t timer_timeout = GetParam().timeout;
  uint32_t sleep_timeout = GetParam().sleep;

  // Act: Start timer and check remaining time and state after a set period
  EEBUS_TIMER_START(eebus_timer, timer_timeout, false);
  EebusTimerState state_1 = EEBUS_TIMER_GET_TIMER_STATE(eebus_timer);

  sleep(sleep_timeout);
  uint32_t time           = EEBUS_TIMER_GET_REMAINING_TIME(eebus_timer);
  EebusTimerState state_2 = EEBUS_TIMER_GET_TIMER_STATE(eebus_timer);

  EEBUS_TIMER_STOP(eebus_timer);
  EebusTimerState state_3 = EEBUS_TIMER_GET_TIMER_STATE(eebus_timer);

  // Assert: Verify that the remaining time and timer state are correct
  EXPECT_EQ(state_1, GetParam().timer_state_1);
  EXPECT_EQ(state_2, GetParam().timer_state_2);
  EXPECT_EQ(state_3, GetParam().timer_state_3);
  EXPECT_NEAR(time, GetParam().time, GetParam().tolerance);
}

INSTANTIATE_TEST_SUITE_P(
    EebusTimerTests,
    EebusTimerTests,
    ::testing::Values(
        EebusTimerTestInput{
            .description   = "Timer does not start",
            .timeout       = SECONDS(0),
            .sleep         = 1,
            .time          = SECONDS(0),
            .tolerance     = MILLISECONDS(0),
            .timer_state_1 = kEebusTimerStateIdle,
            .timer_state_2 = kEebusTimerStateIdle,
            .timer_state_3 = kEebusTimerStateIdle,
        },
        EebusTimerTestInput{
            .description   = "Normal timer start",
            .timeout       = SECONDS(2),
            .sleep         = 1,
            .time          = SECONDS(1),
            .tolerance     = MILLISECONDS(20),
            .timer_state_1 = kEebusTimerStateRunning,
            .timer_state_2 = kEebusTimerStateRunning,
            .timer_state_3 = kEebusTimerStateIdle,
        },
        EebusTimerTestInput{
            .description   = "Timer expires",
            .timeout       = SECONDS(1),
            .sleep         = 2,
            .time          = SECONDS(0),
            .tolerance     = MILLISECONDS(0),
            .timer_state_1 = kEebusTimerStateRunning,
            .timer_state_2 = kEebusTimerStateExpired,
            .timer_state_3 = kEebusTimerStateExpired,
        }
    )
);
