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
 * @brief SPINE Datagram Power Sequences data types declarations
 */

#ifndef SRC_SPINE_MODEL_POWER_SEQUENCES_TYPES_H_
#define SRC_SPINE_MODEL_POWER_SEQUENCES_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/measurement_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t AlternativesIdType;

typedef uint32_t PowerSequenceIdType;

typedef uint32_t PowerTimeSlotNumberType;

typedef int32_t PowerTimeSlotValueTypeType;

enum {
    kPowerTimeSlotValueTypeTypePower,                   /**< "power" */
    kPowerTimeSlotValueTypeTypePowerMin,                /**< "powerMin" */
    kPowerTimeSlotValueTypeTypePowerMax,                /**< "powerMax" */
    kPowerTimeSlotValueTypeTypePowerExpectedValue,      /**< "powerExpectedValue" */
    kPowerTimeSlotValueTypeTypePowerStandardDeviation,  /**< "powerStandardDeviation" */
    kPowerTimeSlotValueTypeTypePowerSkewness,           /**< "powerSkewness" */
    kPowerTimeSlotValueTypeTypeEnergy,                  /**< "energy" */
    kPowerTimeSlotValueTypeTypeEnergyMin,               /**< "energyMin" */
    kPowerTimeSlotValueTypeTypeEnergyMax,               /**< "energyMax" */
    kPowerTimeSlotValueTypeTypeEnergyExpectedValue,     /**< "energyExpectedValue" */
    kPowerTimeSlotValueTypeTypeEnergyStandardDeviation, /**< "energyStandardDeviation" */
    kPowerTimeSlotValueTypeTypeEnergySkewness,          /**< "energySkewness" */
};

typedef int32_t PowerSequenceScopeType;

enum {
    kPowerSequenceScopeTypeForecast,       /**< "forecast" */
    kPowerSequenceScopeTypeMeasurement,    /**< "measurement" */
    kPowerSequenceScopeTypeRecommendation, /**< "recommendation" */
};

typedef int32_t PowerSequenceStateType;

enum {
    kPowerSequenceStateTypeRunning,         /**< "running" */
    kPowerSequenceStateTypePaused,          /**< "paused" */
    kPowerSequenceStateTypeScheduled,       /**< "scheduled" */
    kPowerSequenceStateTypeScheduledPaused, /**< "scheduledPaused" */
    kPowerSequenceStateTypePending,         /**< "pending" */
    kPowerSequenceStateTypeInactive,        /**< "inactive" */
    kPowerSequenceStateTypeCompleted,       /**< "completed" */
    kPowerSequenceStateTypeInvalid,         /**< "invalid" */
};

typedef struct PowerTimeSlotScheduleDataType PowerTimeSlotScheduleDataType;

struct PowerTimeSlotScheduleDataType {
    const PowerSequenceIdType* sequence_id;     /**< "sequenceId" */
    const PowerTimeSlotNumberType* slot_number; /**< "slotNumber" */
    const TimePeriodType* time_period;          /**< "timePeriod" */
    const DurationType* default_duration;       /**< "defaultDuration" */
    const DurationType* duration_uncertainty;   /**< "durationUncertainty" */
    const bool* slot_activated;                 /**< "slotActivated" */
    DescriptionType description;                /**< "description" */
};

typedef struct PowerTimeSlotScheduleDataElementsType PowerTimeSlotScheduleDataElementsType;

struct PowerTimeSlotScheduleDataElementsType {
    ElementTagType sequence_id;          /**< "sequenceId" */
    ElementTagType slot_number;          /**< "slotNumber" */
    ElementTagType time_period;          /**< "timePeriod" */
    ElementTagType default_duration;     /**< "defaultDuration" */
    ElementTagType duration_uncertainty; /**< "durationUncertainty" */
    ElementTagType slot_activated;       /**< "slotActivated" */
    ElementTagType description;          /**< "description" */
};

typedef struct PowerTimeSlotScheduleListDataType PowerTimeSlotScheduleListDataType;

struct PowerTimeSlotScheduleListDataType {
    const PowerTimeSlotScheduleDataType* const* power_time_slot_schedule_data; /**< "powerTimeSlotScheduleData" */
    size_t power_time_slot_schedule_data_size;
};

typedef struct PowerTimeSlotScheduleListDataSelectorsType PowerTimeSlotScheduleListDataSelectorsType;

struct PowerTimeSlotScheduleListDataSelectorsType {
    const PowerSequenceIdType* sequence_id;     /**< "sequenceId" */
    const PowerTimeSlotNumberType* slot_number; /**< "slotNumber" */
};

