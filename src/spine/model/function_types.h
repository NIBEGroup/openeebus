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
 * @brief SPINE Function specific type declarations
 */

#ifndef SRC_SPINE_MODEL_FUNCTION_TYPES_H_
#define SRC_SPINE_MODEL_FUNCTION_TYPES_H_

#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/possible_operations_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef int32_t FunctionType;

enum {
    kFunctionTypeActuatorLevelData,                                /**< "actuatorLevelData" */
    kFunctionTypeActuatorLevelDescriptionData,                     /**< "actuatorLevelDescriptionData" */
    kFunctionTypeActuatorSwitchData,                               /**< "actuatorSwitchData" */
    kFunctionTypeActuatorSwitchDescriptionData,                    /**< "actuatorSwitchDescriptionData" */
    kFunctionTypeAlarmListData,                                    /**< "alarmListData" */
    kFunctionTypeBillConstraintsListData,                          /**< "billConstraintsListData" */
    kFunctionTypeBillDescriptionListData,                          /**< "billDescriptionListData" */
    kFunctionTypeBillListData,                                     /**< "billListData" */
    kFunctionTypeBindingManagementDeleteCall,                      /**< "bindingManagementDeleteCall" */
    kFunctionTypeBindingManagementEntryListData,                   /**< "bindingManagementEntryListData" */
    kFunctionTypeBindingManagementRequestCall,                     /**< "bindingManagementRequestCall" */
    kFunctionTypeCommodityListData,                                /**< "commodityListData" */
    kFunctionTypeDataTunnelingCall,                                /**< "dataTunnelingCall" */
    kFunctionTypeDeviceClassificationManufacturerData,             /**< "deviceClassificationManufacturerData" */
    kFunctionTypeDeviceClassificationUserData,                     /**< "deviceClassificationUserData" */
    kFunctionTypeDeviceConfigurationKeyValueConstraintsListData,   /**< "deviceConfigurationKeyValueConstraintsListData"
                                                                    */
    kFunctionTypeDeviceConfigurationKeyValueDescriptionListData,   /**< "deviceConfigurationKeyValueDescriptionListData"
                                                                    */
    kFunctionTypeDeviceConfigurationKeyValueListData,              /**< "deviceConfigurationKeyValueListData" */
    kFunctionTypeDeviceDiagnosisHeartbeatData,                     /**< "deviceDiagnosisHeartbeatData" */
    kFunctionTypeDeviceDiagnosisServiceData,                       /**< "deviceDiagnosisServiceData" */
    kFunctionTypeDeviceDiagnosisStateData,                         /**< "deviceDiagnosisStateData" */
    kFunctionTypeDirectControlActivityListData,                    /**< "directControlActivityListData" */
    kFunctionTypeDirectControlDescriptionData,                     /**< "directControlDescriptionData" */
    kFunctionTypeElectricalConnectionCharacteristicData,           /**< "electricalConnectionCharacteristicData" */
    kFunctionTypeElectricalConnectionCharacteristicListData,       /**< "electricalConnectionCharacteristicListData" */
    kFunctionTypeElectricalConnectionDescriptionListData,          /**< "electricalConnectionDescriptionListData" */
    kFunctionTypeElectricalConnectionParameterDescriptionListData, /**<
                                                                    * "electricalConnectionParameterDescriptionListData"
                                                                    */
    kFunctionTypeElectricalConnectionPermittedValueSetListData, /**< "electricalConnectionPermittedValueSetListData" */
    kFunctionTypeElectricalConnectionStateListData,             /**< "electricalConnectionStateListData" */
    kFunctionTypeHvacOperationModeDescriptionListData,          /**< "hvacOperationModeDescriptionListData" */
    kFunctionTypeHvacOverrunDescriptionListData,                /**< "hvacOverrunDescriptionListData" */
    kFunctionTypeHvacOverrunListData,                           /**< "hvacOverrunListData" */
    kFunctionTypeHvacSystemFunctionDescriptionListData,         /**< "hvacSystemFunctionDescriptionListData" */
    kFunctionTypeHvacSystemFunctionListData,                    /**< "hvacSystemFunctionListData" */
    kFunctionTypeHvacSystemFunctionOperationModeRelationListData, /**< "hvacSystemFunctionOperationModeRelationListData"
                                                                   */
    kFunctionTypeHvacSystemFunctionPowerSequenceRelationListData, /**< "hvacSystemFunctionPowerSequenceRelationListData"
                                                                   */
    kFunctionTypeHvacSystemFunctionSetpointRelationListData,      /**< "hvacSystemFunctionSetpointRelationListData" */
    kFunctionTypeIdentificationListData,                          /**< "identificationListData" */
    kFunctionTypeIncentiveDescriptionListData,                    /**< "incentiveDescriptionListData" */
    kFunctionTypeIncentiveListData,                               /**< "incentiveListData" */
    kFunctionTypeIncentiveTableConstraintsData,                   /**< "incentiveTableConstraintsData" */
    kFunctionTypeIncentiveTableData,                              /**< "incentiveTableData" */
    kFunctionTypeIncentiveTableDescriptionData,                   /**< "incentiveTableDescriptionData" */
    kFunctionTypeLoadControlEventListData,                        /**< "loadControlEventListData" */
    kFunctionTypeLoadControlLimitConstraintsListData,             /**< "loadControlLimitConstraintsListData" */
    kFunctionTypeLoadControlLimitDescriptionListData,             /**< "loadControlLimitDescriptionListData" */
    kFunctionTypeLoadControlLimitListData,                        /**< "loadControlLimitListData" */
    kFunctionTypeLoadControlNodeData,                             /**< "loadControlNodeData" */
    kFunctionTypeLoadControlStateListData,                        /**< "loadControlStateListData" */
    kFunctionTypeMeasurementConstraintsListData,                  /**< "measurementConstraintsListData" */
    kFunctionTypeMeasurementDescriptionListData,                  /**< "measurementDescriptionListData" */
    kFunctionTypeMeasurementListData,                             /**< "measurementListData" */
    kFunctionTypeMeasurementSeriesListData,                       /**< "measurementSeriesListData" */
    kFunctionTypeMeasurementThresholdRelationListData,            /**< "measurementThresholdRelationListData" */
    kFunctionTypeMessagingListData,                               /**< "messagingListData" */
    kFunctionTypeNetworkManagementAbortCall,                      /**< "networkManagementAbortCall" */
    kFunctionTypeNetworkManagementAddNodeCall,                    /**< "networkManagementAddNodeCall" */
    kFunctionTypeNetworkManagementDeviceDescriptionListData,      /**< "networkManagementDeviceDescriptionListData" */
    kFunctionTypeNetworkManagementDiscoverCall,                   /**< "networkManagementDiscoverCall" */
    kFunctionTypeNetworkManagementEntityDescriptionListData,      /**< "networkManagementEntityDescriptionListData" */
    kFunctionTypeNetworkManagementFeatureDescriptionListData,     /**< "networkManagementFeatureDescriptionListData" */
    kFunctionTypeNetworkManagementJoiningModeData,                /**< "networkManagementJoiningModeData" */
    kFunctionTypeNetworkManagementModifyNodeCall,                 /**< "networkManagementModifyNodeCall" */
    kFunctionTypeNetworkManagementProcessStateData,               /**< "networkManagementProcessStateData" */
    kFunctionTypeNetworkManagementRemoveNodeCall,                 /**< "networkManagementRemoveNodeCall" */
    kFunctionTypeNetworkManagementReportCandidateData,            /**< "networkManagementReportCandidateData" */
    kFunctionTypeNetworkManagementScanNetworkCall,                /**< "networkManagementScanNetworkCall" */
    kFunctionTypeNodeManagementBindingData,                       /**< "nodeManagementBindingData" */
    kFunctionTypeNodeManagementBindingDeleteCall,                 /**< "nodeManagementBindingDeleteCall" */
    kFunctionTypeNodeManagementBindingRequestCall,                /**< "nodeManagementBindingRequestCall" */
    kFunctionTypeNodeManagementDestinationListData,               /**< "nodeManagementDestinationListData" */
    kFunctionTypeNodeManagementDetailedDiscoveryData,             /**< "nodeManagementDetailedDiscoveryData" */
    kFunctionTypeNodeManagementSubscriptionData,                  /**< "nodeManagementSubscriptionData" */
    kFunctionTypeNodeManagementSubscriptionDeleteCall,            /**< "nodeManagementSubscriptionDeleteCall" */
    kFunctionTypeNodeManagementSubscriptionRequestCall,           /**< "nodeManagementSubscriptionRequestCall" */
    kFunctionTypeNodeManagementUseCaseData,                       /**< "nodeManagementUseCaseData" */
    kFunctionTypeOperatingConstraintsDurationListData,            /**< "operatingConstraintsDurationListData" */
    kFunctionTypeOperatingConstraintsInterruptListData,           /**< "operatingConstraintsInterruptListData" */
    kFunctionTypeOperatingConstraintsPowerDescriptionListData,    /**< "operatingConstraintsPowerDescriptionListData" */
    kFunctionTypeOperatingConstraintsPowerLevelListData,          /**< "operatingConstraintsPowerLevelListData" */
    kFunctionTypeOperatingConstraintsPowerRangeListData,          /**< "operatingConstraintsPowerRangeListData" */
    kFunctionTypeOperatingConstraintsResumeImplicationListData, /**< "operatingConstraintsResumeImplicationListData" */
    kFunctionTypePowerSequenceAlternativesRelationListData,     /**< "powerSequenceAlternativesRelationListData" */
    kFunctionTypePowerSequenceDescriptionListData,              /**< "powerSequenceDescriptionListData" */
    kFunctionTypePowerSequenceNodeScheduleInformationData,      /**< "powerSequenceNodeScheduleInformationData" */
    kFunctionTypePowerSequencePriceCalculationRequestCall,      /**< "powerSequencePriceCalculationRequestCall" */
    kFunctionTypePowerSequencePriceListData,                    /**< "powerSequencePriceListData" */
    kFunctionTypePowerSequenceScheduleConfigurationRequestCall, /**< "powerSequenceScheduleConfigurationRequestCall" */
    kFunctionTypePowerSequenceScheduleConstraintsListData,      /**< "powerSequenceScheduleConstraintsListData" */
    kFunctionTypePowerSequenceScheduleListData,                 /**< "powerSequenceScheduleListData" */
    kFunctionTypePowerSequenceSchedulePreferenceListData,       /**< "powerSequenceSchedulePreferenceListData" */
    kFunctionTypePowerSequenceStateListData,                    /**< "powerSequenceStateListData" */
    kFunctionTypePowerTimeSlotScheduleConstraintsListData,      /**< "powerTimeSlotScheduleConstraintsListData" */
    kFunctionTypePowerTimeSlotScheduleListData,                 /**< "powerTimeSlotScheduleListData" */
    kFunctionTypePowerTimeSlotValueListData,                    /**< "powerTimeSlotValueListData" */
    kFunctionTypeResultData,                                    /**< "resultData" */
    kFunctionTypeSensingDescriptionData,                        /**< "sensingDescriptionData" */
    kFunctionTypeSensingListData,                               /**< "sensingListData" */
    kFunctionTypeSessionIdentificationListData,                 /**< "sessionIdentificationListData" */
    kFunctionTypeSessionMeasurementRelationListData,            /**< "sessionMeasurementRelationListData" */
    kFunctionTypeSetpointConstraintsListData,                   /**< "setpointConstraintsListData" */
    kFunctionTypeSetpointDescriptionListData,                   /**< "setpointDescriptionListData" */
    kFunctionTypeSetpointListData,                              /**< "setpointListData" */
    kFunctionTypeSmartEnergyManagementPsConfigurationRequestCall, /**< "smartEnergyManagementPsConfigurationRequestCall"
                                                                   */
    kFunctionTypeSmartEnergyManagementPsData,                     /**< "smartEnergyManagementPsData" */
    kFunctionTypeSmartEnergyManagementPsPriceCalculationRequestCall, /**<
                                                                        "smartEnergyManagementPsPriceCalculationRequestCall"
                                                                      */
    kFunctionTypeSmartEnergyManagementPsPriceData,                   /**< "smartEnergyManagementPsPriceData" */
    kFunctionTypeSpecificationVersionListData,                       /**< "specificationVersionListData" */
    kFunctionTypeStateInformationListData,                           /**< "stateInformationListData" */
    kFunctionTypeSubscriptionManagementDeleteCall,                   /**< "subscriptionManagementDeleteCall" */
    kFunctionTypeSubscriptionManagementEntryListData,                /**< "subscriptionManagementEntryListData" */
    kFunctionTypeSubscriptionManagementRequestCall,                  /**< "subscriptionManagementRequestCall" */
    kFunctionTypeSupplyConditionDescriptionListData,                 /**< "supplyConditionDescriptionListData" */
    kFunctionTypeSupplyConditionListData,                            /**< "supplyConditionListData" */
    kFunctionTypeSupplyConditionThresholdRelationListData,           /**< "supplyConditionThresholdRelationListData" */
    kFunctionTypeTariffBoundaryRelationListData,                     /**< "tariffBoundaryRelationListData" */
    kFunctionTypeTariffDescriptionListData,                          /**< "tariffDescriptionListData" */
    kFunctionTypeTariffListData,                                     /**< "tariffListData" */
    kFunctionTypeTariffOverallConstraintsData,                       /**< "tariffOverallConstraintsData" */
    kFunctionTypeTariffTierRelationListData,                         /**< "tariffTierRelationListData" */
    kFunctionTypeTaskManagementJobDescriptionListData,               /**< "taskManagementJobDescriptionListData" */
    kFunctionTypeTaskManagementJobListData,                          /**< "taskManagementJobListData" */
    kFunctionTypeTaskManagementJobRelationListData,                  /**< "taskManagementJobRelationListData" */
    kFunctionTypeTaskManagementOverviewData,                         /**< "taskManagementOverviewData" */
    kFunctionTypeThresholdConstraintsListData,                       /**< "thresholdConstraintsListData" */
    kFunctionTypeThresholdDescriptionListData,                       /**< "thresholdDescriptionListData" */
    kFunctionTypeThresholdListData,                                  /**< "thresholdListData" */
    kFunctionTypeTierBoundaryDescriptionListData,                    /**< "tierBoundaryDescriptionListData" */
    kFunctionTypeTierBoundaryListData,                               /**< "tierBoundaryListData" */
    kFunctionTypeTierDescriptionListData,                            /**< "tierDescriptionListData" */
    kFunctionTypeTierIncentiveRelationListData,                      /**< "tierIncentiveRelationListData" */
    kFunctionTypeTierListData,                                       /**< "tierListData" */
    kFunctionTypeTimeDistributorData,                                /**< "timeDistributorData" */
    kFunctionTypeTimeDistributorEnquiryCall,                         /**< "timeDistributorEnquiryCall" */
    kFunctionTypeTimeInformationData,                                /**< "timeInformationData" */
    kFunctionTypeTimePrecisionData,                                  /**< "timePrecisionData" */
    kFunctionTypeTimeSeriesConstraintsListData,                      /**< "timeSeriesConstraintsListData" */
    kFunctionTypeTimeSeriesDescriptionListData,                      /**< "timeSeriesDescriptionListData" */
    kFunctionTypeTimeSeriesListData,                                 /**< "timeSeriesListData" */
    kFunctionTypeTimeTableConstraintsListData,                       /**< "timeTableConstraintsListData" */
    kFunctionTypeTimeTableDescriptionListData,                       /**< "timeTableDescriptionListData" */
    kFunctionTypeTimeTableListData,                                  /**< "timeTableListData" */
    kFunctionTypeUseCaseInformationListData,                         /**< "useCaseInformationListData" */
    kFunctionTypeNum,
};

typedef struct FunctionPropertyType FunctionPropertyType;

struct FunctionPropertyType {
    const FunctionType* function;                      /**< "function" */
    const PossibleOperationsType* possible_operations; /**< "possibleOperations" */
};

typedef struct FunctionPropertyElementsType FunctionPropertyElementsType;

struct FunctionPropertyElementsType {
    ElementTagType function;            /**< "function" */
    ElementTagType possible_operations; /**< "possibleOperations" */
};

const FunctionPropertyType*
FunctionPropertyCreate(FunctionType function_type, const PossibleOperationsType* operations);
void FunctionPropertyDelete(FunctionPropertyType* self);
void FunctionPropertyElementsDelete(FunctionPropertyElementsType* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_FUNCTION_TYPES_H_
