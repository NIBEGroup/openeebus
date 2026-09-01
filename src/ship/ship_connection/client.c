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
 * @brief SHIP client state handling function definitions
 */
#include "src/ship/ship_connection/client.h"

#include "src/common/array_util.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/ship_connection/ship_message_deserialize.h"
#include "src/ship/ship_connection/ship_message_serialize.h"

void ClientHandleState(ShipConnection* self) {
    switch (self->sme_state) {
        case kCmiStateClientSend: {
            CmiStateClientSend(self);
            break;
        }

        case kCmiStateClientWait: {
            CmiStateClientWait(self);
            break;
        }

        case kCmiStateClientEvaluate: {
            CmiStateClientEvaluate(self);
            break;
        }

        case kSmeHelloState: {
            ShipConnectionSetSmeState(self, kSmeHelloStateReadyInit);
            break;
        }

        case kSmeHelloStateReadyInit: {
            SmeHelloStateReadyInit(self);
            break;
        }

        case kSmeHelloStateReadyListen: {
            SmeHelloStateReadyListen(self);
            break;
        }

        case kSmeHelloStateReadyTimeout: {
            SmeHelloStateAbort(self);
            break;
        }

        case kSmeHelloStatePendingInit: {
            SmeHelloStatePendingInit(self);
            break;
        }

        case kSmeHelloStatePendingListen: {
            SmeHelloStatePendingListen(self);
            break;
        }

        case kSmeHelloStatePendingTimeout: {
            SmeHelloStatePendingTimeout(self);
            break;
        }

        case kSmeHelloStateOk: {
            ShipConnectionSetSmeState(self, kSmeProtHStateClientInit);
            break;
        }

        case kSmeHelloStateAbort:
        case kSmeHelloStateAbortDone:
        case kSmeHelloStateRemoteAbortDone:
        case kSmeHelloStateRejected: {
            SmeHelloStateAbort(self);
            break;
        }

        case kSmeProtHStateClientInit: {
            SmeProtHandshakeStateClientInit(self);
            break;
        }

        case kSmeProtHStateClientListenChoice: {
            SmeProtHandshakeStateClientListenChoice(self);
            break;
        }

        case kSmeProtHStateClientOk: {
            ShipConnectionSetSmeState(self, kSmePinStateCheckInit);
            break;
        }

        case kSmePinStateCheckInit: {
            SmePinStateCheckInit(self);
            break;
        }

        case kSmePinStateCheckListen: {
            SmePinStateCheckListen(self);
            break;
        }

        case kSmePinStateCheckBusyWait: {
            ShipConnectionSetSmeState(self, kSmePinStateCheckListen);
            break;
        }

        case kSmePinStateCheckOk: {
            ShipConnectionSetSmeState(self, kSmeStateApproved);
            break;
        }

        case kSmeAccessMethodsRequest: {
            // Access Methods Request is done as Data Exchange substate
            break;
        }

        case kSmeStateApproved: {
            SmeStateApproved(self);
            break;
        }

        case kDataExchange: {
            DataExchange(self);
            break;
        }

        case kSmeStateError: {
            break;
        }

        default: {
            EebusThreadSleep(10);
            break;
        }
    }
}

void CmiStateClientSend(ShipConnection* self) {
    MessageBuffer buf = {0};
    MessageBufferInitWithDeallocator(&buf, (uint8_t*)kShipInitMessage, ARRAY_SIZE(kShipInitMessage), NULL);

    if (ShipConnectionSend(self, &buf) == kEebusErrorOk) {
        ShipConnectionSetSmeState(self, kCmiStateClientWait);
    } else {
        ShipConnectionCloseWithError(self, "CMI client send failed");
    }

    MessageBufferRelease(&buf);
}

void CmiStateClientWait(ShipConnection* self) {
    if (ShipConnectionReceive(self, &self->msg, cmiTimeout) == kEebusErrorOk) {
        ShipConnectionSetSmeState(self, kCmiStateClientEvaluate);
    } else {
        ShipConnectionCloseWithError(self, "CMI client wait failed");
    }
}

void CmiStateClientEvaluate(ShipConnection* self) {
    if (ShipConnectionEvaluateInitMsg(&self->msg)) {
        ShipConnectionSetSmeState(self, kSmeHelloState);
    } else {
        ShipConnectionCloseWithError(self, "Invalid init message received");
    }

    MessageBufferRelease(&self->msg);
}

