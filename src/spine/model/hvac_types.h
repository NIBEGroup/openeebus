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
 * @brief SPINE Datagram Hvac data types declarations
 */

#ifndef SRC_SPINE_MODEL_HVAC_TYPES_H_
#define SRC_SPINE_MODEL_HVAC_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/power_sequences_types.h"
#include "src/spine/model/setpoint_types.h"
#include "src/spine/model/timetable_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t HvacSystemFunctionIdType;

typedef int32_t HvacSystemFunctionTypeType;

enum {
    kHvacSystemFunctionTypeTypeHeating,     /**< "heating" */
    kHvacSystemFunctionTypeTypeCooling,     /**< "cooling" */
    kHvacSystemFunctionTypeTypeVentilation, /**< "ventilation" */
    kHvacSystemFunctionTypeTypeDhw,         /**< "dhw" */
};

typedef uint32_t HvacOperationModeIdType;

typedef int32_t HvacOperationModeTypeType;

enum {
    kHvacOperationModeTypeTypeAuto, /**< "auto" */
    kHvacOperationModeTypeTypeOn,   /**< "on" */
    kHvacOperationModeTypeTypeOff,  /**< "off" */
    kHvacOperationModeTypeTypeEco,  /**< "eco" */
};

typedef uint32_t HvacOverrunIdType;

typedef int32_t HvacOverrunTypeType;

enum {
    kHvacOverrunTypeTypeOneTimeDhw,         /**< "oneTimeDhw" */
    kHvacOverrunTypeTypeParty,              /**< "party" */
    kHvacOverrunTypeTypeSgReadyCondition1,  /**< "sgReadyCondition1" */
    kHvacOverrunTypeTypeSgReadyCondition3,  /**< "sgReadyCondition3" */
    kHvacOverrunTypeTypeSgReadyCondition4,  /**< "sgReadyCondition4" */
    kHvacOverrunTypeTypeOneDayAway,         /**< "oneDayAway" */
    kHvacOverrunTypeTypeOneDayAtHome,       /**< "oneDayAtHome" */
    kHvacOverrunTypeTypeOneTimeVentilation, /**< "oneTimeVentilation" */
    kHvacOverrunTypeTypeHvacSystemOff,      /**< "hvacSystemOff" */
    kHvacOverrunTypeTypeValveKick,          /**< "valveKick" */
};

typedef int32_t HvacOverrunStatusType;

enum {
    kHvacOverrunStatusTypeActive,   /**< "active" */
    kHvacOverrunStatusTypeRunning,  /**< "running" */
    kHvacOverrunStatusTypeFinished, /**< "finished" */
    kHvacOverrunStatusTypeInactive, /**< "inactive" */
};

typedef struct HvacSystemFunctionDataType HvacSystemFunctionDataType;

struct HvacSystemFunctionDataType {
    const HvacSystemFunctionIdType* system_function_id;       /**< "systemFunctionId" */
    const HvacOperationModeIdType* current_operation_mode_id; /**< "currentOperationModeId" */
    const bool* is_operation_mode_id_changeable;              /**< "isOperationModeIdChangeable" */
    const SetpointIdType* current_setpoint_id;                /**< "currentSetpointId" */
    const bool* is_setpoint_id_changeable;                    /**< "isSetpointIdChangeable" */
    const bool* is_overrun_active;                            /**< "isOverrunActive" */
};

typedef struct HvacSystemFunctionDataElementsType HvacSystemFunctionDataElementsType;

struct HvacSystemFunctionDataElementsType {
    ElementTagType system_function_id;              /**< "systemFunctionId" */
    ElementTagType current_operation_mode_id;       /**< "currentOperationModeId" */
    ElementTagType is_operation_mode_id_changeable; /**< "isOperationModeIdChangeable" */
    ElementTagType current_setpoint_id;             /**< "currentSetpointId" */
    ElementTagType is_setpoint_id_changeable;       /**< "isSetpointIdChangeable" */
    ElementTagType is_overrun_active;               /**< "isOverrunActive" */
};

