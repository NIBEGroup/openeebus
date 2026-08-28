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
 * @brief Ship Message Deserialize implementation
 */

#ifdef __freertos__
#include <cJSON.h>
#else
#include <cjson/cJSON.h>
#endif  // __freertos__
#include <ctype.h>
#include <string.h>

#include "src/common/array_util.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/message_buffer.h"
#include "src/common/string_util.h"
#include "src/ship/api/ship_message_deserialize_interface.h"
#include "src/ship/model/model.h"

typedef struct ShipMessageDeserialize ShipMessageDeserialize;

struct ShipMessageDeserialize {
    /** Implements the Ship Message Deserialize Interface */
    ShipMessageDeserializeObject obj;

    void* value;
    MsgValueType value_type;
};

#define SHIP_MESSAGE_DESERIALIZE(obj) ((ShipMessageDeserialize*)(obj))

static void Destruct(ShipMessageDeserializeObject* self);
static MsgValueType GetValueType(const ShipMessageDeserializeObject* self);
static void* GetValue(const ShipMessageDeserializeObject* self);

static const ShipMessageDeserializeInterface ship_message_deserialize_methods = {
    .destruct       = Destruct,
    .get_value_type = GetValueType,
    .get_value      = GetValue,
};

static void ShipMessageDeserializeConstruct(ShipMessageDeserialize* self, MessageBuffer* buf);
static EebusError SmeHelloDeserialize(ConnectionHello* sme_hello, const cJSON* sme_hello_ar);
static EebusError SmeProtocolHandshakeDeserialize(MessageProtocolHandshake* sme_prot_hs, const cJSON* sme_prot_hs_ar);
static EebusError
SmeProtocolHandshakeErrorDeserialize(MessageProtocolHandshakeError* sme_prot_hs_err, const cJSON* sme_prot_hs_err_ar);
static EebusError SmeConnectionPinStateDeserialize(ConnectionPinState* sme_pin_state, const cJSON* pin_state_ar);
static EebusError SmeConnectionPinInputDeserialize(ConnectionPinInput* sme_pin_input, const cJSON* pin_input_ar);
static EebusError SmeConnectionPinErrorDeserialize(ConnectionPinError* sme_pin_error, const cJSON* pin_error_ar);
static EebusError DataDeserialize(Data* data, const cJSON* data_ar);
static EebusError SmeConnectionAccessMethodsRequestDeserialize(
    AccessMethodsRequest* access_metods_request,
    const cJSON* access_methods_req_ar
);
static EebusError
SmeConnectionAccessMethodsDeserialize(AccessMethods* sme_access_methods, const cJSON* access_methods_ar);
static EebusError SmeCloseDeserialize(ConnectionClose* sme_close, const cJSON* close_ar);
static void Deserialize(ShipMessageDeserialize* self, MessageBuffer* buf);

void ShipMessageDeserializeConstruct(ShipMessageDeserialize* self, MessageBuffer* buf) {
    // Override "virtual functions table"
    SHIP_MESSAGE_DESERIALIZE_INTERFACE(self) = &ship_message_deserialize_methods;

    self->value      = NULL;
    self->value_type = kValueUndefined;

    Deserialize(self, buf);
}

ShipMessageDeserializeObject* ShipMessageDeserializeCreate(MessageBuffer* buf) {
    ShipMessageDeserialize* const ship_message_deserialize
        = (ShipMessageDeserialize*)EEBUS_MALLOC(sizeof(ShipMessageDeserialize));

    ShipMessageDeserializeConstruct(ship_message_deserialize, buf);

    return SHIP_MESSAGE_DESERIALIZE_OBJECT(ship_message_deserialize);
}

void DeserializeReset(ShipMessageDeserialize* self) {
    if (self->value != NULL) {
        if (self->value_type == kSmeConnectionAccessMethods) {
            AccessMethods* const sme_access_methods = (AccessMethods*)self->value;
            StringDelete((char*)sme_access_methods->id);
            StringDelete((char*)sme_access_methods->dns.uri);
        } else if (self->value_type == kData) {
            Data* const data = (Data*)self->value;
            MessageBufferRelease(&data->payload);
        } else if (self->value_type == kSmeClose) {
            ConnectionClose* const sme_close = (ConnectionClose*)self->value;
            StringDelete((char*)sme_close->reason);
        }

        EEBUS_FREE(self->value);
        self->value = NULL;
    }

    self->value_type = kValueUndefined;
}

