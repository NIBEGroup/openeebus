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
 * @brief SPINE Datagram Tariff Information data types declarations
 */

#ifndef SRC_SPINE_MODEL_TARIFF_INFORMATION_TYPES_H_
#define SRC_SPINE_MODEL_TARIFF_INFORMATION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/measurement_types.h"
#include "src/spine/model/timetable_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t TariffIdType;

typedef TariffIdType TariffCountType;

typedef uint32_t TierBoundaryIdType;

typedef TierBoundaryIdType TierBoundaryCountType;

typedef int32_t TierBoundaryTypeType;

enum {
    kTierBoundaryTypeTypePowerBoundary,  /**< "powerBoundary" */
    kTierBoundaryTypeTypeEnergyBoundary, /**< "energyBoundary" */
    kTierBoundaryTypeTypeCountBoundary,  /**< "countBoundary" */
};

typedef uint32_t CommodityIdType;

typedef uint32_t TierIdType;

typedef TierIdType TierCountType;

typedef int32_t TierTypeType;

enum {
    kTierTypeTypeFixedCost,   /**< "fixedCost" */
    kTierTypeTypeDynamicCost, /**< "dynamicCost" */
};

typedef uint32_t IncentiveIdType;

typedef IncentiveIdType IncentiveCountType;

typedef int32_t IncentiveTypeType;

enum {
    kIncentiveTypeTypeAbsoluteCost,              /**< "absoluteCost" */
    kIncentiveTypeTypeRelativeCost,              /**< "relativeCost" */
    kIncentiveTypeTypeRenewableEnergyPercentage, /**< "renewableEnergyPercentage" */
    kIncentiveTypeTypeCo2Emission,               /**< "co2Emission" */
};

typedef uint32_t IncentivePriorityType;

typedef int32_t IncentiveValueTypeType;

enum {
    kIncentiveValueTypeTypeValue,        /**< "value" */
    kIncentiveValueTypeTypeAverageValue, /**< "averageValue" */
    kIncentiveValueTypeTypeMinvalue,     /**< "minValue" */
    kIncentiveValueTypeTypeMaxvalue,     /**< "maxValue" */
};

typedef struct TariffOverallConstraintsDataType TariffOverallConstraintsDataType;

struct TariffOverallConstraintsDataType {
    const TariffCountType* max_tariff_count;                /**< "maxTariffCount" */
    const TierBoundaryCountType* max_boundary_count;        /**< "maxBoundaryCount" */
    const TierCountType* max_tier_count;                    /**< "maxTierCount" */
    const IncentiveCountType* max_incentive_count;          /**< "maxIncentiveCount" */
    const TierBoundaryCountType* max_boundaries_per_tariff; /**< "maxBoundariesPerTariff" */
    const TierCountType* max_tiers_per_tariff;              /**< "maxTiersPerTariff" */
    const TierBoundaryCountType* max_boundaries_per_tier;   /**< "maxBoundariesPerTier" */
    const IncentiveCountType* max_incentives_per_tier;      /**< "maxIncentivesPerTier" */
};

typedef struct TariffOverallConstraintsDataElementsType TariffOverallConstraintsDataElementsType;

struct TariffOverallConstraintsDataElementsType {
    ElementTagType max_tariff_count;          /**< "maxTariffCount" */
    ElementTagType max_boundary_count;        /**< "maxBoundaryCount" */
    ElementTagType max_tier_count;            /**< "maxTierCount" */
    ElementTagType max_incentive_count;       /**< "maxIncentiveCount" */
    ElementTagType max_boundaries_per_tariff; /**< "maxBoundariesPerTariff" */
    ElementTagType max_tiers_per_tariff;      /**< "maxTiersPerTariff" */
    ElementTagType max_boundaries_per_tier;   /**< "maxBoundariesPerTier" */
    ElementTagType max_incentives_per_tier;   /**< "maxIncentivesPerTier" */
};

typedef struct TariffDataType TariffDataType;

struct TariffDataType {
    const TariffIdType* tariff_id;           /**< "tariffId" */
    const TierIdType* const* active_tier_id; /**< "activeTierId" */
    size_t active_tier_id_size;
};

typedef struct TariffDataElementsType TariffDataElementsType;

