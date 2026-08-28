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
 * @brief Websocket debug logging implementation
 */

#include "ship_connection_debug.h"

#if SHIP_CONNECTION_DEBUG
#include "src/common/array_util.h"

typedef struct SmeStateMapping SmeStateMapping;

struct SmeStateMapping {
    SmeState state;
    const char* state_txt;
};

#define SME_STATE_MAPPING(s) {s, #s}

const char* SmeStateToString(SmeState state) {
    static const SmeStateMapping sme_state_lut[] = {
        SME_STATE_MAPPING(kCmiStateInitStart),
        SME_STATE_MAPPING(kCmiStateClientSend),
        SME_STATE_MAPPING(kCmiStateClientWait),
        SME_STATE_MAPPING(kCmiStateClientEvaluate),
        SME_STATE_MAPPING(kCmiStateServerWait),
        SME_STATE_MAPPING(kCmiStateServerEvaluate),
        SME_STATE_MAPPING(kSmeHelloState),
        SME_STATE_MAPPING(kSmeHelloStateReadyInit),
        SME_STATE_MAPPING(kSmeHelloStateReadyListen),
        SME_STATE_MAPPING(kSmeHelloStateReadyTimeout),
        SME_STATE_MAPPING(kSmeHelloStatePendingInit),
        SME_STATE_MAPPING(kSmeHelloStatePendingListen),
        SME_STATE_MAPPING(kSmeHelloStatePendingTimeout),
        SME_STATE_MAPPING(kSmeHelloStateOk),
        SME_STATE_MAPPING(kSmeHelloStateAbort),
        SME_STATE_MAPPING(kSmeHelloStateAbortDone),
        SME_STATE_MAPPING(kSmeHelloStateRemoteAbortDone),
        SME_STATE_MAPPING(kSmeHelloStateRejected),
        SME_STATE_MAPPING(kSmeProtHStateServerInit),
        SME_STATE_MAPPING(kSmeProtHStateClientInit),
        SME_STATE_MAPPING(kSmeProtHStateServerListenProposal),
        SME_STATE_MAPPING(kSmeProtHStateServerListenConfirm),
        SME_STATE_MAPPING(kSmeProtHStateClientListenChoice),
        SME_STATE_MAPPING(kSmeProtHStateTimeout),
        SME_STATE_MAPPING(kSmeProtHStateClientOk),
        SME_STATE_MAPPING(kSmeProtHStateServerOk),
        SME_STATE_MAPPING(kSmePinStateCheckInit),
        SME_STATE_MAPPING(kSmePinStateCheckListen),
        SME_STATE_MAPPING(kSmePinStateCheckError),
        SME_STATE_MAPPING(kSmePinStateCheckBusyInit),
        SME_STATE_MAPPING(kSmePinStateCheckBusyWait),
        SME_STATE_MAPPING(kSmePinStateCheckOk),
        SME_STATE_MAPPING(kSmePinStateAskInit),
        SME_STATE_MAPPING(kSmePinStateAskProcess),
        SME_STATE_MAPPING(kSmePinStateAskRestrictedOk),
        SME_STATE_MAPPING(kSmePinStateAskOk),
        SME_STATE_MAPPING(kSmeAccessMethodsRequest),
        SME_STATE_MAPPING(kSmeStateApproved),
        SME_STATE_MAPPING(kDataExchange),
        SME_STATE_MAPPING(kSmeStateError),
    };

    for (size_t i = 0; i < ARRAY_SIZE(sme_state_lut); ++i) {
        if (sme_state_lut[i].state == state) {
            return sme_state_lut[i].state_txt;
        }
    }

    return "";
}

#else

#include "src/common/eebus_arguments.h"

const char* SmeStateToString(SmeState state) {
    UNUSED(state);

    return "";
}

#endif  // #if SHIP_CONNECTION_DEBUG