typedef struct PowerTimeSlotValueDataType PowerTimeSlotValueDataType;

struct PowerTimeSlotValueDataType {
    const PowerSequenceIdType* sequence_id;       /**< "sequenceId" */
    const PowerTimeSlotNumberType* slot_number;   /**< "slotNumber" */
    const PowerTimeSlotValueTypeType* value_type; /**< "valueType" */
    const ScaledNumberType* value;                /**< "value" */
};

typedef struct PowerTimeSlotValueDataElementsType PowerTimeSlotValueDataElementsType;

struct PowerTimeSlotValueDataElementsType {
    ElementTagType sequence_id;            /**< "sequenceId" */
    ElementTagType slot_number;            /**< "slotNumber" */
    ElementTagType value_type;             /**< "valueType" */
    const ScaledNumberElementsType* value; /**< "value" */
};

typedef struct PowerTimeSlotValueListDataType PowerTimeSlotValueListDataType;

struct PowerTimeSlotValueListDataType {
    const PowerTimeSlotValueDataType* const* power_time_slot_value_data; /**< "powerTimeSlotValueListData" */
    size_t power_time_slot_value_data_size;
};

typedef struct PowerTimeSlotValueListDataSelectorsType PowerTimeSlotValueListDataSelectorsType;

struct PowerTimeSlotValueListDataSelectorsType {
    const PowerSequenceIdType* sequence_id;       /**< "sequenceId" */
    const PowerTimeSlotNumberType* slot_number;   /**< "slotNumber" */
    const PowerTimeSlotValueTypeType* value_type; /**< "valueType" */
};

typedef struct PowerTimeSlotScheduleConstraintsDataType PowerTimeSlotScheduleConstraintsDataType;

struct PowerTimeSlotScheduleConstraintsDataType {
    const PowerSequenceIdType* sequence_id;                /**< "sequenceId" */
    const PowerTimeSlotNumberType* slot_number;            /**< "slotNumber" */
    const AbsoluteOrRelativeTimeType* earliest_start_time; /**< "earliestStartTime" */
    const AbsoluteOrRelativeTimeType* latest_end_time;     /**< "latestEndTime" */
    const DurationType* min_duration;                      /**< "minDuration" */
    const DurationType* max_duration;                      /**< "maxDuration" */
    const bool* optional_slot;                             /**< "optionalSlot" */
};

typedef struct PowerTimeSlotScheduleConstraintsDataElementsType PowerTimeSlotScheduleConstraintsDataElementsType;

struct PowerTimeSlotScheduleConstraintsDataElementsType {
    ElementTagType sequence_id;         /**< "sequenceId" */
    ElementTagType slot_number;         /**< "slotNumber" */
    ElementTagType earliest_start_time; /**< "earliestStartTime" */
    ElementTagType latest_end_time;     /**< "latestEndTime" */
    ElementTagType min_duration;        /**< "minDuration" */
    ElementTagType max_duration;        /**< "maxDuration" */
    ElementTagType optional_slot;       /**< "optionalSlot" */
};

typedef struct PowerTimeSlotScheduleConstraintsListDataType PowerTimeSlotScheduleConstraintsListDataType;

struct PowerTimeSlotScheduleConstraintsListDataType {
    const PowerTimeSlotScheduleConstraintsDataType* const*
        power_time_slot_schedule_constraints_data; /**< "powerTimeSlotScheduleConstraintsData" */
    size_t power_time_slot_schedule_constraints_data_size;
};

typedef struct PowerTimeSlotScheduleConstraintsListDataSelectorsType
    PowerTimeSlotScheduleConstraintsListDataSelectorsType;

struct PowerTimeSlotScheduleConstraintsListDataSelectorsType {
    const PowerSequenceIdType* sequence_id;     /**< "sequenceId" */
    const PowerTimeSlotNumberType* slot_number; /**< "slotNumber" */
};

typedef struct PowerSequenceAlternativesRelationDataType PowerSequenceAlternativesRelationDataType;

struct PowerSequenceAlternativesRelationDataType {
    const AlternativesIdType* alternatives_id;     /**< "alternativesId" */
    const PowerSequenceIdType* const* sequence_id; /**< "sequenceId" */
    size_t sequence_id_size;
};

typedef struct PowerSequenceAlternativesRelationDataElementsType PowerSequenceAlternativesRelationDataElementsType;

