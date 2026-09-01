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
 * @brief SPINE Datagram Usecase Information data types declarations
 */

#ifndef SRC_SPINE_MODEL_USECASE_INFORMATION_TYPES_H_
#define SRC_SPINE_MODEL_USECASE_INFORMATION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/spine/model/common_data_types.h"
#include "src/spine/model/feature_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef int32_t UseCaseActorType;

enum {
    kUseCaseActorTypeBattery,                  /**< "Battery" */
    kUseCaseActorTypeBatterySystem,            /**< "BatterySystem" */
    kUseCaseActorTypeCEM,                      /**< "CEM" */
    kUseCaseActorTypeConfigurationAppliance,   /**< "ConfigurationAppliance" */
    kUseCaseActorTypeCompressor,               /**< "Compressor" */
    kUseCaseActorTypeControllableSystem,       /**< "ControllableSystem" */
    kUseCaseActorTypeDHWCircuit,               /**< "DHWCircuit" */
    kUseCaseActorTypeEnergyBroker,             /**< "EnergyBroker" */
    kUseCaseActorTypeEnergyConsumer,           /**< "EnergyConsumer" */
    kUseCaseActorTypeEnergyGuard,              /**< "EnergyGuard" */
    kUseCaseActorTypeEVSE,                     /**< "EVSE" */
    kUseCaseActorTypeEV,                       /**< "EV" */
    kUseCaseActorTypeGridConnectionPoint,      /**< "GridConnectionPoint" */
    kUseCaseActorTypeHeatPump,                 /**< "HeatPump" */
    kUseCaseActorTypeHeatingCircuit,           /**< "HeatingCircuit" */
    kUseCaseActorTypeHeatingZone,              /**< "HeatingZone" */
    kUseCaseActorTypeHVACRoom,                 /**< "HVACRoom" */
    kUseCaseActorTypeInverter,                 /**< "Inverter" */
    kUseCaseActorTypeMonitoredUnit,            /**< "MonitoredUnit" */
    kUseCaseActorTypeMonitoringAppliance,      /**< "MonitoringAppliance" */
    kUseCaseActorTypeOutdoorTemperatureSensor, /**< "OutdoorTemperatureSensor" */
    kUseCaseActorTypePVString,                 /**< "PVString" */
    kUseCaseActorTypePVSystem,                 /**< "PVSystem" */
    kUseCaseActorTypeSmartAppliance,           /**< "SmartAppliance" */
    kUseCaseActorTypeVisualizationAppliance,   /**< "VisualizationAppliance" */
};

typedef int32_t UseCaseNameType;

enum {
    kUseCaseNameTypeConfigurationOfDhwSystemFunction,               /**< "configurationOfDhwSystemFunction" */
    kUseCaseNameTypeConfigurationOfDhwTemperature,                  /**< "configurationOfDhwTemperature" */
    kUseCaseNameTypeConfigurationOfRoomCoolingSystemFunction,       /**< "configurationOfRoomCoolingSystemFunction" */
    kUseCaseNameTypeConfigurationOfRoomCoolingTemperature,          /**< "configurationOfRoomCoolingTemperature" */
    kUseCaseNameTypeConfigurationOfRoomHeatingSystemFunction,       /**< "configurationOfRoomHeatingSystemFunction" */
    kUseCaseNameTypeConfigurationOfRoomHeatingTemperature,          /**< "configurationOfRoomHeatingTemperature" */
    kUseCaseNameTypeControlOfBattery,                               /**< "controlOfBattery" */
    kUseCaseNameTypeCoordinatedEVCharging,                          /**< "coordinatedEvCharging" */
    kUseCaseNameTypeEVChargingSummary,                              /**< "evChargingSummary" */
    kUseCaseNameTypeEVCommissioningAndConfiguration,                /**< "evCommissioningAndConfiguration" */
    kUseCaseNameTypeEVSECommissioningAndConfiguration,              /**< "evseCommissioningAndConfiguration" */
    kUseCaseNameTypeEVStateOfCharge,                                /**< "evStateOfCharge" */
    kUseCaseNameTypeFlexibleLoad,                                   /**< "flexibleLoad" */
    kUseCaseNameTypeFlexibleStartForWhiteGoods,                     /**< "flexibleStartForWhiteGoods" */
    kUseCaseNameTypeLimitationOfPowerConsumption,                   /**< "limitationOfPowerConsumption" */
    kUseCaseNameTypeLimitationOfPowerProduction,                    /**< "limitationOfPowerProduction" */
    kUseCaseNameTypeIncentiveTableBasedPowerConsumptionManagement,  /**< "incentiveTableBasedPowerConsumptionManagement"
                                                                     */
    kUseCaseNameTypeMeasurementOfElectricityDuringEVCharging,       /**< "measurementOfElectricityDuringEvCharging" */
    kUseCaseNameTypeMonitoringAndControlOfSmartGridReadyConditions, /**<
                                                                     * "monitoringAndControlOfSmartGridReadyConditions"
                                                                     */
    kUseCaseNameTypeMonitoringOfBattery,                            /**< "monitoringOfBattery" */
    kUseCaseNameTypeMonitoringOfDhwSystemFunction,                  /**< "monitoringOfDhwSystemFunction" */
    kUseCaseNameTypeMonitoringOfDhwTemperature,                     /**< "monitoringOfDhwTemperature" */
    kUseCaseNameTypeMonitoringOfGridConnectionPoint,                /**< "monitoringOfGridConnectionPoint" */
    kUseCaseNameTypeMonitoringOfInverter,                           /**< "monitoringOfInverter" */
    kUseCaseNameTypeMonitoringOfOutdoorTemperature,                 /**< "monitoringOfOutdoorTemperature" */
    kUseCaseNameTypeMonitoringOfPowerConsumption,                   /**< "monitoringOfPowerConsumption" */
    kUseCaseNameTypeMonitoringOfPvString,                           /**< "monitoringOfPvString" */
    kUseCaseNameTypeMonitoringOfRoomCoolingSystemFunction,          /**< "monitoringOfRoomCoolingSystemFunction" */
    kUseCaseNameTypeMonitoringOfRoomHeatingSystemFunction,          /**< "monitoringOfRoomHeatingSystemFunction" */
    kUseCaseNameTypeMonitoringOfRoomTemperature,                    /**< "monitoringOfRoomTemperature" */
    kUseCaseNameTypeOptimizationOfSelfConsumptionByHeatPumpCompressorFlexibility, /**<
                                                                                     "optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility"
                                                                                   */
    kUseCaseNameTypeOptimizationOfSelfConsumptionDuringEVCharging, /**< "optimizationOfSelfConsumptionDuringEvCharging"
                                                                    */
    kUseCaseNameTypeOverloadProtectionByEVChargingCurrentCurtailment, /**<
                                                                         "overloadProtectionByEvChargingCurrentCurtailment"
                                                                       */
    kUseCaseNameTypeVisualizationOfAggregatedBatteryData,             /**< "visualizationOfAggregatedBatteryData" */
    kUseCaseNameTypeVisualizationOfAggregatedPhotovoltaicData, /**< "visualizationOfAggregatedPhotovoltaicData" */
    kUseCaseNameTypeVisualizationOfHeatingAreaName,            /**< "visualizationOfHeatingAreaName" */
};

