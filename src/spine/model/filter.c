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

#include "src/spine/model/filter.h"

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_sequence.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/model.h"

const CmdControlType ctrl_partial = {.partial = EEBUS_TAG_SET, .delete_ = EEBUS_TAG_RESET};
const CmdControlType ctrl_delete  = {.partial = EEBUS_TAG_RESET, .delete_ = EEBUS_TAG_SET};

static FilterType* FilterCreate(
    FunctionType function_type,
    const FilterIdType* filter_id,
    const CmdControlType* cmd_control,
    const void* selectors,
    const void* elements
);

FilterType* FilterCreate(
    FunctionType function_type,
    const FilterIdType* filter_id,
    const CmdControlType* cmd_control,
    const void* selectors,
    const void* elements
) {
    FilterType filter_tmp = {
        .filter_id                     = filter_id,
        .cmd_ctrl                      = cmd_control,
        .data_selectors_choice         = selectors,
        .data_selectors_choice_type_id = function_type,
        .data_elements_choice          = elements,
        .data_elements_choice_type_id  = function_type,
    };

    const EebusDataCfg* const cfg  = GetFilterCfg();
    const FilterType* p_filter_tmp = &filter_tmp;

    FilterType* filter = NULL;
    if (EEBUS_DATA_COPY(cfg, &p_filter_tmp, &filter) != kEebusErrorOk) {
        EEBUS_DATA_DELETE(cfg, &filter);
        return NULL;
    }

    return filter;
}

FilterType* FilterPartialCreate(
    FunctionType function_type,
    const FilterIdType* filter_id,
    const void* selectors,
    const void* elements
) {
    return FilterCreate(function_type, filter_id, &ctrl_partial, selectors, elements);
}

FilterType* FilterDeleteCreate(
    FunctionType function_type,
    const FilterIdType* filter_id,
    const void* selectors,
    const void* elements
) {
    return FilterCreate(function_type, filter_id, &ctrl_delete, selectors, elements);
}

void FilterDelete(FilterType* filter) {
    const EebusDataCfg* const cfg = GetFilterCfg();
    EEBUS_DATA_DELETE(cfg, &filter);
}

FilterTypeType FilterGetType(const FilterType* filter) {
    if ((filter == NULL) || (filter->cmd_ctrl == NULL)) {
        return kFilterTypeUndefined;
    }

    const bool is_partial = EEBUS_TAG_TO_BOOL(filter->cmd_ctrl->partial);
    const bool is_delete  = EEBUS_TAG_TO_BOOL(filter->cmd_ctrl->delete_);

    if (is_partial && is_delete) {
        return kFilterTypeUndefined;
    } else if (is_partial) {
        return kFilterTypePartial;
    } else if (is_delete) {
        return kFilterTypeDelete;
    } else {
        return kFilterTypeUndefined;
    }
}