struct TariffDataElementsType {
    ElementTagType tariff_id;      /**< "tariffId" */
    ElementTagType active_tier_id; /**< "activeTierId" */
};

typedef struct TariffListDataType TariffListDataType;

struct TariffListDataType {
    const TariffDataType* const* tariff_data; /**< "tariffData" */
    size_t tariff_data_size;
};

typedef struct TariffListDataSelectorsType TariffListDataSelectorsType;

struct TariffListDataSelectorsType {
    const TariffIdType* tariff_id;    /**< "tariffId" */
    const TierIdType* active_tier_id; /**< "activeTierId" */
};

typedef struct TariffTierRelationDataType TariffTierRelationDataType;

struct TariffTierRelationDataType {
    const TariffIdType* tariff_id;    /**< "tariffId" */
    const TierIdType* const* tier_id; /**< "tierId" */
    size_t tier_id_size;
};

typedef struct TariffTierRelationDataElementsType TariffTierRelationDataElementsType;

struct TariffTierRelationDataElementsType {
    ElementTagType tariff_id; /**< "tariffId" */
    ElementTagType tier_id;   /**< "tierId" */
};

typedef struct TariffTierRelationListDataType TariffTierRelationListDataType;

struct TariffTierRelationListDataType {
    const TariffTierRelationDataType* const* tariff_tier_relation_data; /**< "tariffTierRelationData" */
    size_t tariff_tier_relation_data_size;
};

typedef struct TariffTierRelationListDataSelectorsType TariffTierRelationListDataSelectorsType;

struct TariffTierRelationListDataSelectorsType {
    const TariffIdType* tariff_id; /**< "tariffId" */
    const TierIdType* tier_id;     /**< "tierId" */
};

typedef struct TariffBoundaryRelationDataType TariffBoundaryRelationDataType;

struct TariffBoundaryRelationDataType {
    const TariffIdType* tariff_id;                /**< "tariffId" */
    const TierBoundaryIdType* const* boundary_id; /**< "boundaryId" */
    size_t boundary_id_size;
};

typedef struct TariffBoundaryRelationDataElementsType TariffBoundaryRelationDataElementsType;

struct TariffBoundaryRelationDataElementsType {
    ElementTagType tariff_id;   /**< "tariffId" */
    ElementTagType boundary_id; /**< "boundaryId" */
};

typedef struct TariffBoundaryRelationListDataType TariffBoundaryRelationListDataType;

struct TariffBoundaryRelationListDataType {
    const TariffBoundaryRelationDataType* const* tariff_boundary_relation_data; /**< "tariffBoundaryRelationData" */
    size_t tariff_boundary_relation_data_size;
};

typedef struct TariffBoundaryRelationListDataSelectorsType TariffBoundaryRelationListDataSelectorsType;

struct TariffBoundaryRelationListDataSelectorsType {
    const TariffIdType* tariff_id;         /**< "tariffId" */
    const TierBoundaryIdType* boundary_id; /**< "boundaryId" */
};

typedef struct TariffDescriptionDataType TariffDescriptionDataType;

struct TariffDescriptionDataType {
    const TariffIdType* tariff_id;           /**< "tariffId" */
    const CommodityIdType* commodity_id;     /**< "commodityId" */
    const MeasurementIdType* measurement_id; /**< "measurementId" */
    const bool* tariff_writeable;            /**< "tariffWriteable" */
    const bool* update_required;             /**< "updateRequired" */
    const ScopeTypeType* scope_type;         /**< "scopeType" */
    LabelType label;                         /**< "label" */
    DescriptionType description;             /**< "description" */
    const bool* slot_id_support;             /**< "slotIdSupport" */
};

typedef struct TariffDescriptionDataElementsType TariffDescriptionDataElementsType;

struct TariffDescriptionDataElementsType {
    ElementTagType tariff_id;        /**< "tariffId" */
    ElementTagType commodity_id;     /**< "commodityId" */
    ElementTagType measurement_id;   /**< "measurementId" */
    ElementTagType tariff_writeable; /**< "tariffWriteable" */
    ElementTagType update_required;  /**< "updateRequired" */
    ElementTagType scope_type;       /**< "scopeType" */
    ElementTagType label;            /**< "label" */
    ElementTagType description;      /**< "description" */
    ElementTagType slot_id_support;  /**< "slotIdSupport" */
};