struct PowerSequenceAlternativesRelationDataElementsType {
    ElementTagType alternatives_id; /**< "alternativesId" */
    ElementTagType sequence_id;     /**< "sequenceId" */
};

typedef struct PowerSequenceAlternativesRelationListDataType PowerSequenceAlternativesRelationListDataType;

struct PowerSequenceAlternativesRelationListDataType {
    const PowerSequenceAlternativesRelationDataType* const*
        power_sequence_alternatives_relation_data; /**< "powerSequenceAlternativesRelationData" */
    size_t power_sequence_alternatives_relation_data_size;
};

typedef struct PowerSequenceAlternativesRelationListDataSelectorsType
    PowerSequenceAlternativesRelationListDataSelectorsType;

struct PowerSequenceAlternativesRelationListDataSelectorsType {
    const AlternativesIdType* alternatives_id;     /**< "alternativesId" */
    const PowerSequenceIdType* const* sequence_id; /**< "sequenceId" */
    size_t sequence_id_size;
};

typedef struct PowerSequenceDescriptionDataType PowerSequenceDescriptionDataType;

struct PowerSequenceDescriptionDataType {
    const PowerSequenceIdType* sequence_id;               /**< "sequenceId" */
    DescriptionType description;                          /**< "description" */
    const EnergyDirectionType* positive_energy_direction; /**< "positiveEnergyDirection" */
    const UnitOfMeasurementType* power_unit;              /**< "powerUnit" */
    const UnitOfMeasurementType* energy_unit;             /**< "energyUnit" */
    const MeasurementValueSourceType* value_source;       /**< "valueSource" */
    const PowerSequenceScopeType* scope;                  /**< "scope" */
    const uint32_t* task_identifier;                      /**< "taskIdentifier" */
    const uint32_t* repetitions_total;                    /**< "repetitionsTotal" */
};

typedef struct PowerSequenceDescriptionDataElementsType PowerSequenceDescriptionDataElementsType;

struct PowerSequenceDescriptionDataElementsType {
    ElementTagType sequence_id;               /**< "sequenceId" */
    ElementTagType description;               /**< "description" */
    ElementTagType positive_energy_direction; /**< "positiveEnergyDirection" */
    ElementTagType power_unit;                /**< "powerUnit" */
    ElementTagType energy_unit;               /**< "energyUnit" */
    ElementTagType value_source;              /**< "valueSource" */
    ElementTagType scope;                     /**< "scope" */
    ElementTagType task_identifier;           /**< "taskIdentifier" */
    ElementTagType repetitions_total;         /**< "repetitionsTotal" */
};

typedef struct PowerSequenceDescriptionListDataType PowerSequenceDescriptionListDataType;

struct PowerSequenceDescriptionListDataType {
    const PowerSequenceDescriptionDataType* const*
        power_sequence_description_data; /**< "powerSequenceDescriptionData" */
    size_t power_sequence_description_data_size;
};

typedef struct PowerSequenceDescriptionListDataSelectorsType PowerSequenceDescriptionListDataSelectorsType;

struct PowerSequenceDescriptionListDataSelectorsType {
    const PowerSequenceIdType* const* sequence_id; /**< "sequenceId" */
    size_t sequence_id_size;
};

typedef struct PowerSequenceStateDataType PowerSequenceStateDataType;

struct PowerSequenceStateDataType {
    const PowerSequenceIdType* sequence_id;            /**< "sequenceId" */
    const PowerSequenceStateType* state;               /**< "state" */
    const PowerTimeSlotNumberType* active_slot_number; /**< "activeSlotNumber" */
    const DurationType* elapsed_slot_time;             /**< "elapsedSlotTime" */
    const DurationType* remaining_slot_time;           /**< "remainingSlotTime" */
    const bool* sequence_remote_controllable;          /**< "sequenceRemoteControllable" */
    const uint32_t* active_repetition_number;          /**< "activeRepetitionNumber" */
    const DurationType* remaining_pause_time;          /**< "remainingPauseTime" */
};

typedef struct PowerSequenceStateDataElementsType PowerSequenceStateDataElementsType;

struct PowerSequenceStateDataElementsType {
    ElementTagType sequence_id;                  /**< "sequenceId" */
    ElementTagType state;                        /**< "state" */
    ElementTagType active_slot_number;           /**< "activeSlotNumber" */
    ElementTagType elapsed_slot_time;            /**< "elapsedSlotTime" */
    ElementTagType remaining_slot_time;          /**< "remainingSlotTime" */
    ElementTagType sequence_remote_controllable; /**< "sequenceRemoteControllable" */
    ElementTagType active_repetition_number;     /**< "activeRepetitionNumber" */
    ElementTagType remaining_pause_time;         /**< "remainingPauseTime" */
};

