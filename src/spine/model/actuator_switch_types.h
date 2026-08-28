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
 * @brief SPINE Datagram Actuator Switch data types declarations
 */

#ifndef SRC_SPINE_MODEL_ACTUATOR_SWITCH_TYPES_H_
#define SRC_SPINE_MODEL_ACTUATOR_SWITCH_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef int32_t ActuatorSwitchFctType;

enum {
    kActuatorSwitchFctTypeOn,     /**< "on" */
    kActuatorSwitchFctTypeOff,    /**< "off" */
    kActuatorSwitchFctTypeToggle, /**< "toggle" */
};

typedef struct ActuatorSwitchDataType ActuatorSwitchDataType;

struct ActuatorSwitchDataType {
    const ActuatorSwitchFctType* function; /**< "function" */
};

typedef struct ActuatorSwitchDataElementsType ActuatorSwitchDataElementsType;

struct ActuatorSwitchDataElementsType {
    ElementTagType function; /**< "function" */
};

typedef struct ActuatorSwitchDescriptionDataType ActuatorSwitchDescriptionDataType;

struct ActuatorSwitchDescriptionDataType {
    LabelType label;             /**< "label" */
    DescriptionType description; /**< "description" */
};

typedef struct ActuatorSwitchDescriptionDataElementsType ActuatorSwitchDescriptionDataElementsType;

struct ActuatorSwitchDescriptionDataElementsType {
    ElementTagType label;       /**< "label" */
    ElementTagType description; /**< "description" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_ACTUATOR_SWITCH_TYPES_H_
