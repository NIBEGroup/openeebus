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
 * @brief SPINE Datagram Measurement data types declarations
 */

#ifndef SRC_SPINE_MODEL_MEASUREMENT_TYPES_H_
#define SRC_SPINE_MODEL_MEASUREMENT_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/threshold_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t MeasurementIdType;

typedef int32_t MeasurementTypeType;

enum {
    kMeasurementTypeTypeAcceleration,        /**< "acceleration" */
    kMeasurementTypeTypeAngle,               /**< "angle" */
    kMeasurementTypeTypeAngularVelocity,     /**< "angularVelocity" */
    kMeasurementTypeTypeArea,                /**< "area" */
    kMeasurementTypeTypeAtmosphericPressure, /**< "atmosphericPressure" */
    kMeasurementTypeTypeCapacity,            /**< "capacity" */
    kMeasurementTypeTypeConcentration,       /**< "concentration" */
    kMeasurementTypeTypeCount,               /**< "count" */
    kMeasurementTypeTypeCurrent,             /**< "current" */
    kMeasurementTypeTypeDensity,             /**< "density" */
    kMeasurementTypeTypeDistance,            /**< "distance" */
    kMeasurementTypeTypeElectricField,       /**< "electricField" */
    kMeasurementTypeTypeEnergy,              /**< "energy" */
    kMeasurementTypeTypeForce,               /**< "force" */
    kMeasurementTypeTypeFrequency,           /**< "frequency" */
    kMeasurementTypeTypeHarmonicDistortion,  /**< "harmonicDistortion" */
    kMeasurementTypeTypeHeat,                /**< "heat" */
    kMeasurementTypeTypeHeatFlux,            /**< "heatFlux" */
    kMeasurementTypeTypeIlluminance,         /**< "illuminance" */
    kMeasurementTypeTypeImpulse,             /**< "impulse" */
    kMeasurementTypeTypeLevel,               /**< "level" */
    kMeasurementTypeTypeMagneticField,       /**< "magneticField" */
    kMeasurementTypeTypeMass,                /**< "mass" */
    kMeasurementTypeTypeMassFlow,            /**< "massFlow" */
    kMeasurementTypeTypeParticles,           /**< "particles" */
    kMeasurementTypeTypePercentage,          /**< "percentage" */
    kMeasurementTypeTypePower,               /**< "power" */
    kMeasurementTypeTypePowerFactor,         /**< "powerFactor" */
    kMeasurementTypeTypePressure,            /**< "pressure" */
    kMeasurementTypeTypeRadonActivity,       /**< "radonActivity" */
    kMeasurementTypeTypeRelativeHumidity,    /**< "relativeHumidity" */
    kMeasurementTypeTypeResistance,          /**< "resistance" */
    kMeasurementTypeTypeSolarRadiation,      /**< "solarRadiation" */
    kMeasurementTypeTypeSpeed,               /**< "speed" */
    kMeasurementTypeTypeTemperature,         /**< "temperature" */
    kMeasurementTypeTypeTime,                /**< "time" */
    kMeasurementTypeTypeTorque,              /**< "torque" */
    kMeasurementTypeTypeUnknown,             /**< "unknown" */
    kMeasurementTypeTypeVelocity,            /**< "velocity" */
    kMeasurementTypeTypeVoltage,             /**< "voltage" */
    kMeasurementTypeTypeVolume,              /**< "volume" */
    kMeasurementTypeTypeVolumetricFlow,      /**< "volumetricFlow" */
};

typedef int32_t MeasurementValueTypeType;

enum {
    kMeasurementValueTypeTypeValue,             /**< "value" */
    kMeasurementValueTypeTypeAverageValue,      /**< "averageValue" */
    kMeasurementValueTypeTypeMinValue,          /**< "minValue" */
    kMeasurementValueTypeTypeMaxValue,          /**< "maxValue" */
    kMeasurementValueTypeTypeStandardDeviation, /**< "standardDeviation" */
};

typedef int32_t MeasurementValueSourceType;

enum {
    kMeasurementValueSourceTypeMeasuredValue,   /**< "measuredValue" */
    kMeasurementValueSourceTypeCalculatedValue, /**< "calculatedValue" */
    kMeasurementValueSourceTypeEmpiricalValue,  /**< "empiricalValue" */
};

typedef int32_t MeasurementValueTendencyType;

enum {
    kMeasurementValueTendencyTypeRising,  /**< "rising" */
    kMeasurementValueTendencyTypeStable,  /**< "stable" */
    kMeasurementValueTendencyTypeFalling, /**< "falling" */
};

typedef int32_t MeasurementValueStateType;

enum {
    kMeasurementValueStateTypeNormal,     /**< "normal" */
    kMeasurementValueStateTypeOutofrange, /**< "outOfRange" */
    kMeasurementValueStateTypeError,      /**< "error" */
};

