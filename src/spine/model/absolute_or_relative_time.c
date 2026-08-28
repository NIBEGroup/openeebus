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
 * @brief Absolute or Relative Time utility implementation
 */

#include "src/spine/model/common_data_types.h"

EebusDateTime AbsoluteOrRelativeTimeGetTime(const AbsoluteOrRelativeTimeType* self) {
    if (self->type == kAbsoluteOrRelativeTimeTypeDateTime) {
        // Absolute time
        return self->date_time;
    } else {
        // Relative time
        EebusDateTime date_time = EebusDateTimeNow();
        EebusDateTimeAddDuration(&date_time, &self->duration);
        return date_time;
    }
}