void ShipMessageDeserializeDelete(ShipMessageDeserializeObject* self) {
    if (self == NULL) {
        return;
    }

    SHIP_MESSAGE_DESERIALIZE_DESTRUCT(self);
    EEBUS_FREE(self);
}

void Destruct(ShipMessageDeserializeObject* self) {
    DeserializeReset(SHIP_MESSAGE_DESERIALIZE(self));
}

MsgValueType GetValueType(const ShipMessageDeserializeObject* self) {
    return SHIP_MESSAGE_DESERIALIZE(self)->value_type;
}

void* GetValue(const ShipMessageDeserializeObject* self) {
    return SHIP_MESSAGE_DESERIALIZE(self)->value;
}

bool ShipMessageToString(MessageBuffer* buf) {
    if (buf == NULL) {
        return false;
    }

    if (buf->data_size < 2) {
        return false;
    }

    for (size_t i = 0; i < buf->data_size - 1; ++i) {
        buf->data[i] = buf->data[i + 1];
    }

    buf->data[buf->data_size - 1] = '\0';
    return true;
}

bool ConnectionHelloParsePhase(ConnectionHello* sme_hello, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const phase          = cJSON_GetObjectItem(json_obj, "phase");
    const char* const phase_str = cJSON_GetStringValue(phase);
    if (phase_str == NULL) {
        return false;
    }

    if (!strcmp(phase_str, "pending")) {
        sme_hello->phase = kConnectionHelloPhasePending;
    } else if (!strcmp(phase_str, "ready")) {
        sme_hello->phase = kConnectionHelloPhaseReady;
    } else if (!strcmp(phase_str, "aborted")) {
        sme_hello->phase = kConnectionHelloPhaseAborted;
    } else {
        return false;
    }

    return true;
}

bool ConnectionHelloParseWaiting(ConnectionHello* sme_hello, cJSON* json_obj) {
    if (!cJSON_HasObjectItem(json_obj, "waiting")) {
        return false;
    }

    cJSON* const waiting = cJSON_GetObjectItem(json_obj, "waiting");
    if (!cJSON_IsNumber(waiting)) {
        return false;
    }

    sme_hello->waiting_val = (uint32_t)cJSON_GetNumberValue(waiting);
    sme_hello->waiting     = &sme_hello->waiting_val;
    return true;
}

bool ConnectionHelloParseProlongReq(ConnectionHello* sme_hello, cJSON* json_obj) {
    if (!cJSON_HasObjectItem(json_obj, "prolongationRequest")) {
        return false;
    }

    cJSON* const prolong_req = cJSON_GetObjectItem(json_obj, "prolongationRequest");
    if (!cJSON_IsBool(prolong_req)) {
        return false;
    }

    sme_hello->prolongation_request_val = cJSON_IsTrue(prolong_req);
    sme_hello->prolongation_request     = &sme_hello->prolongation_request_val;
    return true;
}

