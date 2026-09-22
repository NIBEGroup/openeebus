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
 * @brief MA MPC public API implementation
 */

#include "src/common/eebus_errors.h"

#include "src/use_case/actor/ma/mpc/ma_mpc.h"
#include "src/use_case/actor/ma/mpc/ma_mpc_internal.h"
#include "src/use_case/actor/ma/mpc/ma_mpc_measurement.h"
#include "src/use_case/specialization/measurement/measurement_client.h"
#include "src/use_case/use_case.h"

static EebusError MaMpcReadMeasurementsDataInternal(
    const MaMpcUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
  const UseCase* const use_case = USE_CASE(self);

  EntityRemoteObject* const remote_entity
      = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

  if (remote_entity == NULL) {
    return kEebusErrorNoChange;
  }

  MeasurementClient mcl = {0};

  const EebusError err = MeasurementClientConstruct(&mcl, use_case->local_entity, remote_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  return MeasurementClientRequestData(&mcl, NULL, NULL, cb, ctx);
}

EebusError MaMpcReadMeasurementsData(
    const MaMpcUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
  const UseCase* const use_case = USE_CASE(self);

  if (remote_entity_addr == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);
  err = MaMpcReadMeasurementsDataInternal(MA_MPC_USE_CASE(self), remote_entity_addr, cb, ctx);
  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}

EebusError MaMpcGetMeasurementData(
    const MaMpcUseCaseObject* self,
    MuMpcMeasurementNameId measurement_name_id,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* measurement_value
) {
  const UseCase* const use_case = USE_CASE(self);

  if (measurement_value == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  EntityRemoteObject* const remote_entity
      = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(use_case), remote_entity_addr);

  const MaMeasurementObject* const measurement = MaMpcMeasurementGetInstanceWithNameId(measurement_name_id);

  if (measurement != NULL) {
    err = MA_MEASUREMENT_GET_DATA(measurement, use_case->local_entity, remote_entity, measurement_value);
  } else {
    err = kEebusErrorNotSupported;
  }

  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}
