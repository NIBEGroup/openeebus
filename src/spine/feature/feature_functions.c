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
 * @brief SPINE Feature related functions table implementation
 */

#include "src/spine/feature/feature_functions.h"

#include "src/common/array_util.h"

#define FEATURE_FUNCTIONS(feature, functions) {feature, functions, ARRAY_SIZE(functions)}

static const FunctionType actuator_level_functions[] = {
    kFunctionTypeActuatorLevelData,
    kFunctionTypeActuatorLevelDescriptionData,
};

static const FunctionType actuator_switch_functions[] = {
    kFunctionTypeActuatorSwitchData,
    kFunctionTypeActuatorSwitchDescriptionData,
};

static const FunctionType alarm_functions[] = {
    kFunctionTypeAlarmListData,
};

static const FunctionType data_tunneling_functions[] = {
    kFunctionTypeDataTunnelingCall,
};

static const FunctionType device_classification_functions[] = {
    kFunctionTypeDeviceClassificationManufacturerData,
    kFunctionTypeDeviceClassificationUserData,
};

static const FunctionType device_diagnosis_functions[] = {
    kFunctionTypeDeviceDiagnosisStateData,
    kFunctionTypeDeviceDiagnosisHeartbeatData,
    kFunctionTypeDeviceDiagnosisServiceData,
};

static const FunctionType direct_control_functions[] = {
    kFunctionTypeDirectControlActivityListData,
    kFunctionTypeDirectControlDescriptionData,
};

static const FunctionType electrical_connection_functions[] = {
    kFunctionTypeElectricalConnectionDescriptionListData,
    kFunctionTypeElectricalConnectionParameterDescriptionListData,
    kFunctionTypeElectricalConnectionPermittedValueSetListData,
    kFunctionTypeElectricalConnectionStateListData,
    kFunctionTypeElectricalConnectionCharacteristicData,
    kFunctionTypeElectricalConnectionCharacteristicListData,
};

