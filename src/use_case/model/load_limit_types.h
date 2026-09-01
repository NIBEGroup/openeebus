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
 * @brief Load Limit data types declarations
 */

#ifndef SRC_USE_CASE_MODEL_LOAD_LIMIT_TYPES_H_
#define SRC_USE_CASE_MODEL_LOAD_LIMIT_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/spine/model/common_data_types.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/model/scaled_value.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brif Load Control Limit (LPC) data
 */
typedef struct LoadLimit LoadLimit;

struct LoadLimit {
    ScaledValue value;     /**< Limit value */
    DurationType duration; /**< Duration of the limit */
    bool is_changeable;    /**< If the value can be changed via write, ignored when writing data */
    bool is_active;        /**< If the limit is active */
    bool delete_duration;  /**< If the Duration (TimePeriod in SPINE) is absent or should be deleted.
                              On read: true means no time period was present (unlimited duration).
                              On write: true means the time period should be removed. Relevant for LPC & LPP only) */
};

/**
 * @brief Initialize LoadLimit structure from LoadControlLimitDataType structure
 *
 * @param self LoadLimit structure to be initialized
 * @param limit_data LoadControlLimitDataType structure to initialize from
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError LoadLimitInitWithLoadControlLimitData(LoadLimit* self, const LoadControlLimitDataType* limit_data);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_MODEL_LOAD_LIMIT_TYPES_H_
