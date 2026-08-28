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
 * @brief Ship Message Serialize implementation
 */

#include "src/ship/ship_connection/ship_message_serialize.h"

#ifdef __freertos__
#include <cJSON.h>
#else
#include <cjson/cJSON.h>
#endif  // __freertos__
#include <string.h>

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/json.h"
#include "src/common/message_buffer.h"
#include "src/ship/api/ship_message_serialize_interface.h"
#include "src/ship/model/model.h"

typedef struct ShipMessageSerialize ShipMessageSerialize;

struct ShipMessageSerialize {
    /** Implements the Ship Message Serialize Interface */
    ShipMessageSerializeObject obj;

    MessageBuffer* buf;
    cJSON* json_obj;
};

#define SHIP_MESSAGE_SERIALIZE(obj) ((ShipMessageSerialize*)(obj))

static void Destruct(ShipMessageSerializeObject* self);
static MessageBuffer* GetBuffer(const ShipMessageSerializeObject* self);

static const ShipMessageSerializeInterface ship_message_serialize_methods = {
    .destruct   = Destruct,
    .get_buffer = GetBuffer,
};

static void ShipMessageSerializeConstruct(ShipMessageSerialize* self, const void* value, MsgValueType value_type);
static void SerializeReset(ShipMessageSerialize* self);
static EebusError SmeHelloSerialize(const ConnectionHello* sme_hello, cJSON* sme_hello_obj);
static EebusError SmeProtocolHandshakeSerialize(const MessageProtocolHandshake* sme_prot_hs, cJSON* sme_prot_hs_obj);
static EebusError
SmeProtocolHandshakeErrorSerialize(const MessageProtocolHandshakeError* sme_prot_hs_err, cJSON* sme_prot_hs_obj);
static EebusError SmeConnectionPinStateSerialize(const ConnectionPinState* sme_pin_state, cJSON* pin_state_obj);
static EebusError SmeConnectionPinInputSerialize(const ConnectionPinInput* sme_pin_input, cJSON* pin_input_obj);
static EebusError SmeConnectionPinErrorSerialize(const ConnectionPinError* sme_pin_error, cJSON* pin_error_obj);
static EebusError DataSerialize(const Data* data, cJSON* data_obj);
static EebusError SmeConnectionAccessMethodsRequestSerialize(
    const AccessMethodsRequest* access_metods_request,
    cJSON* access_metods_req_obj
);
static EebusError
SmeConnectionAccessMethodsSerialize(const AccessMethods* sme_access_methods, cJSON* access_methods_obj);
static EebusError SmeCloseSerialize(const ConnectionClose* sme_close, cJSON* sme_close_obj);
static EebusError Serialize(ShipMessageSerialize* self, const void* value, MsgValueType value_type);

void ShipMessageSerializeConstruct(ShipMessageSerialize* self, const void* value, MsgValueType value_type) {
    // Override "virtual functions table"
    SHIP_MESSAGE_SERIALIZE_INTERFACE(self) = &ship_message_serialize_methods;

    self->buf      = NULL;
    self->json_obj = NULL;

    if ((value == NULL) || (value_type == kValueUndefined)) {
        return;
    }

    if (Serialize(self, value, value_type) != kEebusErrorOk) {
        SerializeReset(self);
    }
}

ShipMessageSerializeObject* ShipMessageSerializeCreate(const void* value, MsgValueType value_type) {
    ShipMessageSerialize* const ship_message_serialize
        = (ShipMessageSerialize*)EEBUS_MALLOC(sizeof(ShipMessageSerialize));

    ShipMessageSerializeConstruct(ship_message_serialize, value, value_type);

    return SHIP_MESSAGE_SERIALIZE_OBJECT(ship_message_serialize);
}

void SerializeReset(ShipMessageSerialize* self) {
    if (self->json_obj != NULL) {
        cJSON_Delete(self->json_obj);
        self->json_obj = NULL;
    }

    if (self->buf != NULL) {
        MessageBufferRelease(self->buf);
        EEBUS_FREE(self->buf);
        self->buf = NULL;
    }
}

void Destruct(ShipMessageSerializeObject* self) {
    SerializeReset(SHIP_MESSAGE_SERIALIZE(self));
}

void ShipMessageSerializeDelete(ShipMessageSerializeObject* self) {
    if (self == NULL) {
        return;
    }

    SHIP_MESSAGE_SERIALIZE_DESTRUCT(self);
    EEBUS_FREE(self);
}

MessageBuffer* GetBuffer(const ShipMessageSerializeObject* self) {
    return SHIP_MESSAGE_SERIALIZE(self)->buf;
}

