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
 * @brief SPINE Datagram Electrical Connection data types declarations
 */

#ifndef SRC_SPINE_MODEL_ELECTRICAL_CONNECTION_TYPES_H_
#define SRC_SPINE_MODEL_ELECTRICAL_CONNECTION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/measurement_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t ElectricalConnectionIdType;

typedef uint32_t ElectricalConnectionParameterIdType;

typedef uint32_t ElectricalConnectionCharacteristicIdType;

typedef int32_t ElectricalConnectionMeasurandVariantType;

enum {
    kElectricalConnectionMeasurandVariantTypeAmplitude,     /**< "amplitude" */
    kElectricalConnectionMeasurandVariantTypeRms,           /**< "rms" */
    kElectricalConnectionMeasurandVariantTypeInstantaneous, /**< "instantaneous" */
    kElectricalConnectionMeasurandVariantTypeAngle,         /**< "angle" */
    kElectricalConnectionMeasurandVariantTypeCosphi,        /**< "cosPhi" */
};

#define ELECTRICAL_CONNECTION_MEASURAND_VARIANT(name)   \
    (ElectricalConnectionMeasurandVariantType) {        \
        kElectricalConnectionMeasurandVariantType##name \
    }

typedef int32_t ElectricalConnectionVoltageTypeType;

enum {
    kElectricalConnectionVoltageTypeTypeAc, /**< "ac" */
    kElectricalConnectionVoltageTypeTypeDc, /**< "dc" */
};

#define ELECTRICAL_CONNECTION_VOLTAGE_TYPE(name)   \
    (ElectricalConnectionVoltageTypeType) {        \
        kElectricalConnectionVoltageTypeType##name \
    }

typedef int32_t ElectricalConnectionAcMeasurementTypeType;

enum {
    kElectricalConnectionAcMeasurementTypeTypeReal,     /**< "real" */
    kElectricalConnectionAcMeasurementTypeTypeReactive, /**< "reactive" */
    kElectricalConnectionAcMeasurementTypeTypeApparent, /**< "apparent" */
    kElectricalConnectionAcMeasurementTypeTypePhase,    /**< "phase" */
};

#define ELECTRICAL_CONNECTION_AC_MEASUREMENT_TYPE(name)  \
    (ElectricalConnectionAcMeasurementTypeType) {        \
        kElectricalConnectionAcMeasurementTypeType##name \
    }

enum Phase {
    kPhaseA       = 1,
    kPhaseB       = 2,
    kPhaseC       = 4,
    kPhaseNeutral = 8,
    kPhaseGround  = 16,
    kPhaseNone    = 0,
};

typedef int32_t ElectricalConnectionPhaseNameType;

enum {
    kElectricalConnectionPhaseNameTypeA       = kPhaseA,                     /**< "a" */
    kElectricalConnectionPhaseNameTypeB       = kPhaseB,                     /**< "b" */
    kElectricalConnectionPhaseNameTypeC       = kPhaseC,                     /**< "c" */
    kElectricalConnectionPhaseNameTypeAb      = kPhaseA | kPhaseB,           /**< "ab" */
    kElectricalConnectionPhaseNameTypeBc      = kPhaseB | kPhaseC,           /**< "bc" */
    kElectricalConnectionPhaseNameTypeAc      = kPhaseA | kPhaseC,           /**< "ac" */
    kElectricalConnectionPhaseNameTypeAbc     = kPhaseA | kPhaseB | kPhaseC, /**< "abc" */
    kElectricalConnectionPhaseNameTypeNeutral = kPhaseNeutral,               /**< "neutral" */
    kElectricalConnectionPhaseNameTypeGround  = kPhaseGround,                /**< "ground" */
    kElectricalConnectionPhaseNameTypeNone    = kPhaseNone,                  /**< "none" */
};

#define ELECTRICAL_CONNECTION_PHASE_NAME(name)   \
    (ElectricalConnectionPhaseNameType) {        \
        kElectricalConnectionPhaseNameType##name \
    }

typedef int32_t ElectricalConnectionConnectionPointType;

enum {
    kElectricalConnectionConnectionPointTypeGrid,  /**< "grid" */
    kElectricalConnectionConnectionPointTypeHome,  /**< "home" */
    kElectricalConnectionConnectionPointTypePv,    /**< "pv" */
    kElectricalConnectionConnectionPointTypeSd,    /**< "sd" */
    kElectricalConnectionConnectionPointTypeOther, /**< "other" */
};

typedef int32_t ElectricalConnectionCharacteristicContextType;

enum {
    kElectricalConnectionCharacteristicContextTypeDevice,   /**< "device" */
    kElectricalConnectionCharacteristicContextTypeEntity,   /**< "entity" */
    kElectricalConnectionCharacteristicContextTypeInverter, /**< "inverter" */
    kElectricalConnectionCharacteristicContextTypePvString, /**< "pvString" */
    kElectricalConnectionCharacteristicContextTypeBattery,  /**< "battery" */
};

