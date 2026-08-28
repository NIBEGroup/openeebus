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
 * @brief Scaled Value declarations and utilities implementation
 */

#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

#include "src/common/eebus_math/eebus_math.h"
#include "src/common/string_util.h"
#include "src/spine/model/common_data_types.h"
#include "src/use_case/model/scaled_value.h"

/* Maximum |scale| for which PowerOfTen() is safe: 10^18 < INT64_MAX < 10^19 */
static const int kMaxSafeScale = 18;

EebusError ScaledValueInitWithScaledNumber(ScaledValue* scaled_value, const ScaledNumberType* scaled_number) {
    if ((scaled_value == NULL) || (scaled_number == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    if (scaled_number->number == NULL) {
        return kEebusErrorInputArgument;
    }

    scaled_value->value = *scaled_number->number;
    scaled_value->scale = (scaled_number->scale == NULL) ? 0 : *scaled_number->scale;

    return kEebusErrorOk;
}

const char* ScaledValueToString(const ScaledValue* scaled_value) {
    if (scaled_value == NULL) {
        return NULL;
    }

    if ((scaled_value->scale == 0) || (scaled_value->value == 0)) {
        return StringFmtSprintf("%" PRId64, scaled_value->value);
    }

    if ((scaled_value->scale > kMaxSafeScale) || (scaled_value->scale < -kMaxSafeScale)) {
        return NULL;
    }

    if (scaled_value->scale > 0) {
        /* Append scale zeros — avoids int64_t overflow from multiplication */
        return StringFmtSprintf("%" PRId64 "%.*d", scaled_value->value, (int)scaled_value->scale, 0);
    }

    /* scale < 0: split into integer and fractional parts */
    /* Use int to avoid int8_t overflow when negating INT8_MIN (-128) */
    const int scale_abs        = -(int)scaled_value->scale;
    const int8_t scale_tmp     = (int8_t)scale_abs;
    const int64_t power_of_ten = PowerOfTen(scale_tmp);
    const int64_t value        = scaled_value->value / power_of_ten;
    int64_t fraction           = scaled_value->value % power_of_ten;
    if (fraction < 0) {
        fraction = -fraction;
    }

    if (fraction == 0) {
        return StringFmtSprintf("%" PRId64, value);
    }

    if ((value == 0) && (scaled_value->value < 0)) {
        return StringFmtSprintf("-%" PRId64 ".%.*" PRId64, value, (int)scale_tmp, fraction);
    }

    return StringFmtSprintf("%" PRId64 ".%.*" PRId64, value, (int)scale_tmp, fraction);
}

void ScaledValuePrint(const char* fmt, const ScaledValue* scaled_value) {
    if (scaled_value == NULL) {
        printf(fmt, "NULL");
        return;
    }

    char* scaled_value_str = (char*)ScaledValueToString(scaled_value);
    if (scaled_value_str != NULL) {
        printf(fmt, scaled_value_str);
    } else {
        printf(fmt, "<error converting to string>");
    }

    StringDelete(scaled_value_str);
}

EebusError ScaledValueParse(const char* s, ScaledValue* scaled_value) {
    if ((s == NULL) || (scaled_value == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    char* end_ptr          = NULL;
    const char* p          = s;
    const bool is_negative = (*p == '-');
    if ((*p == '-') || (*p == '+')) {
        p++;
    }

    if (!isdigit((int)*p)) {
        return kEebusErrorParse;
    }

    int64_t value = strtoll(p, &end_ptr, 10);
    if (end_ptr == p) {
        return kEebusErrorParse;
    }

    int8_t scale = 0;
    if (*end_ptr == '.') {
        end_ptr++;
        const char* frac_start = end_ptr;
        while (isdigit((int)*end_ptr)) {
            --scale;
            ++end_ptr;
        }

        if ((end_ptr == frac_start) || (*end_ptr != '\0')) {
            return kEebusErrorParse;
        }

        int64_t fraction = strtoll(frac_start, NULL, 10);
        if (fraction == 0) {
            scale = 0;
        } else {
            const int64_t power_of_ten = PowerOfTen(-scale);
            value *= power_of_ten;
            value += (value >= 0) ? fraction : -fraction;
        }
    }

    if (*end_ptr != '\0') {
        return kEebusErrorParse;
    }

    if (is_negative && (value > 0)) {
        value = -value;
    }

    scaled_value->value = value;
    scaled_value->scale = scale;

    return kEebusErrorOk;
}

static int64_t RoundToInt64(double d) {
    return (int64_t)(d >= 0.0 ? d + 0.5 : d - 0.5);
}

EebusError ScaledValueToDouble(const ScaledValue* scaled_value, double* value) {
    if ((scaled_value == NULL) || (value == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    if (scaled_value->value == 0) {
        *value = 0.0;
        return kEebusErrorOk;
    }

    if ((scaled_value->scale > kMaxSafeScale) || (scaled_value->scale < -kMaxSafeScale)) {
        return kEebusErrorInputArgumentOutOfRange;
    }

    if (scaled_value->scale >= 0) {
        *value = (double)scaled_value->value * (double)PowerOfTen(scaled_value->scale);
    } else {
        const int8_t scale = (int8_t)(-scaled_value->scale);
        *value             = (double)scaled_value->value / (double)PowerOfTen(scale);
    }

    return kEebusErrorOk;
}

EebusError ScaledValueWithDouble(ScaledValue* scaled_value, double value) {
    if (scaled_value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    /* Most negative scale produced: value is multiplied by 10^4 then trailing zeros
     * are stripped, so precision is at most 4 decimal places */
    static const int8_t kMaxNegativeScale = -4;

    const int64_t scale_factor = PowerOfTen(-kMaxNegativeScale);
    const int64_t max_abs_int  = INT64_MAX / scale_factor;
    const double max_abs_value = (double)max_abs_int;

    if ((value != value) || (value < -max_abs_value) || (value > max_abs_value)) {
        return kEebusErrorInputArgumentOutOfRange;
    }

    const double scaled     = value * (double)scale_factor;
    const double scaled_max = max_abs_value * (double)scale_factor;
    if ((scaled != scaled) || (scaled > scaled_max) || (scaled < -scaled_max)) {
        return kEebusErrorInputArgumentOutOfRange;
    }

    int64_t int_value = RoundToInt64(scaled);
    int8_t scale      = kMaxNegativeScale;

    while (scale < 0 && int_value % 10 == 0) {
        int_value /= 10;
        ++scale;
    }

    scaled_value->value = int_value;
    scaled_value->scale = scale;
    return kEebusErrorOk;
}