static const FunctionType generic_functions[] = {
    kFunctionTypeActuatorLevelData,
    kFunctionTypeActuatorLevelDescriptionData,
    kFunctionTypeActuatorSwitchData,
    kFunctionTypeActuatorSwitchDescriptionData,
    kFunctionTypeAlarmListData,
    kFunctionTypeBillDescriptionListData,
    kFunctionTypeBillConstraintsListData,
    kFunctionTypeBillListData,
    kFunctionTypeDataTunnelingCall,
    kFunctionTypeDeviceClassificationManufacturerData,
    kFunctionTypeDeviceClassificationUserData,
    kFunctionTypeDeviceConfigurationKeyValueConstraintsListData,
    kFunctionTypeDeviceConfigurationKeyValueDescriptionListData,
    kFunctionTypeDeviceConfigurationKeyValueListData,
    kFunctionTypeDeviceDiagnosisStateData,
    kFunctionTypeDeviceDiagnosisHeartbeatData,
    kFunctionTypeDeviceDiagnosisServiceData,
    kFunctionTypeDirectControlActivityListData,
    kFunctionTypeDirectControlDescriptionData,
    kFunctionTypeElectricalConnectionDescriptionListData,
    kFunctionTypeElectricalConnectionParameterDescriptionListData,
    kFunctionTypeElectricalConnectionPermittedValueSetListData,
    kFunctionTypeElectricalConnectionStateListData,
    kFunctionTypeElectricalConnectionCharacteristicData,
    kFunctionTypeElectricalConnectionCharacteristicListData,
    kFunctionTypeHvacOperationModeDescriptionListData,
    kFunctionTypeHvacOverrunDescriptionListData,
    kFunctionTypeHvacOverrunListData,
    kFunctionTypeHvacSystemFunctionDescriptionListData,
    kFunctionTypeHvacSystemFunctionListData,
    kFunctionTypeHvacSystemFunctionOperationModeRelationListData,
    kFunctionTypeHvacSystemFunctionPowerSequenceRelationListData,
    kFunctionTypeHvacSystemFunctionSetpointRelationListData,
    kFunctionTypeIdentificationListData,
    kFunctionTypeSessionIdentificationListData,
    kFunctionTypeSessionMeasurementRelationListData,
    kFunctionTypeIncentiveTableDescriptionData,
    kFunctionTypeIncentiveTableConstraintsData,
    kFunctionTypeIncentiveTableData,
    kFunctionTypeLoadControlEventListData,
    kFunctionTypeLoadControlLimitConstraintsListData,
    kFunctionTypeLoadControlLimitDescriptionListData,
    kFunctionTypeLoadControlLimitListData,
    kFunctionTypeLoadControlNodeData,
    kFunctionTypeLoadControlStateListData,
    kFunctionTypeMeasurementListData,
    kFunctionTypeMeasurementDescriptionListData,
    kFunctionTypeMeasurementConstraintsListData,
    kFunctionTypeMeasurementThresholdRelationListData,
    kFunctionTypeMeasurementSeriesListData,
    kFunctionTypeMessagingListData,
    kFunctionTypeNetworkManagementAbortCall,
    kFunctionTypeNetworkManagementAddNodeCall,
    kFunctionTypeNetworkManagementDeviceDescriptionListData,
    kFunctionTypeNetworkManagementDiscoverCall,
    kFunctionTypeNetworkManagementEntityDescriptionListData,
    kFunctionTypeNetworkManagementFeatureDescriptionListData,
    kFunctionTypeNetworkManagementJoiningModeData,
    kFunctionTypeNetworkManagementModifyNodeCall,
    kFunctionTypeNetworkManagementProcessStateData,
    kFunctionTypeNetworkManagementRemoveNodeCall,
    kFunctionTypeNetworkManagementReportCandidateData,
    kFunctionTypeNetworkManagementScanNetworkCall,
    kFunctionTypeOperatingConstraintsDurationListData,
    kFunctionTypeOperatingConstraintsInterruptListData,
    kFunctionTypeOperatingConstraintsPowerDescriptionListData,
    kFunctionTypeOperatingConstraintsPowerLevelListData,
    kFunctionTypeOperatingConstraintsPowerRangeListData,
    kFunctionTypeOperatingConstraintsResumeImplicationListData,
    kFunctionTypePowerSequenceAlternativesRelationListData,
    kFunctionTypePowerSequenceDescriptionListData,
    kFunctionTypePowerSequenceNodeScheduleInformationData,
    kFunctionTypePowerSequencePriceCalculationRequestCall,
    kFunctionTypePowerSequencePriceListData,
    kFunctionTypePowerSequenceScheduleConfigurationRequestCall,
    kFunctionTypePowerSequenceScheduleConstraintsListData,
    kFunctionTypePowerSequenceScheduleListData,
    kFunctionTypePowerSequenceSchedulePreferenceListData,
    kFunctionTypePowerSequenceStateListData,
    kFunctionTypePowerTimeSlotScheduleConstraintsListData,
    kFunctionTypePowerTimeSlotScheduleListData,
    kFunctionTypePowerTimeSlotValueListData,
    kFunctionTypeSensingDescriptionData,
    kFunctionTypeSensingListData,
    kFunctionTypeSetpointConstraintsListData,
    kFunctionTypeSetpointDescriptionListData,
    kFunctionTypeSetpointListData,
    kFunctionTypeSmartEnergyManagementPsConfigurationRequestCall,
    kFunctionTypeSmartEnergyManagementPsData,
    kFunctionTypeSmartEnergyManagementPsPriceCalculationRequestCall,
    kFunctionTypeSmartEnergyManagementPsPriceData,
    kFunctionTypeStateInformationListData,
    kFunctionTypeSupplyConditionDescriptionListData,
    kFunctionTypeSupplyConditionListData,
    kFunctionTypeSupplyConditionThresholdRelationListData,
    kFunctionTypeIncentiveDescriptionListData,
    kFunctionTypeIncentiveListData,
    kFunctionTypeTariffBoundaryRelationListData,
    kFunctionTypeTariffDescriptionListData,
    kFunctionTypeTariffListData,
    kFunctionTypeTariffOverallConstraintsData,
    kFunctionTypeTariffTierRelationListData,
    kFunctionTypeTierBoundaryDescriptionListData,
    kFunctionTypeTierBoundaryListData,
    kFunctionTypeTierDescriptionListData,
    kFunctionTypeTierIncentiveRelationListData,
    kFunctionTypeTierListData,
    kFunctionTypeTaskManagementJobDescriptionListData,
    kFunctionTypeTaskManagementJobListData,
    kFunctionTypeTaskManagementJobRelationListData,
    kFunctionTypeTaskManagementOverviewData,
    kFunctionTypeThresholdConstraintsListData,
    kFunctionTypeThresholdDescriptionListData,
    kFunctionTypeThresholdListData,
    kFunctionTypeTimeDistributorData,
    kFunctionTypeTimeDistributorEnquiryCall,
    kFunctionTypeTimeInformationData,
    kFunctionTypeTimePrecisionData,
    kFunctionTypeTimeSeriesDescriptionListData,
    kFunctionTypeTimeSeriesConstraintsListData,
    kFunctionTypeTimeSeriesListData,
    kFunctionTypeTimeTableConstraintsListData,
    kFunctionTypeTimeTableDescriptionListData,
    kFunctionTypeTimeTableListData,
};

