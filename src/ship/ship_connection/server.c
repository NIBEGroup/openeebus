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
 * @brief SHIP server state handling function definitions
 */

#include "src/ship/ship_connection/server.h"

#include "src/common/array_util.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/ship_connection/ship_message_deserialize.h"
#include "src/ship/ship_connection/ship_message_serialize.h"

void ServerHandleState(ShipConnection* self) {
    switch (self->sme_state) {
        case kCmiStateServerWait: {
            CmiStateServerWait(self);
            break;
        }

        case kCmiStateServerEvaluate: {
            CmiStateServerEvaluate(self);
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
            ShipConnectionSetSmeState(self, kSmeProtHStateServerInit);
            break;
        }

        case kSmeHelloStateAbort:
        case kSmeHelloStateAbortDone:
        case kSmeHelloStateRemoteAbortDone:
        case kSmeHelloStateRejected: {
            SmeHelloStateAbort(self);
            break;
        }

        case kSmeProtHStateServerInit: {
            SmeProtHandshakeStateServerInit(self);
            break;
        }

        case kSmeProtHStateServerListenProposal: {
            SmeProtHandshakeStateServerListenProposal(self);
            break;
        }

        case kSmeProtHStateServerListenConfirm: {
            SmeProtHandshakeStateServerListenConfirm(self);
            break;
        }

        case kSmeProtHStateServerOk: {
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

void CmiStateServerWait(ShipConnection* self) {
    if (ShipConnectionReceive(self, &self->msg, cmiTimeout) == kEebusErrorOk) {
        ShipConnectionSetSmeState(self, kCmiStateServerEvaluate);
    } else {
        ShipConnectionCloseWithError(self, "CMI server wait failed");
    }
}

void CmiStateServerEvaluate(ShipConnection* self) {
    MessageBuffer buf = {0};
    MessageBufferInitWithDeallocator(&buf, (uint8_t*)kShipInitMessage, ARRAY_SIZE(kShipInitMessage), NULL);

    if (!ShipConnectionEvaluateInitMsg(&self->msg)) {
        MessageBufferRelease(&self->msg);
        ShipConnectionCloseWithError(self, "Invalid init message received");
        return;
    }

    if (ShipConnectionSend(self, &buf) != kEebusErrorOk) {
        MessageBufferRelease(&self->msg);
        ShipConnectionCloseWithError(self, "Server CMI message send failed");
        return;
    }

    ShipConnectionSetSmeState(self, kSmeHelloState);
    MessageBufferRelease(&self->msg);
}

void SmeProtHandshakeStateServerInit(ShipConnection* self) {
    EEBUS_TIMER_STOP(self->wait_for_ready_timer);
    // The Wait-For-Ready timer is set to default timeout value in the next state
    ShipConnectionSetSmeState(self, kSmeProtHStateServerListenProposal);
}

EebusError SmeProtHandshakeStateSendAgreementMessage(ShipConnection* self, uint8_t major, uint8_t minor) {
    const MessageProtocolHandshake sme_prot_hs_send = {
      .handshake_type = kProtocolHandshakeTypeSelect,
      .version = {
          .major = major,
          .minor = minor,
      },

      .formats = {
          .format      = {kMessageProtocolFormatTypeUTF8},
          .format_size = 1,
      },
  };

    return ShipConnectionSerializeAndSendMessage(self, &sme_prot_hs_send, kSmeProtocolHandshake);
}

bool SmeProtHandshakeStateAgreeOnProtocolVersion(
    const MessageProtocolHandshake* sme_prot_hs,
    uint8_t* agreed_major,
    uint8_t* agreed_minor
) {
    if ((sme_prot_hs->version.major <= SHIP_PROTOCOL_MAX_SUPPORTED_MAJOR_VERSION)
        && (sme_prot_hs->version.minor <= SHIP_PROTOCOL_MAX_SUPPORTED_MINOR_VERSION)) {
        *agreed_major = sme_prot_hs->version.major;
        *agreed_minor = sme_prot_hs->version.minor;
        return true;
    }

    return false;
}

void SmeProtHandshakeStateServerListenProposal(ShipConnection* self) {
    if (ShipConnectionReceive(self, &self->msg, cmiTimeout) != kEebusErrorOk) {
        SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeUnexpectedMessage);
        return;
    }

    ShipMessageDeserializeObject* const deserialize = ShipMessageDeserializeCreate(&self->msg);

    const MsgValueType msg_value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);
    const MessageProtocolHandshake* const msg_value
        = (MessageProtocolHandshake*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);

    if ((msg_value_type == kSmeProtocolHandshake) && (msg_value != NULL)) {
        EEBUS_TIMER_STOP(self->wait_for_ready_timer);

        if (msg_value->handshake_type != kProtocolHandshakeTypeAnnounceMax) {
            ShipMessageDeserializeDelete(deserialize);
            MessageBufferRelease(&self->msg);
            SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeUnexpectedMessage);
            return;
        }

        uint8_t agreed_version_major = SHIP_PROTOCOL_MAX_SUPPORTED_MAJOR_VERSION;
        uint8_t agreed_version_minor = SHIP_PROTOCOL_MAX_SUPPORTED_MINOR_VERSION;

        if (!SmeProtHandshakeStateAgreeOnProtocolVersion(msg_value, &agreed_version_major, &agreed_version_minor)) {
            ShipMessageDeserializeDelete(deserialize);
            MessageBufferRelease(&self->msg);
            SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeSelectionMismatch);
            return;
        }

        if (SmeProtHandshakeStateSendAgreementMessage(self, agreed_version_major, agreed_version_minor)
            != kEebusErrorOk) {
            ShipMessageDeserializeDelete(deserialize);
            MessageBufferRelease(&self->msg);
            ShipConnectionCloseWithError(self, "Error serializing protocol handshake ship message");
            return;
        }

        // The Wait-For-Ready timer is set to default timeout value in the next state, during message receive
        ShipConnectionSetSmeState(self, kSmeProtHStateServerListenConfirm);
    } else {
        SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeUnexpectedMessage);
    }

    ShipMessageDeserializeDelete(deserialize);
    MessageBufferRelease(&self->msg);
}

void SmeProtHandshakeStateServerListenConfirm(ShipConnection* self) {
    if (ShipConnectionReceive(self, &self->msg, cmiTimeout) != kEebusErrorOk) {
        SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeUnexpectedMessage);
        return;
    }

    ShipMessageDeserializeObject* const deserialize = ShipMessageDeserializeCreate(&self->msg);

    const MsgValueType msg_value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);
    const MessageProtocolHandshake* const sme_prot_hs
        = (MessageProtocolHandshake*)SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);

    if ((msg_value_type == kSmeProtocolHandshake) && (sme_prot_hs != NULL)) {
        if (sme_prot_hs->handshake_type != kProtocolHandshakeTypeSelect) {
            SHIP_CONNECTION_DEBUG_PRINTF("Invalid protocol handshake response: kProtocolHandshakeTypeSelect\n");
            ShipMessageDeserializeDelete(deserialize);
            MessageBufferRelease(&self->msg);
            SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeSelectionMismatch);
            return;
        }

        EEBUS_TIMER_STOP(self->wait_for_ready_timer);
        ShipConnectionSetSmeState(self, kSmeProtHStateServerOk);
    } else {
        SmeProtHandshakeStateAbort(self, kMessageProtocolHandshakeErrorTypeSelectionMismatch);
    }

    ShipMessageDeserializeDelete(deserialize);
    MessageBufferRelease(&self->msg);
}
