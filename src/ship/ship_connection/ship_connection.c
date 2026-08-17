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
#include "ship_connection.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "src/common/array_util.h"
#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_queue/eebus_queue.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/common/string_util.h"
#include "src/ship/api/connection_state.h"
#include "src/ship/api/data_reader_interface.h"
#include "src/ship/api/data_writer_interface.h"
#include "src/ship/api/info_provider_interface.h"
#include "src/ship/api/ship_message.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/model/model.h"
#include "src/ship/model/types.h"
#include "src/ship/ship_connection/client.h"
#include "src/ship/ship_connection/server.h"
#include "src/ship/ship_connection/ship_connection_debug.h"
#include "src/ship/ship_connection/ship_connection_internal.h"
#include "src/ship/ship_connection/ship_message_deserialize.h"
#include "src/ship/ship_connection/ship_message_serialize.h"
#include "src/ship/ship_connection/types.h"
#include "src/ship/websocket/websocket.h"

static EebusError Start(ShipConnectionObject* self, WebsocketCreatorObject* websocket_creator);
static void Stop(ShipConnectionObject* self);
static void Destruct(DataWriterObject* self);
static void WriteMessage(DataWriterObject* self, const uint8_t* message, size_t messageSize);

static WebsocketObject* GetWebsocketConnection(ShipConnectionObject* self);
static void CloseConnection(ShipConnectionObject* self, bool safe, int32_t code, const char* reason);
static const char* GetRemoteSki(ShipConnectionObject* self);
static void ApprovePendingHandshake(ShipConnectionObject* self);
static void AbortPendingHandshake(ShipConnectionObject* self);

static SmeState GetState(ShipConnectionObject* self, EebusError* err);
static void ShipConnectionTimeoutCallback(void* timer_data);

static const ShipConnectionInterface ship_connection_methods = {
    .data_writer_interface =
        {
            .destruct = Destruct,
            .write_message = WriteMessage,
        },

    .start = Start,
    .stop = Stop,
    .get_websocket_connection = GetWebsocketConnection,
    .close_connection = CloseConnection,
    .get_remote_ski = GetRemoteSki,
    .approve_pending_handshake = ApprovePendingHandshake,
    .abort_pending_handshake = AbortPendingHandshake,
    .get_state = GetState,
};

void ShipConnectionConstruct(
    ShipConnection* self,
    InfoProviderObject* info_provider,
    ShipRole role,
    const char* local_ship_id,
    const char* remote_ski,
    const char* remote_ship_id
) {
  // Override "virtual functions table"
  SHIP_CONNECTION_INTERFACE(self) = &ship_connection_methods;

  self->role           = role;
  self->remote_ski     = StringCopy(remote_ski);
  self->remote_ship_id = StringCopy(remote_ship_id);
  self->local_ship_id  = local_ship_id;
  self->info_provider  = info_provider;
  self->data_reader    = NULL;
  self->websocket      = NULL;
  self->sme_state      = (role == kShipRoleServer) ? kCmiStateServerWait : kCmiStateClientSend;
  self->sme_error      = kEebusErrorOk;

  self->is_access_methods_req_sent = false;

  self->wait_for_ready_timer             = EebusTimerCreate(ShipConnectionTimeoutCallback, self);
  self->send_prolongation_request_timer  = EebusTimerCreate(ShipConnectionTimeoutCallback, self);
  self->prolongation_request_reply_timer = EebusTimerCreate(ShipConnectionTimeoutCallback, self);

  self->last_received_waiting_value = 0;
  self->shutdown_once               = false;

  self->cancel    = false;
  self->msg_queue = NULL;
  self->thread    = NULL;
  MessageBufferInit(&self->msg, NULL, 0);
}

ShipConnectionObject* ShipConnectionCreate(
    InfoProviderObject* info_provider,
    ShipRole role,
    const char* local_ship_id,
    const char* remote_ski,
    const char* remote_ship_id
) {
  ShipConnection* sc = (ShipConnection*)EEBUS_MALLOC(sizeof(ShipConnection));

  ShipConnectionConstruct(sc, info_provider, role, local_ship_id, remote_ski, remote_ship_id);

  return SHIP_CONNECTION_OBJECT(sc);
}

void ShipConnectionQueueMsgDeallocator(void* msg) {
  if (msg == NULL) {
    return;
  }

  ShipConnectionQueueMessage* queue_msg = (ShipConnectionQueueMessage*)msg;

  if ((queue_msg->type == kShipConnectionQueueMsgTypeDataReceived)
      || (queue_msg->type == kShipConnectionQueueMsgTypeSpineDataToSend)) {
    MessageBufferRelease(&queue_msg->msg_buf);
  }
}

void Destruct(DataWriterObject* self) {
  ShipConnection* const sc = SHIP_CONNECTION(self);

  sc->cancel = true;

#ifndef GTEST
  if (sc->thread != NULL) {
    EEBUS_THREAD_JOIN(sc->thread);
    EebusThreadDelete(sc->thread);
    sc->thread = NULL;
  }
#endif

  if (sc->websocket != NULL) {
    WebsocketDelete(sc->websocket);
    sc->websocket = NULL;
  }

  EebusTimerDelete(sc->wait_for_ready_timer);
  sc->wait_for_ready_timer = NULL;

  EebusTimerDelete(sc->send_prolongation_request_timer);
  sc->send_prolongation_request_timer = NULL;

  EebusTimerDelete(sc->prolongation_request_reply_timer);
  sc->prolongation_request_reply_timer = NULL;

  EebusQueueDelete(sc->msg_queue);
  sc->msg_queue = NULL;

  MessageBufferRelease(&sc->msg);

  StringDelete((char*)sc->remote_ship_id);
  sc->remote_ship_id = NULL;

  StringDelete((char*)sc->remote_ski);
  sc->remote_ski = NULL;
}