static const FunctionType hvac_functions[] = {
    kFunctionTypeHvacOperationModeDescriptionListData,
    kFunctionTypeHvacOverrunDescriptionListData,
    kFunctionTypeHvacOverrunListData,
    kFunctionTypeHvacSystemFunctionDescriptionListData,
    kFunctionTypeHvacSystemFunctionListData,
    kFunctionTypeHvacSystemFunctionOperationModeRelationListData,
    kFunctionTypeHvacSystemFunctionPowerSequenceRelationListData,
    kFunctionTypeHvacSystemFunctionSetpointRelationListData,
};

static const FunctionType load_control_functions[] = {
    kFunctionTypeLoadControlEventListData,
    kFunctionTypeLoadControlLimitConstraintsListData,
    kFunctionTypeLoadControlLimitDescriptionListData,
    kFunctionTypeLoadControlLimitListData,
    kFunctionTypeLoadControlNodeData,
    kFunctionTypeLoadControlStateListData,
};

static const FunctionType measurement_functions[] = {
    kFunctionTypeMeasurementListData,
    kFunctionTypeMeasurementDescriptionListData,
    kFunctionTypeMeasurementConstraintsListData,
    kFunctionTypeMeasurementThresholdRelationListData,
    kFunctionTypeMeasurementSeriesListData,
};

static const FunctionType messaging_functions[] = {
    kFunctionTypeMessagingListData,
};

static const FunctionType network_management_functions[] = {
    kFunctionTypeNetworkManagementAbortCall,
    kFunctionTypeNetworkManagementAddNodeCall,
    kFunctionTypeNetworkManagementDeviceDescriptionListData,
    kFunctionTypeNetworkManagementDiscoverCall,
    kFunctionTypeNetworkManagementEntityDescriptionListData,
    kFunctionTypeNetworkManagementFeatureDescriptionListData,
    kFunctionTypeNetworkManagementJoiningModeData,
    kFunctionTypeNetworkManagementModifyNodeCall,
    kFunctionTypeNetworkManagementProcessStateData,
    kFunctionTypeNetworkManagementRemoveNodeCall,
    kFunctionTypeNetworkManagementReportCandidateData,
    kFunctionTypeNetworkManagementScanNetworkCall,
};

