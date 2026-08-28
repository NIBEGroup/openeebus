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
 * @brief SPINE Datagram Actuator Level data types declarations
 */

#ifndef SRC_SPINE_MODEL_ACTUATOR_LEVEL_TYPES_H_
#define SRC_SPINE_MODEL_ACTUATOR_LEVEL_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef int32_t ActuatorLevelFctType;

enum {
    kActuatorLevelFctTypeStart,              /**< "start" */
    kActuatorLevelFctTypeUp,                 /**< "up" */
    kActuatorLevelFctTypeDown,               /**< "down" */
    kActuatorLevelFctTypeStop,               /**< "stop" */
    kActuatorLevelFctTypePercentageAbsolute, /**< "percentageAbsolute" */
    kActuatorLevelFctTypePercentageRelative, /**< "percentageRelative" */
    kActuatorLevelFctTypeAbsolut,            /**< "absolut" */
    kActuatorLevelFctTypeRelative,           /**< "relative" */
};

typedef struct ActuatorLevelDataType ActuatorLevelDataType;

struct ActuatorLevelDataType {
    const ActuatorLevelFctType* function; /**< "function" */
    const ScaledNumberType* value;        /**< "value" */
};

typedef struct ActuatorLevelDataElementsType ActuatorLevelDataElementsType;

struct ActuatorLevelDataElementsType {
    ElementTagType function;               /**< "function" */
    const ScaledNumberElementsType* value; /**< "value" */
};

typedef struct ActuatorLevelDescriptionDataType ActuatorLevelDescriptionDataType;

struct ActuatorLevelDescriptionDataType {
    LabelType label;                                 /**< "label" */
    DescriptionType description;                     /**< "description" */
    const UnitOfMeasurementType* level_default_unit; /**< "levelDefaultUnit" */
};

typedef struct ActuatorLevelDescriptionDataElementsType ActuatorLevelDescriptionDataElementsType;

struct ActuatorLevelDescriptionDataElementsType {
    ElementTagType label;              /**< "label" */
    ElementTagType description;        /**< "description" */
    ElementTagType level_default_unit; /**< "levelDefaultUnit" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_ACTUATOR_LEVEL_TYPES_H_
