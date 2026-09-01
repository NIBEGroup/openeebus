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

#ifndef SRC_USE_CASE_SPECIALIZATION_DEVICE_DIAGNOSIS_DEVICE_DIAGNOSIS_CLIENT_H_
#define SRC_USE_CASE_SPECIALIZATION_DEVICE_DIAGNOSIS_DEVICE_DIAGNOSIS_CLIENT_H_

#include "src/use_case/specialization/device_diagnosis/device_diagnosis_common.h"
#include "src/use_case/specialization/feature_info_client.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct DeviceDiagnosisClient DeviceDiagnosisClient;

struct DeviceDiagnosisClient {
    FeatureInfoClient feature_info_client;
    DeviceDiagnosisCommon device_diag_common;
};

/**
 * @brief Constructs a DeviceDiagnosisClient instance.
 *
 * This function initializes a DeviceDiagnosisClient instance by associating it
 * with a local entity and a remote entity. It sets up the necessary structures
 * for managing device diagnosis communication between the entities.
 *
 * @param self A pointer to the DeviceDiagnosisClient instance to be initialized.
 * @param local_entity A pointer to the local entity object.
 * @param remote_entity A pointer to the remote entity object.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the construction was successful.
 *         - Other error codes: If the construction failed.
 */
EebusError DeviceDiagnosisClientConstruct(
    DeviceDiagnosisClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
);

/**
 * @brief Creates and initializes a new DeviceDiagnosisClient instance.
 *
 * This function allocates memory for a new DeviceDiagnosisClient instance and
 * initializes it by associating it with a local entity and a remote entity. It sets
 * up the necessary structures for managing device diagnosis communication between
 * the entities.
 *
 * @param local_entity A pointer to the local entity object.
 * @param remote_entity A pointer to the remote entity object.
 * @return A pointer to the newly created DeviceDiagnosisClient instance, or NULL
 *         if the creation or initialization failed.
 */
DeviceDiagnosisClient* DeviceDiagnosisClientCreate(EntityLocalObject* local_entity, EntityRemoteObject* remote_entity);

/**
 * @brief Deletes a DeviceDiagnosisClient instance.
 *
 * This function frees the memory allocated for a DeviceDiagnosisClient instance.
 *
 * @param self A pointer to the DeviceDiagnosisClient instance to be deleted.
 *             If self is NULL, the function does nothing.
 */
static inline void DeviceDiagnosisClientDelete(DeviceDiagnosisClient* self) {
    if (self != NULL) {
        EEBUS_FREE(self);
    }
}

/**
 * @brief Requests the current state from the device diagnosis server.
 *
 * This function sends a request to the remote device diagnosis server to retrieve
 * the current state of the device. The response will contain the state information
 * if the request is successful.
 *
 * @param self A pointer to the DeviceDiagnosisClient instance making the request.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the request was successfully sent.
 *         - Other error codes: If the request failed.
 */
EebusError DeviceDiagnosisClientRequestState(DeviceDiagnosisClient* self);

/**
 * @brief Sends a heartbeat request to the device diagnosis server.
 *
 * This function sends a heartbeat request to the remote device diagnosis server
 * to verify the connection and ensure that the communication link is active.
 *
 * @param self A pointer to the DeviceDiagnosisClient instance making the request.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the heartbeat request initiation was successful.
 *         - Other error codes: If the request failed.
 */
EebusError DeviceDiagnosisClientRequestHeartbeat(DeviceDiagnosisClient* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_DEVICE_DIAGNOSIS_DEVICE_DIAGNOSIS_CLIENT_H_
