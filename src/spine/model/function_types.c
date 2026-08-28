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
 * @brief SPINE Datagram Function payload functions implementation
 */

#include "src/spine/model/function_types.h"

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/model.h"

const FunctionPropertyType*
FunctionPropertyCreate(FunctionType function_type, const PossibleOperationsType* operations) {
    const FunctionPropertyType function_property_tmp = {
        .function            = &function_type,
        .possible_operations = operations,
    };

    const FunctionPropertyType* p_function_property_tmp = &function_property_tmp;
    const FunctionPropertyType* p_function_property     = NULL;
    EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(FunctionPropertyType, ModelGetFunctionPropertyCfg());
    EEBUS_DATA_COPY(&cfg, &p_function_property_tmp, &p_function_property);
    return p_function_property;
}

void FunctionPropertyDelete(FunctionPropertyType* self) {
    EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(FunctionPropertyType, ModelGetFunctionPropertyCfg());
    EEBUS_DATA_DELETE(&cfg, &self);
}

void FunctionPropertyElementsDelete(FunctionPropertyElementsType* self) {
    EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(FunctionPropertyElementsType, ModelGetFunctionPropertyElementsCfg());
    EEBUS_DATA_DELETE(&cfg, &self);
}