typedef struct TariffDescriptionListDataType TariffDescriptionListDataType;

struct TariffDescriptionListDataType {
    const TariffDescriptionDataType* const* tariff_description_data; /**< "tariffDescriptionData" */
    size_t tariff_description_data_size;
};

typedef struct TariffDescriptionListDataSelectorsType TariffDescriptionListDataSelectorsType;

struct TariffDescriptionListDataSelectorsType {
    const TariffIdType* tariff_id;           /**< "tariffId" */
    const CommodityIdType* commodity_id;     /**< "commodityId" */
    const MeasurementIdType* measurement_id; /**< "measurementId" */
    const ScopeTypeType* scope_type;         /**< "scopeType" */
};

typedef struct TierBoundaryDataType TierBoundaryDataType;

struct TierBoundaryDataType {
    const TierBoundaryIdType* boundary_id;        /**< "boundaryId" */
    const TimePeriodType* time_period;            /**< "timePeriod" */
    const TimeTableIdType* time_table_id;         /**< "timeTableId" */
    const ScaledNumberType* lower_boundary_value; /**< "lowerBoundaryValue" */
    const ScaledNumberType* upper_boundary_value; /**< "upperBoundaryValue" */
};

typedef struct TierBoundaryDataElementsType TierBoundaryDataElementsType;

struct TierBoundaryDataElementsType {
    ElementTagType boundary_id;                           /**< "boundaryId" */
    const TimePeriodElementsType* time_period;            /**< "timePeriod" */
    ElementTagType time_table_id;                         /**< "timeTableId" */
    const ScaledNumberElementsType* lower_boundary_value; /**< "lowerBoundaryValue" */
    const ScaledNumberElementsType* upper_boundary_value; /**< "upperBoundaryValue" */
};

typedef struct TierBoundaryListDataType TierBoundaryListDataType;

struct TierBoundaryListDataType {
    const TierBoundaryDataType* const* tier_boundary_data; /**< "tierBoundaryData" */
    size_t tier_boundary_data_size;
};

typedef struct TierBoundaryListDataSelectorsType TierBoundaryListDataSelectorsType;

struct TierBoundaryListDataSelectorsType {
    const TierBoundaryIdType* boundary_id; /**< "boundaryId" */
};

typedef struct TierBoundaryDescriptionDataType TierBoundaryDescriptionDataType;

struct TierBoundaryDescriptionDataType {
    const TierBoundaryIdType* boundary_id;           /**< "boundaryId" */
    const TierBoundaryTypeType* boundary_type;       /**< "boundaryType" */
    const TierIdType* valid_for_tier_id;             /**< "validForTierId" */
    const TierIdType* switch_to_tier_id_when_lower;  /**< "switchToTierIdWhenLower" */
    const TierIdType* switch_to_tier_id_when_higher; /**< "switchToTierIdWhenHigher" */
    const UnitOfMeasurementType* boundary_unit;      /**< "boundaryUnit" */
    LabelType label;                                 /**< "label" */
    DescriptionType description;                     /**< "description" */
};

typedef struct TierBoundaryDescriptionDataElementsType TierBoundaryDescriptionDataElementsType;

struct TierBoundaryDescriptionDataElementsType {
    ElementTagType boundary_id;                   /**< "boundaryId" */
    ElementTagType boundary_type;                 /**< "boundaryType" */
    ElementTagType valid_for_tier_id;             /**< "validForTierId" */
    ElementTagType switch_to_tier_id_when_lower;  /**< "switchToTierIdWhenLower" */
    ElementTagType switch_to_tier_id_when_higher; /**< "switchToTierIdWhenHigher" */
    ElementTagType boundary_unit;                 /**< "boundaryUnit" */
    ElementTagType label;                         /**< "label" */
    ElementTagType description;                   /**< "description" */
};

typedef struct TierBoundaryDescriptionListDataType TierBoundaryDescriptionListDataType;

struct TierBoundaryDescriptionListDataType {
    const TierBoundaryDescriptionDataType* const* tier_boundary_description_data; /**< "tierBoundaryDescriptionData" */
    size_t tier_boundary_description_data_size;
};

