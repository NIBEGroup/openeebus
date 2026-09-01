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
 * @brief EEEBUS Data Absolute or Relative Time implementation
 */

#include "src/common/eebus_data/eebus_data_date_time.h"

#include <stdbool.h>
#include <stdint.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_simple.h"
#include "src/common/eebus_date_time/eebus_date.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/common/eebus_date_time/eebus_time.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/spine/model/common_data_types.h"

#define DATE_TIME_PARSE(interface, s, buf, buf_size) ((interface)->parse(s, buf, buf_size))

#define DATE_TIME_TO_STRING(interface, buf, buf_size) ((interface)->to_string(buf, buf_size))

#define DATE_TIME_PARSE_DECL(name, type)                                       \
    static EebusError Parse##type(const char* s, void* buf, size_t buf_size) { \
        if (buf_size != sizeof(type)) {                                        \
            return kEebusErrorInputArgument;                                   \
        }                                                                      \
                                                                               \
        return type##Parse(s, (type*)buf);                                     \
    }                                                                          \
                                                                               \
    static char* ToString##type(const void* buf, size_t buf_size) {            \
        if (buf_size != sizeof(type)) {                                        \
            return NULL;                                                       \
        }                                                                      \
                                                                               \
        return type##ToString((const type*)buf);                               \
    }                                                                          \
                                                                               \
    const DateTimeParseInterface name = {                                      \
        .parse     = Parse##type,                                              \
        .to_string = ToString##type,                                           \
    };

DATE_TIME_PARSE_DECL(duration_parser, EebusDuration);
DATE_TIME_PARSE_DECL(date_parser, EebusDate);
DATE_TIME_PARSE_DECL(time_parser, EebusTime);
DATE_TIME_PARSE_DECL(date_time_parser, EebusDateTime);

static EebusError ParseAbsoluteOrRelativeTime(const char* s, void* buf, size_t buf_size) {
    if (buf_size != sizeof(AbsoluteOrRelativeTimeType)) {
        return kEebusErrorInputArgument;
    }

    AbsoluteOrRelativeTimeType* const time_buf = (AbsoluteOrRelativeTimeType*)buf;
    if (EebusDurationParse(s, &time_buf->duration) == kEebusErrorOk) {
        time_buf->type = kAbsoluteOrRelativeTimeTypeDuration;
        return kEebusErrorOk;
    } else if (EebusDateTimeParse(s, &time_buf->date_time) == kEebusErrorOk) {
        time_buf->type = kAbsoluteOrRelativeTimeTypeDateTime;
        return kEebusErrorOk;
    } else {
        return kEebusErrorParse;
    }
}

static char* ToStringAbsoluteOrRelativeTime(const void* buf, size_t buf_size) {
    if (buf_size != sizeof(AbsoluteOrRelativeTimeType)) {
        return NULL;
    }

    const AbsoluteOrRelativeTimeType* const time_buf = (AbsoluteOrRelativeTimeType*)buf;
    if (time_buf->type == kAbsoluteOrRelativeTimeTypeDuration) {
        return EebusDurationToString(&time_buf->duration);
    } else if ((time_buf->type == kAbsoluteOrRelativeTimeTypeDateTime)) {
        return EebusDateTimeToString(&time_buf->date_time);
    } else {
        return NULL;  // Invalid type
    }
}

const DateTimeParseInterface absolute_or_relative_time_parser = {
    .parse     = ParseAbsoluteOrRelativeTime,
    .to_string = ToStringAbsoluteOrRelativeTime,
};

static EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_item);
static EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_item);

const EebusDataInterface eebus_data_absolute_or_relative_time_methods = {
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
    if (!JsonIsString(json_obj)) {
        return kEebusErrorParse;
    }

    void* const buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
    if (buf == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    const DateTimeParseInterface* const parser = (const DateTimeParseInterface*)cfg->metadata;

    const char* s = JsonGetString(json_obj);
    if (DATE_TIME_PARSE(parser, s, buf, cfg->size) != kEebusErrorOk) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        return kEebusErrorParse;
    }

    return kEebusErrorOk;
}

EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        *json_obj = NULL;
        return kEebusErrorOk;
    }

    const DateTimeParseInterface* const parser = (const DateTimeParseInterface*)cfg->metadata;

    const char* const s = DATE_TIME_TO_STRING(parser, *buf, cfg->size);

    *json_obj = JsonCreateString(s);
    StringDelete((char*)s);
    return (*json_obj != NULL) ? kEebusErrorOk : kEebusErrorMemoryAllocate;
}
