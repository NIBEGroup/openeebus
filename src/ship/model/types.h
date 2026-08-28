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
 * @brief Transformed model/types.go
 */

#ifndef SRC_SHIP_MODEL_TYPES_H_
#define SRC_SHIP_MODEL_TYPES_H_

#include <stdint.h>

#include "model.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum SmeState {
    // Connection Mode Initialisation (CMI) SHIP 13.4.3
    kCmiStateInitStart = 0,
    kCmiStateClientSend,
    kCmiStateClientWait,
    kCmiStateClientEvaluate,
    kCmiStateServerWait,
    kCmiStateServerEvaluate,
    // Connection Data Preparation SHIP 13.4.4
    kSmeHelloState,
    kSmeHelloStateReadyInit,
    kSmeHelloStateReadyListen,
    kSmeHelloStateReadyTimeout,
    kSmeHelloStatePendingInit,
    kSmeHelloStatePendingListen,
    kSmeHelloStatePendingTimeout,
    kSmeHelloStateOk,
    kSmeHelloStateAbort,            // Sent abort to remote
    kSmeHelloStateAbortDone,        // Sending abort to remote is done
    kSmeHelloStateRemoteAbortDone,  // Received abort from remote
    kSmeHelloStateRejected,         // Connection closed after remote pending: "4452:
                                    // Node rejected by application"

    // Connection State Protocol Handhsake SHIP 13.4.4.2
    kSmeProtHStateServerInit,
    kSmeProtHStateClientInit,
    kSmeProtHStateServerListenProposal,
    kSmeProtHStateServerListenConfirm,
    kSmeProtHStateClientListenChoice,
    kSmeProtHStateTimeout,
    kSmeProtHStateClientOk,
    kSmeProtHStateServerOk,
    // Connection PIN State 13.4.5
    kSmePinStateCheckInit,
    kSmePinStateCheckListen,
    kSmePinStateCheckError,
    kSmePinStateCheckBusyInit,
    kSmePinStateCheckBusyWait,
    kSmePinStateCheckOk,
    kSmePinStateAskInit,
    kSmePinStateAskProcess,
    kSmePinStateAskRestrictedOk,
    kSmePinStateAskOk,
    // ConnectionAccess Methods Identification 13.4.6
    kSmeAccessMethodsRequest,

    // Handshake approved on both ends
    kSmeStateApproved,

    // Handshake process is successfully completed
    kDataExchange,

    // Handshake ended with an error
    kSmeStateError,
};

typedef enum SmeState SmeState;

static const uint8_t kShipInitMessage[] = {kMsgTypeInit, 0x00};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_MODEL_TYPES_H_