typedef struct TierBoundaryDescriptionListDataSelectorsType TierBoundaryDescriptionListDataSelectorsType;

struct TierBoundaryDescriptionListDataSelectorsType {
    const TierBoundaryIdType* boundary_id;     /**< "boundaryId" */
    const TierBoundaryTypeType* boundary_type; /**< "boundaryType" */
};

typedef struct CommodityDataType CommodityDataType;

struct CommodityDataType {
    const CommodityIdType* commodity_id;                  /**< "commodityId" */
    const CommodityTypeType* commodity_type;              /**< "commodityType" */
    const EnergyDirectionType* positive_energy_direction; /**< "positiveEnergyDirection" */
    LabelType label;                                      /**< "label" */
    DescriptionType description;                          /**< "description" */
};

typedef struct CommodityDataElementsType CommodityDataElementsType;

struct CommodityDataElementsType {
    ElementTagType commodity_id;              /**< "commodityId" */
    ElementTagType commodity_type;            /**< "commodityType" */
    ElementTagType positive_energy_direction; /**< "positiveEnergyDirection" */
    ElementTagType label;                     /**< "label" */
    ElementTagType description;               /**< "description" */
};

typedef struct CommodityListDataType CommodityListDataType;

struct CommodityListDataType {
    const CommodityDataType* const* commodity_data; /**< "commodityData" */
    size_t commodity_data_size;
};

typedef struct CommodityListDataSelectorsType CommodityListDataSelectorsType;

struct CommodityListDataSelectorsType {
    const CommodityIdType* commodity_id;     /**< "commodityId" */
    const CommodityTypeType* commodity_type; /**< "commodityType" */
};

typedef struct TierDataType TierDataType;

struct TierDataType {
    const TierIdType* tier_id;                         /**< "tierId" */
    const TimePeriodType* time_period;                 /**< "timePeriod" */
    const TimeTableIdType* time_table_id;              /**< "timeTableId" */
    const IncentiveIdType* const* active_incentive_id; /**< "activeIncentiveId" */
    size_t active_incentive_id_size;
};

typedef struct TierDataElementsType TierDataElementsType;

struct TierDataElementsType {
    ElementTagType tier_id;             /**< "tierId" */
    ElementTagType time_period;         /**< "timePeriod" */
    ElementTagType time_table_id;       /**< "timeTableId" */
    ElementTagType active_incentive_id; /**< "activeIncentiveId" */
};

typedef struct TierListDataType TierListDataType;

struct TierListDataType {
    const TierDataType* const* tier_data; /**< "tierData" */
    size_t tier_data_size;
};

typedef struct TierListDataSelectorsType TierListDataSelectorsType;

struct TierListDataSelectorsType {
    const TierIdType* tier_id;                  /**< "tierId" */
    const IncentiveIdType* active_incentive_id; /**< "activeIncentiveId" */
};

typedef struct TierIncentiveRelationDataType TierIncentiveRelationDataType;

struct TierIncentiveRelationDataType {
    const TierIdType* tier_id;                  /**< "tierId" */
    const IncentiveIdType* const* incentive_id; /**< "incentiveId" */
    size_t incentive_id_size;
};

typedef struct TierIncentiveRelationDataElementsType TierIncentiveRelationDataElementsType;

struct TierIncentiveRelationDataElementsType {
    ElementTagType tier_id;      /**< "tierId" */
    ElementTagType incentive_id; /**< "incentiveId" */
};

typedef struct TierIncentiveRelationListDataType TierIncentiveRelationListDataType;

struct TierIncentiveRelationListDataType {
    const TierIncentiveRelationDataType* const* tier_incentive_relation_data; /**< "tierIncentiveRelationData" */
    size_t tier_incentive_relation_data_size;
};

typedef struct TierIncentiveRelationListDataSelectorsType TierIncentiveRelationListDataSelectorsType;

struct TierIncentiveRelationListDataSelectorsType {
    const TierIdType* tier_id;           /**< "tierId" */
    const IncentiveIdType* incentive_id; /**< "incentiveId" */
};

typedef struct TierDescriptionDataType TierDescriptionDataType;

struct TierDescriptionDataType {
    const TierIdType* tier_id;     /**< "tierId" */
    const TierTypeType* tier_type; /**< "tierType" */
    LabelType label;               /**< "label" */
    DescriptionType description;   /**< "description" */
};