typedef struct HvacSystemFunctionListDataType HvacSystemFunctionListDataType;

struct HvacSystemFunctionListDataType {
    const HvacSystemFunctionDataType* const* hvac_system_function_data; /**< "hvacSystemFunctionData" */
    size_t hvac_system_function_data_size;
};

typedef struct HvacSystemFunctionListDataSelectorsType HvacSystemFunctionListDataSelectorsType;

struct HvacSystemFunctionListDataSelectorsType {
    const HvacSystemFunctionIdType* const* system_function_id; /**< "systemFunctionId" */
    size_t system_function_id_size;
};

typedef struct HvacSystemFunctionOperationModeRelationDataType HvacSystemFunctionOperationModeRelationDataType;

struct HvacSystemFunctionOperationModeRelationDataType {
    const HvacSystemFunctionIdType* system_function_id; /**< "systemFunctionId" */
    const HvacOperationModeIdType* operation_mode_id;   /**< "operationModeId" */
};

typedef struct HvacSystemFunctionOperationModeRelationDataElementsType
    HvacSystemFunctionOperationModeRelationDataElementsType;

struct HvacSystemFunctionOperationModeRelationDataElementsType {
    ElementTagType system_function_id; /**< "systemFunctionId" */
    ElementTagType operation_mode_id;  /**< "operationModeId" */
};

typedef struct HvacSystemFunctionOperationModeRelationListDataType HvacSystemFunctionOperationModeRelationListDataType;

struct HvacSystemFunctionOperationModeRelationListDataType {
    const HvacSystemFunctionOperationModeRelationDataType* const*
        hvac_system_function_operation_mode_relation_data; /**< "hvacSystemFunctionOperationModeRelationData" */
    size_t hvac_system_function_operation_mode_relation_data_size;
};

typedef struct HvacSystemFunctionOperationModeRelationListDataSelectorsType
    HvacSystemFunctionOperationModeRelationListDataSelectorsType;

struct HvacSystemFunctionOperationModeRelationListDataSelectorsType {
    const HvacSystemFunctionIdType* const* system_function_id; /**< "systemFunctionId" */
    size_t system_function_id_size;
};

typedef struct HvacSystemFunctionSetpointRelationDataType HvacSystemFunctionSetpointRelationDataType;

struct HvacSystemFunctionSetpointRelationDataType {
    const HvacSystemFunctionIdType* system_function_id; /**< "systemFunctionId" */
    const HvacOperationModeIdType* operation_mode_id;   /**< "operationModeId" */
    const SetpointIdType* setpoint_id;                  /**< "setpointId" */
};

typedef struct HvacSystemFunctionSetpointRelationDataElementsType HvacSystemFunctionSetpointRelationDataElementsType;

struct HvacSystemFunctionSetpointRelationDataElementsType {
    ElementTagType system_function_id; /**< "systemFunctionId" */
    ElementTagType operation_mode_id;  /**< "operationModeId" */
    ElementTagType setpoint_id;        /**< "setpointId" */
};

typedef struct HvacSystemFunctionSetpointRelationListDataType HvacSystemFunctionSetpointRelationListDataType;

struct HvacSystemFunctionSetpointRelationListDataType {
    const HvacSystemFunctionSetpointRelationDataType* const*
        hvac_system_function_setpoint_relation_data; /**< "hvacSystemFunctionSetpointRelationData" */
    size_t hvac_system_function_setpoint_relation_data_size;
};

typedef struct HvacSystemFunctionSetpointRelationListDataSelectorsType
    HvacSystemFunctionSetpointRelationListDataSelectorsType;

struct HvacSystemFunctionSetpointRelationListDataSelectorsType {
    const HvacSystemFunctionIdType* system_function_id; /**< "systemFunctionId" */
    const HvacOperationModeIdType* operation_mode_id;   /**< "operationModeId" */
};

