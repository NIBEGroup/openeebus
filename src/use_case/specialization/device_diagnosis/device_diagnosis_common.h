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
 * @brief Device Diagnosis Client functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_DEVICE_DIAGNOSIS_DEVICE_DIAGNOSIS_COMMON_H_
#define SRC_USE_CASE_SPECIALIZATION_DEVICE_DIAGNOSIS_DEVICE_DIAGNOSIS_COMMON_H_

#include <stdbool.h>

#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_errors.h"
#include "src/spine/model/device_diagnosis_types.h"
#include "src/use_case/specialization/feature_info_client.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct DeviceDiagnosisCommon DeviceDiagnosisCommon;

struct DeviceDiagnosisCommon {
    FeatureLocalObject* feature_local;
    FeatureRemoteObject* feature_remote;
};

/**
 * @brief Constructs a DeviceDiagnosisCommon instance.
 *
 * This function initializes a DeviceDiagnosisCommon instance by associating it
 * with a local feature object and a remote feature object. It sets up the necessary
 * structures for managing device diagnosis communication between the local and
 * remote features.
 *
 * @param self A pointer to the DeviceDiagnosisCommon instance to be initialized.
 * @param feature_local A pointer to the local feature object.
 * @param feature_remote A pointer to the remote feature object.
 */
void DeviceDiagnosisCommonConstruct(
    DeviceDiagnosisCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
);

/**
 * @brief Retrieves the current state data from the device diagnosis common instance.
 *
 * This function returns a pointer to the DeviceDiagnosisStateDataType structure
 * that contains the current state data of the device diagnosis. The state data
 * provides information about the current status of the device.
 *
 * @param self A pointer to the DeviceDiagnosisCommon instance.
 * @return A pointer to the DeviceDiagnosisStateDataType structure containing
 *         the current state data, or NULL if no state data is available.
 */
const DeviceDiagnosisStateDataType* DeviceDiagnosisCommonGetState(const DeviceDiagnosisCommon* self);

/**
 * @brief Checks if the currently available heartbeat data is within a specified time duration.
 *
 * This function evaluates whether the heartbeat data associated with the given
 * DeviceDiagnosisCommon instance falls within the specified time duration.
 * It is used to verify if the heartbeat signal is recent and within the expected
 * time frame.
 *
 * @param self A pointer to the DeviceDiagnosisCommon instance to check the heartbeat data with.
 * @param duration A pointer to the DurationType structure specifying the time duration to check against.
 * @return true if the heartbeat data is within the specified duration, false otherwise.
 */
bool DeviceDiagnosisCommonIsHeartbeatWithinDuration(const DeviceDiagnosisCommon* self, const DurationType* duration);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_DEVICE_DIAGNOSIS_DEVICE_DIAGNOSIS_COMMON_H_