typedef struct TierDescriptionDataElementsType TierDescriptionDataElementsType;

struct TierDescriptionDataElementsType {
    ElementTagType tier_id;     /**< "tierId" */
    ElementTagType tier_type;   /**< "tierType" */
    ElementTagType label;       /**< "label" */
    ElementTagType description; /**< "description" */
};

typedef struct TierDescriptionListDataType TierDescriptionListDataType;

struct TierDescriptionListDataType {
    const TierDescriptionDataType* const* tier_description_data; /**< "tierDescriptionData" */
    size_t tier_description_data_size;
};

typedef struct TierDescriptionListDataSelectorsType TierDescriptionListDataSelectorsType;

struct TierDescriptionListDataSelectorsType {
    const TierIdType* tier_id;     /**< "tierId" */
    const TierTypeType* tier_type; /**< "tierType" */
};

typedef struct IncentiveDataType IncentiveDataType;

struct IncentiveDataType {
    const IncentiveIdType* incentive_id;         /**< "incentiveId" */
    const IncentiveValueTypeType* value_type;    /**< "valueType" */
    const AbsoluteOrRelativeTimeType* timestamp; /**< "timestamp" */
    const TimePeriodType* time_period;           /**< "timePeriod" */
    const TimeTableIdType* time_table_id;        /**< "timeTableId" */
    const ScaledNumberType* value;               /**< "value" */
};

typedef struct IncentiveDataElementsType IncentiveDataElementsType;

struct IncentiveDataElementsType {
    ElementTagType incentive_id;               /**< "incentiveId" */
    ElementTagType value_type;                 /**< "valueType" */
    ElementTagType timestamp;                  /**< "timestamp" */
    const TimePeriodElementsType* time_period; /**< "timePeriod" */
    ElementTagType time_table_id;              /**< "timeTableId" */
    const ScaledNumberElementsType* value;     /**< "value" */
};

typedef struct IncentiveListDataType IncentiveListDataType;

struct IncentiveListDataType {
    const IncentiveDataType* const* incentive_data; /**< "incentiveData" */
    size_t incentive_data_size;
};

typedef struct IncentiveListDataSelectorsType IncentiveListDataSelectorsType;

struct IncentiveListDataSelectorsType {
    const IncentiveIdType* incentive_id;             /**< "incentiveId" */
    const IncentiveValueTypeType* value_type;        /**< "valueType" */
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
};

typedef struct IncentiveDescriptionDataType IncentiveDescriptionDataType;

struct IncentiveDescriptionDataType {
    const IncentiveIdType* incentive_id;             /**< "incentiveId" */
    const IncentiveTypeType* incentive_type;         /**< "incentiveType" */
    const IncentivePriorityType* incentive_priority; /**< "incentivePriority" */
    const CurrencyType* currency;                    /**< "currency" */
    const UnitOfMeasurementType* unit;               /**< "unit" */
    LabelType label;                                 /**< "label" */
    DescriptionType description;                     /**< "description" */
};

typedef struct IncentiveDescriptionDataElementsType IncentiveDescriptionDataElementsType;

struct IncentiveDescriptionDataElementsType {
    ElementTagType incentive_id;       /**< "incentiveId" */
    ElementTagType incentive_type;     /**< "incentiveType" */
    ElementTagType incentive_priority; /**< "incentivePriority" */
    ElementTagType currency;           /**< "currency" */
    ElementTagType unit;               /**< "unit" */
    ElementTagType label;              /**< "label" */
    ElementTagType description;        /**< "description" */
};

typedef struct IncentiveDescriptionListDataType IncentiveDescriptionListDataType;

struct IncentiveDescriptionListDataType {
    const IncentiveDescriptionDataType* const* incentive_description_data; /**< "incentiveDescriptionData" */
    size_t incentive_description_data_size;
};

typedef struct IncentiveDescriptionListDataSelectorsType IncentiveDescriptionListDataSelectorsType;

struct IncentiveDescriptionListDataSelectorsType {
    const IncentiveIdType* incentive_id;     /**< "incentiveId" */
    const IncentiveTypeType* incentive_type; /**< "incentiveType" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_TARIFF_INFORMATION_TYPES_H_
