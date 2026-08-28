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
 * @brief SPINE Datagram Supply Conditions data types declarations
 */

#ifndef SRC_SPINE_MODEL_SUPPLY_CONDITIONS_TYPES_H_
#define SRC_SPINE_MODEL_SUPPLY_CONDITIONS_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/threshold_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t ConditionIdType;

typedef int32_t SupplyConditionEventTypeType;

enum {
    kSupplyConditionEventTypeTypeThesholdExceeded,     /**< "thesholdExceeded" */
    kSupplyConditionEventTypeTypeFallenBelowThreshold, /**< "fallenBelowThreshold" */
    kSupplyConditionEventTypeTypeSupplyInterrupt,      /**< "supplyInterrupt" */
    kSupplyConditionEventTypeTypeReleaseOfLimitations, /**< "releaseOfLimitations" */
    kSupplyConditionEventTypeTypeOtherProblem,         /**< "otherProblem" */
    kSupplyConditionEventTypeTypeGridConditionUpdate,  /**< "gridConditionUpdate" */
};

typedef int32_t SupplyConditionOriginatorType;

enum {
    kSupplyConditionOriginatorTypeExternDSO,       /**< "externDSO" */
    kSupplyConditionOriginatorTypeExternSupplier,  /**< "externSupplier" */
    kSupplyConditionOriginatorTypeInternalLimit,   /**< "internalLimit" */
    kSupplyConditionOriginatorTypeInternalService, /**< "internalService" */
    kSupplyConditionOriginatorTypeInternalUser,    /**< "internalUser" */
};

typedef int32_t GridConditionType;

enum {
    kGridConditionTypeConsumptionRed,    /**< "consumptionRed" */
    kGridConditionTypeConsumptionYellow, /**< "consumptionYellow" */
    kGridConditionTypeGood,              /**< "good" */
    kGridConditionTypeProductionRed,     /**< "productionRed" */
    kGridConditionTypeProductionYellow,  /**< "productionYellow" */
};

typedef struct SupplyConditionDataType SupplyConditionDataType;

struct SupplyConditionDataType {
    const ConditionIdType* condition_id;             /**< "conditionId" */
    const AbsoluteOrRelativeTimeType* timestamp;     /**< "timestamp" */
    const SupplyConditionEventTypeType* event_type;  /**< "eventType" */
    const SupplyConditionOriginatorType* originator; /**< "originator" */
    const ThresholdIdType* threshold_id;             /**< "thresholdId" */
    const ScaledNumberType* threshold_percentage;    /**< "thresholdPercentage" */
    const TimePeriodType* relevant_period;           /**< "relevantPeriod" */
    DescriptionType description;                     /**< "description" */
    const GridConditionType* grid_condition;         /**< "gridCondition" */
};

typedef struct SupplyConditionDataElementsType SupplyConditionDataElementsType;

struct SupplyConditionDataElementsType {
    ElementTagType condition_id;                          /**< "conditionId" */
    ElementTagType timestamp;                             /**< "timestamp" */
    ElementTagType event_type;                            /**< "eventType" */
    ElementTagType originator;                            /**< "originator" */
    ElementTagType threshold_id;                          /**< "thresholdId" */
    const ScaledNumberElementsType* threshold_percentage; /**< "thresholdPercentage" */
    const TimePeriodElementsType* relevant_period;        /**< "relevantPeriod" */
    ElementTagType description;                           /**< "description" */
    ElementTagType grid_condition;                        /**< "gridCondition" */
};

typedef struct SupplyConditionListDataType SupplyConditionListDataType;

struct SupplyConditionListDataType {
    const SupplyConditionDataType* const* supply_condition_data; /**< "supplyConditionData" */
    size_t supply_condition_data_size;
};

typedef struct SupplyConditionListDataSelectorsType SupplyConditionListDataSelectorsType;

struct SupplyConditionListDataSelectorsType {
    const ConditionIdType* condition_id;             /**< "conditionId" */
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
    const SupplyConditionEventTypeType* event_type;  /**< "eventType" */
    const SupplyConditionOriginatorType* originator; /**< "originator" */
};

typedef struct SupplyConditionDescriptionDataType SupplyConditionDescriptionDataType;

struct SupplyConditionDescriptionDataType {
    const ConditionIdType* condition_id;                  /**< "conditionId" */
    const CommodityTypeType* commodity_type;              /**< "commodityType" */
    const EnergyDirectionType* positive_energy_direction; /**< "positiveEnergyDirection" */
    LabelType label;                                      /**< "label" */
    DescriptionType description;                          /**< "description" */
};

typedef struct SupplyConditionDescriptionDataElementsType SupplyConditionDescriptionDataElementsType;

struct SupplyConditionDescriptionDataElementsType {
    ElementTagType condition_id;              /**< "conditionId" */
    ElementTagType commodity_type;            /**< "commodityType" */
    ElementTagType positive_energy_direction; /**< "positiveEnergyDirection" */
    ElementTagType label;                     /**< "label" */
    ElementTagType description;               /**< "description" */
};

typedef struct SupplyConditionDescriptionListDataType SupplyConditionDescriptionListDataType;

struct SupplyConditionDescriptionListDataType {
    const SupplyConditionDescriptionDataType* const*
        supply_condition_description_data; /**< "supplyConditionDescriptionData" */
    size_t supply_condition_description_data_size;
};

typedef struct SupplyConditionDescriptionListDataSelectorsType SupplyConditionDescriptionListDataSelectorsType;

struct SupplyConditionDescriptionListDataSelectorsType {
    const ConditionIdType* condition_id; /**< "conditionId" */
};

typedef struct SupplyConditionThresholdRelationDataType SupplyConditionThresholdRelationDataType;

struct SupplyConditionThresholdRelationDataType {
    const ConditionIdType* condition_id;        /**< "conditionId" */
    const ThresholdIdType* const* threshold_id; /**< "thresholdId" */
    size_t threshold_id_size;
};

typedef struct SupplyConditionThresholdRelationDataElementsType SupplyConditionThresholdRelationDataElementsType;

struct SupplyConditionThresholdRelationDataElementsType {
    ElementTagType condition_id; /**< "conditionId" */
    ElementTagType threshold_id; /**< "thresholdId" */
};

typedef struct SupplyConditionThresholdRelationListDataType SupplyConditionThresholdRelationListDataType;

struct SupplyConditionThresholdRelationListDataType {
    const SupplyConditionThresholdRelationDataType* const*
        supply_condition_threshold_relation_data; /**< "SupplyConditionThresholdRelationDataType" */
    size_t supply_condition_threshold_relation_data_size;
};

typedef struct SupplyConditionThresholdRelationListDataSelectorsType
    SupplyConditionThresholdRelationListDataSelectorsType;

struct SupplyConditionThresholdRelationListDataSelectorsType {
    const ConditionIdType* condition_id; /**< "conditionId" */
    const ThresholdIdType* threshold_id; /**< "thresholdId" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_SUPPLY_CONDITIONS_TYPES_H_