void* ShipConnectionLoop(void* parameters) {
  ShipConnection* const sc = (ShipConnection*)parameters;

  while ((!sc->cancel) && (!sc->shutdown_once)) {
    HandleState(sc);
  }

  return NULL;
}

EebusError ShipConnectionTryStart(ShipConnection* self, WebsocketCreatorObject* websocket_creator) {
  static const size_t kQueueMaxMsg = 30;

  self->msg_queue
      = EebusQueueCreate(kQueueMaxMsg, sizeof(ShipConnectionQueueMessage), ShipConnectionQueueMsgDeallocator);
  if (self->msg_queue == NULL) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), initialising write queue failed\n", __func__);
    return kEebusErrorMemory;
  }

  self->websocket = WEBSOCKET_CREATOR_CREATE_WEBSOCKET(websocket_creator, ShipConnectionWebsocketCallback, self);
  if (self->websocket == NULL) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), opening websocket failed\n", __func__);
    return kEebusErrorCommunicationBegin;
  }

#ifndef GTEST
  self->thread = EebusThreadCreate(ShipConnectionLoop, self, 4 * 1024);
  if (self->thread == NULL) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), start thread failed\n", __func__);
    return kEebusErrorThread;
  }
#endif

  return kEebusErrorOk;
}

EebusError Start(ShipConnectionObject* self, WebsocketCreatorObject* websocket_creator) {
  ShipConnection* const sc = SHIP_CONNECTION(self);

  EebusError ret = ShipConnectionTryStart(sc, websocket_creator);
  if (ret != kEebusErrorOk) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), start SHIP connection failed\n", __func__);
  }

  return ret;
}

void Stop(ShipConnectionObject* self) {
  ShipConnection* const sc = SHIP_CONNECTION(self);

  sc->cancel = true;

  ShipConnectionQueueMessage queue_msg = {.type = kShipConnectionQueueMsgTypeCancel};
  EEBUS_QUEUE_SEND(sc->msg_queue, &queue_msg, kTimeoutInfinite);

  if (sc->thread != NULL) {
    EEBUS_THREAD_JOIN(sc->thread);
    EebusThreadDelete(sc->thread);
    sc->thread = NULL;
  }

  SHIP_CONNECTION_CLOSE_CONNECTION(self, false, 0, "");
}

const char* GetRemoteSki(ShipConnectionObject* self) {
  return SHIP_CONNECTION(self)->remote_ski;
}

WebsocketObject* GetWebsocketConnection(ShipConnectionObject* self) {
  return SHIP_CONNECTION(self)->websocket;
}

SmeState GetState(ShipConnectionObject* self, EebusError* error) {
  const ShipConnection* const sc = SHIP_CONNECTION(self);

  if (error != NULL) {
    *error = sc->sme_error;
  }

  return sc->sme_state;
}

void ApprovePendingHandshake(ShipConnectionObject* self) {
  ShipConnection* const sc = SHIP_CONNECTION(self);

  if (sc->sme_state != kSmeHelloStatePendingListen) {
    // TODO: what to do if the state is different?
    return;
  }

  // TODO: move this into hs_hello.go and add tests

  // HELLO_OK
  ShipConnectionSetSmeState(sc, kSmeHelloStateReadyInit);
  HandleState(sc);

  // TODO: check if we need to do some validations before moving on to the next
  // state
  ShipConnectionSetSmeState(sc, kSmeHelloStateOk);
  HandleState(sc);
}

void AbortPendingHandshake(ShipConnectionObject* self) {
  ShipConnection* const sc = SHIP_CONNECTION(self);

  const SmeState state = sc->sme_state;
  if (state != kSmeHelloStatePendingListen && state != kSmeHelloStateReadyListen) {
    // TODO: what to do if the state is differnet?
    return;
  }

  // TODO: Move this into hs_hello.go and add tests

  ShipConnectionSetSmeState(sc, kSmeHelloStateAbort);
}

void CloseConnection(ShipConnectionObject* self, bool safe, int32_t code, const char* reason) {
  ShipConnection* const sc = SHIP_CONNECTION(self);
  if (sc->shutdown_once) {
    return;
  };

  EEBUS_TIMER_STOP(sc->wait_for_ready_timer);
  EEBUS_TIMER_STOP(sc->prolongation_request_reply_timer);
  EEBUS_TIMER_STOP(sc->send_prolongation_request_timer);

  // handshake is completed if approved or aborted
  const SmeState state   = sc->sme_state;
  const bool is_hs_ended = (state == kDataExchange) || (state == kSmeHelloStateAbortDone)
                           || (state == kSmeHelloStateRemoteAbortDone) || (state == kSmeHelloStateRejected);

  // this may not be used for Connection Data Exchange is entered!
  if (safe && state == kDataExchange) {
    // SHIP 13.4.7: Connection Termination Announce
    const ConnectionClose sme_close = {
        .phase  = kConnectionClosePhaseTypeAnnounce,
        .reason = reason,
    };

    ShipConnectionSerializeAndSendMessage(sc, &sme_close, kSmeClose);
    // TODO: Instead of sleeping unconditionally, dequeue messages here and wait
    // for kConnectionClosePhaseTypeConfirm (via DataExchangeHandleClose) with a
    // 500ms timeout, closing immediately on Confirm or on timeout expiry.
    EebusThreadUsleep(500000);  // Wait 0.5s for the message to be sent
    WEBSOCKET_CLOSE(sc->websocket, 4001, "close");
  } else {
    WEBSOCKET_CLOSE(sc->websocket, (code != 0) ? code : 4001, reason);
  }

  sc->cancel        = true;
  sc->shutdown_once = true;
  INFO_PROVIDER_HANDLE_CONNECTION_CLOSED(sc->info_provider, self, is_hs_ended);
}