bool StringToShipMessage(char* s, MessageBuffer* buf, MsgType msg_type) {
    if ((s == NULL) || (buf == NULL)) {
        return false;
    }

    size_t len = strlen(s);

    if (len < 2) {
        return false;
    }

    for (size_t i = len; i >= 1; --i) {
        s[i] = s[i - 1];
    }

    s[0] = (char)msg_type;

    MessageBufferInitWithDeallocator(buf, (uint8_t*)s, len + 1, cJSON_free);
    return true;
}

bool ConnectionHelloAddPhase(const ConnectionHello* sme_hello, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const phase_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);

    const char* phase = NULL;
    if (sme_hello->phase == kConnectionHelloPhasePending) {
        phase = "pending";
    } else if (sme_hello->phase == kConnectionHelloPhaseReady) {
        phase = "ready";
    } else if (sme_hello->phase == kConnectionHelloPhaseAborted) {
        phase = "aborted";
    } else {
        return false;
    }

    return cJSON_AddStringToObject(phase_obj, "phase", phase) != NULL;
}

bool ConnectionHelloAddWaiting(const ConnectionHello* sme_hello, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    if (sme_hello->waiting == NULL) {
        // "waiting" field is optional - nothing to be added here
        return true;
    }

    cJSON* const waiting_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    return cJSON_AddNumberToObject(waiting_obj, "waiting", *sme_hello->waiting) != NULL;
}

bool ConnectionHelloAddProlongationRequest(const ConnectionHello* sme_hello, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    const bool* const prolong_req = sme_hello->prolongation_request;
    if (prolong_req == NULL) {
        // "prolongationRequest" field is optional - nothing to be added here
        return true;
    }

    cJSON* const prolong_req_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    return cJSON_AddBoolToObject(prolong_req_obj, "prolongationRequest", *prolong_req) != NULL;
}

EebusError SmeHelloSerialize(const ConnectionHello* sme_hello, cJSON* sme_hello_obj) {
    cJSON* const sme_hello_ar = cJSON_AddArrayToObject(sme_hello_obj, "connectionHello");

    bool ok = (sme_hello_ar != NULL);

    ok = ok && ConnectionHelloAddPhase(sme_hello, sme_hello_ar);
    ok = ok && ConnectionHelloAddWaiting(sme_hello, sme_hello_ar);
    ok = ok && ConnectionHelloAddProlongationRequest(sme_hello, sme_hello_ar);

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

bool MessageProtocolHandshakeAddType(const MessageProtocolHandshake* sme_prot_hs, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const handshake_type_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);

    const char* handshake_type_str = NULL;
    if (sme_prot_hs->handshake_type == kProtocolHandshakeTypeAnnounceMax) {
        handshake_type_str = "announceMax";
    } else if (sme_prot_hs->handshake_type == kProtocolHandshakeTypeSelect) {
        handshake_type_str = "select";
    }

    cJSON* const handshake_type = cJSON_AddStringToObject(handshake_type_obj, "handshakeType", handshake_type_str);
    return handshake_type != NULL;
}

bool MessageProtocolHandshakeAddVersion(const MessageProtocolHandshake* sme_prot_hs, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const version_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    cJSON* const version_ar  = cJSON_AddArrayToObject(version_obj, "version");

    cJSON* const version_0 = (cJSON*)JsonAddObjectToArray((JsonObject*)version_ar);
    cJSON* const major     = cJSON_AddNumberToObject(version_0, "major", sme_prot_hs->version.major);

    cJSON* const version_1 = (cJSON*)JsonAddObjectToArray((JsonObject*)version_ar);
    cJSON* const minor     = cJSON_AddNumberToObject(version_1, "minor", sme_prot_hs->version.minor);

    return (major != NULL) && (minor != NULL);
}

bool MessageProtocolHandshakeAddFormats(const MessageProtocolHandshake* sme_prot_hs, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    if (sme_prot_hs->formats.format_size > ARRAY_SIZE(sme_prot_hs->formats.format)) {
        return false;
    }

    cJSON* const formats_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    cJSON* const formats_ar  = cJSON_AddArrayToObject(formats_obj, "formats");
    cJSON* const formats_0   = (cJSON*)JsonAddObjectToArray((JsonObject*)formats_ar);
    if (sme_prot_hs->formats.format_size == 0) {
        return cJSON_AddNullToObject(formats_0, "format") != NULL;
    }

    cJSON* const format_ar = cJSON_AddArrayToObject(formats_0, "format");
    if (format_ar == NULL) {
        return false;
    }

    for (size_t i = 0; i < sme_prot_hs->formats.format_size; ++i) {
        const char* format_str = NULL;
        if (sme_prot_hs->formats.format[i] == kMessageProtocolFormatTypeUTF8) {
            format_str = "JSON-UTF8";
        } else if (sme_prot_hs->formats.format[i] == kMessageProtocolFormatTypeUTF16) {
            format_str = "JSON-UTF16";
        }

        if (JsonAddStringToArray((JsonObject*)format_ar, format_str) == NULL) {
            return false;
        }
    }

    return true;
}

