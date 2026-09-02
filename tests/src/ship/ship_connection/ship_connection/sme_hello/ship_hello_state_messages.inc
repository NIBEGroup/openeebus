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
#include <string_view>

using std::literals::string_view_literals::operator""sv;

static constexpr std::string_view kSmeHelloStateAbortMsg =
R"({
  "connectionHello": [
    {
      "phase": "aborted"
    }
  ]
})"sv;

static constexpr std::string_view kSmeHelloPendingInitMsg =
R"({
  "connectionHello": [
    {
      "phase": "pending"
    },
    {
      "waiting": 60000
    }
  ]
})"sv;

static constexpr std::string_view kSmeHelloPendingInitMsgWaitingPeriodLessThanThreshold =
R"({
  "connectionHello": [
    {
      "phase": "pending"
    },
    {
      "waiting": 1000
    }
  ]
})"sv;

static constexpr std::string_view kSmeHelloReadyMsgMissingWaiting =
R"({
  "connectionHello": [
    {
      "phase": "ready"
    }
  ]
})"sv;

static constexpr std::string_view kSmeHelloReadyMsg =
R"({
  "connectionHello": [
    {
      "phase": "ready"
    },
    {
      "waiting": 60000
    }
  ]
})"sv;

static constexpr std::string_view kSmeHelloReadyMsgWaitingPeriodLessThanThreshold =
R"({
  "connectionHello": [
    {
      "phase": "ready"
    },
    {
      "waiting": 1000
    }
  ]
})"sv;

static constexpr std::string_view kSmeHelloPendingMsgMissingProlongationRequest =
R"({
  "connectionHello": [
    {
      "phase": "pending"
    }
  ]
})"sv;

static constexpr std::string_view kSmeHelloPendingMsgWithProlongationRequestFalse =
R"({
  "connectionHello": [
    {
      "phase": "pending"
    },
    {
      "prolongationRequest": false
    }
  ]
})"sv;

static constexpr std::string_view kSmeHelloPendingMsgWithProlongationRequestTrue =
R"({
  "connectionHello": [
    {
      "phase": "pending"
    },
    {
      "prolongationRequest": true
    }
  ]
})"sv;