void WriteMessage(DataWriterObject* self, const uint8_t* msg, size_t msg_size) {
  ShipConnection* const sc = SHIP_CONNECTION(self);

  ShipConnectionQueueMessage queue_msg;

  queue_msg.type          = kShipConnectionQueueMsgTypeSpineDataToSend;
  uint8_t* const msg_copy = (uint8_t*)ArrayCopy(msg, msg_size, sizeof(msg[0]));
  MessageBufferInit(&queue_msg.msg_buf, msg_copy, msg_size);

  EEBUS_QUEUE_SEND(sc->msg_queue, &queue_msg, kTimeoutInfinite);
}

void ReportConnectionError(ShipConnection* self, EebusError err) {
  UNUSED(err);
  // if the handshake is aborted, a closed connection is no error
  const SmeState state = self->sme_state;

  // rejections are also received by sending
  // `{"connectionHello":[{"phase":"pending"},{"waiting":60000}]}` and then
  // closing the websocket connection with `4452: Node rejected by application.`
  if (state == kSmeHelloStateReadyListen) {
    ShipConnectionSetSmeState(self, kSmeHelloStateRejected);
    SHIP_CONNECTION_CLOSE_CONNECTION(self, false, 0, "");
    return;
  }

  if (state == kSmeHelloStateRemoteAbortDone) {
    // remote service should close the connection
    SHIP_CONNECTION_CLOSE_CONNECTION(self, false, 0, "");
    return;
  }

  if ((state == kSmeHelloStateAbort) || (state == kSmeHelloStateAbortDone)) {
    SHIP_CONNECTION_CLOSE_CONNECTION(self, false, 4452, "Node rejected by application");
    return;
  }

  ShipConnectionSetSmeState(self, kSmeStateError);

  SHIP_CONNECTION_CLOSE_CONNECTION(self, false, 0, "");

  INFO_PROVIDER_HANDLE_SHIP_STATE_UPDATE(self->info_provider, self->remote_ski, kSmeStateError, "");
}

void ShipConnectionWebsocketCallback(WebsocketCallbackType type, const void* in, size_t size, void* ctx) {
  ShipConnection* const sc = SHIP_CONNECTION(ctx);
  if ((sc->cancel) || (sc->shutdown_once)) {
    return;
  }

  if (type == kWebsocketCallbackTypeRead) {
    ShipConnectionQueueMessage queue_msg;
    queue_msg.type = kShipConnectionQueueMsgTypeDataReceived;

    uint8_t* const msg_copy = (uint8_t*)ArrayCopy(in, size, sizeof(uint8_t));
    MessageBufferInit(&queue_msg.msg_buf, msg_copy, size);
    EEBUS_QUEUE_SEND(sc->msg_queue, &queue_msg, kTimeoutInfinite);
  } else if (type == kWebsocketCallbackTypeError) {
    static const ShipConnectionQueueMessage err_msg = {.type = kShipConnectionQueueMsgTypeWebsocketError};
    EEBUS_QUEUE_SEND(sc->msg_queue, &err_msg, kTimeoutInfinite);
    // TODO: Call the ReportConnectionError(sc, -1) in other than DataExchange states
  } else if (type == kWebsocketCallbackTypeClose) {
    static const ShipConnectionQueueMessage close_msg = {.type = kShipConnectionQueueMsgTypeWebsocketClose};
    EEBUS_QUEUE_SEND(sc->msg_queue, &close_msg, kTimeoutInfinite);
  }
}

void ShipConnectionTimeoutCallback(void* ctx) {
  ShipConnection* const sc             = (ShipConnection*)ctx;
  ShipConnectionQueueMessage queue_msg = {
      kShipConnectionQueueMsgTypeTimeout,
      {NULL, 0, NULL}
  };
  EEBUS_QUEUE_SEND(sc->msg_queue, &queue_msg, kTimeoutInfinite);
}

void ShipConnectionSetSmeState(ShipConnection* self, SmeState state) {
  if (self->sme_state != state) {
    self->sme_state = state;
    SHIP_CONNECTION_DEBUG_PRINTF("SHIP state changed to: %s (%d)\n", SmeStateToString(state), state);
    INFO_PROVIDER_HANDLE_SHIP_STATE_UPDATE(self->info_provider, SHIP_CONNECTION_GET_REMOTE_SKI(self), state, "");
  }
}

void HandleState(ShipConnection* self) {
  switch (self->role) {
    case kShipRoleClient: {
      ClientHandleState(self);
      break;
    }

    case kShipRoleServer: {
      ServerHandleState(self);
      break;
    }

    default: {
      break;
    }
  }
}

void ShipConnectionCloseWithError(ShipConnection* self, const char* err) {
  ShipConnectionSetSmeState(self, kSmeStateError);
  SHIP_CONNECTION_DEBUG_PRINTF("%s(), error: %s\n", __func__, err);
  SHIP_CONNECTION_CLOSE_CONNECTION(self, true, 0, err);
}