typedef struct MeasurementDataType MeasurementDataType;

struct MeasurementDataType {
    const MeasurementIdType* measurement_id;            /**< "measurementId" */
    const MeasurementValueTypeType* value_type;         /**< "valueType" */
    const AbsoluteOrRelativeTimeType* timestamp;        /**< "timestamp" */
    const ScaledNumberType* value;                      /**< "value" */
    const TimePeriodType* evaluation_period;            /**< "evaluationPeriod" */
    const MeasurementValueSourceType* value_source;     /**< "valueSource" */
    const MeasurementValueTendencyType* value_tendency; /**< "valueTendency" */
    const MeasurementValueStateType* value_state;       /**< "valueState" */
};

typedef struct MeasurementDataElementsType MeasurementDataElementsType;

struct MeasurementDataElementsType {
    ElementTagType measurement_id;    /**< "measurementId" */
    ElementTagType value_type;        /**< "valueType" */
    ElementTagType timestamp;         /**< "timestamp" */
    ElementTagType value;             /**< "value" */
    ElementTagType evaluation_period; /**< "evaluationPeriod" */
    ElementTagType value_source;      /**< "valueSource" */
    ElementTagType value_tendency;    /**< "valueTendency" */
    ElementTagType value_state;       /**< "valueState" */
};

typedef struct MeasurementListDataType MeasurementListDataType;

struct MeasurementListDataType {
    const MeasurementDataType* const* measurement_data; /**< "measurementData" */
    size_t measurement_data_size;
};

typedef struct MeasurementListDataSelectorsType MeasurementListDataSelectorsType;

struct MeasurementListDataSelectorsType {
    const MeasurementIdType* measurement_id;         /**< "measurementId" */
    const MeasurementValueTypeType* value_type;      /**< "valueType" */
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
};

typedef struct MeasurementSeriesDataType MeasurementSeriesDataType;

struct MeasurementSeriesDataType {
    const MeasurementIdType* measurement_id;            /**< "measurementId" */
    const MeasurementValueTypeType* value_type;         /**< "valueType" */
    const AbsoluteOrRelativeTimeType* timestamp;        /**< "timestamp" */
    const ScaledNumberType* value;                      /**< "value" */
    const TimePeriodType* evaluation_period;            /**< "evaluationPeriod" */
    const MeasurementValueSourceType* value_source;     /**< "valueSource" */
    const MeasurementValueTendencyType* value_tendency; /**< "valueTendency" */
    const MeasurementValueStateType* value_state;       /**< "valueState" */
};

typedef struct MeasurementSeriesDataElementsType MeasurementSeriesDataElementsType;

struct MeasurementSeriesDataElementsType {
    ElementTagType measurement_id;    /**< "measurementId" */
    ElementTagType value_type;        /**< "valueType" */
    ElementTagType timestamp;         /**< "timestamp" */
    ElementTagType value;             /**< "value" */
    ElementTagType evaluation_period; /**< "evaluationPeriod" */
    ElementTagType value_source;      /**< "valueSource" */
    ElementTagType value_tendency;    /**< "valueTendency" */
    ElementTagType value_state;       /**< "valueState" */
};

typedef struct MeasurementSeriesListDataType MeasurementSeriesListDataType;

struct MeasurementSeriesListDataType {
    const MeasurementSeriesDataType* const* measurement_series_data; /**< "measurementSeriesData" */
    size_t measurement_series_data_size;
};

typedef struct MeasurementSeriesListDataSelectorsType MeasurementSeriesListDataSelectorsType;

struct MeasurementSeriesListDataSelectorsType {
    const MeasurementIdType* measurement_id;         /**< "measurementId" */
    const MeasurementValueTypeType* value_type;      /**< "valueType" */
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
};

typedef struct MeasurementConstraintsDataType MeasurementConstraintsDataType;

struct MeasurementConstraintsDataType {
    const MeasurementIdType* measurement_id; /**< "measurementId" */
    const ScaledNumberType* value_range_min; /**< "valueRangeMin" */
    const ScaledNumberType* value_range_max; /**< "valueRangeMax" */
    const ScaledNumberType* value_step_size; /**< "valueStepSize" */
};

typedef struct MeasurementConstraintsDataElementsType MeasurementConstraintsDataElementsType;

struct MeasurementConstraintsDataElementsType {
    ElementTagType measurement_id;                   /**< "measurementId" */
    const ScaledNumberElementsType* value_range_min; /**< "valueRangeMin" */
    const ScaledNumberElementsType* value_range_max; /**< "valueRangeMax" */
    const ScaledNumberElementsType* value_step_size; /**< "valueStepSize" */
};

typedef struct MeasurementConstraintsListDataType MeasurementConstraintsListDataType;

