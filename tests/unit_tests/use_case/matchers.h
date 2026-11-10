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

#ifndef TESTS_UNIT_TESTS_USE_CASE_MATCHERS_H
#define TESTS_UNIT_TESTS_USE_CASE_MATCHERS_H

#include <gmock/gmock.h>

#include <cstring>
#include <memory>

#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/spine/model/datagram.h"
#include "src/spine/model/device_diagnosis_types.h"
#include "src/use_case/model/scaled_value.h"
#include "tests/unit_tests/json.h"

MATCHER_P2(ScaledValueEq, value, scale, "") {
  return arg != nullptr && arg->value == value && arg->scale == scale;
}

MATCHER_P3(DurationTypeEq, hours, minutes, seconds, "") {
  return arg != nullptr && arg->hours == hours && arg->minutes == minutes && arg->seconds == seconds;
}

MATCHER_P(JsonMsgEq, expected_json, "") {
  const auto& [msg, msg_size] = arg;
  const std::unique_ptr<char, decltype(&JsonFree)> expected_compact{JsonUnformat(expected_json), JsonFree};
  if (expected_compact == nullptr) {
    *result_listener << "expected JSON could not be parsed";
    return false;
  }

  const size_t expected_size = strlen(expected_compact.get()) + 1;
  return msg_size == expected_size && memcmp(msg, expected_compact.get(), expected_size) == 0;
}

MATCHER_P(HeartbeatMsgEq, expected_json, "") {
  auto actual_json = reinterpret_cast<const char*>(arg);
  const std::unique_ptr<DatagramType, decltype(&DatagramDelete)> expected{DatagramParse(expected_json), DatagramDelete};
  const std::unique_ptr<DatagramType, decltype(&DatagramDelete)> actual{DatagramParse(actual_json), DatagramDelete};

  if ((expected == nullptr) || (actual == nullptr)) {
    return false;
  }

  if (!DatagramHeaderCompare(expected->header, actual->header)) {
    return false;
  }

  auto get_heartbeat = [](const DatagramType* dg) -> const DeviceDiagnosisHeartbeatDataType* {
    if ((dg == nullptr) || (dg->payload == nullptr) || (dg->payload->cmd_size == 0)) {
      return nullptr;
    }

    const CmdType* cmd{dg->payload->cmd[0]};
    if ((cmd == nullptr) || (cmd->data_choice_type_id != kFunctionTypeDeviceDiagnosisHeartbeatData)) {
      return nullptr;
    }

    return reinterpret_cast<const DeviceDiagnosisHeartbeatDataType*>(cmd->data_choice);
  };

  const auto* eh{get_heartbeat(expected.get())};
  const auto* ah{get_heartbeat(actual.get())};
  if ((eh == nullptr) || (ah == nullptr) || (eh->heartbeat_counter == nullptr) || (ah->heartbeat_counter == nullptr)) {
    return false;
  }

  if (*ah->heartbeat_counter != *eh->heartbeat_counter) {
    return false;
  }

  if ((eh->heartbeat_timeout == nullptr) || (ah->heartbeat_timeout == nullptr)) {
    return false;
  }

  return EebusDurationCompare(ah->heartbeat_timeout, eh->heartbeat_timeout) == 0;
}

#endif  // TESTS_UNIT_TESTS_USE_CASE_MATCHERS_H
