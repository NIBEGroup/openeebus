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
 * @brief Transformed model/model.go
 */

#ifndef SRC_SHIP_MODEL_MODEL_H_
#define SRC_SHIP_MODEL_MODEL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/common/message_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum MsgType {
    kMsgTypeInit    = 0,
    kMsgTypeControl = 1,
    kMsgTypeData    = 2,
    kMsgTypeEnd     = 3,
};

typedef enum MsgType MsgType;

enum MsgValueType {
    kValueUndefined = -1,
    kSmeHello       = 0,
    kSmeProtocolHandshake,
    kSmeProtocolHandshakeError,
    kSmeConnectionPinState,
    kSmeConnectionPinInput,
    kSmeConnectionPinError,
    kData,
    kSmeConnectionAccessMethodsRequest,
    kSmeConnectionAccessMethods,
    kSmeClose,
};

typedef enum MsgValueType MsgValueType;

#define SHIP_PROTOCOL_ID "ee1.0"
#define SHIP_PROTOCOL_MAX_SUPPORTED_MAJOR_VERSION 1
#define SHIP_PROTOCOL_MAX_SUPPORTED_MINOR_VERSION 0
#define SKI_SIZE 40

enum ConnectionHelloPhase {
    kConnectionHelloPhasePending = 0,  // "pending"
    kConnectionHelloPhaseReady   = 1,  // "ready"
    kConnectionHelloPhaseAborted = 2,  // "aborted"
};

typedef enum ConnectionHelloPhase ConnectionHelloPhase;

typedef struct {
    ConnectionHelloPhase phase;     // `json:"phase"`
    uint32_t waiting_val;           // Buffer to be addressed with waiting
    uint32_t* waiting;              // `json:"waiting,omitempty"`
    bool prolongation_request_val;  // Buffer to be addressed with prolongation_request
    bool* prolongation_request;     // `json:"prolongationRequest,omitempty"`
} ConnectionHello;                  // `json:"connectionHello"`

enum MessageProtocolFormatType {
    kMessageProtocolFormatTypeUTF8  = 0,  // "JSON-UTF8"
    kMessageProtocolFormatTypeUTF16 = 1,  // "JSON-UTF16"
};

typedef enum MessageProtocolFormatType MessageProtocolFormatType;

typedef struct {
    // 13.4.4.2.1, "Permitted values for the child element "format"
    // are "JSON-UTF8" and "JSON-UTF16""
    // so currently limit formats array to contain 2 elements
    MessageProtocolFormatType format[2];  // `json:"format"`
    size_t format_size;                   // Actual number of formats used
} MessageProtocolFormats;

enum ProtocolHandshakeType {
    kProtocolHandshakeTypeAnnounceMax = 0,  // "announceMax"
    kProtocolHandshakeTypeSelect      = 1,  // "select"
};

typedef enum ProtocolHandshakeType ProtocolHandshakeType;

typedef struct {
    uint8_t major;  // `json:"major"`
    uint8_t minor;  // `json:"minor"`
} Version;

typedef struct {
    ProtocolHandshakeType handshake_type;  // `json:"handshakeType"`
    Version version;                       // `json:"version"`
    MessageProtocolFormats formats;        // `json:"formats"`
} MessageProtocolHandshake;                // `json:"messageProtocolHandshake"`

enum MessageProtocolHandshakeErrorType {
    kMessageProtocolHandshakeErrorTypeRFU               = 0,
    kMessageProtocolHandshakeErrorTypeTimeout           = 1,
    kMessageProtocolHandshakeErrorTypeUnexpectedMessage = 2,
    kMessageProtocolHandshakeErrorTypeSelectionMismatch = 3,
};

typedef enum MessageProtocolHandshakeErrorType MessageProtocolHandshakeErrorType;

typedef struct {
    MessageProtocolHandshakeErrorType error;  // `json:"error"`
} MessageProtocolHandshakeError;              // `json:"messageProtocolHandshakeError"`

enum PinStateType {
    kPinStateTypeRequired = 0,  // "required"
    kPinStateTypeOptional = 1,  // "optional"
    kPinStateTypePinOk    = 2,  // "pinOk"
    kPinStateTypeNone     = 3,  // "none"
};

typedef enum PinStateType PinStateType;

enum PinInputPermissionType {
    kPinInputPermissionTypeBusy = 0,  // "busy"
    kPinInputPermissionTypeOk   = 1,  // "ok"
};

typedef enum PinInputPermissionType PinInputPermissionType;

typedef struct {
    PinStateType pin_state;                       // `json:"pinState"`
    PinInputPermissionType input_permission_val;  // Buffer to be addressed with input_permission
    PinInputPermissionType* input_permission;     // `json:"inputPermission,omitempty"`
} ConnectionPinState;                             // `json:"connectionPinState"`

typedef struct {
    uint64_t pin;      // `json:"pin"`
} ConnectionPinInput;  // `json:"connectionPinInput"`

enum ConnectionPinErrorType {
    kConnectionPinErrorTypeRfu0     = 0,
    kConnectionPinErrorTypeWrongPin = 1,
    kConnectionPinErrorTypeRfu4     = 4,
};

typedef enum ConnectionPinErrorType ConnectionPinErrorType;

typedef struct {
    ConnectionPinErrorType error;  // `json:"error"`
} ConnectionPinError;              // `json:"connectionPinError"`

typedef struct {
    // Consider protocol ID format "eeMM.MM" (e.g. "ee1.0", "ee99.99"),
    // reserve up to 7 symbols + null terminator
    char protocol_id[8];  // `json:"protocolId"`
} ShipHeaderType;

typedef struct {
    char* extension_id;     // `json:"extensionId,omitempty"`
    MessageBuffer* binary;  // `json:"binary,omitempty"` // HexBinary
    char* string;           // `json:"string,omitempty"`
} ExtensionType;

typedef struct {
    ShipHeaderType header;     // `json:"header"`
    MessageBuffer payload;     // `json:"payload"`
    ExtensionType* extension;  // `json:"extension,omitempty"`
} Data;                        // `json:"data"`

typedef struct {
    uint8_t unused;
} AccessMethodsRequest;  // `json:"accessMethodsRequest"`

typedef struct {
    const char* uri;  // `json:"uri"`
} Dns;

typedef struct {
    const char* id;    // `json:"id"`
    bool dns_sd_mdns;  // `json:"dnsSd_mDns,omitempty"`
    // According to the Spec Dns should be of type *Dns, but the SHM 2.0 only uses
    // a string and would cause a crash
    Dns dns;      // `json:"dns,omitempty"`
} AccessMethods;  // `json:"accessMethods"`

enum ConnectionClosePhaseType {
    kConnectionClosePhaseTypeAnnounce = 0,  // "announce"
    kConnectionClosePhaseTypeConfirm  = 1,  // "confirm"
};

typedef enum ConnectionClosePhaseType ConnectionClosePhaseType;

enum ConnectionCloseReasonType {
    kConnectionCloseReasonTypeUnspecific        = 0,  // "unspecific"
    kConnectionCloseReasonTypeRemovedConnection = 1,  // "removedConnection"
};

typedef enum ConnectionCloseReasonType ConnectionCloseReasonType;

typedef struct {
    ConnectionClosePhaseType phase;  // `json:"phase"`
    uint32_t max_time_val;           // Buffer to be addressed with max_time
    uint32_t* max_time;              // `json:"maxTime,omitempty"`
    const char* reason;              // `json:"reason,omitempty"`
} ConnectionClose;                   // `json:"connectionClose"`

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_MODEL_MODEL_H_