EebusError SmeProtocolHandshakeSerialize(const MessageProtocolHandshake* sme_prot_hs, cJSON* sme_prot_hs_obj) {
    cJSON* const sme_prot_hs_ar = cJSON_AddArrayToObject(sme_prot_hs_obj, "messageProtocolHandshake");

    bool ok = (sme_prot_hs_ar != NULL);

    ok = ok && MessageProtocolHandshakeAddType(sme_prot_hs, sme_prot_hs_ar);
    ok = ok && MessageProtocolHandshakeAddVersion(sme_prot_hs, sme_prot_hs_ar);
    ok = ok && MessageProtocolHandshakeAddFormats(sme_prot_hs, sme_prot_hs_ar);

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

bool MessageProtocolHandshakeErrorAddError(const MessageProtocolHandshakeError* msg_prot_err_hs, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const handshake_err_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    cJSON* const handshake_err     = cJSON_AddNumberToObject(handshake_err_obj, "error", msg_prot_err_hs->error);
    return handshake_err != NULL;
}

EebusError
SmeProtocolHandshakeErrorSerialize(const MessageProtocolHandshakeError* sme_prot_hs_err, cJSON* sme_prot_hs_obj) {
    cJSON* const sme_prot_hs_err_ar = cJSON_AddArrayToObject(sme_prot_hs_obj, "messageProtocolHandshakeError");

    bool ok = (sme_prot_hs_err_ar != NULL);

    ok = ok && MessageProtocolHandshakeErrorAddError(sme_prot_hs_err, sme_prot_hs_err_ar);

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

bool ConnectionPinStateAddPinState(const ConnectionPinState* sme_pin_state, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const pin_state_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);

    const char* pin_state_str = NULL;
    if (sme_pin_state->pin_state == kPinStateTypeRequired) {
        pin_state_str = "required";
    } else if (sme_pin_state->pin_state == kPinStateTypeOptional) {
        pin_state_str = "optional";
    } else if (sme_pin_state->pin_state == kPinStateTypePinOk) {
        pin_state_str = "pinOk";
    } else if (sme_pin_state->pin_state == kPinStateTypeNone) {
        pin_state_str = "none";
    } else {
        return false;
    }

    return cJSON_AddStringToObject(pin_state_obj, "pinState", pin_state_str) != NULL;
}

bool ConnectionPinStateAddInputPermission(const ConnectionPinState* sme_pin_state, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    if (sme_pin_state->input_permission == NULL) {
        // "inputPermission" field is optional - nothing to be added here
        return true;
    }

    PinInputPermissionType input_permission = *(sme_pin_state->input_permission);

    cJSON* const in_permission_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);

    const char* input_permission_str = NULL;
    if (input_permission == kPinInputPermissionTypeBusy) {
        input_permission_str = "busy";
    } else if (input_permission == kPinInputPermissionTypeOk) {
        input_permission_str = "ok";
    } else {
        return false;
    }

    return cJSON_AddStringToObject(in_permission_obj, "inputPermission", input_permission_str) != NULL;
}

EebusError SmeConnectionPinStateSerialize(const ConnectionPinState* sme_pin_state, cJSON* pin_state_obj) {
    cJSON* const pin_state_ar = cJSON_AddArrayToObject(pin_state_obj, "connectionPinState");

    bool ok = (pin_state_ar != NULL);

    ok = ok && ConnectionPinStateAddPinState(sme_pin_state, pin_state_ar);
    ok = ok && ConnectionPinStateAddInputPermission(sme_pin_state, pin_state_ar);

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

bool ConnectionPinInputAddPin(const ConnectionPinInput* sme_pin_input, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const pin_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);

    // Up to 16 hexadecimal digits + \0
    char pin_str[17];
    size_t pin_str_len = 0;
    uint64_t pin       = sme_pin_input->pin;
    while (pin > 0) {
        char d = pin & 0xF;
        d += (d < 10) ? '0' : ('A' - 10);
        pin_str[pin_str_len++] = d;
        pin >>= 4;
    }

    pin_str[pin_str_len] = '\0';

    for (size_t i = 0; i < pin_str_len / 2; ++i) {
        char tmp   = pin_str[i];
        pin_str[i] = pin_str[pin_str_len - i - 1];

        pin_str[pin_str_len - i - 1] = tmp;
    }

    return cJSON_AddStringToObject(pin_obj, "pin", pin_str) != NULL;
}