EebusError ShipConnectionSend(ShipConnection* self, const MessageBuffer* buf) {
  const size_t ret = WEBSOCKET_WRITE(self->websocket, buf->data, buf->data_size);
  if (ret != buf->data_size) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), websocket write error\n", __func__);
    return kEebusErrorCommunication;
  }

  return kEebusErrorOk;
}

EebusError ShipConnectionReceive(ShipConnection* self, MessageBuffer* buf, uint32_t timeout) {
  EEBUS_TIMER_START(self->wait_for_ready_timer, timeout, false);

  ShipConnectionQueueMessage queue_msg;
  const EebusError queue_recv_ret = EEBUS_QUEUE_RECEIVE(self->msg_queue, &queue_msg, kTimeoutInfinite);

  EEBUS_TIMER_STOP(self->wait_for_ready_timer);

  if (queue_recv_ret != kEebusErrorOk) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), error receiving the message from queue\n", __func__);
    return queue_recv_ret;
  }

  if (queue_msg.type == kShipConnectionQueueMsgTypeDataReceived) {
    *buf = queue_msg.msg_buf;
    return kEebusErrorOk;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeTimeout) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), timed out\n", __func__);
    return kEebusErrorTime;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeCancel) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), cancelled\n", __func__);
    return kEebusErrorDeactivate;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeWebsocketError) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), websocket error\n", __func__);
    return kEebusErrorCommunication;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeWebsocketClose) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), websocket closed\n", __func__);
    return kEebusErrorCommunicationEnd;
  } else {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), invalid queue message type\n", __func__);
    return kEebusErrorInputType;
  }
}

bool ShipConnectionEvaluateInitMsg(const MessageBuffer* buf) {
  if ((buf->data == NULL) || (buf->data_size != ARRAY_SIZE(kShipInitMessage))) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), invalid input message\n", __func__);
    return false;
  }

  if (buf->data[0] != kShipInitMessage[0]) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), wrong message type\n", __func__);
    return false;
  }

  if (buf->data[1] != kShipInitMessage[1]) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s, wrong message value\n", __func__);
    return false;
  }

  return true;
}

EebusError ShipConnectionSerializeAndSendMessage(ShipConnection* self, const void* message, MsgValueType value_type) {
  ShipMessageSerializeObject* const serialize = ShipMessageSerializeCreate(message, value_type);

  MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  if (buf == NULL) {
    return kEebusErrorParse;
  }

  const EebusError ret = ShipConnectionSend(self, buf);
  ShipMessageSerializeDelete(serialize);
  return ret;
}

EebusError
SmeHelloStateSendHelloMsg(ShipConnection* self, ConnectionHelloPhase phase, uint32_t wait_duration, bool prolongation) {
  ConnectionHello sme_hello = {.phase = phase};

  if (wait_duration > 0) {
    sme_hello.waiting  = &sme_hello.waiting_val;
    *sme_hello.waiting = wait_duration;  // milliseconds assumed
  } else {
    sme_hello.waiting = NULL;
  }

  if (prolongation) {
    sme_hello.prolongation_request  = &sme_hello.prolongation_request_val;
    *sme_hello.prolongation_request = prolongation;
  } else {
    sme_hello.prolongation_request = NULL;
  }

  const EebusError ret = ShipConnectionSerializeAndSendMessage(self, &sme_hello, kSmeHello);
  return ret;
}

void SmeHelloStateReadyInit(ShipConnection* self) {
  // After sending the hello update message, the SHIP user waits on the update
  // message in SmeHelloReadyListen state. The wait for ready timer is set
  // there.
  EEBUS_TIMER_STOP(self->send_prolongation_request_timer);
  EEBUS_TIMER_STOP(self->prolongation_request_reply_timer);
  if (SmeHelloStateSendHelloMsg(self, kConnectionHelloPhaseReady, tHelloInit, false) == kEebusErrorOk) {
    ShipConnectionSetSmeState(self, kSmeHelloStateReadyListen);
  } else {
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
  }
}

void SmeHelloStateCommonUpdateHelloMessageSendProcedure(ShipConnection* self, const ConnectionHello* msg) {
  uint32_t remaining_time = 0;

  if (EEBUS_TIMER_GET_TIMER_STATE(self->wait_for_ready_timer) == kEebusTimerStateRunning) {
    remaining_time = EEBUS_TIMER_GET_REMAINING_TIME(self->wait_for_ready_timer);
  }

  if (SmeHelloStateSendHelloMsg(self, msg->phase, remaining_time, false) != kEebusErrorOk) {
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
  }
}

void SmeHelloStateCommonProlongationRequestAndUpdateMessageSendProcedure(
    ShipConnection* self,
    const ConnectionHello* msg
) {
  if ((msg->prolongation_request != NULL) && (*msg->prolongation_request)) {
    uint32_t remaining_time = EEBUS_TIMER_GET_REMAINING_TIME(self->wait_for_ready_timer) + tHelloInc;
    EEBUS_TIMER_START(self->wait_for_ready_timer, remaining_time, false);
    SmeHelloStateCommonUpdateHelloMessageSendProcedure(self, msg);
  } else {
    // Message is ignored, no action required (13.4.4.1.3 Sub-state
    // SME_HELLO_STATE_READY_LISTEN)
  }
}

