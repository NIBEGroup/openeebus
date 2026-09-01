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
 * @brief Device Diagnosis Client functionality implementation
 */

#include "src/use_case/specialization/device_diagnosis/device_diagnosis_client.h"

#include "src/common/eebus_malloc.h"
#include "src/spine/model/device_diagnosis_types.h"

EebusError DeviceDiagnosisClientConstruct(
    DeviceDiagnosisClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
    const EebusError err = FeatureInfoClientConstruct(
        &self->feature_info_client,
        kFeatureTypeTypeDeviceDiagnosis,
        local_entity,
        remote_entity
    );

    if (err != kEebusErrorOk) {
        return err;
    }

    DeviceDiagnosisCommonConstruct(&self->device_diag_common, NULL, self->feature_info_client.remote_feature);
    return kEebusErrorOk;
}

DeviceDiagnosisClient* DeviceDiagnosisClientCreate(EntityLocalObject* local_entity, EntityRemoteObject* remote_entity) {
    DeviceDiagnosisClient* const ddc = EEBUS_MALLOC(sizeof(*ddc));
    if (ddc == NULL) {
        return NULL;
    }

    if (DeviceDiagnosisClientConstruct(ddc, local_entity, remote_entity) != kEebusErrorOk) {
        EEBUS_FREE(ddc);
        return NULL;
    }

    return ddc;
}

EebusError DeviceDiagnosisClientRequestState(DeviceDiagnosisClient* self) {
    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        kFunctionTypeDeviceDiagnosisStateData,
        NULL,
        NULL,
        NULL,
        NULL
    );
}

EebusError DeviceDiagnosisClientRequestHeartbeat(DeviceDiagnosisClient* self) {
    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        kFunctionTypeDeviceDiagnosisHeartbeatData,
        NULL,
        NULL,
        NULL,
        NULL
    );
}
