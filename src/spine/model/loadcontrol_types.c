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
 * @brief SPINE Datagram Loadcontrol functions implementation
 */

#include "src/spine/model/loadcontrol_types.h"

#include "src/common/eebus_errors.h"

bool LoadControlLimitIsValid(const LoadControlLimitDataType* limit) {
    if (limit == NULL) {
        return false;
    }

    return (limit->limit_id != NULL) && (limit->value != NULL) && ((limit->value->number != NULL));
}

bool LoadControlLimitIsLimitChangeable(const LoadControlLimitDataType* limit) {
    if (limit == NULL) {
        return false;
    }

    if (limit->is_limit_changeable == NULL) {
        return false;
    }

    return *limit->is_limit_changeable;
}

bool LoadControlLimitIsActive(const LoadControlLimitDataType* limit) {
    if (limit == NULL) {
        return false;
    }

    if (limit->is_limit_active == NULL) {
        return false;
    }

    return *limit->is_limit_active;
}

EebusError LoadControlLimitGetDuration(const LoadControlLimitDataType* limit, DurationType* duration, bool* is_null) {
    if ((duration == NULL) || (is_null == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    *is_null = (limit->time_period == NULL) || (limit->time_period->end_time == NULL);
    if (*is_null) {
        return kEebusErrorOk;
    }

    if (limit->time_period->end_time->type != kAbsoluteOrRelativeTimeTypeDuration) {
        return kEebusErrorNoChange;
    }

    *duration = limit->time_period->end_time->duration;
    return kEebusErrorOk;
}

const ScaledNumberType* LoadControlLimitGetValue(const LoadControlLimitDataType* limit) {
    if (limit == NULL) {
        return NULL;
    }

    return limit->value;
}