void SmeHelloReadyStateEvaluateReceivedHelloMessage(ShipConnection* self) {
  ShipMessageDeserializeObject* const deserialize = ShipMessageDeserializeCreate(&self->msg);

  if (SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize) != kSmeHello) {
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
    ShipMessageDeserializeDelete(deserialize);
    MessageBufferRelease(&self->msg);
    return;
  }

  const ConnectionHello* const sme_hello = (ConnectionHello*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);

  if (sme_hello != NULL) {
    switch (sme_hello->phase) {
      case kConnectionHelloPhaseReady: {
        ShipConnectionSetSmeState(self, kSmeHelloStateOk);
        break;
      }

      case kConnectionHelloPhasePending: {
        SmeHelloStateCommonProlongationRequestAndUpdateMessageSendProcedure(self, sme_hello);
        break;
      }

      case kConnectionHelloPhaseAborted: {
        ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
        break;
      }

      default: {
        ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
        break;
      }
    }
  } else {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), Error deserializing SME hello message\n", __func__);
    SmeHelloStateAbort(self);
  }

  ShipMessageDeserializeDelete(deserialize);
  MessageBufferRelease(&self->msg);
}

void SmeHelloStateReadyListen(ShipConnection* self) {
  const EebusError error = ShipConnectionReceive(self, &self->msg, tHelloInit);

  switch (error) {
    case kEebusErrorOk: {
      SmeHelloReadyStateEvaluateReceivedHelloMessage(self);
      break;
    }

    case kEebusErrorTime: {
      ShipConnectionSetSmeState(self, kSmeHelloStateReadyTimeout);
      break;
    }

    default: {
      ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
      break;
    }
  }
}

void SmeHelloStatePendingInit(ShipConnection* self) {
  // After sending the hello update message, the SHIP user waits on the update
  // message in SmeHelloPendingListen state. The wait for ready timer is set
  // there.
  EEBUS_TIMER_STOP(self->send_prolongation_request_timer);
  EEBUS_TIMER_STOP(self->prolongation_request_reply_timer);
  if (SmeHelloStateSendHelloMsg(self, kConnectionHelloPhasePending, tHelloInit, false) == kEebusErrorOk) {
    ShipConnectionSetSmeState(self, kSmeHelloStatePendingListen);
  } else {
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
  }
}

void SmeHelloCalculateNewWaitValueAndSetTimer(ShipConnection* self, const ConnectionHello* msg) {
  if (*msg->waiting >= tHelloProlongThrInc) {
    uint32_t new_wait_duration = *msg->waiting - tHelloProlongWaitingGap;

    if (new_wait_duration >= tHelloProlongMin) {
      EEBUS_TIMER_START(self->send_prolongation_request_timer, new_wait_duration, false);
    } else {
      EEBUS_TIMER_STOP(self->send_prolongation_request_timer);
    }
  } else {
    EEBUS_TIMER_STOP(self->send_prolongation_request_timer);
  }
}

void SmeHelloStateCheckWaitAndProlongationRequestSubelements(ShipConnection* self, const ConnectionHello* msg) {
  if ((msg->waiting != NULL) && (msg->prolongation_request == NULL)) {
    EEBUS_TIMER_STOP(self->prolongation_request_reply_timer);
    SmeHelloCalculateNewWaitValueAndSetTimer(self, msg);
  } else {
    SmeHelloStateCommonProlongationRequestAndUpdateMessageSendProcedure(self, msg);
  }
}

void SmeHelloStateCheckWaitingSubelement(ShipConnection* self, const ConnectionHello* msg) {
  if (msg->waiting != NULL) {
    EEBUS_TIMER_STOP(self->wait_for_ready_timer);
    EEBUS_TIMER_STOP(self->prolongation_request_reply_timer);
    SmeHelloCalculateNewWaitValueAndSetTimer(self, msg);
  } else {
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
  }
}

void SmeHelloPendingStateEvaluateReceivedHelloMessage(ShipConnection* self) {
  ShipMessageDeserializeObject* const deserialize = ShipMessageDeserializeCreate(&self->msg);

  if (SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize) != kSmeHello) {
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
    ShipMessageDeserializeDelete(deserialize);
    MessageBufferRelease(&self->msg);
    return;
  }

  const ConnectionHello* const sme_hello = (ConnectionHello*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);

  if (sme_hello != NULL) {
    switch (sme_hello->phase) {
      case kConnectionHelloPhaseReady: {
        SmeHelloStateCheckWaitingSubelement(self, sme_hello);
        break;
      }

      case kConnectionHelloPhasePending: {
        SmeHelloStateCheckWaitAndProlongationRequestSubelements(self, sme_hello);
        break;
      }

      case kConnectionHelloPhaseAborted: {
        ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
        break;
      }

      default: {
        ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
        break;
      }
    }
  } else {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), Error deserializing SME hello message\n", __func__);
    SmeHelloStateAbort(self);
  }

  ShipMessageDeserializeDelete(deserialize);
  MessageBufferRelease(&self->msg);
}

void SmeHelloStatePendingListen(ShipConnection* self) {
  const EebusError error = ShipConnectionReceive(self, &self->msg, tHelloInit);

  switch (error) {
    case kEebusErrorOk: {
      SmeHelloPendingStateEvaluateReceivedHelloMessage(self);
      break;
    }

    case kEebusErrorTime: {
      ShipConnectionSetSmeState(self, kSmeHelloStatePendingTimeout);
      break;
    }

    default: {
      ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
      break;
    }
  }
}

