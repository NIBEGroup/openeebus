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
 * @brief SPINE Datagram Operating Constraints data types declarations
 */

#ifndef SRC_SPINE_MODEL_OPERATING_CONSTRAINTS_TYPES_H_
#define SRC_SPINE_MODEL_OPERATING_CONSTRAINTS_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/power_sequences_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct OperatingConstraintsInterruptDataType OperatingConstraintsInterruptDataType;

struct OperatingConstraintsInterruptDataType {
    const PowerSequenceIdType* sequence_id;      /**< "sequenceId" */
    const bool* is_pausable;                     /**< "isPausable" */
    const bool* is_stoppable;                    /**< "isStoppable" */
    const bool* not_interruptible_at_high_power; /**< "notInterruptibleAtHighPower" */
    const uint32_t* max_cycles_per_day;          /**< "maxCyclesPerDay" */
};

typedef struct OperatingConstraintsInterruptDataElementsType OperatingConstraintsInterruptDataElementsType;

struct OperatingConstraintsInterruptDataElementsType {
    ElementTagType sequence_id;                     /**< "sequenceId" */
    ElementTagType is_pausable;                     /**< "isPausable" */
    ElementTagType is_stoppable;                    /**< "isStoppable" */
    ElementTagType not_interruptible_at_high_power; /**< "notInterruptibleAtHighPower" */
    ElementTagType max_cycles_per_day;              /**< "maxCyclesPerDay" */
};

typedef struct OperatingConstraintsInterruptListDataType OperatingConstraintsInterruptListDataType;

struct OperatingConstraintsInterruptListDataType {
    const OperatingConstraintsInterruptDataType* const*
        operating_constraints_interrupt_data; /**< "operatingConstraintsInterruptData" */
    size_t operating_constraints_interrupt_data_size;
};

typedef struct OperatingConstraintsInterruptListDataSelectorsType OperatingConstraintsInterruptListDataSelectorsType;

struct OperatingConstraintsInterruptListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct OperatingConstraintsDurationDataType OperatingConstraintsDurationDataType;

struct OperatingConstraintsDurationDataType {
    const PowerSequenceIdType* sequence_id;      /**< "sequenceId" */
    const DurationType* active_duration_min;     /**< "activeDurationMin" */
    const DurationType* active_duration_max;     /**< "activeDurationMax" */
    const DurationType* pause_duration_min;      /**< "pauseDurationMin" */
    const DurationType* pause_duration_max;      /**< "pauseDurationMax" */
    const DurationType* active_duration_sum_min; /**< "activeDurationSumMin" */
    const DurationType* active_duration_sum_max; /**< "activeDurationSumMax" */
};

typedef struct OperatingConstraintsDurationDataElementsType OperatingConstraintsDurationDataElementsType;

struct OperatingConstraintsDurationDataElementsType {
    ElementTagType sequence_id;             /**< "sequenceId" */
    ElementTagType active_duration_min;     /**< "activeDurationMin" */
    ElementTagType active_duration_max;     /**< "activeDurationMax" */
    ElementTagType pause_duration_min;      /**< "pauseDurationMin" */
    ElementTagType pause_duration_max;      /**< "pauseDurationMax" */
    ElementTagType active_duration_sum_min; /**< "activeDurationSumMin" */
    ElementTagType active_duration_sum_max; /**< "activeDurationSumMax" */
};

typedef struct OperatingConstraintsDurationListDataType OperatingConstraintsDurationListDataType;

struct OperatingConstraintsDurationListDataType {
    const OperatingConstraintsDurationDataType* const*
        operating_constraints_duration_data; /**< "operatingConstraintsDurationData" */
    size_t operating_constraints_duration_data_size;
};

typedef struct OperatingConstraintsDurationListDataSelectorsType OperatingConstraintsDurationListDataSelectorsType;

struct OperatingConstraintsDurationListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct OperatingConstraintsPowerDescriptionDataType OperatingConstraintsPowerDescriptionDataType;

struct OperatingConstraintsPowerDescriptionDataType {
    const PowerSequenceIdType* sequence_id;               /**< "sequenceId" */
    const EnergyDirectionType* positive_energy_direction; /**< "positiveEnergyDirection" */
    const UnitOfMeasurementType* power_unit;              /**< "powerUnit" */
    const UnitOfMeasurementType* energy_unit;             /**< "energyUnit" */
    DescriptionType description;                          /**< "description" */
};

typedef struct OperatingConstraintsPowerDescriptionDataElementsType
    OperatingConstraintsPowerDescriptionDataElementsType;

struct OperatingConstraintsPowerDescriptionDataElementsType {
    ElementTagType sequence_id;               /**< "sequenceId" */
    ElementTagType positive_energy_direction; /**< "positiveEnergyDirection" */
    ElementTagType power_unit;                /**< "powerUnit" */
    ElementTagType energy_unit;               /**< "energyUnit" */
    ElementTagType description;               /**< "description" */
};