typedef int32_t ElectricalConnectionCharacteristicTypeType;

enum {
    kElectricalConnectionCharacteristicTypeTypePowerConsumptionMin,              /**< "powerConsumptionMin" */
    kElectricalConnectionCharacteristicTypeTypePowerConsumptionMax,              /**< "powerConsumptionMax" */
    kElectricalConnectionCharacteristicTypeTypePowerConsumptionNominalMin,       /**< "powerConsumptionNominalMin" */
    kElectricalConnectionCharacteristicTypeTypePowerConsumptionNominalMax,       /**< "powerConsumptionNominalMax" */
    kElectricalConnectionCharacteristicTypeTypePowerProductionMin,               /**< "powerProductionMin" */
    kElectricalConnectionCharacteristicTypeTypePowerProductionMax,               /**< "powerProductionMax" */
    kElectricalConnectionCharacteristicTypeTypePowerProductionNominalMin,        /**< "powerProductionNominalMin" */
    kElectricalConnectionCharacteristicTypeTypePowerProductionNominalMax,        /**< "powerProductionNominalMax" */
    kElectricalConnectionCharacteristicTypeTypeEnergyCapacityNominalMax,         /**< "energyCapacityNominalMax" */
    kElectricalConnectionCharacteristicTypeTypeContractualConsumptionNominalMax, /**< "contractualConsumptionNominalMax"
                                                                                  */
    kElectricalConnectionCharacteristicTypeTypeContractualProductionNominalMax,  /**< "contractualProductionNominalMax"
                                                                                  */
    kElectricalConnectionCharacteristicTypeTypeApparentPowerProductionNominalMax,  /**<
                                                                                    * "apparentPowerProductionNominalMax"
                                                                                    */
    kElectricalConnectionCharacteristicTypeTypeApparentPowerConsumptionNominalMax, /**<
                                                                                      "apparentPowerConsumptionNominalMax"
                                                                                    */
};

typedef struct ElectricalConnectionParameterDescriptionDataType ElectricalConnectionParameterDescriptionDataType;

struct ElectricalConnectionParameterDescriptionDataType {
    const ElectricalConnectionIdType* electrical_connection_id;             /**< "electricalConnectionId" */
    const ElectricalConnectionParameterIdType* parameter_id;                /**< "parameterId" */
    const MeasurementIdType* measurement_id;                                /**< "measurementId" */
    const ElectricalConnectionVoltageTypeType* voltage_type;                /**< "voltageType" */
    const ElectricalConnectionPhaseNameType* ac_measured_phases;            /**< "acMeasuredPhases" */
    const ElectricalConnectionPhaseNameType* ac_measured_in_reference_to;   /**< "acMeasuredInReferenceTo" */
    const ElectricalConnectionAcMeasurementTypeType* ac_measurement_type;   /**< "acMeasurementType" */
    const ElectricalConnectionMeasurandVariantType* ac_measurement_variant; /**< "acMeasurementVariant" */
    const uint8_t* ac_measured_harmonic;                                    /**< "acMeasuredHarmonic" */
    const ScopeTypeType* scope_type;                                        /**< "scopeType" */
    LabelType label;                                                        /**< "label" */
    DescriptionType description;                                            /**< "description" */
};

typedef struct ElectricalConnectionParameterDescriptionDataElementsType
    ElectricalConnectionParameterDescriptionDataElementsType;

struct ElectricalConnectionParameterDescriptionDataElementsType {
    ElementTagType electrical_connection_id;    /**< "electricalConnectionId" */
    ElementTagType parameter_id;                /**< "parameterId" */
    ElementTagType measurement_id;              /**< "measurementId" */
    ElementTagType voltage_type;                /**< "voltageType" */
    ElementTagType ac_measured_phases;          /**< "acMeasuredPhases" */
    ElementTagType ac_measured_in_reference_to; /**< "acMeasuredInReferenceTo" */
    ElementTagType ac_measurement_type;         /**< "acMeasurementType" */
    ElementTagType ac_measurement_variant;      /**< "acMeasurementVariant" */
    ElementTagType ac_measured_harmonic;        /**< "acMeasuredHarmonic" */
    ElementTagType scope_type;                  /**< "scopeType" */
    ElementTagType label;                       /**< "label" */
    ElementTagType description;                 /**< "description" */
};

typedef struct ElectricalConnectionParameterDescriptionListDataType
    ElectricalConnectionParameterDescriptionListDataType;

