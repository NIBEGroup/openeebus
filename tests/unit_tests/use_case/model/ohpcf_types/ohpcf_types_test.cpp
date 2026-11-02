/*
 * Copyright 2026 NIBE AB
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
 * @brief OHPCF types tests
 */

#include <gtest/gtest.h>

#include <string_view>

#include "src/spine/model/power_sequences_types.h"
#include "src/use_case/model/ohpcf_types.h"

using std::literals::string_view_literals::operator""sv;

//-------------------------------------------------------------------------------------------//
//
// CompressorOhpcfStateGetStateWithPowerSequenceState() test
//
//-------------------------------------------------------------------------------------------//
struct CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput {
  std::string_view description{""sv};
  PowerSequenceStateType ps_state{kPowerSequenceStateTypeInvalid};
  CompressorOhpcfState expected_state{kCompressorOhpcfStateUndefined};
};

std::ostream& operator<<(std::ostream& os, CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput test_input) {
  return os << test_input.description;
}

class CompressorOhpcfStateGetStateWithPowerSequenceStateTests
    : public ::testing::TestWithParam<CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput> {};

TEST_P(
    CompressorOhpcfStateGetStateWithPowerSequenceStateTests,
    CompressorOhpcfStateGetStateWithPowerSequenceStateTests
) {
  // Act: Map the received power sequence state to the OHPCF state
  const CompressorOhpcfState state = CompressorOhpcfStateGetStateWithPowerSequenceState(GetParam().ps_state);

  // Assert: Verify with expected OHPCF state
  EXPECT_EQ(state, GetParam().expected_state);
}

INSTANTIATE_TEST_SUITE_P(
    CompressorOhpcfStateGetStateWithPowerSequenceStateTests,
    CompressorOhpcfStateGetStateWithPowerSequenceStateTests,
    ::testing::Values(
        // [OHPCF-011/1] optional process announced but not scheduled yet
        CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput{
            .description    = "inactive is announced"sv,
            .ps_state       = kPowerSequenceStateTypeInactive,
            .expected_state = kCompressorOhpcfStateAnnounced,
        },
        CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput{
            .description    = "scheduled is scheduled"sv,
            .ps_state       = kPowerSequenceStateTypeScheduled,
            .expected_state = kCompressorOhpcfStateScheduled,
        },
        CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput{
            .description    = "running is running"sv,
            .ps_state       = kPowerSequenceStateTypeRunning,
            .expected_state = kCompressorOhpcfStateRunning,
        },
        CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput{
            .description    = "paused is paused"sv,
            .ps_state       = kPowerSequenceStateTypePaused,
            .expected_state = kCompressorOhpcfStatePaused,
        },
        // [OHPCF-006/1], [OHPCF-012/2/4] process stopped/aborted
        CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput{
            .description    = "invalid is stopped"sv,
            .ps_state       = kPowerSequenceStateTypeInvalid,
            .expected_state = kCompressorOhpcfStateStopped,
        },
        CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput{
            .description    = "completed is completed"sv,
            .ps_state       = kPowerSequenceStateTypeCompleted,
            .expected_state = kCompressorOhpcfStateCompleted,
        },
        // States not used by OHPCF
        CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput{
            .description    = "scheduledPaused is undefined"sv,
            .ps_state       = kPowerSequenceStateTypeScheduledPaused,
            .expected_state = kCompressorOhpcfStateUndefined,
        },
        CompressorOhpcfStateGetStateWithPowerSequenceStateTestInput{
            .description    = "pending is undefined"sv,
            .ps_state       = kPowerSequenceStateTypePending,
            .expected_state = kCompressorOhpcfStateUndefined,
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// CompressorOhpcfStateGetPowerSequenceState() test
//
//-------------------------------------------------------------------------------------------//
struct CompressorOhpcfStateGetPowerSequenceStateTestInput {
  std::string_view description{""sv};
  CompressorOhpcfState ohpcf_state{kCompressorOhpcfStateUndefined};
  PowerSequenceStateType expected_ps_state{kPowerSequenceStateTypeInvalid};
};

std::ostream& operator<<(std::ostream& os, CompressorOhpcfStateGetPowerSequenceStateTestInput test_input) {
  return os << test_input.description;
}

class CompressorOhpcfStateGetPowerSequenceStateTests
    : public ::testing::TestWithParam<CompressorOhpcfStateGetPowerSequenceStateTestInput> {};

TEST_P(CompressorOhpcfStateGetPowerSequenceStateTests, CompressorOhpcfStateGetPowerSequenceStateTests) {
  // Act: Map the OHPCF state to the power sequence state to be sent
  const PowerSequenceStateType ps_state = CompressorOhpcfStateGetPowerSequenceState(GetParam().ohpcf_state);

  // Assert: Verify with expected power sequence state
  EXPECT_EQ(ps_state, GetParam().expected_ps_state);
}

INSTANTIATE_TEST_SUITE_P(
    CompressorOhpcfStateGetPowerSequenceStateTests,
    CompressorOhpcfStateGetPowerSequenceStateTests,
    ::testing::Values(
        // [OHPCF-011/1] optional process announced but not scheduled yet
        CompressorOhpcfStateGetPowerSequenceStateTestInput{
            .description       = "announced is inactive"sv,
            .ohpcf_state       = kCompressorOhpcfStateAnnounced,
            .expected_ps_state = kPowerSequenceStateTypeInactive,
        },
        CompressorOhpcfStateGetPowerSequenceStateTestInput{
            .description       = "scheduled is scheduled"sv,
            .ohpcf_state       = kCompressorOhpcfStateScheduled,
            .expected_ps_state = kPowerSequenceStateTypeScheduled,
        },
        CompressorOhpcfStateGetPowerSequenceStateTestInput{
            .description       = "running is running"sv,
            .ohpcf_state       = kCompressorOhpcfStateRunning,
            .expected_ps_state = kPowerSequenceStateTypeRunning,
        },
        CompressorOhpcfStateGetPowerSequenceStateTestInput{
            .description       = "paused is paused"sv,
            .ohpcf_state       = kCompressorOhpcfStatePaused,
            .expected_ps_state = kPowerSequenceStateTypePaused,
        },
        // [OHPCF-006/1], [OHPCF-012/2/4] process stopped/aborted
        CompressorOhpcfStateGetPowerSequenceStateTestInput{
            .description       = "stopped is invalid"sv,
            .ohpcf_state       = kCompressorOhpcfStateStopped,
            .expected_ps_state = kPowerSequenceStateTypeInvalid,
        },
        CompressorOhpcfStateGetPowerSequenceStateTestInput{
            .description       = "completed is completed"sv,
            .ohpcf_state       = kCompressorOhpcfStateCompleted,
            .expected_ps_state = kPowerSequenceStateTypeCompleted,
        },
        CompressorOhpcfStateGetPowerSequenceStateTestInput{
            .description       = "undefined is invalid"sv,
            .ohpcf_state       = kCompressorOhpcfStateUndefined,
            .expected_ps_state = kPowerSequenceStateTypeInvalid,
        }
    )
);