typedef struct HvacSystemFunctionPowerSequenceRelationDataType HvacSystemFunctionPowerSequenceRelationDataType;

struct HvacSystemFunctionPowerSequenceRelationDataType {
    const HvacSystemFunctionIdType* system_function_id; /**< "systemFunctionId" */
    const PowerSequenceIdType* const* sequence_id;      /**< "sequenceId" */
    size_t sequence_id_size;
};

typedef struct HvacSystemFunctionPowerSequenceRelationDataElementsType
    HvacSystemFunctionPowerSequenceRelationDataElementsType;

struct HvacSystemFunctionPowerSequenceRelationDataElementsType {
    ElementTagType system_function_id; /**< "systemFunctionId" */
    ElementTagType sequence_id;        /**< "sequenceId" */
};

typedef struct HvacSystemFunctionPowerSequenceRelationListDataType HvacSystemFunctionPowerSequenceRelationListDataType;

struct HvacSystemFunctionPowerSequenceRelationListDataType {
    const HvacSystemFunctionPowerSequenceRelationDataType* const*
        hvac_system_function_power_sequence_relation_data; /**< "hvacSystemFunctionPowerSequenceRelationData" */
    size_t hvac_system_function_power_sequence_relation_data_size;
};

typedef struct HvacSystemFunctionPowerSequenceRelationListDataSelectorsType
    HvacSystemFunctionPowerSequenceRelationListDataSelectorsType;

struct HvacSystemFunctionPowerSequenceRelationListDataSelectorsType {
    const HvacSystemFunctionIdType* system_function_id; /**< "systemFunctionId" */
};

typedef struct HvacSystemFunctionDescriptionDataType HvacSystemFunctionDescriptionDataType;

struct HvacSystemFunctionDescriptionDataType {
    const HvacSystemFunctionIdType* system_function_id;     /**< "systemFunctionId" */
    const HvacSystemFunctionTypeType* system_function_type; /**< "systemFunctionType" */
    LabelType label;                                        /**< "label" */
    DescriptionType description;                            /**< "description" */
};

typedef struct HvacSystemFunctionDescriptionDataElementsType HvacSystemFunctionDescriptionDataElementsType;

struct HvacSystemFunctionDescriptionDataElementsType {
    ElementTagType system_function_id;   /**< "systemFunctionId" */
    ElementTagType system_function_type; /**< "systemFunctionType" */
    ElementTagType label;                /**< "label" */
    ElementTagType description;          /**< "description" */
};

typedef struct HvacSystemFunctionDescriptionListDataType HvacSystemFunctionDescriptionListDataType;

struct HvacSystemFunctionDescriptionListDataType {
    const HvacSystemFunctionDescriptionDataType* const*
        hvac_system_function_description_data; /**< "hvacSystemFunctionDescriptionData" */
    size_t hvac_system_function_description_data_size;
};

typedef struct HvacSystemFunctionDescriptionListDataSelectorsType HvacSystemFunctionDescriptionListDataSelectorsType;

struct HvacSystemFunctionDescriptionListDataSelectorsType {
    const HvacSystemFunctionIdType* system_function_id; /**< "systemFunctionId" */
};

typedef struct HvacOperationModeDescriptionDataType HvacOperationModeDescriptionDataType;

struct HvacOperationModeDescriptionDataType {
    const HvacOperationModeIdType* operation_mode_id;     /**< "operationModeId" */
    const HvacOperationModeTypeType* operation_mode_type; /**< "operationModeType" */
    LabelType label;                                      /**< "label" */
    DescriptionType description;                          /**< "description" */
};

typedef struct HvacOperationModeDescriptionDataElementsType HvacOperationModeDescriptionDataElementsType;