typedef struct OperatingConstraintsPowerDescriptionListDataType OperatingConstraintsPowerDescriptionListDataType;

struct OperatingConstraintsPowerDescriptionListDataType {
    const OperatingConstraintsPowerDescriptionDataType* const*
        operating_constraints_power_description_data; /**< "operatingConstraintsPowerDescriptionData" */
    size_t operating_constraints_power_description_data_size;
};

typedef struct OperatingConstraintsPowerDescriptionListDataSelectorsType
    OperatingConstraintsPowerDescriptionListDataSelectorsType;

struct OperatingConstraintsPowerDescriptionListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct OperatingConstraintsPowerRangeDataType OperatingConstraintsPowerRangeDataType;

struct OperatingConstraintsPowerRangeDataType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
    const ScaledNumberType* power_min;      /**< "powerMin" */
    const ScaledNumberType* power_max;      /**< "powerMax" */
    const ScaledNumberType* energy_min;     /**< "energyMin" */
    const ScaledNumberType* energy_max;     /**< "energyMax" */
};

typedef struct OperatingConstraintsPowerRangeDataElementsType OperatingConstraintsPowerRangeDataElementsType;

struct OperatingConstraintsPowerRangeDataElementsType {
    ElementTagType sequence_id; /**< "sequenceId" */
    ElementTagType power_min;   /**< "powerMin" */
    ElementTagType power_max;   /**< "powerMax" */
    ElementTagType energy_min;  /**< "energyMin" */
    ElementTagType energy_max;  /**< "energyMax" */
};

typedef struct OperatingConstraintsPowerRangeListDataType OperatingConstraintsPowerRangeListDataType;

struct OperatingConstraintsPowerRangeListDataType {
    const OperatingConstraintsPowerRangeDataType* const*
        operating_constraints_power_range_data; /**< "operatingConstraintsPowerRangeData" */
    size_t operating_constraints_power_range_data_size;
};

typedef struct OperatingConstraintsPowerRangeListDataSelectorsType OperatingConstraintsPowerRangeListDataSelectorsType;

struct OperatingConstraintsPowerRangeListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct OperatingConstraintsPowerLevelDataType OperatingConstraintsPowerLevelDataType;

struct OperatingConstraintsPowerLevelDataType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
    const ScaledNumberType* power;          /**< "power" */
};

typedef struct OperatingConstraintsPowerLevelDataElementsType OperatingConstraintsPowerLevelDataElementsType;

struct OperatingConstraintsPowerLevelDataElementsType {
    ElementTagType sequence_id; /**< "sequenceId" */
    ElementTagType power;       /**< "power" */
};

typedef struct OperatingConstraintsPowerLevelListDataType OperatingConstraintsPowerLevelListDataType;

struct OperatingConstraintsPowerLevelListDataType {
    const OperatingConstraintsPowerLevelDataType* const*
        operating_constraints_power_level_data; /**< "operatingConstraintsPowerLevelData" */
    size_t operating_constraints_power_level_data_size;
};

typedef struct OperatingConstraintsPowerLevelListDataSelectorsType OperatingConstraintsPowerLevelListDataSelectorsType;

struct OperatingConstraintsPowerLevelListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct OperatingConstraintsResumeImplicationDataType OperatingConstraintsResumeImplicationDataType;

struct OperatingConstraintsResumeImplicationDataType {
    const PowerSequenceIdType* sequence_id;          /**< "sequenceId" */
    const ScaledNumberType* resume_energy_estimated; /**< "resumeEnergyEstimated" */
    const UnitOfMeasurementType* energy_unit;        /**< "energyUnit" */
    const ScaledNumberType* resume_cost_estimated;   /**< "resumeCostEstimated" */
    const CurrencyType* currency;                    /**< "currency" */
};

typedef struct OperatingConstraintsResumeImplicationDataElementsType
    OperatingConstraintsResumeImplicationDataElementsType;

struct OperatingConstraintsResumeImplicationDataElementsType {
    ElementTagType sequence_id;                              /**< "sequenceId" */
    const ScaledNumberElementsType* resume_energy_estimated; /**< "resumeEnergyEstimated" */
    ElementTagType energy_unit;                              /**< "energyUnit" */
    const ScaledNumberElementsType* resume_cost_estimated;   /**< "resumeCostEstimated" */
    ElementTagType currency;                                 /**< "currency" */
};

typedef struct OperatingConstraintsResumeImplicationListDataType OperatingConstraintsResumeImplicationListDataType;

struct OperatingConstraintsResumeImplicationListDataType {
    const OperatingConstraintsResumeImplicationDataType* const*
        operating_constraints_resume_implication_data; /**< "operatingConstraintsResumeImplicationData" */
    size_t operating_constraints_resume_implication_data_size;
};

typedef struct OperatingConstraintsResumeImplicationListDataSelectorsType
    OperatingConstraintsResumeImplicationListDataSelectorsType;

struct OperatingConstraintsResumeImplicationListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_OPERATING_CONSTRAINTS_TYPES_H_