// TODO: Check if we have to add only 3 following functions to node_management_functions:
// 1) kFunctionTypeNodeManagementDestinationListData;
// 2) kFunctionTypeNodeManagementDetailedDiscoveryData;
// 3) kFunctionTypeNodeManagementUseCaseData.

static const FunctionType node_management_functions[] = {
    kFunctionTypeNodeManagementBindingData,
    kFunctionTypeNodeManagementBindingDeleteCall,
    kFunctionTypeNodeManagementBindingRequestCall,
    kFunctionTypeNodeManagementDestinationListData,
    kFunctionTypeNodeManagementDetailedDiscoveryData,
    kFunctionTypeNodeManagementSubscriptionData,
    kFunctionTypeNodeManagementSubscriptionDeleteCall,
    kFunctionTypeNodeManagementSubscriptionRequestCall,
    kFunctionTypeNodeManagementUseCaseData,
};

static const FunctionType operating_constraints_functions[] = {
    kFunctionTypeOperatingConstraintsDurationListData,
    kFunctionTypeOperatingConstraintsInterruptListData,
    kFunctionTypeOperatingConstraintsPowerDescriptionListData,
    kFunctionTypeOperatingConstraintsPowerLevelListData,
    kFunctionTypeOperatingConstraintsPowerRangeListData,
    kFunctionTypeOperatingConstraintsResumeImplicationListData,
};

static const FunctionType power_sequences_functions[] = {
    kFunctionTypePowerSequenceAlternativesRelationListData,
    kFunctionTypePowerSequenceDescriptionListData,
    kFunctionTypePowerSequenceNodeScheduleInformationData,
    kFunctionTypePowerSequencePriceCalculationRequestCall,
    kFunctionTypePowerSequencePriceListData,
    kFunctionTypePowerSequenceScheduleConfigurationRequestCall,
    kFunctionTypePowerSequenceScheduleConstraintsListData,
    kFunctionTypePowerSequenceScheduleListData,
    kFunctionTypePowerSequenceSchedulePreferenceListData,
    kFunctionTypePowerSequenceStateListData,
    kFunctionTypePowerTimeSlotScheduleConstraintsListData,
    kFunctionTypePowerTimeSlotScheduleListData,
    kFunctionTypePowerTimeSlotValueListData,
};

static const FunctionType sensing_functions[] = {
    kFunctionTypeSensingDescriptionData,
    kFunctionTypeSensingListData,
};

static const FunctionType setpoint_functions[] = {
    kFunctionTypeSetpointConstraintsListData,
    kFunctionTypeSetpointDescriptionListData,
    kFunctionTypeSetpointListData,
};

static const FunctionType smart_energy_management_ps_functions[] = {
    kFunctionTypeSmartEnergyManagementPsConfigurationRequestCall,
    kFunctionTypeSmartEnergyManagementPsData,
    kFunctionTypeSmartEnergyManagementPsPriceCalculationRequestCall,
    kFunctionTypeSmartEnergyManagementPsPriceData,
};

static const FunctionType task_management_functions[] = {
    kFunctionTypeTaskManagementJobDescriptionListData,
    kFunctionTypeTaskManagementJobListData,
    kFunctionTypeTaskManagementJobRelationListData,
    kFunctionTypeTaskManagementOverviewData,
};

static const FunctionType threshold_functions[] = {
    kFunctionTypeThresholdConstraintsListData,
    kFunctionTypeThresholdDescriptionListData,
    kFunctionTypeThresholdListData,
};

static const FunctionType time_information_functions[] = {
    kFunctionTypeTimeDistributorData,
    kFunctionTypeTimeDistributorEnquiryCall,
    kFunctionTypeTimeInformationData,
    kFunctionTypeTimePrecisionData,
};

static const FunctionType timetable_functions[] = {
    kFunctionTypeTimeTableConstraintsListData,
    kFunctionTypeTimeTableDescriptionListData,
    kFunctionTypeTimeTableListData,
};

