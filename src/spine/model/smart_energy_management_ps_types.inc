/**
 * @file
 * @brief SPINE Datagram Smart Energy Management Ps payload Data Configuration
 */

#ifndef SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_TYPES_INC_
#define SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_TYPES_INC_

#include <stddef.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/common_data_types.inc"
#include "src/spine/model/operating_constraints_types.inc"
#include "src/spine/model/power_sequences_types.inc"
#include "src/spine/model/smart_energy_management_ps_types.h"

static const EebusDataCfg smart_energy_management_ps_power_time_slot_value_list_cfg[] = {
    EEBUS_DATA_LIST_SIMPLE("value", SmartEnergyManagementPsPowerTimeSlotValueListType, value, &power_time_slot_value_data_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_power_time_slot_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "schedule", SmartEnergyManagementPsPowerTimeSlotType, schedule, power_time_slot_schedule_data_cfg),
    EEBUS_DATA_SEQUENCE("valueList", SmartEnergyManagementPsPowerTimeSlotType, value_list,
        smart_energy_management_ps_power_time_slot_value_list_cfg),
    EEBUS_DATA_SEQUENCE("scheduleConstraints", SmartEnergyManagementPsPowerTimeSlotType, schedule_constraints,
        power_time_slot_schedule_constraints_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_time_slot_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(SmartEnergyManagementPsPowerTimeSlotType), smart_energy_management_ps_power_time_slot_cfg);

static const EebusDataCfg smart_energy_management_ps_power_sequence_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "description", SmartEnergyManagementPsPowerSequenceType, description, power_sequence_description_data_cfg),
    EEBUS_DATA_SEQUENCE("state", SmartEnergyManagementPsPowerSequenceType, state, power_sequence_state_data_cfg),
    EEBUS_DATA_SEQUENCE(
        "schedule", SmartEnergyManagementPsPowerSequenceType, schedule, power_sequence_schedule_data_cfg),
    EEBUS_DATA_SEQUENCE("scheduleConstraints", SmartEnergyManagementPsPowerSequenceType, schedule_constraints,
        power_sequence_schedule_constraints_data_cfg),
    EEBUS_DATA_SEQUENCE("schedulePreference", SmartEnergyManagementPsPowerSequenceType, schedule_preference,
        power_sequence_schedule_preference_data_cfg),
    EEBUS_DATA_SEQUENCE("operatingConstraintsInterrupt", SmartEnergyManagementPsPowerSequenceType,
        operating_constraints_interrupt, operating_constraints_interrupt_data_cfg),
    EEBUS_DATA_SEQUENCE("operatingConstraintsDuration", SmartEnergyManagementPsPowerSequenceType,
        operating_constraints_duration, operating_constraints_duration_data_cfg),
    EEBUS_DATA_SEQUENCE("operatingConstraintsResumeImplication", SmartEnergyManagementPsPowerSequenceType,
        operating_constraints_resume_implication, operating_constraints_resume_implication_data_cfg),
    EEBUS_DATA_LIST(
        "powerTimeSlot", SmartEnergyManagementPsPowerSequenceType, power_time_slot, &power_time_slot_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg power_sequence_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(SmartEnergyManagementPsPowerSequenceType), smart_energy_management_ps_power_sequence_cfg);

