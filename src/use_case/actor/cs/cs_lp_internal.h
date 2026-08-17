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
 * @brief Controllable System LP use case internal declarations
 */

#ifndef SRC_USE_CASE_ACTOR_CS_CS_LP_INTERNAL_H_
#define SRC_USE_CASE_ACTOR_CS_CS_LP_INTERNAL_H_

#include <stdbool.h>

#include "src/spine/model/device_configuration_types.h"
#include "src/spine/model/electrical_connection_types.h"
#include "src/use_case/api/cs_lp_listener_interface.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/specialization/device_diagnosis/device_diagnosis_client.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct CsLpUseCase CsLpUseCase;
struct CsLpUseCase {
  /** Inherits the Use Case */
  UseCase obj;

  EnergyDirectionType energy_direction;

  DeviceConfigurationKeyNameType failsafe_power_limit_key;

  ElectricalConnectionIdType electrical_connection_id;
  ElectricalConnectionCharacteristicTypeType nominal_max_characteristic;

  CsLpListenerObject* cs_lp_listener;

  EntityAddressType* remote_eg_entity_addr;

  DeviceDiagnosisClient* heartbeat_diag_client;

  // KEO Stack uses multiple identical entities for the same functionality,
  // and it is not clear which to use
  bool heartbeat_keo_workaround;
};

#define CS_LP_USE_CASE(obj) ((CsLpUseCase*)(obj))

void RemoveDeviceDiagnosisClient(CsLpUseCase* self);

EebusError CsLpGetActivePowerLimitInternal(const CsLpUseCase* self, LoadLimit* limit);
EebusError
CsLpSetActivePowerLimitInternal(CsLpUseCase* self, const ScaledValue* limit, bool is_active, bool is_changeable);

EebusError
CsLpGetFailsafeActivePowerLimitInternal(const CsLpUseCase* self, ScaledValue* power_limit, bool* is_changeable);
EebusError
CsLpSetFailsafeActivePowerLimitInternal(CsLpUseCase* self, const ScaledValue* power_limit, bool is_changeable);

EebusError
CsLpGetFailsafeDurationMinimumInternal(const CsLpUseCase* self, EebusDuration* duration, bool* is_changeable);
EebusError CsLpSetFailsafeDurationMinimumInternal(CsLpUseCase* self, const EebusDuration* duration, bool is_changeable);

EebusError CsLpGetNominalMaxInternal(const CsLpUseCase* self, ScaledValue* nominal_max);
EebusError CsLpSetNominalMaxInternal(CsLpUseCase* self, const ScaledValue* nominal_max);

ElectricalConnectionCharacteristicTypeType CsLpGetElectricalConnectionCharacteristicTypeInternal(const CsLpUseCase* self
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_CS_CS_LP_INTERNAL_H_