void SmeHelloStatePendingTimeout(ShipConnection* self) {
  if ((EEBUS_TIMER_GET_TIMER_STATE(self->wait_for_ready_timer) == kEebusTimerStateExpired)
      || (EEBUS_TIMER_GET_TIMER_STATE(self->prolongation_request_reply_timer) == kEebusTimerStateExpired)) {
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
    return;
  }

  if (EEBUS_TIMER_GET_TIMER_STATE(self->send_prolongation_request_timer) == kEebusTimerStateExpired) {
    if (SmeHelloStateSendHelloMsg(self, kConnectionHelloPhasePending, 0, true) != kEebusErrorOk) {
      ShipConnectionCloseWithError(self, "Error sending hello message");
      return;
    }

    if (self->last_received_waiting_value == 0) {
      self->last_received_waiting_value
          = (uint32_t)(EEBUS_TIMER_GET_REMAINING_TIME(self->prolongation_request_reply_timer) * 1.1);
    }

    EEBUS_TIMER_START(self->prolongation_request_reply_timer, self->last_received_waiting_value, false);

    ShipConnectionSetSmeState(self, kSmeHelloStatePendingListen);
  }
}

void SmeHelloStateAbort(ShipConnection* self) {
  const EebusError error = SmeHelloStateSendHelloMsg(self, kConnectionHelloPhaseAborted, 0, false);
  if (error != kEebusErrorOk) {
    SHIP_CONNECTION_DEBUG_PRINTF("Error sending hello state abort message\n");
  }

  ShipConnectionCloseWithError(self, "SME Hello state connection aborted");
}

void SmeProtHandshakeStateAbort(ShipConnection* self, MessageProtocolHandshakeErrorType error) {
  EEBUS_TIMER_STOP(self->wait_for_ready_timer);
  MessageProtocolHandshakeError sme_prot_hs_err = {.error = error};

  const EebusError ret = ShipConnectionSerializeAndSendMessage(self, &sme_prot_hs_err, kSmeProtocolHandshakeError);

  if (ret != kEebusErrorOk) {
    ShipConnectionCloseWithError(self, "Error sending ship message");
    return;
  }

  ShipConnectionSetSmeState(self, kSmeStateError);
  ShipConnectionCloseWithError(self, "Abort protocol handshake");
}

EebusError SmePinStateCommonPinRequirementSendProcedure(
    ShipConnection* self,
    PinStateType node_pin_state,
    const PinInputPermissionType* node_input_permission
) {
  ConnectionPinState sme_pin_state = {
      .pin_state = node_pin_state,
  };

  if ((node_pin_state == kPinStateTypeNone) || (node_pin_state == kPinStateTypePinOk)) {
    sme_pin_state.input_permission = NULL;
  } else {
    if (node_input_permission != NULL) {
      sme_pin_state.input_permission  = &sme_pin_state.input_permission_val;
      *sme_pin_state.input_permission = *node_input_permission;
    } else {
      return kEebusErrorInputArgument;
    }
  }

  const EebusError ret = ShipConnectionSerializeAndSendMessage(self, &sme_pin_state, kSmeConnectionPinState);

  return ret;
}

void SmePinStateCommonConnectionDataExchangeEnableProcedure(ShipConnection* self, PinStateType node_pin_state) {
  UNUSED(self);
  UNUSED(node_pin_state);
}

void SmePinStateHandlePermissionType(ShipConnection* self, const PinInputPermissionType* node_pin_requirement) {
  if (node_pin_requirement == NULL) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(): node_pin_requirement is NULL\n", __func__);
    ShipConnectionCloseWithError(self, "node_pin_requirement is missing");
    return;
  }

  switch (*node_pin_requirement) {
    case kPinInputPermissionTypeBusy: {
      ShipConnectionSetSmeState(self, kSmePinStateCheckBusyWait);
      break;
    }
    case kPinInputPermissionTypeOk: {
      ShipConnectionSetSmeState(self, kSmePinStateCheckListen);
      break;
    }
    default: {
      break;
    }
  }
}

void SmePinStateCheckPinStateAndRequirement(
    ShipConnection* self,
    PinStateType node_pin_state,
    const PinInputPermissionType* node_pin_requirement
) {
  switch (node_pin_state) {
    case kPinStateTypeRequired: {
      SmePinStateHandlePermissionType(self, node_pin_requirement);
      break;
    }

    case kPinStateTypeOptional: {
      SmePinStateCommonConnectionDataExchangeEnableProcedure(self, node_pin_state);
      SmePinStateHandlePermissionType(self, node_pin_requirement);
      break;
    }

    case kPinStateTypePinOk:
    case kPinStateTypeNone: {
      ShipConnectionSetSmeState(self, kSmePinStateCheckOk);
      break;
    }

    default: {
      break;
    }
  }
}

void SmePinStateCheckInit(ShipConnection* self) {
  PinStateType node_pin_state = kPinStateTypeNone;

  PinInputPermissionType node_pin_requirement = kPinInputPermissionTypeOk;
  if (SmePinStateCommonPinRequirementSendProcedure(self, node_pin_state, &node_pin_requirement) != kEebusErrorOk) {
    ShipConnectionCloseWithError(self, "Error sending PIN requirement message");
    return;
  }

  if (ShipConnectionReceive(self, &self->msg, cmiTimeout) != kEebusErrorOk) {
    ShipConnectionCloseWithError(self, "Error receiving pin requirement");
    return;
  }

  ShipMessageDeserializeObject* const deserialize = ShipMessageDeserializeCreate(&self->msg);
  if (SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize) != kSmeConnectionPinState) {
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
    ShipMessageDeserializeDelete(deserialize);
    MessageBufferRelease(&self->msg);
    return;
  }

  const ConnectionPinState* const pin_state = (ConnectionPinState*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  if (pin_state != NULL) {
    SmePinStateCheckPinStateAndRequirement(self, pin_state->pin_state, pin_state->input_permission);
  } else {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), Error deserializing PIN init message\n", __func__);
    ShipConnectionSetSmeState(self, kSmeHelloStateAbort);
  }

  ShipMessageDeserializeDelete(deserialize);
  MessageBufferRelease(&self->msg);
}

