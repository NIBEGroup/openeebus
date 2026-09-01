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
 * @brief EEBUS Data Absolute or Relative Time "class" related declarations
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME_H_

#include "src/common/api/eebus_data_interface.h"

/**
 * @brief EEBUS Data Absolute or Relative Time Interface
 */
extern const EebusDataInterface eebus_data_absolute_or_relative_time_methods;

/**
 * @brief Json Date Time Converter interface type definition
 */
typedef struct DateTimeParseInterface DateTimeParseInterface;

/**
 * @brief Json Date Time Converter interface type definition
 */
struct DateTimeParseInterface {
    EebusError (*parse)(const char* s, void* buf, size_t buf_size);
    char* (*to_string)(const void* buf, size_t buf_size);
};

/**
 * @defgroup DateTimeParsers Date & Time parsers
 * @{
 */
extern const DateTimeParseInterface duration_parser;
extern const DateTimeParseInterface date_parser;
extern const DateTimeParseInterface time_parser;
extern const DateTimeParseInterface date_time_parser;
extern const DateTimeParseInterface absolute_or_relative_time_parser;
/** } */

/**
 * @brief EEBUS Data Absolute or Relative Time type check
 */
#define EEBUS_DATA_IS_ABSOLUTE_OR_RELATIVE_TIME(cfg) \
    (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_absolute_or_relative_time_methods)

/**
 * @brief A helper definition to declare Date & Time configurations
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * @param parser Specific parser to be used (duration, date, time, etc.)
 * are used to calcualte the offset of filed within structure
 */
#define EEBUS_DATA_DATE_TIME_TEMPLATE(ed_name, struct_name, struct_field, parser) \
    {                                                                             \
        .interface_ = &eebus_data_absolute_or_relative_time_methods,              \
        .name       = ed_name,                                                    \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),            \
        .size       = sizeof(*STRUCT_MEMBER(struct_name, struct_field)),          \
        .metadata   = parser,                                                     \
    }

/**
 * @brief Duration Data configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure
 */
#define EEBUS_DATA_DURATION(ed_name, struct_name, struct_field) \
    EEBUS_DATA_DATE_TIME_TEMPLATE(ed_name, struct_name, struct_field, &duration_parser)

/**
 * @brief Time Data configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure
 */
#define EEBUS_DATA_TIME(ed_name, struct_name, struct_field) \
    EEBUS_DATA_DATE_TIME_TEMPLATE(ed_name, struct_name, struct_field, &time_parser)

/**
 * @brief Date Data configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure
 */
#define EEBUS_DATA_DATE(ed_name, struct_name, struct_field) \
    EEBUS_DATA_DATE_TIME_TEMPLATE(ed_name, struct_name, struct_field, &date_parser)

/**
 * @brief Date & Time Data configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure
 */
#define EEBUS_DATA_DATE_TIME(ed_name, struct_name, struct_field) \
    EEBUS_DATA_DATE_TIME_TEMPLATE(ed_name, struct_name, struct_field, &date_time_parser)

/**
 * @brief Absolute or Relative Time Data configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure
 */
#define EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME(ed_name, struct_name, struct_field) \
    EEBUS_DATA_DATE_TIME_TEMPLATE(ed_name, struct_name, struct_field, &absolute_or_relative_time_parser)

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_EEBUS_DATA_ABSOLUTE_OR_RELATIVE_TIME_H_
