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
 * @brief SPINE Datagram Filter helper subroutines
 */

#ifndef SRC_EEBUS_SRC_SPINE_MODEL_FILTER_H_
#define SRC_EEBUS_SRC_SPINE_MODEL_FILTER_H_

#include "src/spine/model/command_frame_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum FilterTypeType {
    kFilterTypeUndefined = -1,
    kFilterTypePartial,
    kFilterTypeDelete,
};

typedef enum FilterTypeType FilterTypeType;

extern const CmdControlType ctrl_partial;
extern const CmdControlType ctrl_delete;

#define FILTER(function_type, ctrl, id, selectors, elements)                                  \
    (FilterType) {                                                                            \
        .filter_id = (id), .cmd_ctrl = (ctrl), .data_selectors_choice = (selectors),          \
        .data_selectors_choice_type_id = (function_type), .data_elements_choice = (elements), \
        .data_elements_choice_type_id = (function_type),                                      \
    }

#define FILTER_PARTIAL(function_type, filter_id, selectors, elements) \
    FILTER(function_type, &ctrl_partial, filter_id, selectors, elements)

#define FILTER_DELETE(function_type, filter_id, selectors, elements) \
    FILTER(function_type, &ctrl_delete, filter_id, selectors, elements)

FilterType* FilterPartialCreate(
    FunctionType function_type,
    const FilterIdType* filter_id,
    const void* selectors,
    const void* elements
);

FilterType* FilterDeleteCreate(
    FunctionType function_type,
    const FilterIdType* filter_id,
    const void* selectors,
    const void* elements
);

void FilterDelete(FilterType* filter);

FilterTypeType FilterGetType(const FilterType* filter);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_SPINE_MODEL_FILTER_H_
