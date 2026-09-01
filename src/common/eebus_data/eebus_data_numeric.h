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
 * @brief EEBUS Data Numeric "classes" related declarations
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_NUMERIC_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_NUMERIC_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_malloc.h"
#include "src/common/json.h"

/**
 * @brief EEBUS Data Numeric Interface
 */
extern const EebusDataInterface eebus_data_numeric_methods;

/**
 * @brief Json Numeric Converter interface type definition
 */
typedef struct JsonNumConvInterface JsonNumConvInterface;

/**
 * @brief Json Numeric Converter interface type definition
 */
struct JsonNumConvInterface {
    EebusError (*json_to_num)(const JsonObject* json_obj, void* buf, size_t buf_size);
    JsonObject* (*num_to_json)(const void* buf, size_t buf_size);
};

/**
 * @defgroup JsonNumberCovertors Json Numeric Covertors
 * @{
 */
extern const JsonNumConvInterface json_num_conv_uint8;
extern const JsonNumConvInterface json_num_conv_uint16;
extern const JsonNumConvInterface json_num_conv_uint32;
extern const JsonNumConvInterface json_num_conv_uint64;
extern const JsonNumConvInterface json_num_conv_int8;
extern const JsonNumConvInterface json_num_conv_int16;
extern const JsonNumConvInterface json_num_conv_int32;
extern const JsonNumConvInterface json_num_conv_int64;
/** } */

/**
 * @brief EEBUS Data Numeric type check
 */
#define EEBUS_DATA_IS_NUMERIC(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_numeric_methods)

/**
 * @brief Numeric Data configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure
 * @param conv Numeric data Json converter
 */
#define EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, conv)     \
    {                                                                    \
        .interface_ = &eebus_data_numeric_methods,                       \
        .name       = ed_name,                                           \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),   \
        .size       = sizeof(*STRUCT_MEMBER(struct_name, struct_field)), \
        .metadata   = conv,                                              \
    }

/**
 * @brief Numeric Data configuration with specific flags
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure
 * @param conv Numeric data Json converter
 * @param ed_flags EebusData flags to be applied (see @EebusDataFlag)
 */
#define EEBUS_DATA_NUMERIC_WITH_FLAGS(ed_name, struct_name, struct_field, ed_flags, conv) \
    {                                                                                     \
        .interface_ = &eebus_data_numeric_methods,                                        \
        .name       = ed_name,                                                            \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),                    \
        .size       = sizeof(*STRUCT_MEMBER(struct_name, struct_field)),                  \
        .flags      = ed_flags,                                                           \
        .metadata   = conv,                                                               \
    }

/**
 * @brief EEBUS Data Uint8 configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be uint8_t*
 */
#define EEBUS_DATA_UINT8(ed_name, struct_name, struct_field) \
    EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, &json_num_conv_uint8)

/**
 * @brief EEBUS Data Uint16 configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be uint16_t*
 */
#define EEBUS_DATA_UINT16(ed_name, struct_name, struct_field) \
    EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, &json_num_conv_uint16)

/**
 * @brief EEBUS Data Uint32 configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be uint32_t*
 */
#define EEBUS_DATA_UINT32(ed_name, struct_name, struct_field) \
    EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, &json_num_conv_uint32)

/**
 * @brief EEBUS Data Uint32 configuration with specific flags
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be uint32_t*
 * @param ed_flags EebusData flags to be applied (see @EebusDataFlag)
 */
#define EEBUS_DATA_UINT32_WITH_FLAGS(ed_name, struct_name, struct_field, ed_flags) \
    EEBUS_DATA_NUMERIC_WITH_FLAGS(ed_name, struct_name, struct_field, ed_flags, &json_num_conv_uint32)

/**
 * @brief EEBUS Data Uint64 configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be uint64_t*
 */
#define EEBUS_DATA_UINT64(ed_name, struct_name, struct_field) \
    EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, &json_num_conv_uint64)

/**
 * @brief EEBUS Data Int8 configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be int8_t*
 */
#define EEBUS_DATA_INT8(ed_name, struct_name, struct_field) \
    EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, &json_num_conv_int8)

/**
 * @brief EEBUS Data Int16 configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be int16_t*
 */
#define EEBUS_DATA_INT16(ed_name, struct_name, struct_field) \
    EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, &json_num_conv_int16)

/**
 * @brief EEBUS Data Int32 configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be int32_t*
 */
#define EEBUS_DATA_INT32(ed_name, struct_name, struct_field) \
    EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, &json_num_conv_int32)

/**
 * @brief EEBUS Data Int64 configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be int64_t*
 */
#define EEBUS_DATA_INT64(ed_name, struct_name, struct_field) \
    EEBUS_DATA_NUMERIC(ed_name, struct_name, struct_field, &json_num_conv_int64)

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_NUMERIC_H_