struct MeasurementConstraintsListDataType {
    const MeasurementConstraintsDataType* const* measurement_constraints_data; /**< "measurementConstraintsData" */
    size_t measurement_constraints_data_size;
};

typedef struct MeasurementConstraintsListDataSelectorsType MeasurementConstraintsListDataSelectorsType;

struct MeasurementConstraintsListDataSelectorsType {
    const MeasurementIdType* measurement_id; /**< "measurementId" */
};

typedef struct MeasurementDescriptionDataType MeasurementDescriptionDataType;

struct MeasurementDescriptionDataType {
    const MeasurementIdType* measurement_id;     /**< "measurementId" */
    const MeasurementTypeType* measurement_type; /**< "measurementType" */
    const CommodityTypeType* commodity_type;     /**< "commodityType" */
    const UnitOfMeasurementType* unit;           /**< "unit" */
    const ScaledNumberType* calibration_value;   /**< "calibrationValue" */
    const ScopeTypeType* scope_type;             /**< "scopeType" */
    LabelType label;                             /**< "label" */
    DescriptionType description;                 /**< "description" */
};

typedef struct MeasurementDescriptionDataElementsType MeasurementDescriptionDataElementsType;

struct MeasurementDescriptionDataElementsType {
    ElementTagType measurement_id;                     /**< "measurementId" */
    ElementTagType measurement_type;                   /**< "measurementType" */
    ElementTagType commodity_type;                     /**< "commodityType" */
    ElementTagType unit;                               /**< "unit" */
    const ScaledNumberElementsType* calibration_value; /**< "calibrationValue" */
    ElementTagType scope_type;                         /**< "scopeType" */
    ElementTagType label;                              /**< "label" */
    ElementTagType description;                        /**< "description" */
};

typedef struct MeasurementDescriptionListDataType MeasurementDescriptionListDataType;

struct MeasurementDescriptionListDataType {
    const MeasurementDescriptionDataType* const* measurement_description_data; /**< "measurementDescriptionData" */
    size_t measurement_description_data_size;
};

typedef struct MeasurementDescriptionListDataSelectorsType MeasurementDescriptionListDataSelectorsType;

struct MeasurementDescriptionListDataSelectorsType {
    const MeasurementIdType* measurement_id;     /**< "measurementId" */
    const MeasurementTypeType* measurement_type; /**< "measurementType" */
    const CommodityTypeType* commodity_type;     /**< "commodityType" */
    const ScopeTypeType* scope_type;             /**< "scopeType" */
};

typedef struct MeasurementThresholdRelationDataType MeasurementThresholdRelationDataType;

struct MeasurementThresholdRelationDataType {
    const MeasurementIdType* measurement_id;    /**< "measurementId" */
    const ThresholdIdType* const* threshold_id; /**< "thresholdId" */
    size_t threshold_id_size;
};

typedef struct MeasurementThresholdRelationDataElementsType MeasurementThresholdRelationDataElementsType;

struct MeasurementThresholdRelationDataElementsType {
    ElementTagType measurement_id; /**< "measurementId" */
    ElementTagType threshold_id;   /**< "thresholdId" */
};

typedef struct MeasurementThresholdRelationListDataType MeasurementThresholdRelationListDataType;

struct MeasurementThresholdRelationListDataType {
    const MeasurementThresholdRelationDataType* const*
        measurement_threshold_relation_data; /**< "measurementThresholdRelationData" */
    size_t measurement_threshold_relation_data_size;
};

typedef struct MeasurementThresholdRelationListDataSelectorsType MeasurementThresholdRelationListDataSelectorsType;

struct MeasurementThresholdRelationListDataSelectorsType {
    const MeasurementIdType* measurement_id; /**< "measurementId" */
    const ThresholdIdType* threshold_id;     /**< "thresholdId" */
};

MeasurementConstraintsListDataType* MeasurementConstraintsCreateEmpty(void);

EebusError MeasurementConstraintsAdd(
    MeasurementConstraintsListDataType* measurements_constraints,
    const MeasurementConstraintsDataType* constraints_data
);

MeasurementListDataType* MeasurementsCreateEmpty(void);

void MeasurementsDelete(MeasurementListDataType* measurement_data_list);

void MeasurementConstraintsDelete(MeasurementConstraintsListDataType* measurement_constraints);

MeasurementDataType* MeasurementDataCopy(const MeasurementDataType* measurement_data);

void MeasurementDataDelete(MeasurementDataType* measurement_data);

MeasurementConstraintsDataType* MeasurementConstraintsDataCopy(const MeasurementConstraintsDataType* constraints_data);

void MeasurementConstraintsDataDelete(MeasurementConstraintsDataType* constraints_data);

EebusError MeasurementConstraintsSetId(MeasurementConstraintsDataType* constraints_data, MeasurementIdType id);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_MEASUREMENT_TYPES_H_