void SmePinStateCheckListen(ShipConnection* self) {
  const EebusError error = ShipConnectionReceive(self, &self->msg, tHelloInit);

  if (error != kEebusErrorOk) {
    ShipConnectionCloseWithError(self, "SmePinStateCheckListen: Error receiving SHIP message");
    return;
  }

  ShipMessageDeserializeObject* const deserialize = ShipMessageDeserializeCreate(&self->msg);

  if (SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize) != kSmeConnectionPinState) {
    ShipConnectionCloseWithError(self, "Error deserializing PIN message");
    ShipMessageDeserializeDelete(deserialize);
    MessageBufferRelease(&self->msg);
    return;
  }

  const ConnectionPinState* const sme_pin_state = (ConnectionPinState*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);

  if (sme_pin_state != NULL) {
    switch (sme_pin_state->pin_state) {
      case kPinStateTypeNone: {
        ShipConnectionSetSmeState(self, kSmePinStateCheckOk);
        break;
      }

      case kPinStateTypeRequired: {
        ShipConnectionCloseWithError(self, "Unsupported PIN state: required");
        break;
      }

      case kPinStateTypeOptional: {
        ShipConnectionCloseWithError(self, "Unsupported PIN state: optional");
        break;
      }

      case kPinStateTypePinOk: {
        ShipConnectionCloseWithError(self, "Unsupported PIN state: ok");
        break;
      }

      default: {
        ShipConnectionCloseWithError(self, "Invalid PIN state");
        break;
      }
    }
  } else {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), Error deserializing PIN message\n", __func__);
    ShipConnectionCloseWithError(self, "Error deserializing PIN message");
  }

  ShipMessageDeserializeDelete(deserialize);
  MessageBufferRelease(&self->msg);
}

void SmeHandshakeAccessMethodsInit(ShipConnection* self) {
  const AccessMethodsRequest access_methods_request_msg = {0};
  const EebusError error
      = ShipConnectionSerializeAndSendMessage(self, &access_methods_request_msg, kSmeConnectionAccessMethodsRequest);

  if (error != kEebusErrorOk) {
    ShipConnectionCloseWithError(
        self,
        "SmeAccessMethodsRequest: Error sending "
        "kSmeConnectionAccessMethodsRequest message"
    );
  } else {
    EEBUS_TIMER_START(self->wait_for_ready_timer, cmiTimeout, false);
    // Access Methods is now a kind of Data Exchange substate
    // so no switch state is required
  }
}

EebusError SmeHandshakeAccessMethodsRequestHandle(ShipConnection* self) {
  const AccessMethods access_methods_msg = {.id = self->local_ship_id};
  const EebusError error
      = ShipConnectionSerializeAndSendMessage(self, &access_methods_msg, kSmeConnectionAccessMethods);

  if (error != kEebusErrorOk) {
    SHIP_CONNECTION_DEBUG_PRINTF("SmeAccessMethodsRequest: Error sending kSmeConnectionAccessMethods message");
  }

  return error;
}

bool SmeHandshakeAccessMethodsCheckMessageVal(ShipConnection* self, ShipMessageDeserializeObject* deserialize) {
  const AccessMethods* access_msg_val = (AccessMethods*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);

  if (access_msg_val == NULL) {
    SHIP_CONNECTION_DEBUG_PRINTF("kSmeAccessMethodsRequest: Error deserializing msg value");
    return false;
  }

  if (access_msg_val->id == NULL) {
    SHIP_CONNECTION_DEBUG_PRINTF("kSmeAccessMethodsRequest: Access methods response does not contain SHIP ID");
    return false;
  }

  if (!StringIsEmpty(self->remote_ship_id)) {
    const size_t remote_ship_id_len = strlen(self->remote_ship_id);
    if (!StringNCompare(self->remote_ship_id, access_msg_val->id, remote_ship_id_len)) {
      SHIP_CONNECTION_DEBUG_PRINTF("Saved remote id: %s, Received id: %s\n", self->remote_ship_id, access_msg_val->id);
      return false;
    }
  } else {
    StringDelete((char*)self->remote_ship_id);
    self->remote_ship_id = StringCopy(access_msg_val->id);
    SHIP_CONNECTION_DEBUG_PRINTF("Saved remote SHIP id: %s\n", self->remote_ship_id);
  }

  return true;
}

EebusError SmeHandshakeAccessMethodsHandle(ShipConnection* self, ShipMessageDeserializeObject* deserialize) {
  EEBUS_TIMER_STOP(self->wait_for_ready_timer);

  if (!SmeHandshakeAccessMethodsCheckMessageVal(self, deserialize)) {
    return kEebusErrorCommunication;
  }

  // remote_ship_id is populated by SmeHandshakeAccessMethodsCheckMessageVal above.
  // Report it now so info-provider clients can display the remote device name.
  INFO_PROVIDER_REPORT_SERVICE_SHIP_ID(self->info_provider, self->remote_ski, self->remote_ship_id);
  return kEebusErrorOk;
}

void SmeStateApproved(ShipConnection* self) {
  self->data_reader
      = INFO_PROVIDER_SETUP_REMOTE_DEVICE(self->info_provider, self->remote_ski, DATA_WRITER_OBJECT(self));
  EEBUS_TIMER_STOP(self->wait_for_ready_timer);
  ShipConnectionSetSmeState(self, kDataExchange);
}

