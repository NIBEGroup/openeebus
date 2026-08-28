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
 * @brief SPINE Datagram Smart Energy Management Ps data types declarations
 */

#ifndef SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_TYPES_H_
#define SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/operating_constraints_types.h"
#include "src/spine/model/power_sequences_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef PowerSequenceAlternativesRelationDataType SmartEnergyManagementPsAlternativesRelationType;

typedef PowerSequenceAlternativesRelationDataElementsType SmartEnergyManagementPsAlternativesRelationElementsType;

typedef struct SmartEnergyManagementPsPowerTimeSlotValueListType SmartEnergyManagementPsPowerTimeSlotValueListType;

struct SmartEnergyManagementPsPowerTimeSlotValueListType {
    const PowerTimeSlotValueDataType* const* value; /**< "value" */
    size_t value_size;
};

typedef struct SmartEnergyManagementPsPowerTimeSlotType SmartEnergyManagementPsPowerTimeSlotType;

struct SmartEnergyManagementPsPowerTimeSlotType {
    const PowerTimeSlotScheduleDataType* schedule;                        /**< "schedule" */
    const SmartEnergyManagementPsPowerTimeSlotValueListType* value_list;  /**< "valueList" */
    const PowerTimeSlotScheduleConstraintsDataType* schedule_constraints; /**< "scheduleConstraints" */
};

typedef struct SmartEnergyManagementPsPowerSequenceType SmartEnergyManagementPsPowerSequenceType;

struct SmartEnergyManagementPsPowerSequenceType {
    const PowerSequenceDescriptionDataType* description;                  /**< "description" */
    const PowerSequenceStateDataType* state;                              /**< "state" */
    const PowerSequenceScheduleDataType* schedule;                        /**< "schedule" */
    const PowerSequenceScheduleConstraintsDataType* schedule_constraints; /**< "scheduleConstraints" */
    const PowerSequenceSchedulePreferenceDataType* schedule_preference;   /**< "schedulePreference" */
    const OperatingConstraintsInterruptDataType*
        operating_constraints_interrupt;                                        /**< "operatingConstraintsInterrupt" */
    const OperatingConstraintsDurationDataType* operating_constraints_duration; /**< "operatingConstraintsDuration" */
    const OperatingConstraintsResumeImplicationDataType*
        operating_constraints_resume_implication;                     /**< "operatingConstraintsResumeImplication" */
    SmartEnergyManagementPsPowerTimeSlotType* const* power_time_slot; /**< "powerTimeSlot" */
    size_t power_time_slot_size;
};

typedef struct SmartEnergyManagementPsAlternativesType SmartEnergyManagementPsAlternativesType;

struct SmartEnergyManagementPsAlternativesType {
    const SmartEnergyManagementPsAlternativesRelationType* relation;       /**< "relation" */
    const SmartEnergyManagementPsPowerSequenceType* const* power_sequence; /**< "powerSequence" */
    size_t power_sequence_size;
};

typedef struct SmartEnergyManagementPsPowerTimeSlotValueListElementsType
    SmartEnergyManagementPsPowerTimeSlotValueListElementsType;

struct SmartEnergyManagementPsPowerTimeSlotValueListElementsType {
    const PowerTimeSlotValueDataElementsType* value; /**< "value" */
};

typedef struct SmartEnergyManagementPsPowerTimeSlotElementsType SmartEnergyManagementPsPowerTimeSlotElementsType;

struct SmartEnergyManagementPsPowerTimeSlotElementsType {
    const PowerTimeSlotScheduleDataElementsType* schedule;                        /**< "schedule" */
    const SmartEnergyManagementPsPowerTimeSlotValueListElementsType* value_list;  /**< "valueList" */
    const PowerTimeSlotScheduleConstraintsDataElementsType* schedule_constraints; /**< "scheduleConstraints" */
};

typedef struct SmartEnergyManagementPsPowerSequenceElementsType SmartEnergyManagementPsPowerSequenceElementsType;

struct SmartEnergyManagementPsPowerSequenceElementsType {
    const PowerSequenceDescriptionDataElementsType* description;                  /**< "description" */
    const PowerSequenceStateDataElementsType* state;                              /**< "state" */
    const PowerSequenceScheduleDataElementsType* schedule;                        /**< "schedule" */
    const PowerSequenceScheduleConstraintsDataElementsType* schedule_constraints; /**< "scheduleConstraints" */
    const PowerSequenceSchedulePreferenceDataElementsType* schedule_preference;   /**< "schedulePreference" */
    const OperatingConstraintsInterruptDataElementsType*
        operating_constraints_interrupt; /**< "operatingConstraintsInterrupt" */
    const OperatingConstraintsDurationDataElementsType*
        operating_constraints_duration; /**< "operatingConstraintsDuration" */
    const OperatingConstraintsResumeImplicationDataElementsType*
        operating_constraints_resume_implication; /**< "operatingConstraintsResumeImplication" */
    const SmartEnergyManagementPsPowerTimeSlotElementsType* power_time_slot; /**< "powerTimeSlot" */
};

