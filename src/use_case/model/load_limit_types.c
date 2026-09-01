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
 * @brief Load Limit helper functions implementation
 */

#include <stdio.h>

#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_errors.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/model/scaled_value.h"

EebusError LoadLimitInitWithLoadControlLimitData(LoadLimit* self, const LoadControlLimitDataType* limit_data) {
    if (!LoadControlLimitIsValid(limit_data)) {
        return kEebusErrorOther;
    }

    const ScaledNumberType* const power_limit_value = LoadControlLimitGetValue(limit_data);

    const EebusError err = ScaledValueInitWithScaledNumber(&self->value, power_limit_value);
    if (err != kEebusErrorOk) {
        return err;
    }

    self->is_changeable = LoadControlLimitIsLimitChangeable(limit_data);
    self->is_active     = LoadControlLimitIsActive(limit_data);

    return LoadControlLimitGetDuration(limit_data, &self->duration, &self->delete_duration);
}

void LoadLimitPrint(const LoadLimit* self) {
    printf("{\n");
    ScaledValuePrint("  value         = %s,\n", &self->value);
    EebusDurationPrint("  duration      = %s\n", &self->duration);
    printf("  is_changeable = %s\n", self->is_changeable ? "true" : "false");
    printf("  is_active     = %s\n", self->is_active ? "true" : "false");
    printf("}\n");
}
