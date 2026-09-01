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
#ifndef SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_INTERNAL_H_
#define SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_INTERNAL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/common/api/eebus_queue_interface.h"
#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/message_buffer.h"
#include "src/ship/api/data_writer_interface.h"
#include "src/ship/api/info_provider_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"
#include "src/ship/ship_connection/ship_connection_debug.h"
#include "src/ship/ship_connection/types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/** SHIP Connection debug printf(), enabled whith SHIP_CONNECTION_DEBUG = 1 */
#if SHIP_CONNECTION_DEBUG
#define SHIP_CONNECTION_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define SHIP_CONNECTION_DEBUG_PRINTF(fmt, ...)
#endif  // SHIP_CONNECTION_DEBUG

enum ShipConnectionQueueMsgType {
    kShipConnectionQueueMsgTypeDataReceived,
    kShipConnectionQueueMsgTypeSpineDataToSend,
    kShipConnectionQueueMsgTypeTimeout,
    kShipConnectionQueueMsgTypeWebsocketError,
    kShipConnectionQueueMsgTypeWebsocketClose,
    kShipConnectionQueueMsgTypeCancel,
};

typedef enum ShipConnectionQueueMsgType ShipConnectionQueueMsgType;

typedef struct ShipConnectionQueueMessage ShipConnectionQueueMessage;

struct ShipConnectionQueueMessage {
    ShipConnectionQueueMsgType type;
    MessageBuffer msg_buf;
};

typedef struct {
    /** Implements the Ship Connecion Interface */
    ShipConnectionObject sc_object;

    // The ship connection mode of this connection
    ShipRole role;
    const TlsCertificateObject* tls_cert;
    const char* remote_ski;
    const char* remote_ship_id;
    const char* local_ship_id;
    InfoProviderObject* info_provider;
    DataReaderObject* data_reader;
    WebsocketObject* websocket;
    SmeState sme_state;
    EebusError sme_error;
    bool is_access_methods_req_sent;
    EebusTimerObject* wait_for_ready_timer;
    EebusTimerObject* send_prolongation_request_timer;
    EebusTimerObject* prolongation_request_reply_timer;
    uint32_t last_received_waiting_value;
    // TODO: investigate better approach for call once in POSIX, e.g.
    // pthread_once_t shutdownOnce;
    bool shutdown_once;

    bool cancel;

    EebusQueueObject* msg_queue;
    EebusThreadObject* thread;
    MessageBuffer msg;
} ShipConnection;

#define SHIP_CONNECTION(obj) ((ShipConnection*)(obj))

void ShipConnectionConstruct(
    ShipConnection* self,
    InfoProviderObject* info_provider,
    ShipRole role,
    const char* local_ship_id,
    const char* remote_ski,
    const char* remote_ship_id
);

void ShipConnectionSetSmeState(ShipConnection* self, SmeState state);

void ShipConnectionCloseWithError(ShipConnection* self, const char* error);

EebusError ShipConnectionSend(ShipConnection* self, const MessageBuffer* buf);

EebusError ShipConnectionReceive(ShipConnection* self, MessageBuffer* buf, uint32_t timeout);

bool ShipConnectionEvaluateInitMsg(const MessageBuffer* buf);

void SmeProtHandshakeStateAbort(ShipConnection* self, MessageProtocolHandshakeErrorType error);

EebusError ShipConnectionSerializeAndSendMessage(ShipConnection* self, const void* message, MsgValueType value_type);

void SmeHelloStateReadyInit(ShipConnection* self);

void SmeHelloStateReadyListen(ShipConnection* self);

void SmeHelloStatePendingInit(ShipConnection* self);

void SmeHelloStatePendingTimeout(ShipConnection* self);

void SmeHelloStatePendingListen(ShipConnection* self);

void SmeHelloStateAbort(ShipConnection* self);

void SmePinStateCheckInit(ShipConnection* self);

void SmePinStateCheckListen(ShipConnection* self);

void SmeHandshakeAccessMethodsInit(ShipConnection* self);

void HandleState(ShipConnection* self);

void SmeStateApproved(ShipConnection* self);

void DataExchange(ShipConnection* self);

void ShipConnectionWebsocketCallback(WebsocketCallbackType type, const void* in, size_t size, void* ctx);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_INTERNAL_H_