typedef struct SmartEnergyManagementPsAlternativesElementsType SmartEnergyManagementPsAlternativesElementsType;

struct SmartEnergyManagementPsAlternativesElementsType {
    const SmartEnergyManagementPsAlternativesRelationElementsType* relation; /**< "relation" */
    const SmartEnergyManagementPsPowerSequenceElementsType* power_sequence;  /**< "powerSequence" */
};

typedef struct SmartEnergyManagementPsDataType SmartEnergyManagementPsDataType;

struct SmartEnergyManagementPsDataType {
    const PowerSequenceNodeScheduleInformationDataType* node_schedule_information; /**< "nodeScheduleInformation" */
    const SmartEnergyManagementPsAlternativesType* const* alternatives;            /**< "alternatives" */
    size_t alternatives_size;
};

typedef struct SmartEnergyManagementPsDataElementsType SmartEnergyManagementPsDataElementsType;

struct SmartEnergyManagementPsDataElementsType {
    const PowerSequenceNodeScheduleInformationDataElementsType*
        node_schedule_information;                                       /**< "nodeScheduleInformation" */
    const SmartEnergyManagementPsAlternativesElementsType* alternatives; /**< "alternatives" */
};

typedef struct SmartEnergyManagementPsDataSelectorsType SmartEnergyManagementPsDataSelectorsType;

struct SmartEnergyManagementPsDataSelectorsType {
    const PowerSequenceAlternativesRelationListDataSelectorsType* alternatives_relation; /**< "alternativesRelation" */
    const PowerSequenceDescriptionListDataSelectorsType* power_sequence_description; /**< "powerSequenceDescription" */
    const PowerTimeSlotScheduleListDataSelectorsType* power_time_slot_schedule;      /**< "powerTimeSlotSchedule" */
    const PowerTimeSlotValueListDataSelectorsType* power_time_slot_value;            /**< "powerTimeSlotValue" */
};

typedef struct SmartEnergyManagementPsPriceDataType SmartEnergyManagementPsPriceDataType;

struct SmartEnergyManagementPsPriceDataType {
    const PowerSequencePriceDataType* price; /**< "price" */
};

typedef struct SmartEnergyManagementPsPriceDataElementsType SmartEnergyManagementPsPriceDataElementsType;

struct SmartEnergyManagementPsPriceDataElementsType {
    const PowerSequencePriceDataElementsType* price; /**< "price" */
};

typedef struct SmartEnergyManagementPsPriceDataSelectorsType SmartEnergyManagementPsPriceDataSelectorsType;

struct SmartEnergyManagementPsPriceDataSelectorsType {
    const PowerSequencePriceListDataSelectorsType* price; /**< "price" */
};

typedef struct SmartEnergyManagementPsConfigurationRequestCallType SmartEnergyManagementPsConfigurationRequestCallType;

struct SmartEnergyManagementPsConfigurationRequestCallType {
    const PowerSequenceScheduleConfigurationRequestCallType*
        schedule_configuration_request; /**< "scheduleConfigurationRequest" */
};

typedef struct SmartEnergyManagementPsConfigurationRequestCallElementsType
    SmartEnergyManagementPsConfigurationRequestCallElementsType;

struct SmartEnergyManagementPsConfigurationRequestCallElementsType {
    const PowerSequenceScheduleConfigurationRequestCallElementsType*
        schedule_configuration_request; /**< "scheduleConfigurationRequest" */
};

typedef struct SmartEnergyManagementPsPriceCalculationRequestCallType
    SmartEnergyManagementPsPriceCalculationRequestCallType;

struct SmartEnergyManagementPsPriceCalculationRequestCallType {
    const PowerSequencePriceCalculationRequestCallType* price_calculation_request; /**< "priceCalculationRequest" */
};

typedef struct SmartEnergyManagementPsPriceCalculationRequestCallElementsType
    SmartEnergyManagementPsPriceCalculationRequestCallElementsType;

struct SmartEnergyManagementPsPriceCalculationRequestCallElementsType {
    const PowerSequencePriceCalculationRequestCallElementsType*
        price_calculation_request; /**< "priceCalculationRequest" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_TYPES_H_