typedef struct PowerSequenceStateListDataType PowerSequenceStateListDataType;

struct PowerSequenceStateListDataType {
    const PowerSequenceStateDataType* const* power_sequence_state_data; /**< "powerSequenceStateData" */
    size_t power_sequence_state_data_size;
};

typedef struct PowerSequenceStateListDataSelectorsType PowerSequenceStateListDataSelectorsType;

struct PowerSequenceStateListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct PowerSequenceScheduleDataType PowerSequenceScheduleDataType;

struct PowerSequenceScheduleDataType {
    const PowerSequenceIdType* sequence_id;       /**< "sequenceId" */
    const AbsoluteOrRelativeTimeType* start_time; /**< "startTime" */
    const AbsoluteOrRelativeTimeType* end_time;   /**< "endTime" */
};

typedef struct PowerSequenceScheduleDataElementsType PowerSequenceScheduleDataElementsType;

struct PowerSequenceScheduleDataElementsType {
    ElementTagType sequence_id; /**< "sequenceId" */
    ElementTagType start_time;  /**< "startTime" */
    ElementTagType end_time;    /**< "endTime" */
};

typedef struct PowerSequenceScheduleListDataType PowerSequenceScheduleListDataType;

struct PowerSequenceScheduleListDataType {
    const PowerSequenceScheduleDataType* const* power_sequence_schedule_data; /**< "powerSequenceScheduleData" */
    size_t power_sequence_schedule_data_size;
};

typedef struct PowerSequenceScheduleListDataSelectorsType PowerSequenceScheduleListDataSelectorsType;

struct PowerSequenceScheduleListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct PowerSequenceScheduleConstraintsDataType PowerSequenceScheduleConstraintsDataType;

struct PowerSequenceScheduleConstraintsDataType {
    const PowerSequenceIdType* sequence_id;                /**< "sequenceId" */
    const AbsoluteOrRelativeTimeType* earliest_start_time; /**< "earliestStartTime" */
    const AbsoluteOrRelativeTimeType* latest_start_time;   /**< "latestStartTime" */
    const AbsoluteOrRelativeTimeType* earliest_end_time;   /**< "earliestEndTime" */
    const AbsoluteOrRelativeTimeType* latest_end_time;     /**< "latestEndTime" */
    const bool* optional_sequence;                         /**< "optionalSequence" */
};

typedef struct PowerSequenceScheduleConstraintsDataElementsType PowerSequenceScheduleConstraintsDataElementsType;

struct PowerSequenceScheduleConstraintsDataElementsType {
    ElementTagType sequence_id;         /**< "sequenceId" */
    ElementTagType earliest_start_time; /**< "earliestStartTime" */
    ElementTagType latest_start_time;   /**< "latestStartTime" */
    ElementTagType earliest_end_time;   /**< "earliestEndTime" */
    ElementTagType latest_end_time;     /**< "latestEndTime" */
    ElementTagType optional_sequence;   /**< "optionalSequence" */
};

typedef struct PowerSequenceScheduleConstraintsListDataType PowerSequenceScheduleConstraintsListDataType;

struct PowerSequenceScheduleConstraintsListDataType {
    const PowerSequenceScheduleConstraintsDataType* const*
        power_sequence_schedule_constraints_data; /**< "powerSequenceScheduleConstraintsData" */
    size_t power_sequence_schedule_constraints_data_size;
};

typedef struct PowerSequenceScheduleConstraintsListDataSelectorsType
    PowerSequenceScheduleConstraintsListDataSelectorsType;

struct PowerSequenceScheduleConstraintsListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct PowerSequencePriceDataType PowerSequencePriceDataType;

struct PowerSequencePriceDataType {
    const PowerSequenceIdType* sequence_id;                 /**< "sequenceId" */
    const AbsoluteOrRelativeTimeType* potential_start_time; /**< "potentialStartTime" */
    const ScaledNumberType* price;                          /**< "price" */
    const CurrencyType* currency;                           /**< "currency" */
};

typedef struct PowerSequencePriceDataElementsType PowerSequencePriceDataElementsType;

struct PowerSequencePriceDataElementsType {
    ElementTagType sequence_id;          /**< "sequenceId" */
    ElementTagType potential_start_time; /**< "potentialStartTime" */
    ElementTagType price;                /**< "price" */
    ElementTagType currency;             /**< "currency" */
};