static const EebusDataCfg smart_energy_management_ps_alternatives_cfg[] = {
    EEBUS_DATA_SEQUENCE("relation", SmartEnergyManagementPsAlternativesType, relation,
        power_sequence_alternatives_relation_data_cfg),
    EEBUS_DATA_LIST(
        "powerSequence", SmartEnergyManagementPsAlternativesType, power_sequence, &power_sequence_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_power_time_slot_value_list_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("value", SmartEnergyManagementPsPowerTimeSlotValueListElementsType, value,
        power_time_slot_value_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_power_time_slot_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("schedule", SmartEnergyManagementPsPowerTimeSlotElementsType, schedule,
        power_time_slot_schedule_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("valueList", SmartEnergyManagementPsPowerTimeSlotElementsType, value_list,
        smart_energy_management_ps_power_time_slot_value_list_elements_cfg),
    EEBUS_DATA_SEQUENCE("scheduleConstraints", SmartEnergyManagementPsPowerTimeSlotElementsType, schedule_constraints,
        power_time_slot_schedule_constraints_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_power_sequence_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("description", SmartEnergyManagementPsPowerSequenceElementsType, description,
        power_sequence_description_data_elements_cfg),
    EEBUS_DATA_SEQUENCE(
        "state", SmartEnergyManagementPsPowerSequenceElementsType, state, power_sequence_state_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("schedule", SmartEnergyManagementPsPowerSequenceElementsType, schedule,
        power_sequence_schedule_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("scheduleConstraints", SmartEnergyManagementPsPowerSequenceElementsType, schedule_constraints,
        power_sequence_schedule_constraints_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("schedulePreference", SmartEnergyManagementPsPowerSequenceElementsType, schedule_preference,
        power_sequence_schedule_preference_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("operatingConstraintsInterrupt", SmartEnergyManagementPsPowerSequenceElementsType,
        operating_constraints_interrupt, operating_constraints_interrupt_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("operatingConstraintsDuration", SmartEnergyManagementPsPowerSequenceElementsType,
        operating_constraints_duration, operating_constraints_duration_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("operatingConstraintsResumeImplication", SmartEnergyManagementPsPowerSequenceElementsType,
        operating_constraints_resume_implication, operating_constraints_resume_implication_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("powerTimeSlot", SmartEnergyManagementPsPowerSequenceElementsType, power_time_slot,
        smart_energy_management_ps_power_time_slot_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_alternatives_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("relation", SmartEnergyManagementPsAlternativesElementsType, relation,
        power_sequence_alternatives_relation_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("powerSequence", SmartEnergyManagementPsAlternativesElementsType, power_sequence,
        smart_energy_management_ps_power_sequence_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg alternatives_element_data_cfg = EEBUS_DATA_LIST_ELEMENT(
    sequence, sizeof(SmartEnergyManagementPsAlternativesType), smart_energy_management_ps_alternatives_cfg);

static const EebusDataCfg smart_energy_management_ps_data_cfg[] = {
    EEBUS_DATA_SEQUENCE("nodeScheduleInformation", SmartEnergyManagementPsDataType, node_schedule_information,
        power_sequence_node_schedule_information_data_cfg),
    EEBUS_DATA_LIST("alternatives", SmartEnergyManagementPsDataType, alternatives, &alternatives_element_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("nodeScheduleInformation", SmartEnergyManagementPsDataElementsType, node_schedule_information,
        power_sequence_node_schedule_information_data_elements_cfg),
    EEBUS_DATA_SEQUENCE("alternatives", SmartEnergyManagementPsDataElementsType, alternatives,
        smart_energy_management_ps_alternatives_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE("alternativesRelation", SmartEnergyManagementPsDataSelectorsType, alternatives_relation,
        power_sequence_alternatives_relation_list_data_selectors_cfg),
    EEBUS_DATA_SEQUENCE("powerSequenceDescription", SmartEnergyManagementPsDataSelectorsType,
        power_sequence_description, power_sequence_description_list_data_selectors_cfg),
    EEBUS_DATA_SEQUENCE("powerTimeSlotSchedule", SmartEnergyManagementPsDataSelectorsType, power_time_slot_schedule,
        power_time_slot_schedule_list_data_selectors_cfg),
    EEBUS_DATA_SEQUENCE("powerTimeSlotValue", SmartEnergyManagementPsDataSelectorsType, power_time_slot_value,
        power_time_slot_value_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_price_data_cfg[] = {
    EEBUS_DATA_SEQUENCE("price", SmartEnergyManagementPsPriceDataType, price, power_sequence_price_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_price_data_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "price", SmartEnergyManagementPsPriceDataElementsType, price, power_sequence_price_data_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_price_data_selectors_cfg[] = {
    EEBUS_DATA_SEQUENCE(
        "price", SmartEnergyManagementPsPriceDataSelectorsType, price, power_sequence_price_list_data_selectors_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_configuration_request_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("scheduleConfigurationRequest", SmartEnergyManagementPsConfigurationRequestCallType,
        schedule_configuration_request, power_sequence_schedule_configuration_request_call_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_configuration_request_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("scheduleConfigurationRequest", SmartEnergyManagementPsConfigurationRequestCallElementsType,
        schedule_configuration_request, power_sequence_schedule_configuration_request_call_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_price_calculation_request_call_cfg[] = {
    EEBUS_DATA_SEQUENCE("priceCalculationRequest", SmartEnergyManagementPsPriceCalculationRequestCallType,
        price_calculation_request, power_sequence_price_calculation_request_call_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg smart_energy_management_ps_price_calculation_request_call_elements_cfg[] = {
    EEBUS_DATA_SEQUENCE("priceCalculationRequest", SmartEnergyManagementPsPriceCalculationRequestCallElementsType,
        price_calculation_request, power_sequence_price_calculation_request_call_elements_cfg),
    EEBUS_DATA_END,
};

#endif  // SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_TYPES_INC_