static const FunctionType device_configuration_functions[] = {
    kFunctionTypeDeviceConfigurationKeyValueConstraintsListData,
    kFunctionTypeDeviceConfigurationKeyValueDescriptionListData,
    kFunctionTypeDeviceConfigurationKeyValueListData,
};

static const FunctionType supply_condition_functions[] = {
    kFunctionTypeSupplyConditionDescriptionListData,
    kFunctionTypeSupplyConditionListData,
    kFunctionTypeSupplyConditionThresholdRelationListData,
};

static const FunctionType time_series_functions[] = {
    kFunctionTypeTimeSeriesDescriptionListData,
    kFunctionTypeTimeSeriesConstraintsListData,
    kFunctionTypeTimeSeriesListData,
};

static const FunctionType tariff_information_functions[] = {
    kFunctionTypeIncentiveDescriptionListData,
    kFunctionTypeIncentiveListData,
    kFunctionTypeTariffBoundaryRelationListData,
    kFunctionTypeTariffDescriptionListData,
    kFunctionTypeTariffListData,
    kFunctionTypeTariffOverallConstraintsData,
    kFunctionTypeTariffTierRelationListData,
    kFunctionTypeTierBoundaryDescriptionListData,
    kFunctionTypeTierBoundaryListData,
    kFunctionTypeTierDescriptionListData,
    kFunctionTypeTierIncentiveRelationListData,
    kFunctionTypeTierListData,
};

static const FunctionType incentive_table_functions[] = {
    kFunctionTypeIncentiveTableDescriptionData,
    kFunctionTypeIncentiveTableConstraintsData,
    kFunctionTypeIncentiveTableData,
};

static const FunctionType bill_functions[] = {
    kFunctionTypeBillDescriptionListData,
    kFunctionTypeBillConstraintsListData,
    kFunctionTypeBillListData,
};

static const FunctionType identification_functions[] = {
    kFunctionTypeIdentificationListData,
    kFunctionTypeSessionIdentificationListData,
    kFunctionTypeSessionMeasurementRelationListData,
};

static const FunctionType state_information_functions[] = {
    kFunctionTypeStateInformationListData,
};

static const FeatureFunctions feature_functions_table[] = {
    FEATURE_FUNCTIONS(kFeatureTypeTypeActuatorLevel, actuator_level_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeActuatorSwitch, actuator_switch_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeAlarm, alarm_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeDataTunneling, data_tunneling_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeDeviceClassification, device_classification_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeDeviceDiagnosis, device_diagnosis_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeDirectControl, direct_control_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeElectricalConnection, electrical_connection_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeGeneric, generic_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeHvac, hvac_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeLoadControl, load_control_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeMeasurement, measurement_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeMessaging, messaging_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeNetworkManagement, network_management_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeNodeManagement, node_management_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeOperatingConstraints, operating_constraints_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypePowerSequences, power_sequences_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeSensing, sensing_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeSetpoint, setpoint_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeSmartEnergyManagementPs, smart_energy_management_ps_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeTaskManagement, task_management_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeThreshold, threshold_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeTimeInformation, time_information_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeTimeTable, timetable_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeDeviceConfiguration, device_configuration_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeSupplyCondition, supply_condition_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeTimeSeries, time_series_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeTariffInformation, tariff_information_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeIncentiveTable, incentive_table_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeBill, bill_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeIdentification, identification_functions),
    FEATURE_FUNCTIONS(kFeatureTypeTypeStateInformation, state_information_functions),
};

const FeatureFunctions* GetFeatureFunctions(FeatureTypeType feature_type) {
    for (size_t i = 0; i < ARRAY_SIZE(feature_functions_table); ++i) {
        if (feature_functions_table[i].feature_type == feature_type) {
            return &feature_functions_table[i];
        }
    }

    return NULL;
}