typedef uint32_t UseCaseScenarioSupportType;

typedef struct UseCaseSupportType UseCaseSupportType;

struct UseCaseSupportType {
    const UseCaseNameType* use_case_name;                      /**< "useCaseName" */
    SpecificationVersionType use_case_version;                 /**< "useCaseVersion" */
    const bool* use_case_available;                            /**< "useCaseAvailable" */
    const UseCaseScenarioSupportType* const* scenario_support; /**< "scenarioSupport" */
    size_t scenario_support_size;
    const char* use_case_document_sub_revision; /**< "useCaseDocumentSubRevision" */
};

typedef struct UseCaseSupportElementsType UseCaseSupportElementsType;

struct UseCaseSupportElementsType {
    ElementTagType use_case_name;                  /**< "useCaseName" */
    ElementTagType use_case_version;               /**< "useCaseVersion" */
    ElementTagType use_case_available;             /**< "useCaseAvailable" */
    ElementTagType scenario_support;               /**< "scenarioSupport" */
    ElementTagType use_case_document_sub_revision; /**< "useCaseDocumentSubRevision" */
};

typedef struct UseCaseSupportSelectorsType UseCaseSupportSelectorsType;

struct UseCaseSupportSelectorsType {
    const UseCaseNameType* use_case_name;               /**< "useCaseName" */
    SpecificationVersionType use_case_version;          /**< "useCaseVersion" */
    const UseCaseScenarioSupportType* scenario_support; /**< "scenarioSupport" */
};

typedef struct UseCaseInformationDataType UseCaseInformationDataType;

struct UseCaseInformationDataType {
    const FeatureAddressType* address;                 /**< "address" */
    const UseCaseActorType* actor;                     /**< "actor" */
    const UseCaseSupportType* const* use_case_support; /**< "useCaseSupport" */
    size_t use_case_support_size;
};

typedef struct UseCaseInformationDataElementsType UseCaseInformationDataElementsType;

struct UseCaseInformationDataElementsType {
    ElementTagType address;          /**< "address" */
    ElementTagType actor;            /**< "actor" */
    ElementTagType use_case_support; /**< "useCaseSupport" */
};

typedef struct UseCaseInformationListDataType UseCaseInformationListDataType;

struct UseCaseInformationListDataType {
    const UseCaseInformationDataType* const* use_case_information_data; /**< "useCaseInformationData" */
    size_t use_case_information_data_size;
};

typedef struct UseCaseInformationListDataSelectorsType UseCaseInformationListDataSelectorsType;

struct UseCaseInformationListDataSelectorsType {
    const FeatureAddressType* address;                   /**< "address" */
    const UseCaseActorType* actor;                       /**< "actor" */
    const UseCaseSupportSelectorsType* use_case_support; /**< "useCaseSupport" */
};

UseCaseInformationDataType* UseCaseInformationDataCreate(
    const FeatureAddressType* addr,
    UseCaseActorType actor,
    const UseCaseSupportType* use_case_support
);

void UseCaseInformationDataDelete(UseCaseInformationDataType* use_case_info);

bool UseCaseInformationMatch(
    const UseCaseInformationDataType* use_case_info,
    const FeatureAddressType* addr,
    const UseCaseActorType* actor,
    const UseCaseNameType* use_case_name_id
);

EebusError UseCaseInformationAdd(UseCaseInformationDataType* use_case_info, const UseCaseSupportType* use_case_support);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_USECASE_INFORMATION_TYPES_H_