struct ElectricalConnectionParameterDescriptionListDataType {
    const ElectricalConnectionParameterDescriptionDataType* const*
        electrical_connection_parameter_description_data; /**< "electricalConnectionParameterDescriptionData" */
    size_t electrical_connection_parameter_description_data_size;
};

typedef struct ElectricalConnectionParameterDescriptionListDataSelectorsType
    ElectricalConnectionParameterDescriptionListDataSelectorsType;

struct ElectricalConnectionParameterDescriptionListDataSelectorsType {
    const ElectricalConnectionIdType* electrical_connection_id; /**< "electricalConnectionId" */
    const ElectricalConnectionParameterIdType* parameter_id;    /**< "parameterId" */
    const MeasurementIdType* measurement_id;                    /**< "measurementId" */
    const ScopeTypeType* scope_type;                            /**< "scopeType" */
};

typedef struct ElectricalConnectionPermittedValueSetDataType ElectricalConnectionPermittedValueSetDataType;

struct ElectricalConnectionPermittedValueSetDataType {
    const ElectricalConnectionIdType* electrical_connection_id; /**< "electricalConnectionId" */
    const ElectricalConnectionParameterIdType* parameter_id;    /**< "parameterId" */
    const ScaledNumberSetType* const* permitted_value_set;      /**< "permittedValueSet" */
    size_t permitted_value_set_size;
};

typedef struct ElectricalConnectionPermittedValueSetDataElementsType
    ElectricalConnectionPermittedValueSetDataElementsType;

struct ElectricalConnectionPermittedValueSetDataElementsType {
    ElementTagType electrical_connection_id; /**< "electricalConnectionId" */
    ElementTagType parameter_id;             /**< "parameterId" */
    ElementTagType permitted_value_set;      /**< "permittedValueSet" */
};

typedef struct ElectricalConnectionPermittedValueSetListDataType ElectricalConnectionPermittedValueSetListDataType;

struct ElectricalConnectionPermittedValueSetListDataType {
    const ElectricalConnectionPermittedValueSetDataType* const*
        electrical_connection_permitted_value_set_data; /**< "electricalConnectionPermittedValueSetData" */
    size_t electrical_connection_permitted_value_set_data_size;
};

typedef struct ElectricalConnectionPermittedValueSetListDataSelectorsType
    ElectricalConnectionPermittedValueSetListDataSelectorsType;

struct ElectricalConnectionPermittedValueSetListDataSelectorsType {
    const ElectricalConnectionIdType* electrical_connection_id; /**< "electricalConnectionId" */
    const ElectricalConnectionParameterIdType* parameter_id;    /**< "parameterId" */
};

typedef struct ElectricalConnectionStateDataType ElectricalConnectionStateDataType;

struct ElectricalConnectionStateDataType {
    const ElectricalConnectionIdType* electrical_connection_id; /**< "electricalConnectionId" */
    const AbsoluteOrRelativeTimeType* timestamp;                /**< "timestamp" */
    const EnergyModeType* current_energy_mode;                  /**< "currentEnergyMode" */
    const DurationType* consumption_time;                       /**< "consumptionTime" */
    const DurationType* production_time;                        /**< "productionTime" */
    const DurationType* total_consumption_time;                 /**< "totalConsumptionTime" */
    const DurationType* total_production_time;                  /**< "totalProductionTime" */
};

typedef struct ElectricalConnectionStateDataElementsType ElectricalConnectionStateDataElementsType;

struct ElectricalConnectionStateDataElementsType {
    ElementTagType electrical_connection_id; /**< "electricalConnectionId" */
    ElementTagType timestamp;                /**< "timestamp" */
    ElementTagType current_energy_mode;      /**< "currentEnergyMode" */
    ElementTagType consumption_time;         /**< "consumptionTime" */
    ElementTagType production_time;          /**< "productionTime" */
    ElementTagType total_consumption_time;   /**< "totalConsumptionTime" */
    ElementTagType total_production_time;    /**< "totalProductionTime" */
};

typedef struct ElectricalConnectionStateListDataType ElectricalConnectionStateListDataType;

struct ElectricalConnectionStateListDataType {
    const ElectricalConnectionStateDataType* const*
        electrical_connection_state_data; /**< "electricalConnectionStateData" */
    size_t electrical_connection_state_data_size;
};

typedef struct ElectricalConnectionStateListDataSelectorsType ElectricalConnectionStateListDataSelectorsType;

struct ElectricalConnectionStateListDataSelectorsType {
    const ElectricalConnectionIdType* electrical_connection_id; /**< "electricalConnectionId" */
};

typedef struct ElectricalConnectionDescriptionDataType ElectricalConnectionDescriptionDataType;