EebusError SmeConnectionPinInputSerialize(const ConnectionPinInput* sme_pin_input, cJSON* pin_input_obj) {
    cJSON* const pin_input_ar = cJSON_AddArrayToObject(pin_input_obj, "connectionPinInput");

    bool ok = (pin_input_ar != NULL);

    ok = ok && ConnectionPinInputAddPin(sme_pin_input, pin_input_ar);

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

bool ConnectionPinErrorAddError(const ConnectionPinError* sme_pin_error, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const error_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);

    return cJSON_AddNumberToObject(error_obj, "error", sme_pin_error->error) != NULL;
}

EebusError SmeConnectionPinErrorSerialize(const ConnectionPinError* sme_pin_error, cJSON* pin_error_obj) {
    cJSON* const pin_error_ar = cJSON_AddArrayToObject(pin_error_obj, "connectionPinError");

    bool ok = (pin_error_ar != NULL);

    ok = ok && ConnectionPinErrorAddError(sme_pin_error, pin_error_ar);

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

bool DataAddHeader(const Data* data, cJSON* data_ar) {
    cJSON* const header_obj      = (cJSON*)JsonAddObjectToArray((JsonObject*)data_ar);
    cJSON* const header_ar       = cJSON_AddArrayToObject(header_obj, "header");
    cJSON* const protocol_id_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)header_ar);
    return cJSON_AddStringToObject(protocol_id_obj, "protocolId", data->header.protocol_id) != NULL;
}

bool DataAddPayload(const Data* data, cJSON* data_ar) {
    cJSON* const payload_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)data_ar);
    return cJSON_AddRawToObject(payload_obj, "payload", (char*)data->payload.data) != NULL;
}

EebusError DataSerialize(const Data* data, cJSON* data_obj) {
    cJSON* const data_ar = cJSON_AddArrayToObject(data_obj, "data");

    bool ok = (data_ar != NULL);

    ok = ok && DataAddHeader(data, data_ar);
    ok = ok && DataAddPayload(data, data_ar);

    // TODO: add extension parsing

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

EebusError SmeConnectionAccessMethodsRequestSerialize(
    const AccessMethodsRequest* sme_access_metods_req,
    cJSON* access_metods_req_obj
) {
    UNUSED(sme_access_metods_req);
    cJSON* const access_methods_req_ar = cJSON_AddArrayToObject(access_metods_req_obj, "accessMethodsRequest");

    return (access_methods_req_ar != NULL) ? kEebusErrorOk : kEebusErrorMemory;
}

bool AccessMethodsAddId(const AccessMethods* sme_access_methods, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const id_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    return cJSON_AddStringToObject(id_obj, "id", sme_access_methods->id) != NULL;
}

bool AccessMethodsAddDnsSdMdns(const AccessMethods* sme_access_methods, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    if (sme_access_methods->dns_sd_mdns == false) {
        // dnsSd_mDns is optional field so this is ok
        return true;
    }

    cJSON* const dns_sd_mdns_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    return cJSON_AddArrayToObject(dns_sd_mdns_obj, "dnsSd_mDns") != NULL;
}

bool AccessMethodsAddDns(const AccessMethods* sme_access_methods, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    if (sme_access_methods->dns.uri == false) {
        // dns is optional field so this is ok
        return true;
    }

    cJSON* const dns_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    cJSON* const uri_ar  = cJSON_AddArrayToObject(dns_obj, "dns");
    cJSON* const uri_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)uri_ar);
    return cJSON_AddStringToObject(uri_obj, "uri", sme_access_methods->dns.uri) != NULL;
}

EebusError SmeConnectionAccessMethodsSerialize(const AccessMethods* sme_access_methods, cJSON* access_methods_obj) {
    cJSON* const access_methods_ar = cJSON_AddArrayToObject(access_methods_obj, "accessMethods");

    bool ok = (access_methods_ar != NULL);

    ok = ok && AccessMethodsAddId(sme_access_methods, access_methods_ar);
    ok = ok && AccessMethodsAddDnsSdMdns(sme_access_methods, access_methods_ar);
    ok = ok && AccessMethodsAddDns(sme_access_methods, access_methods_ar);

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

bool ConnectionCloseAddPhase(const ConnectionClose* sme_close, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    cJSON* const phase_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);

    const char* phase_str = NULL;
    if (sme_close->phase == kConnectionClosePhaseTypeAnnounce) {
        phase_str = "announce";
    } else if (sme_close->phase == kConnectionClosePhaseTypeConfirm) {
        phase_str = "confirm";
    } else {
        return false;
    }

    return cJSON_AddStringToObject(phase_obj, "phase", phase_str) != NULL;
}