EebusError DataExchangeHandleClose(ShipConnection* self, const ConnectionClose* sme_close) {
  if (sme_close->phase == kConnectionClosePhaseTypeAnnounce) {
    // SHIP 13.4.7: Connection Termination Confirm
    const ConnectionClose sme_close_confirm = {
        .phase = kConnectionClosePhaseTypeConfirm,
    };

    ShipConnectionSerializeAndSendMessage(self, &sme_close_confirm, kSmeClose);
    EebusThreadUsleep(500000);  // Wait 0.5s for the message to be sent
    WEBSOCKET_CLOSE(self->websocket, 4001, "close");
  } else if (sme_close->phase == kConnectionClosePhaseTypeConfirm) {
    // We got a confirmation so close this connection
    WEBSOCKET_CLOSE(self->websocket, 4001, "close");
  }

  // Mirror what CloseConnection does after WEBSOCKET_CLOSE: stop timers, set shutdown_once
  // so that the subsequent CloseConnection call from Stop() is a no-op and does not send a
  // duplicate kShipConnectionClosed message with a dangling sc pointer.
  EEBUS_TIMER_STOP(self->wait_for_ready_timer);
  EEBUS_TIMER_STOP(self->send_prolongation_request_timer);
  EEBUS_TIMER_STOP(self->prolongation_request_reply_timer);
  self->cancel        = true;
  self->shutdown_once = true;
  INFO_PROVIDER_HANDLE_CONNECTION_CLOSED(self->info_provider, SHIP_CONNECTION_OBJECT(self), true);
  return kEebusErrorOk;
}

EebusError DataExchangeHandleReceive(ShipConnection* self, const MessageBuffer* buf) {
  ShipMessageDeserializeObject* const deserialize = ShipMessageDeserializeCreate((MessageBuffer*)buf);

  if (deserialize == NULL) {
    return kEebusErrorParse;
  }

  const MsgValueType message_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EebusError ret = kEebusErrorOk;
  if (message_type == kData) {
    Data* const data = (Data*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);

    if (data != NULL) {
      // Pass the payload to the SPINE read handler
      SHIP_CONNECTION_DEBUG_PRINTF("Recv:    %s\n", (const char*)data->payload.data);
      DATA_READER_HANDLE_MESSAGE(self->data_reader, &data->payload);
      ret = kEebusErrorOk;
    } else {
      SHIP_CONNECTION_DEBUG_PRINTF("Error deserializing message value\n");
      ret = kEebusErrorParse;
    }

  } else if (message_type == kSmeConnectionAccessMethodsRequest) {
    ret = SmeHandshakeAccessMethodsRequestHandle(self);
  } else if (message_type == kSmeConnectionAccessMethods) {
    ret = SmeHandshakeAccessMethodsHandle(self, deserialize);
  } else if (message_type == kSmeClose) {
    const ConnectionClose* sme_close = (const ConnectionClose*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);

    if (sme_close != NULL) {
      ret = DataExchangeHandleClose(self, sme_close);
    } else {
      SHIP_CONNECTION_DEBUG_PRINTF("Error deserializing kSmeClose message value\n");
      ret = kEebusErrorParse;
    }
  } else {
    ret = kEebusErrorCommunication;
  }

  ShipMessageDeserializeDelete(deserialize);
  return ret;
}

EebusError DataExchangeHandleSendSpineData(ShipConnection* self, const MessageBuffer* buf) {
  SHIP_CONNECTION_DEBUG_PRINTF("Send:    %s\n", (const char*)buf->data);
  const Data data = {
      .header = {
          .protocol_id = SHIP_PROTOCOL_ID,
      },

      .payload   = *buf,
      .extension = NULL,
  };

  return ShipConnectionSerializeAndSendMessage(self, &data, kData);
}

EebusError DataExchangeHandle(ShipConnection* self) {
  ShipConnectionQueueMessage queue_msg;
  const EebusError queue_recv_ret = EEBUS_QUEUE_RECEIVE(self->msg_queue, &queue_msg, kTimeoutInfinite);

  if (queue_recv_ret != kEebusErrorOk) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), error receiving the message from queue\n", __func__);
    return queue_recv_ret;
  }

  if (queue_msg.type == kShipConnectionQueueMsgTypeDataReceived) {
    const EebusError ret = DataExchangeHandleReceive(self, &queue_msg.msg_buf);
    MessageBufferRelease(&queue_msg.msg_buf);
    return ret;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeSpineDataToSend) {
    const EebusError ret = DataExchangeHandleSendSpineData(self, &queue_msg.msg_buf);
    MessageBufferRelease(&queue_msg.msg_buf);
    return ret;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeCancel) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), cancelled\n", __func__);
    return kEebusErrorDeactivate;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeTimeout) {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), timed out\n", __func__);
    return kEebusErrorCommunication;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeWebsocketClose) {
    SHIP_CONNECTION_CLOSE_CONNECTION(self, true, 0, NULL);
    return kEebusErrorCommunicationEnd;
  } else if (queue_msg.type == kShipConnectionQueueMsgTypeWebsocketError) {
    return kEebusErrorCommunication;
  } else {
    SHIP_CONNECTION_DEBUG_PRINTF("%s(), invalid queue message type\n", __func__);
    return kEebusErrorInputType;
  }
}

void DataExchange(ShipConnection* self) {
  if (!self->is_access_methods_req_sent) {
    SmeHandshakeAccessMethodsInit(self);
    self->is_access_methods_req_sent = true;
  }

  if (DataExchangeHandle(self) != kEebusErrorOk) {
    ShipConnectionCloseWithError(self, "kDataExchange: data exchange failed");
  }
}
