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
 * @brief Absolute or Relative Time utility
 */

#ifndef SRC_SPINE_MODEL_ABSOLUTE_OR_RELATIVE_TIME_H_
#define SRC_SPINE_MODEL_ABSOLUTE_OR_RELATIVE_TIME_H_

#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define ABSOLUTE_OR_RELATIVE_TIME_NOW                                                \
    (AbsoluteOrRelativeTimeType) {                                                   \
        .type = kAbsoluteOrRelativeTimeTypeDateTime, .date_time = EebusDateTimeNow() \
    }

/**
 * @brief Creates an AbsoluteOrRelativeTimeType pointer from an EebusDateTime pointer.
 * @note Cannot be returned from a function, as it is a pointer to a temporary object.
 */
#define ABSOLUTE_OR_RELATIVE_TIME_PTR(time)                             \
    ((time) == NULL) ? NULL : &(AbsoluteOrRelativeTimeType) {           \
        .type = kAbsoluteOrRelativeTimeTypeDateTime, .date_time = *time \
    }

#define ABSOLUTE_OR_RELATIVE_TIME_WITH_DURATION(dur)                 \
    (AbsoluteOrRelativeTimeType) {                                   \
        .type = kAbsoluteOrRelativeTimeTypeDuration, .duration = dur \
    }

/**
 * @brief Retrieves the absolute or relative time from the given AbsoluteOrRelativeTimeType object.
 *
 * This function extracts the time information stored in the provided
 * AbsoluteOrRelativeTimeType instance and returns it as an EebusDateTime object.
 *
 * @param self A pointer to an AbsoluteOrRelativeTimeType object containing the time data.
 * @return An EebusDateTime object representing the absolute or relative time.
 */
EebusDateTime AbsoluteOrRelativeTimeGetTime(const AbsoluteOrRelativeTimeType* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_ABSOLUTE_OR_RELATIVE_TIME_H_