bool ConnectionCloseAddMaxTime(const ConnectionClose* sme_close, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    if (sme_close->max_time == NULL) {
        // maxTime is optional field so this is ok
        return true;
    }

    cJSON* const max_time_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    return cJSON_AddNumberToObject(max_time_obj, "maxTime", *sme_close->max_time) != NULL;
}

bool ConnectionCloseAddReason(const ConnectionClose* sme_close, cJSON* json_ar) {
    if (json_ar == NULL) {
        return false;
    }

    if (sme_close->reason == NULL) {
        // reason is optional field so this is ok
        return true;
    }

    cJSON* const reason_obj = (cJSON*)JsonAddObjectToArray((JsonObject*)json_ar);
    return cJSON_AddStringToObject(reason_obj, "reason", sme_close->reason) != NULL;
}

EebusError SmeCloseSerialize(const ConnectionClose* sme_close, cJSON* sme_close_obj) {
    cJSON* const connection_close_ar = cJSON_AddArrayToObject(sme_close_obj, "connectionClose");

    bool ok = (connection_close_ar != NULL);

    ok = ok && ConnectionCloseAddPhase(sme_close, connection_close_ar);
    ok = ok && ConnectionCloseAddMaxTime(sme_close, connection_close_ar);
    ok = ok && ConnectionCloseAddReason(sme_close, connection_close_ar);

    return ok ? kEebusErrorOk : kEebusErrorMemory;
}

EebusError SerializeMessage(ShipMessageSerialize* self, const void* value, MsgValueType value_type) {
    switch (value_type) {
        case kSmeHello: {
            return SmeHelloSerialize((ConnectionHello*)value, self->json_obj);
        }

        case kSmeProtocolHandshake: {
            return SmeProtocolHandshakeSerialize((MessageProtocolHandshake*)value, self->json_obj);
        }

        case kSmeProtocolHandshakeError: {
            return SmeProtocolHandshakeErrorSerialize((MessageProtocolHandshakeError*)value, self->json_obj);
        }

        case kSmeConnectionPinState: {
            return SmeConnectionPinStateSerialize((ConnectionPinState*)value, self->json_obj);
        }

        case kSmeConnectionPinInput: {
            return SmeConnectionPinInputSerialize((ConnectionPinInput*)value, self->json_obj);
        }

        case kSmeConnectionPinError: {
            return SmeConnectionPinErrorSerialize((ConnectionPinError*)value, self->json_obj);
        }

        case kData: {
            return DataSerialize((Data*)value, self->json_obj);
        }

        case kSmeConnectionAccessMethodsRequest: {
            return SmeConnectionAccessMethodsRequestSerialize((AccessMethodsRequest*)value, self->json_obj);
        }

        case kSmeConnectionAccessMethods: {
            return SmeConnectionAccessMethodsSerialize((AccessMethods*)value, self->json_obj);
        }

        case kSmeClose: {
            return SmeCloseSerialize((ConnectionClose*)value, self->json_obj);
        }

        default: {
            return kEebusErrorInputArgument;
        }
    }
}

EebusError Serialize(ShipMessageSerialize* self, const void* value, MsgValueType value_type) {
    self->buf      = (MessageBuffer*)EEBUS_MALLOC(sizeof(MessageBuffer));
    self->json_obj = cJSON_CreateObject();

    if ((self->json_obj == NULL) || (self->buf == NULL)) {
        return kEebusErrorMemory;
    }

    EebusError ret = SerializeMessage(self, value, value_type);
    if (ret != kEebusErrorOk) {
        return ret;
    }

    char* const s = cJSON_PrintUnformatted(self->json_obj);
    if (s == NULL) {
        return kEebusErrorMemory;
    }

    const MsgType msg_type = (value_type == kSmeClose) ? kMsgTypeEnd
                             : (value_type == kData)   ? kMsgTypeData
                                                       : kMsgTypeControl;

    if (!StringToShipMessage(s, self->buf, msg_type)) {
        return kEebusErrorMemory;
    }

    // cJSON object is not required anymore
    cJSON_Delete(self->json_obj);
    self->json_obj = NULL;
    return kEebusErrorOk;
}
