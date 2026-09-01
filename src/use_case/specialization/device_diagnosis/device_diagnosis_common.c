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
 * @brief Device Diagnosis Common functionality implementation
 */

#include "src/use_case/specialization/device_diagnosis/device_diagnosis_common.h"
#include "src/spine/model/absolute_or_relative_time.h"
#include "src/use_case/specialization/helper.h"

void DeviceDiagnosisCommonConstruct(
    DeviceDiagnosisCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
) {
    self->feature_local  = feature_local;
    self->feature_remote = feature_remote;
}

const DeviceDiagnosisStateDataType* DeviceDiagnosisCommonGetState(const DeviceDiagnosisCommon* self) {
    return HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeDeviceDiagnosisStateData);
}

bool DeviceDiagnosisCommonIsHeartbeatWithinDuration(const DeviceDiagnosisCommon* self, const DurationType* duration) {
    if (duration == NULL) {
        return false;
    }

    const DeviceDiagnosisHeartbeatDataType* const data
        = HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeDeviceDiagnosisHeartbeatData);

    if ((data == NULL) || (data->timestamp == NULL)) {
        return false;
    }

    const EebusDateTime time_value = AbsoluteOrRelativeTimeGetTime(data->timestamp);
    if (!EebusDateTimeIsValid(&time_value)) {
        return false;
    }

    EebusDuration duration_tmp = *duration;
    EebusDurationInvertSign(&duration_tmp);

    EebusDateTime diff = EebusDateTimeNow();
    EebusDateTimeAddDuration(&diff, &duration_tmp);

    return EebusDateTimeCompare(&diff, &time_value) <= 0;
}
