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
 * @brief OHPCF helper functions that can be used by both Compressor and CEM
 */

#ifndef SRC_USE_CASE_ACTOR_COMMON_OHPCF_H_
#define SRC_USE_CASE_ACTOR_COMMON_OHPCF_H_

#include "src/spine/model/power_sequences_types.h"
#include "src/spine/model/smart_energy_management_ps_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get OHPCF Announce Node Schedule data for initialization purposes
 * @return OHPCF Announce related node schedule data constant
 */
const PowerSequenceNodeScheduleInformationDataType* OhpcfGetNodeScheduleAnnounceData(void);

/**
 * @brief Get OHPCF Clear Process Node Schedule data for initialization purposes
 * @return OHPCF Clear Process related node schedule data constant
 */
const PowerSequenceNodeScheduleInformationDataType* OhpcfGetNodeScheduleClearProcessData(void);

/**
 * @brief Check if provided node schedule information matches OHPCF Announce criteria
 * @param node_schedule_info Pointer to node schedule information data to check
 * @return true if matches OHPCF Announce criteria, false otherwise
 */
bool OhpcfNodeScheduleAnnounceMatch(const PowerSequenceNodeScheduleInformationDataType* node_schedule_info);

/**
 * @brief Check if provided node schedule information matches OHPCF Clear Process criteria
 * @param node_schedule_info Pointer to node schedule information data to check
 * @return true if matches OHPCF Clear Process criteria, false otherwise
 */
bool OhpcfNodeScheduleClearProcessMatch(const PowerSequenceNodeScheduleInformationDataType* node_schedule_info);

/**
 * @brief Get the alternative instance from Smart Energy Management PS data (if available)
 * @param sem_ps_data Smart Energy Management PS data to get the alternative from
 * @return Unique alternative instance on success, NULL otherwise
 */
const SmartEnergyManagementPsAlternativesType* OhpcfSmartEnergyManagementPsGetAlternative(
    const SmartEnergyManagementPsDataType* sem_ps_data
);

/**
 * @brief Get the Alternative ID from Smart Energy Management PS data (if available)
 * @param sem_ps_data Smart Energy Management PS data to get the alternative ID from
 * @return Unique alternative ID instance on success, NULL otherwise
 */
const AlternativesIdType* OhpcfSmartEnergyManagementPsGetAlternativeId(
    const SmartEnergyManagementPsDataType* sem_ps_data
);

/**
 * @brief Get the Power Sequence instance from Smart Energy Management PS data (if available)
 * @param sem_ps_data Smart Energy Management PS data to get the power sequence from
 * @return Unique Power Sequence instance on success, NULL otherwise
 */
const SmartEnergyManagementPsPowerSequenceType* OhpcfSmartEnergyManagementPsGetPowerSequence(
    const SmartEnergyManagementPsDataType* sem_ps_data
);

/**
 * @brief Get the Power Sequence ID from Smart Energy Management PS data (if available)
 * @param sem_ps_data Smart Energy Management PS data to get the power sequence ID from
 * @return Unique Power Sequence ID instance on success, NULL otherwise
 */
const PowerSequenceIdType* OhpcfSmartEnergyManagementPsGetPowerSequenceId(
    const SmartEnergyManagementPsDataType* sem_ps_data
);

/**
 * @brief Get the Power Time Slot from Power Sequence (if available)
 * @param power_sequence Power Sequence to get the power time slot from
 * @return Unique Power Time Slot instance on success, NULL otherwise
 */
const SmartEnergyManagementPsPowerTimeSlotType* OhpcfSmartEnergyManagementPsPowerSequenceGetPowerTimeSlot(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
);

/**
 * @brief Get the Maximal Power from Power Sequence (if available)
 * @param power_sequence Power Sequence to get the Maximal Power from
 * @return Unique Maximal Power instance (Scaled Number type) on success, NULL otherwise
 */
const ScaledNumberType* OhpcfSmartEnergyManagementPsPowerSequenceGetPowerMax(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SRC_USE_CASE_ACTOR_COMMON_OHPCF_H_