EebusError SmeHelloDeserialize(ConnectionHello* sme_hello, const cJSON* sme_hello_ar) {
    if ((sme_hello == NULL) || (sme_hello_ar == NULL)) {
        return kEebusErrorInputArgument;
    }

    cJSON* const sme_hello_0 = cJSON_GetArrayItem(sme_hello_ar, 0);

    bool ok = ConnectionHelloParsePhase(sme_hello, sme_hello_0);

    sme_hello->waiting              = NULL;
    sme_hello->prolongation_request = NULL;

    cJSON* const sme_hello_1 = ok ? cJSON_GetArrayItem(sme_hello_ar, 1) : NULL;
    if (sme_hello_1 != NULL) {
        ok = ok
             && (ConnectionHelloParseWaiting(sme_hello, sme_hello_1)
                 || ConnectionHelloParseProlongReq(sme_hello, sme_hello_1));
    }

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

bool MessageProtocolHandshakeParseType(MessageProtocolHandshake* sme_prot_hs, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const handshake_type          = cJSON_GetObjectItem(json_obj, "handshakeType");
    const char* const handshake_type_str = cJSON_GetStringValue(handshake_type);
    if (handshake_type_str == NULL) {
        return false;
    }

    if (!strcmp(handshake_type_str, "announceMax")) {
        sme_prot_hs->handshake_type = kProtocolHandshakeTypeAnnounceMax;
        return true;
    } else if (!strcmp(handshake_type_str, "select")) {
        sme_prot_hs->handshake_type = kProtocolHandshakeTypeSelect;
        return true;
    } else {
        return false;
    }
}

bool MessageProtocolHandshakeParseVersion(MessageProtocolHandshake* sme_prot_hs, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const version_ar = cJSON_GetObjectItem(json_obj, "version");
    cJSON* const version_0  = cJSON_GetArrayItem(version_ar, 0);
    cJSON* const major      = cJSON_GetObjectItem(version_0, "major");
    if (!cJSON_IsNumber(major)) {
        return false;
    }

    sme_prot_hs->version.major = (uint8_t)cJSON_GetNumberValue(major);

    cJSON* const version_1 = cJSON_GetArrayItem(version_ar, 1);
    cJSON* const minor     = cJSON_GetObjectItem(version_1, "minor");
    if (!cJSON_IsNumber(minor)) {
        return false;
    }

    sme_prot_hs->version.minor = (uint8_t)cJSON_GetNumberValue(minor);
    return true;
}

bool MessageProtocolHandshakeParseFormats(MessageProtocolHandshake* sme_prot_hs, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const formats_ar = cJSON_GetObjectItem(json_obj, "formats");
    cJSON* const formats_0  = cJSON_GetArrayItem(formats_ar, 0);
    cJSON* const format_ar  = cJSON_GetObjectItem(formats_0, "format");
    if (format_ar == NULL) {
        return false;
    }

    const size_t format_size = cJSON_GetArraySize(format_ar);
    if (format_size > ARRAY_SIZE(sme_prot_hs->formats.format)) {
        return false;
    }

    for (size_t i = 0; i < format_size; ++i) {
        cJSON* const format_i    = cJSON_GetArrayItem(format_ar, (int)i);
        const char* const format = cJSON_GetStringValue(format_i);
        if (format == NULL) {
            return false;
        }

        if (!strcmp(format, "JSON-UTF8")) {
            sme_prot_hs->formats.format[i] = kMessageProtocolFormatTypeUTF8;
        } else if (!strcmp(format, "JSON-UTF16")) {
            sme_prot_hs->formats.format[i] = kMessageProtocolFormatTypeUTF16;
        } else {
            return false;
        }
    }

    sme_prot_hs->formats.format_size = format_size;
    return true;
}

EebusError SmeProtocolHandshakeDeserialize(MessageProtocolHandshake* sme_prot_hs, const cJSON* sme_prot_hs_ar) {
    if ((sme_prot_hs == NULL) || (sme_prot_hs_ar == NULL)) {
        return kEebusErrorInputArgument;
    }

    cJSON* const sme_prot_hs_0 = cJSON_GetArrayItem(sme_prot_hs_ar, 0);

    bool ok = MessageProtocolHandshakeParseType(sme_prot_hs, sme_prot_hs_0);

    cJSON* const sme_prot_hs_1 = cJSON_GetArrayItem(sme_prot_hs_ar, 1);

    ok = ok && MessageProtocolHandshakeParseVersion(sme_prot_hs, sme_prot_hs_1);

    cJSON* const sme_prot_hs_2 = cJSON_GetArrayItem(sme_prot_hs_ar, 2);

    ok = ok && MessageProtocolHandshakeParseFormats(sme_prot_hs, sme_prot_hs_2);

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

bool MessageProtocolHandshakeErrorParseError(MessageProtocolHandshakeError* sme_prot_hs_err, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const error_obj = cJSON_GetObjectItem(json_obj, "error");
    if ((error_obj == NULL) || (!cJSON_IsNumber(error_obj))) {
        return false;
    }

    const int32_t error = (int32_t)cJSON_GetNumberValue(error_obj);

    sme_prot_hs_err->error = (MessageProtocolHandshakeErrorType)error;

    return (error >= kMessageProtocolHandshakeErrorTypeRFU)
           && (error <= kMessageProtocolHandshakeErrorTypeSelectionMismatch);
}

EebusError
SmeProtocolHandshakeErrorDeserialize(MessageProtocolHandshakeError* sme_prot_hs_err, const cJSON* sme_prot_hs_err_ar) {
    if ((sme_prot_hs_err == NULL) || (sme_prot_hs_err_ar == NULL)) {
        return kEebusErrorInputArgument;
    }

    cJSON* const sme_prot_hs_err_0 = cJSON_GetArrayItem(sme_prot_hs_err_ar, 0);

    bool ok = MessageProtocolHandshakeErrorParseError(sme_prot_hs_err, sme_prot_hs_err_0);

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

bool ConnectionPinStateParsePinState(ConnectionPinState* sme_pin_state, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const pin_state_obj      = cJSON_GetObjectItem(json_obj, "pinState");
    const char* const pin_state_str = cJSON_GetStringValue(pin_state_obj);

    if (pin_state_str == NULL) {
        return false;
    }

    if (!strcmp(pin_state_str, "required")) {
        sme_pin_state->pin_state = kPinStateTypeRequired;
    } else if (!strcmp(pin_state_str, "optional")) {
        sme_pin_state->pin_state = kPinStateTypeOptional;
    } else if (!strcmp(pin_state_str, "pinOk")) {
        sme_pin_state->pin_state = kPinStateTypePinOk;
    } else if (!strcmp(pin_state_str, "none")) {
        sme_pin_state->pin_state = kPinStateTypeNone;
    } else {
        return false;
    }

    return true;
}

bool ConnectionPinStateParseInputPermission(ConnectionPinState* sme_pin_state, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const input_permission_obj      = cJSON_GetObjectItem(json_obj, "inputPermission");
    const char* const input_permission_str = cJSON_GetStringValue(input_permission_obj);

    if (input_permission_str == NULL) {
        return false;
    }

    if (!strcmp(input_permission_str, "busy")) {
        sme_pin_state->input_permission_val = kPinInputPermissionTypeBusy;
    } else if (!strcmp(input_permission_str, "ok")) {
        sme_pin_state->input_permission_val = kPinInputPermissionTypeOk;
    } else {
        return false;
    }

    sme_pin_state->input_permission = &sme_pin_state->input_permission_val;
    return true;
}

EebusError SmeConnectionPinStateDeserialize(ConnectionPinState* sme_pin_state, const cJSON* pin_state_ar) {
    if ((sme_pin_state == NULL) || (pin_state_ar == NULL)) {
        return kEebusErrorInputArgument;
    }

    cJSON* const pin_state_ar_0 = cJSON_GetArrayItem(pin_state_ar, 0);

    bool ok = ConnectionPinStateParsePinState(sme_pin_state, pin_state_ar_0);

    sme_pin_state->input_permission = NULL;

    cJSON* const pin_state_ar_1 = cJSON_GetArrayItem(pin_state_ar, 1);
    if (pin_state_ar_1 != NULL) {
        ok = ok && ConnectionPinStateParseInputPermission(sme_pin_state, pin_state_ar_1);
    }

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

bool ConnectionPinInputParsePin(ConnectionPinInput* sme_pin_input, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const pin_input_obj = cJSON_GetObjectItem(json_obj, "pin");
    const char* const pin_str  = cJSON_GetStringValue(pin_input_obj);
    if ((pin_str == NULL) || (pin_str[0] == '0')) {
        return false;
    }

    const size_t pin_str_len = strlen(pin_str);
    if ((pin_str_len < 8) || (pin_str_len > 16)) {
        return false;
    }

    uint64_t pin = 0;
    for (size_t i = 0; i < pin_str_len; ++i) {
        pin <<= 4;
        if (isdigit((int)pin_str[i])) {
            pin += pin_str[i] - '0';
        } else if (isalpha((int)pin_str[i])) {
            const char c = toupper((int)pin_str[i]);
            if (c > 'F') {
                return false;
            }

            pin += c - 'A' + 10;
        } else {
            return false;
        }
    }

    sme_pin_input->pin = pin;
    return true;
}

EebusError SmeConnectionPinInputDeserialize(ConnectionPinInput* sme_pin_input, const cJSON* pin_input_ar) {
    if ((sme_pin_input == NULL) || (pin_input_ar == NULL)) {
        return kEebusErrorInputArgument;
    }

    cJSON* const pin_input_ar_0 = cJSON_GetArrayItem(pin_input_ar, 0);

    bool ok = ConnectionPinInputParsePin(sme_pin_input, pin_input_ar_0);

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

bool ConnectionPinErrorParseError(ConnectionPinError* sme_pin_error, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const pin_error_obj = cJSON_GetObjectItem(json_obj, "error");

    if ((pin_error_obj == NULL) || (!cJSON_IsNumber(pin_error_obj))) {
        return false;
    }

    sme_pin_error->error = cJSON_GetNumberValue(pin_error_obj);
    return sme_pin_error->error == kConnectionPinErrorTypeWrongPin;
}

EebusError SmeConnectionPinErrorDeserialize(ConnectionPinError* sme_pin_error, const cJSON* pin_error_ar) {
    if ((sme_pin_error == NULL) || (pin_error_ar == NULL)) {
        return kEebusErrorInputArgument;
    }

    cJSON* const pin_error_ar_0 = cJSON_GetArrayItem(pin_error_ar, 0);

    bool ok = ConnectionPinErrorParseError(sme_pin_error, pin_error_ar_0);

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

bool DataParseHeader(Data* data, const cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const header_ar       = cJSON_GetObjectItem(json_obj, "header");
    cJSON* const header_0        = cJSON_GetArrayItem(header_ar, 0);
    cJSON* const protocol_id_obj = cJSON_GetObjectItem(header_0, "protocolId");

    const char* const protocol_id_str = cJSON_GetStringValue(protocol_id_obj);
    if ((protocol_id_str == NULL) || (strlen(protocol_id_str) >= sizeof(data->header.protocol_id))) {
        return false;
    }

    strcpy(data->header.protocol_id, protocol_id_str);
    return true;
}

bool DataParsePayload(Data* data, const cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const payload_obj = cJSON_GetObjectItem(json_obj, "payload");
    char* const payload_str  = cJSON_PrintUnformatted(payload_obj);
    if (payload_str == NULL) {
        return false;
    }

    MessageBufferInitWithDeallocator(&data->payload, (uint8_t*)payload_str, strlen(payload_str) + 1, cJSON_free);
    return true;
}

EebusError DataDeserialize(Data* data, const cJSON* data_ar) {
    if (data == NULL) {
        return kEebusErrorInputArgument;
    }

    MessageBufferInit(&data->payload, NULL, 0);
    data->extension = NULL;

    if (data_ar == NULL) {
        return kEebusErrorInputArgument;
    }

    cJSON* const data_ar_0 = cJSON_GetArrayItem(data_ar, 0);

    bool ok = DataParseHeader(data, data_ar_0);

    cJSON* const data_ar_1 = cJSON_GetArrayItem(data_ar, 1);

    ok = ok && DataParsePayload(data, data_ar_1);

    // TODO: add extension parsing

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

EebusError SmeConnectionAccessMethodsRequestDeserialize(
    AccessMethodsRequest* sme_access_methods_req,
    const cJSON* access_metods_req_ar
) {
    if ((sme_access_methods_req == NULL) || (access_metods_req_ar == NULL)) {
        return kEebusErrorInputArgument;
    }

    bool ok = cJSON_GetArraySize(access_metods_req_ar) == 0;

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

bool AccessMethodsParseId(AccessMethods* sme_access_methods, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const id_obj  = cJSON_GetObjectItem(json_obj, "id");
    const char* const id = cJSON_GetStringValue(id_obj);
    if (id == NULL) {
        return false;
    }

    sme_access_methods->id = StringCopy(id);
    return sme_access_methods->id != NULL;
}

bool AccessMethodsParseDnsSdMdns(AccessMethods* sme_access_methods, cJSON* json_obj) {
    cJSON* const dns_sd_mdns_obj = cJSON_GetObjectItem(json_obj, "dnsSd_mDns");

    sme_access_methods->dns_sd_mdns = true;

    return cJSON_GetArraySize(dns_sd_mdns_obj) == 0;
}

bool AccessMethodsParseDns(AccessMethods* sme_access_methods, cJSON* json_obj) {
    cJSON* const dns_ar   = cJSON_GetObjectItem(json_obj, "dns");
    cJSON* const dns_0    = cJSON_GetArrayItem(dns_ar, 0);
    cJSON* const uri_obj  = cJSON_GetObjectItem(dns_0, "uri");
    const char* const uri = cJSON_GetStringValue(uri_obj);
    if (uri == NULL) {
        return false;
    }

    sme_access_methods->dns.uri = StringCopy(uri);
    return sme_access_methods->dns.uri != NULL;
}

EebusError SmeConnectionAccessMethodsDeserialize(AccessMethods* sme_access_methods, const cJSON* access_methods_ar) {
    if (sme_access_methods == NULL) {
        return kEebusErrorInputArgument;
    }

    sme_access_methods->id          = NULL;
    sme_access_methods->dns_sd_mdns = false;
    sme_access_methods->dns.uri     = NULL;

    if (access_methods_ar == NULL) {
        return kEebusErrorInputArgument;
    }

    cJSON* const access_methods_ar_0 = cJSON_GetArrayItem(access_methods_ar, 0);

    bool ok = AccessMethodsParseId(sme_access_methods, access_methods_ar_0);

    cJSON* const access_methods_ar_1 = cJSON_GetArrayItem(access_methods_ar, 1);
    if (ok && (access_methods_ar_1 != NULL)) {
        if (cJSON_GetObjectItem(access_methods_ar_1, "dnsSd_mDns") != NULL) {
            ok = ok && AccessMethodsParseDnsSdMdns(sme_access_methods, access_methods_ar_1);
        } else if (cJSON_GetObjectItem(access_methods_ar_1, "Dns") != NULL) {
            ok = ok && AccessMethodsParseDns(sme_access_methods, access_methods_ar_1);
            ok = ok && (cJSON_GetArraySize(access_methods_ar) == 2);
        } else {
            ok = false;
        }
    }

    cJSON* const access_methods_ar_2 = cJSON_GetArrayItem(access_methods_ar, 2);
    if (ok && (access_methods_ar_2 != NULL)) {
        ok = ok && AccessMethodsParseDns(sme_access_methods, access_methods_ar_2);
    }

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

bool ConnectionCloseParsePhase(ConnectionClose* sme_close, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const phase_obj      = cJSON_GetObjectItem(json_obj, "phase");
    const char* const phase_str = cJSON_GetStringValue(phase_obj);
    if (phase_str == NULL) {
        return false;
    }

    if (!strcmp(phase_str, "announce")) {
        sme_close->phase = kConnectionClosePhaseTypeAnnounce;
    } else if (!strcmp(phase_str, "confirm")) {
        sme_close->phase = kConnectionClosePhaseTypeConfirm;
    } else {
        return false;
    }

    return true;
}

bool ConnectionCloseParseMaxTime(ConnectionClose* sme_close, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const max_time_obj = cJSON_GetObjectItem(json_obj, "maxTime");
    if (!cJSON_IsNumber(max_time_obj)) {
        return false;
    }

    sme_close->max_time_val = (uint32_t)cJSON_GetNumberValue(max_time_obj);
    sme_close->max_time     = &sme_close->max_time_val;
    return true;
}

EebusError ConnectionCloseParseReason(ConnectionClose* sme_close, cJSON* json_obj) {
    if (json_obj == NULL) {
        return false;
    }

    cJSON* const reason_obj  = cJSON_GetObjectItem(json_obj, "reason");
    const char* const reason = cJSON_GetStringValue(reason_obj);
    sme_close->reason        = StringCopy(reason);

    return sme_close->reason != NULL;
}

EebusError SmeCloseDeserialize(ConnectionClose* sme_close, const cJSON* close_ar) {
    if (sme_close == NULL) {
        return kEebusErrorInputArgument;
    }

    sme_close->reason   = NULL;
    sme_close->max_time = NULL;
    sme_close->reason   = NULL;

    if (close_ar == NULL) {
        return kEebusErrorInputArgument;
    }

    cJSON* const close_ar_0 = cJSON_GetArrayItem(close_ar, 0);

    bool ok = ConnectionCloseParsePhase(sme_close, close_ar_0);

    cJSON* const close_ar_1 = cJSON_GetArrayItem(close_ar, 1);
    if (ok && (close_ar_1 != NULL)) {
        if (cJSON_GetObjectItem(close_ar_1, "maxTime") != NULL) {
            ok = ok && ConnectionCloseParseMaxTime(sme_close, close_ar_1);
        } else if (cJSON_GetObjectItem(close_ar_1, "reason") != NULL) {
            ok = ok && ConnectionCloseParseReason(sme_close, close_ar_1);
            ok = ok && (cJSON_GetArraySize(close_ar) == 2);
        } else {
            ok = false;
        }
    }

    cJSON* const close_ar_2 = cJSON_GetArrayItem(close_ar, 2);
    if (ok && (close_ar_2 != NULL)) {
        ok = ok && ConnectionCloseParseReason(sme_close, close_ar_2);
    }

    return ok ? kEebusErrorOk : kEebusErrorParse;
}

EebusError DeserializeControlMessage(ShipMessageDeserialize* self, cJSON* json_root) {
    cJSON* json_ar = NULL;

    if ((json_ar = cJSON_GetObjectItem(json_root, "connectionHello")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(ConnectionHello));
        self->value_type = kSmeHello;
        return SmeHelloDeserialize((ConnectionHello*)self->value, json_ar);
    } else if ((json_ar = cJSON_GetObjectItem(json_root, "messageProtocolHandshake")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(MessageProtocolHandshake));
        self->value_type = kSmeProtocolHandshake;
        return SmeProtocolHandshakeDeserialize((MessageProtocolHandshake*)self->value, json_ar);
    } else if ((json_ar = cJSON_GetObjectItem(json_root, "messageProtocolHandshakeError")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(MessageProtocolHandshakeError));
        self->value_type = kSmeProtocolHandshakeError;
        return SmeProtocolHandshakeErrorDeserialize((MessageProtocolHandshakeError*)self->value, json_ar);
    } else if ((json_ar = cJSON_GetObjectItem(json_root, "connectionPinState")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(ConnectionPinState));
        self->value_type = kSmeConnectionPinState;
        return SmeConnectionPinStateDeserialize((ConnectionPinState*)self->value, json_ar);
    } else if ((json_ar = cJSON_GetObjectItem(json_root, "connectionPinInput")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(ConnectionPinInput));
        self->value_type = kSmeConnectionPinInput;
        return SmeConnectionPinInputDeserialize((ConnectionPinInput*)self->value, json_ar);
    } else if ((json_ar = cJSON_GetObjectItem(json_root, "connectionPinError")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(ConnectionPinError));
        self->value_type = kSmeConnectionPinError;
        return SmeConnectionPinErrorDeserialize((ConnectionPinError*)self->value, json_ar);
    } else if ((json_ar = cJSON_GetObjectItem(json_root, "accessMethodsRequest")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(AccessMethodsRequest));
        self->value_type = kSmeConnectionAccessMethodsRequest;
        return SmeConnectionAccessMethodsRequestDeserialize((AccessMethodsRequest*)self->value, json_ar);
    } else if ((json_ar = cJSON_GetObjectItem(json_root, "accessMethods")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(AccessMethods));
        self->value_type = kSmeConnectionAccessMethods;
        return SmeConnectionAccessMethodsDeserialize((AccessMethods*)self->value, json_ar);
    } else {
        return kEebusErrorInputArgument;
    }
}

EebusError DeserializeDataMessage(ShipMessageDeserialize* self, cJSON* json_root) {
    cJSON* json_ar = NULL;
    if ((json_ar = cJSON_GetObjectItem(json_root, "data")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(Data));
        self->value_type = kData;
        return DataDeserialize((Data*)self->value, json_ar);
    } else {
        return kEebusErrorInputArgument;
    }
}

EebusError DeserializeEndMessage(ShipMessageDeserialize* self, cJSON* json_root) {
    cJSON* json_ar = NULL;
    if ((json_ar = cJSON_GetObjectItem(json_root, "connectionClose")) != NULL) {
        self->value      = EEBUS_MALLOC(sizeof(ConnectionClose));
        self->value_type = kSmeClose;
        return SmeCloseDeserialize((ConnectionClose*)self->value, json_ar);
    } else {
        return kEebusErrorInputArgument;
    }
}

void Deserialize(ShipMessageDeserialize* self, MessageBuffer* buf) {
    if ((buf == NULL) || (buf->data == NULL) || (buf->data_size == 0)) {
        return;
    }

    uint8_t msg_type = buf->data[0];
    if ((msg_type > kMsgTypeEnd)) {
        return;
    }

    if (!ShipMessageToString(buf)) {
        return;
    }

    cJSON* const json_root = cJSON_Parse((const char*)buf->data);
    if (json_root == NULL) {
        return;
    }

    EebusError ret = kEebusErrorOk;
    if (msg_type == kMsgTypeControl) {
        ret = DeserializeControlMessage(self, json_root);
    } else if (msg_type == kMsgTypeData) {
        ret = DeserializeDataMessage(self, json_root);
    } else if (msg_type == kMsgTypeEnd) {
        ret = DeserializeEndMessage(self, json_root);
    } else {
        ret = kEebusErrorParse;
    }

    cJSON_Delete(json_root);

    if (ret != kEebusErrorOk) {
        DeserializeReset(self);
    }
}
