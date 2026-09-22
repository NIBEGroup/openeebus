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
 * @brief Energy Guard Limitation of Power use case internal declarations
 */

#ifndef SRC_USE_CASE_ACTOR_EG_EG_LP_INTERNAL_H_
#define SRC_USE_CASE_ACTOR_EG_EG_LP_INTERNAL_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/model/device_configuration_types.h"
#include "src/spine/model/electrical_connection_types.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/api/eg_lp_listener_interface.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EgLpUseCase EgLpUseCase;
struct EgLpUseCase {
  /** Inherits the Entity */
  UseCase obj;

  EnergyDirectionType energy_direction;

  DeviceConfigurationKeyNameType failsafe_power_limit_key;

  ElectricalConnectionCharacteristicTypeType nominal_max_characteristic;

  ElectricalConnectionCharacteristicTypeType contractual_nominal_max_characteristic;

  EgLpListenerObject* eg_lp_listener;
};

#define EG_LP_USE_CASE(obj) ((EgLpUseCase*)(obj))

EebusError
EgLpReadLoadControlLimit(EgLpUseCase* self, EntityRemoteObject* remote_entity, ReplyMessageCallback cb, void* ctx);

EebusError EgLpWriteLoadControlLimit(
    EgLpUseCase* self,
    EntityRemoteObject* remote_entity,
    const LoadLimit* limit,
    ResultMessageCallback cb,
    void* ctx
);

EebusError
EgLpGetActivePowerLimitInternal(const EgLpUseCase* self, const EntityAddressType* remote_entity_addr, LoadLimit* limit);

EebusError EgLpReadActivePowerLimitInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

EebusError EgLpGetFailsafeActivePowerLimitInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
);

EebusError EgLpReadDeviceConfigurationWithKeyName(
    const EgLpUseCase* self,
    EntityRemoteObject* remote_entity,
    DeviceConfigurationKeyNameType key_name,
    ReplyMessageCallback cb,
    void* ctx
);

EebusError EgLpReadFailsafeActivePowerLimitInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

EebusError EgLpReadFailsafeDurationMinimumInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

EebusError EgLpGetFailsafeDurationMinimumInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    DurationType* duration
);

EebusError EgLpGetPowerNominalMaxInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
);

EebusError EgLpReadPowerNominalMaxInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

const ElectricalConnectionCharacteristicDataType* EgLpNominalMaxPrimaryFilter(const EgLpUseCase* self);

const ElectricalConnectionCharacteristicDataType* EgLpNominalMaxContractualFilter(const EgLpUseCase* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_EG_EG_LP_INTERNAL_H_