typedef struct PowerSequencePriceListDataType PowerSequencePriceListDataType;

struct PowerSequencePriceListDataType {
    const PowerSequencePriceDataType* const* power_sequence_price_data; /**< "powerSequencePriceData" */
    size_t power_sequence_price_data_size;
};

typedef struct PowerSequencePriceListDataSelectorsType PowerSequencePriceListDataSelectorsType;

struct PowerSequencePriceListDataSelectorsType {
    const PowerSequenceIdType* sequence_id;                     /**< "sequenceId" */
    const TimestampIntervalType* potential_start_time_interval; /**< "potentialStartTimeInterval" */
};

typedef struct PowerSequenceSchedulePreferenceDataType PowerSequenceSchedulePreferenceDataType;

struct PowerSequenceSchedulePreferenceDataType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
    const bool* greenest;                   /**< "greenest" */
    const bool* cheapest;                   /**< "cheapest" */
};

typedef struct PowerSequenceSchedulePreferenceDataElementsType PowerSequenceSchedulePreferenceDataElementsType;

struct PowerSequenceSchedulePreferenceDataElementsType {
    ElementTagType sequence_id; /**< "sequenceId" */
    ElementTagType greenest;    /**< "greenest" */
    ElementTagType cheapest;    /**< "cheapest" */
};

typedef struct PowerSequenceSchedulePreferenceListDataType PowerSequenceSchedulePreferenceListDataType;

struct PowerSequenceSchedulePreferenceListDataType {
    const PowerSequenceSchedulePreferenceDataType* const*
        power_sequence_schedule_preference_data; /**< "powerSequenceSchedulePreferenceData" */
    size_t power_sequence_schedule_preference_data_size;
};

typedef struct PowerSequenceSchedulePreferenceListDataSelectorsType
    PowerSequenceSchedulePreferenceListDataSelectorsType;

struct PowerSequenceSchedulePreferenceListDataSelectorsType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct PowerSequenceNodeScheduleInformationDataType PowerSequenceNodeScheduleInformationDataType;

struct PowerSequenceNodeScheduleInformationDataType {
    const bool* node_remote_controllable;             /**< "nodeRemoteControllable" */
    const bool* supports_single_slot_scheduling_only; /**< "supportsSingleSlotSchedulingOnly" */
    const uint32_t* alternatives_count;               /**< "alternativesCount" */
    const uint32_t* total_sequences_count_max;        /**< "totalSequencesCountMax" */
    const bool* supports_reselection;                 /**< "supportsReselection" */
};

typedef struct PowerSequenceNodeScheduleInformationDataElementsType
    PowerSequenceNodeScheduleInformationDataElementsType;

struct PowerSequenceNodeScheduleInformationDataElementsType {
    ElementTagType node_remote_controllable;             /**< "nodeRemoteControllable" */
    ElementTagType supports_single_slot_scheduling_only; /**< "supportsSingleSlotSchedulingOnly" */
    ElementTagType alternatives_count;                   /**< "alternativesCount" */
    ElementTagType total_sequences_count_max;            /**< "totalSequencesCountMax" */
    ElementTagType supports_reselection;                 /**< "supportsReselection" */
};

typedef struct PowerSequenceScheduleConfigurationRequestCallType PowerSequenceScheduleConfigurationRequestCallType;

struct PowerSequenceScheduleConfigurationRequestCallType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct PowerSequenceScheduleConfigurationRequestCallElementsType
    PowerSequenceScheduleConfigurationRequestCallElementsType;

struct PowerSequenceScheduleConfigurationRequestCallElementsType {
    ElementTagType sequence_id; /**< "sequenceId" */
};

typedef struct PowerSequencePriceCalculationRequestCallType PowerSequencePriceCalculationRequestCallType;

struct PowerSequencePriceCalculationRequestCallType {
    const PowerSequenceIdType* sequence_id;                 /**< "sequenceId" */
    const AbsoluteOrRelativeTimeType* potential_start_time; /**< "potentialStartTime" */
};

typedef struct PowerSequencePriceCalculationRequestCallElementsType
    PowerSequencePriceCalculationRequestCallElementsType;

struct PowerSequencePriceCalculationRequestCallElementsType {
    ElementTagType sequence_id;          /**< "sequenceId" */
    ElementTagType potential_start_time; /**< "potentialStartTime" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_POWER_SEQUENCES_TYPES_H_