struct ElectricalConnectionDescriptionDataType {
    const ElectricalConnectionIdType* electrical_connection_id;   /**< "electricalConnectionId" */
    const ElectricalConnectionVoltageTypeType* power_supply_type; /**< "powerSupplyType" */
    const uint32_t* ac_connected_phases;                          /**< "acConnectedPhases" */
    const DurationType* ac_rms_period_duration;                   /**< "acRmsPeriodDuration" */
    const EnergyDirectionType* positive_energy_direction;         /**< "positiveEnergyDirection" */
    const ScopeTypeType* scope_type;                              /**< "scopeType" */
    LabelType label;                                              /**< "label" */
    DescriptionType description;                                  /**< "description" */
};

typedef struct ElectricalConnectionDescriptionDataElementsType ElectricalConnectionDescriptionDataElementsType;

struct ElectricalConnectionDescriptionDataElementsType {
    ElementTagType electrical_connection_id;  /**< "electricalConnectionId" */
    ElementTagType power_supply_type;         /**< "powerSupplyType" */
    ElementTagType ac_connected_phases;       /**< "acConnectedPhases" */
    ElementTagType ac_rms_period_duration;    /**< "acRmsPeriodDuration" */
    ElementTagType positive_energy_direction; /**< "positiveEnergyDirection" */
    ElementTagType scope_type;                /**< "scopeType" */
    ElementTagType label;                     /**< "label" */
    ElementTagType description;               /**< "description" */
};

typedef struct ElectricalConnectionDescriptionListDataType ElectricalConnectionDescriptionListDataType;

struct ElectricalConnectionDescriptionListDataType {
    const ElectricalConnectionDescriptionDataType* const*
        electrical_connection_description_data; /**< "electricalConnectionDescriptionData" */
    size_t electrical_connection_description_data_size;
};

typedef struct ElectricalConnectionDescriptionListDataSelectorsType
    ElectricalConnectionDescriptionListDataSelectorsType;

struct ElectricalConnectionDescriptionListDataSelectorsType {
    const ElectricalConnectionIdType* electrical_connection_id; /**< "electricalConnectionId" */
    const ScopeTypeType* scope_type;                            /**< "scopeType" */
};

typedef struct ElectricalConnectionCharacteristicDataType ElectricalConnectionCharacteristicDataType;

struct ElectricalConnectionCharacteristicDataType {
    const ElectricalConnectionIdType* electrical_connection_id;                  /**< "electricalConnectionId" */
    const ElectricalConnectionParameterIdType* parameter_id;                     /**< "parameterId" */
    const ElectricalConnectionCharacteristicIdType* characteristic_id;           /**< "characteristicId" */
    const ElectricalConnectionCharacteristicContextType* characteristic_context; /**< "characteristicContext" */
    const ElectricalConnectionCharacteristicTypeType* characteristic_type;       /**< "characteristicType" */
    const ScaledNumberType* value;                                               /**< "value" */
    const UnitOfMeasurementType* unit;                                           /**< "unit" */
};

typedef struct ElectricalConnectionCharacteristicDataElementsType ElectricalConnectionCharacteristicDataElementsType;

struct ElectricalConnectionCharacteristicDataElementsType {
    ElementTagType electrical_connection_id; /**< "electricalConnectionId" */
    ElementTagType parameter_id;             /**< "parameterId" */
    ElementTagType characteristic_id;        /**< "characteristicId" */
    ElementTagType characteristic_context;   /**< "characteristicContext" */
    ElementTagType characteristic_type;      /**< "characteristicType" */
    ScaledNumberElementsType* value;         /**< "value" */
    ElementTagType unit;                     /**< "unit" */
};

typedef struct ElectricalConnectionCharacteristicListDataType ElectricalConnectionCharacteristicListDataType;

struct ElectricalConnectionCharacteristicListDataType {
    const ElectricalConnectionCharacteristicDataType* const*
        electrical_connection_characteristic_data; /**< "electricalConnectionCharacteristicData" */
    size_t electrical_connection_characteristic_data_size;
};

typedef struct ElectricalConnectionCharacteristicListDataSelectorsType
    ElectricalConnectionCharacteristicListDataSelectorsType;

struct ElectricalConnectionCharacteristicListDataSelectorsType {
    const ElectricalConnectionIdType* electrical_connection_id;                  /**< "electricalConnectionId" */
    const ElectricalConnectionParameterIdType* parameter_id;                     /**< "parameterId" */
    const ElectricalConnectionCharacteristicIdType* characteristic_id;           /**< "characteristicId" */
    const ElectricalConnectionCharacteristicContextType* characteristic_context; /**< "characteristicContext" */
    const ElectricalConnectionCharacteristicTypeType* characteristic_type;       /**< "characteristicType" */
};

/**
 * @brief Shared constant for entity-context electrical connection characteristic filtering.
 */
extern const ElectricalConnectionCharacteristicContextType kEccContextEntity;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_ELECTRICAL_CONNECTION_TYPES_H_
