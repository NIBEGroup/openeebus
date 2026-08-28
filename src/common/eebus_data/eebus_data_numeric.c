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
 * @brief EEEBUS Data Numeric implementation
 */

#include "src/common/eebus_data/eebus_data_numeric.h"

#include <stdbool.h>
#include <stdint.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_simple.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"

#define CONVERT_JSON_TO_NUM(interface, json_obj, buf, buf_size) ((interface)->json_to_num(json_obj, buf, buf_size))

#define CONVERT_NUM_TO_JSON(interface, buf, buf_size) ((interface)->num_to_json(buf, buf_size))

#define JSON_NUM_CONV_DECL(name, type)                                                    \
    EebusError JsonToNum##type(const JsonObject* json_obj, void* buf, size_t buf_size) {  \
        if (buf_size != sizeof(type)) {                                                   \
            return kEebusErrorInputArgument;                                              \
        }                                                                                 \
                                                                                          \
        *(type*)buf = (type)JsonGetNumber(json_obj);                                      \
        return kEebusErrorOk;                                                             \
    }                                                                                     \
                                                                                          \
    JsonObject* NumToJson##type(const void* buf, size_t buf_size) {                       \
        return (buf_size == sizeof(type)) ? JsonCreateNumber((double)*(type*)buf) : NULL; \
    }                                                                                     \
                                                                                          \
    const JsonNumConvInterface name = {                                                   \
        .json_to_num = JsonToNum##type,                                                   \
        .num_to_json = NumToJson##type,                                                   \
    }

JSON_NUM_CONV_DECL(json_num_conv_uint8, uint8_t);
JSON_NUM_CONV_DECL(json_num_conv_uint16, uint16_t);
JSON_NUM_CONV_DECL(json_num_conv_uint32, uint32_t);
JSON_NUM_CONV_DECL(json_num_conv_uint64, uint64_t);

JSON_NUM_CONV_DECL(json_num_conv_int8, int8_t);
JSON_NUM_CONV_DECL(json_num_conv_int16, int16_t);
JSON_NUM_CONV_DECL(json_num_conv_int32, int32_t);
JSON_NUM_CONV_DECL(json_num_conv_int64, int64_t);

static EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_item);
static EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_item);

const EebusDataInterface eebus_data_numeric_methods = {
    .create_empty          = EebusDataBaseCreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = FromJsonObjectItem,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = ToJsonObjectItem,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .compare               = EebusDataSimpleCompare,
    .is_null               = EebusDataSimpleIsNull,
    .is_empty              = EebusDataSimpleIsEmpty,
    .has_identifiers       = EebusDataBaseHasIdentifiers,
    .selectors_match       = EebusDataBaseSelectorsMatch,
    .identifiers_match     = EebusDataBaseIdentifiersMatch,
    .read_elements         = EebusDataSimpleReadElements,
    .write                 = EebusDataSimpleWrite,
    .write_elements        = EebusDataBaseWriteElements,
    .write_partial         = EebusDataBaseWritePartial,
    .delete_elements       = EebusDataSimpleDeleteElements,
    .delete_partial        = EebusDataBaseDeletePartial,
    .delete_               = EebusDataSimpleDelete,
};

EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj) {
    if (!JsonIsNumber(json_obj)) {
        return kEebusErrorParse;
    }

    void* const buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
    if (buf == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    JsonNumConvInterface* const json_num_conv = (JsonNumConvInterface*)cfg->metadata;

    const EebusError ret = CONVERT_JSON_TO_NUM(json_num_conv, json_obj, buf, cfg->size);
    if (ret != kEebusErrorOk) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        return ret;
    }

    return kEebusErrorOk;
}

EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        *json_obj = NULL;
        return kEebusErrorOk;
    }

    JsonNumConvInterface* const json_num_conv = (JsonNumConvInterface*)cfg->metadata;

    *json_obj = CONVERT_NUM_TO_JSON(json_num_conv, *buf, cfg->size);
    return (*json_obj != NULL) ? kEebusErrorOk : kEebusErrorMemoryAllocate;
}