struct HvacOperationModeDescriptionDataElementsType {
    ElementTagType operation_mode_id;   /**< "operationModeId" */
    ElementTagType operation_mode_type; /**< "operationModeType" */
    ElementTagType label;               /**< "label" */
    ElementTagType description;         /**< "description" */
};

typedef struct HvacOperationModeDescriptionListDataType HvacOperationModeDescriptionListDataType;

struct HvacOperationModeDescriptionListDataType {
    const HvacOperationModeDescriptionDataType* const*
        hvac_operation_mode_description_data; /**< "hvacOperationModeDescriptionData" */
    size_t hvac_operation_mode_description_data_size;
};

typedef struct HvacOperationModeDescriptionListDataSelectorsType HvacOperationModeDescriptionListDataSelectorsType;

struct HvacOperationModeDescriptionListDataSelectorsType {
    const HvacOperationModeIdType* operation_mode_id; /**< "operationModeId" */
};

typedef struct HvacOverrunDataType HvacOverrunDataType;

struct HvacOverrunDataType {
    const HvacOverrunIdType* overrun_id;         /**< "overrunId" */
    const HvacOverrunStatusType* overrun_status; /**< "overrunStatus" */
    const TimeTableIdType* time_table_id;        /**< "timeTableId" */
    const bool* is_overrun_status_changeable;    /**< "isOverrunStatusChangeable" */
};

typedef struct HvacOverrunDataElementsType HvacOverrunDataElementsType;

struct HvacOverrunDataElementsType {
    ElementTagType overrun_id;                   /**< "overrunId" */
    ElementTagType overrun_status;               /**< "overrunStatus" */
    ElementTagType time_table_id;                /**< "timeTableId" */
    ElementTagType is_overrun_status_changeable; /**< "isOverrunStatusChangeable" */
};

typedef struct HvacOverrunListDataType HvacOverrunListDataType;

struct HvacOverrunListDataType {
    const HvacOverrunDataType* const* hvac_overrun_data; /**< "hvacOverrunData" */
    size_t hvac_overrun_data_size;
};

typedef struct HvacOverrunListDataSelectorsType HvacOverrunListDataSelectorsType;

struct HvacOverrunListDataSelectorsType {
    const HvacOverrunIdType* overrun_id; /**< "overrunId" */
};

typedef struct HvacOverrunDescriptionDataType HvacOverrunDescriptionDataType;

struct HvacOverrunDescriptionDataType {
    const HvacOverrunIdType* overrun_id;                                /**< "overrunId" */
    const HvacOverrunTypeType* overrun_type;                            /**< "overrunType" */
    const HvacSystemFunctionIdType* const* affected_system_function_id; /**< "affectedSystemFunctionId" */
    size_t affected_system_function_id_size;
    LabelType label;             /**< "label" */
    DescriptionType description; /**< "description" */
};

typedef struct HvacOverrunDescriptionDataElementsType HvacOverrunDescriptionDataElementsType;

struct HvacOverrunDescriptionDataElementsType {
    ElementTagType overrun_id;                  /**< "overrunId" */
    ElementTagType overrun_type;                /**< "overrunType" */
    ElementTagType affected_system_function_id; /**< "affectedSystemFunctionId" */
    ElementTagType label;                       /**< "label" */
    ElementTagType description;                 /**< "description" */
};

typedef struct HvacOverrunDescriptionListDataType HvacOverrunDescriptionListDataType;

struct HvacOverrunDescriptionListDataType {
    const HvacOverrunDescriptionDataType* const* hvac_overrun_description_data; /**< "hvacOverrunDescriptionData" */
    size_t hvac_overrun_description_data_size;
};

typedef struct HvacOverrunDescriptionListDataSelectorsType HvacOverrunDescriptionListDataSelectorsType;

struct HvacOverrunDescriptionListDataSelectorsType {
    const HvacOverrunIdType* overrun_id; /**< "overrunId" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_HVAC_TYPES_H_
