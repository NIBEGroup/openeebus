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
 * @brief SPINE Command frame type declaration
 */

#ifndef SRC_SPINE_MODEL_COMMAND_FRAME_H_
#define SRC_SPINE_MODEL_COMMAND_FRAME_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint64_t MsgCounterType;

typedef int32_t CommandClassifierType;

enum {
    kCommandClassifierTypeRead,   /**< "read" */
    kCommandClassifierTypeReply,  /**< "reply" */
    kCommandClassifierTypeNotify, /**< "notify" */
    kCommandClassifierTypeWrite,  /**< "write" */
    kCommandClassifierTypeCall,   /**< "call" */
    kCommandClassifierTypeResult, /**< "result" */
};

typedef struct CmdControlType CmdControlType;

struct CmdControlType {
    const ElementTagType delete_; /**< "delete" */
    const ElementTagType partial; /**< "partial" */
};

typedef uint32_t FilterIdType;

typedef struct FilterType FilterType;

struct FilterType {
    const FilterIdType* filter_id;  /**< "filterId" */
    const CmdControlType* cmd_ctrl; /**< "cmdControl" */

    // DataSelectorsChoiceGroup
    const void* data_selectors_choice;
    int32_t data_selectors_choice_type_id;

    // DataElementsChoiceGroup
    const void* data_elements_choice;
    int32_t data_elements_choice_type_id;
};

typedef struct CmdType CmdType;

struct CmdType {
    const FunctionType* function; /**< "function" */

    // CmdOptionGroup
    const FilterType* const* filter; /**< "filter" */
    size_t filter_size;

    // DataChoiceGroup
    const void* data_choice;
    int32_t data_choice_type_id;

    // DataExtendGroup
    const AbsoluteOrRelativeTimeType* last_update_at; /**< "lastUpdateAt" */
    const char* manufacturer_specific_extension;      /**< "manufacturerSpecificExtension" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_COMMAND_FRAME_H_