void SmeProtHandshakeStateClientInit(ShipConnection* self) {
    EEBUS_TIMER_STOP(self->wait_for_ready_timer);

    const MessageProtocolHandshake sme_prot_hs = {
      .handshake_type = kProtocolHandshakeTypeAnnounceMax,
      .version= {
          .major = SHIP_PROTOCOL_MAX_SUPPORTED_MAJOR_VERSION, 
          .minor = SHIP_PROTOCOL_MAX_SUPPORTED_MINOR_VERSION,
      },

      .formats = {
          .format      = {kMessageProtocolFormatTypeUTF8},
          .format_size = 1,
      },
    };

    if (ShipConnectionSerializeAndSendMessage(self, &sme_prot_hs, kSmeProtocolHandshake) != kEebusErrorOk) {
        ShipConnectionCloseWithError(self, "Error serializing protocol handshake ship message");
        return;
    }

    // The Wait-For-Ready timer is set to default timeout value in the next state, during message receive
    ShipConnectionSetSmeState(self, kSmeProtHStateClientListenChoice);
}

bool SmeProtHandshakeStateMessageCheck(const MessageProtocolHandshake* sme_prot_hs) {
    if (sme_prot_hs->handshake_type != kProtocolHandshakeTypeSelect) {
        SHIP_CONNECTION_DEBUG_PRINTF("Invalid protocol handshake response\n");
        return false;
    }

    if (sme_prot_hs->version.major != SHIP_PROTOCOL_MAX_SUPPORTED_MAJOR_VERSION) {
        SHIP_CONNECTION_DEBUG_PRINTF("Unsupported protocol major version: %d\n", sme_prot_hs->version.major);
        return false;
    }

    if (sme_prot_hs->version.minor != SHIP_PROTOCOL_MAX_SUPPORTED_MINOR_VERSION) {
        SHIP_CONNECTION_DEBUG_PRINTF("Unsupported protocol minor version: %d\n", sme_prot_hs->version.minor);
        return false;
    }

    if (sme_prot_hs->formats.format_size == 0) {
        SHIP_CONNECTION_DEBUG_PRINTF("Format is missing\n");
        return false;
    }

    if (sme_prot_hs->formats.format[0] != kMessageProtocolFormatTypeUTF8) {
        SHIP_CONNECTION_DEBUG_PRINTF("Unsupported format\n");
        return false;
    }

    return true;
}

EebusError SmeProtHandshakeStateSendMaximumSupportedShipVersion(ShipConnection* self) {
    const MessageProtocolHandshake sme_prot_hs = {
      .handshake_type = kProtocolHandshakeTypeSelect,
      .version= {
          .major = SHIP_PROTOCOL_MAX_SUPPORTED_MAJOR_VERSION, 
          .minor = SHIP_PROTOCOL_MAX_SUPPORTED_MINOR_VERSION,
      },

      .formats = {
          .format      = {kMessageProtocolFormatTypeUTF8},
          .format_size = 1,
      },
  };

    return ShipConnectionSerializeAndSendMessage(self, &sme_prot_hs, kSmeProtocolHandshake);
}

void SmeProtHandshakeStateClientListenChoice(ShipConnection* self) {
    if (ShipConnectionReceive(self, &self->msg, cmiTimeout) != kEebusErrorOk) {
        SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeUnexpectedMessage);
        return;
    }

    ShipMessageDeserializeObject* const deserialize = ShipMessageDeserializeCreate(&self->msg);
    const MessageProtocolHandshake* const sme_prot_hs
        = (MessageProtocolHandshake*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
    const MsgValueType msg_value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

    if ((msg_value_type == kSmeProtocolHandshake) && (sme_prot_hs != NULL)) {
        EEBUS_TIMER_STOP(self->wait_for_ready_timer);

        if (!SmeProtHandshakeStateMessageCheck(sme_prot_hs)) {
            ShipMessageDeserializeDelete(deserialize);
            MessageBufferRelease(&self->msg);
            SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeSelectionMismatch);
            return;
        }

        if (SmeProtHandshakeStateSendMaximumSupportedShipVersion(self) != kEebusErrorOk) {
            ShipMessageDeserializeDelete(deserialize);
            MessageBufferRelease(&self->msg);
            ShipConnectionCloseWithError(self, "Error serializing protocol handshake ship message");
            return;
        }

        ShipConnectionSetSmeState(self, kSmeProtHStateClientOk);
    } else {
        SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeUnexpectedMessage);
    }

    ShipMessageDeserializeDelete(deserialize);
    MessageBufferRelease(&self->msg);
}
