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
 * @brief Controllable System Limitation of Power implementation
 */

#include "src/use_case/actor/cs/cs_lp.h"

#include <stddef.h>

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/model/error_types.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/model.h"
#include "src/spine/model/result_types.h"
#include "src/spine/model/usecase_information_types.h"
#include "src/use_case/actor/cs/cs_lp_events.h"
#include "src/use_case/actor/cs/cs_lp_internal.h"
#include "src/use_case/actor/cs/cs_lp_write_approval_container.h"
#include "src/use_case/specialization/device_configuration/device_configuration_server.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_server.h"
#include "src/use_case/specialization/load_control/load_control_server.h"
#include "src/use_case/use_case.h"

static void CsLpUseCaseDestruct(UseCaseObject* self);

static const UseCaseInterface lp_use_case_methods = {
    .destruct                       = CsLpUseCaseDestruct,
    .is_entity_compatible           = UseCaseIsEntityCompatible,
    .is_use_case_compatible         = UseCaseIsUseCaseCompatible,
    .get_remote_entity_with_address = UseCaseGetRemoteEntityWithAddress,
};

static EebusError AddFeatures(CsLpUseCase* self, EntityLocalObject* entity);
static EebusError CsLpUseCaseConstruct(
    CsLpUseCase* self,
    EnergyDirectionType energy_direction,
    const UseCaseInfo* cs_lp_use_case_info,
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType electrical_connection_id,
    CsLpListenerObject* cs_lp_listener
);

static void CsLpLoadControlCallback(const Message* msg, void* ctx) {
  CsLpWriteApprovalCtx* const approval_ctx = (CsLpWriteApprovalCtx*)ctx;
  CsLpUseCase* const self                  = approval_ctx->use_case;
  FeatureLocalObject* const feature        = approval_ctx->feature;

  if (msg == NULL || msg->request_header == NULL || msg->request_header->msg_cnt == NULL || msg->cmd == NULL
      || msg->cmd->data_choice == NULL || msg->device_remote == NULL) {
    return;
  }

  const char* const ski        = DEVICE_REMOTE_GET_SKI(msg->device_remote);
  const MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

  if (self->cs_lpc_approver == NULL) {
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, ski, msg_cnt);
    return;
  }

  if (msg->cmd->data_choice_type_id != kFunctionTypeLoadControlLimitListData) {
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, ski, msg_cnt);
    return;
  }

  const LoadControlLimitListDataType* const limit_list = (const LoadControlLimitListDataType*)msg->cmd->data_choice;
  if (limit_list->load_control_limit_data_size == 0) {
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, ski, msg_cnt);
    return;
  }

  // This actor only ever exposes a single LoadControl limit (see AddLoadControlFeature). A write
  // applies its whole entry list regardless of what gets shown here, so a write carrying more
  // than one entry, or targeting a limit_id other than the one known one, is rejected outright:
  // any entry beyond the one the approver is shown would otherwise reach device state unreviewed.
  LoadControlServer lcs;
  LoadControlLimitIdType known_limit_id;
  const bool has_known_limit_id = (LoadControlServerConstruct(&lcs, USE_CASE(self)->local_entity) == kEebusErrorOk)
                                  && (CsLpGetLimitId(self, &lcs, &known_limit_id) == kEebusErrorOk);

  const LoadControlLimitDataType* const first_entry = limit_list->load_control_limit_data[0];

  if ((limit_list->load_control_limit_data_size != 1) || !has_known_limit_id || (first_entry->limit_id == NULL)
      || (*first_entry->limit_id != known_limit_id)) {
    const ErrorType err
        = {.error_number = kErrorNumberTypeCommandRejected,
           .description  = "Write must target exactly the single known load control limit"};
    FEATURE_LOCAL_DENY_WRITE(feature, ski, msg_cnt, &err);
    return;
  }

  LoadLimit parsed;
  if (LoadLimitInitWithLoadControlLimitData(&parsed, first_entry) != kEebusErrorOk) {
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, ski, msg_cnt);
    return;
  }

  CS_LP_WRITE_APPROVAL_CONTAINER_ADD(CS_LP_PENDING_APPROVAL_CONTAINER(self), ski, msg_cnt, feature);

  const DurationType* const duration = parsed.delete_duration ? NULL : &parsed.duration;
  CS_LPC_APPROVER_ON_POWER_LIMIT_APPROVAL_REQUESTED(
      self->cs_lpc_approver,
      ski,
      msg_cnt,
      &parsed.value,
      duration,
      parsed.is_active
  );
}

void CsLpWriteExpiryCallback(const char* ski, MsgCounterType msg_cnt, void* ctx) {
  CsLpWriteApprovalCtx* approval_ctx = (CsLpWriteApprovalCtx*)ctx;

  CsLpUseCase* self = approval_ctx->use_case;
  if (CS_LP_WRITE_APPROVAL_CONTAINER_FIND(CS_LP_PENDING_APPROVAL_CONTAINER(self), ski, msg_cnt) == NULL) {
    return;
  }

  if (self->cs_lpc_approver != NULL) {
    CS_LPC_APPROVER_ON_APPROVAL_REQUEST_EXPIRED(self->cs_lpc_approver, ski, msg_cnt);
  }

  CS_LP_WRITE_APPROVAL_CONTAINER_REMOVE(CS_LP_PENDING_APPROVAL_CONTAINER(self), ski, msg_cnt);
}

EebusError AddLoadControlFeature(CsLpUseCase* self, EntityLocalObject* entity) {
  FeatureLocalObject* const fl
      = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeLoadControl, kRoleTypeServer);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeLoadControlLimitDescriptionListData, true, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeLoadControlLimitListData, true, true);
  self->lc_approval_ctx.use_case = self;
  self->lc_approval_ctx.feature  = fl;
  FEATURE_LOCAL_ADD_WRITE_APPROVAL_CALLBACK(fl, CsLpLoadControlCallback, &self->lc_approval_ctx);
  FEATURE_LOCAL_SET_WRITE_EXPIRY_CALLBACK(fl, CsLpWriteExpiryCallback, &self->lc_approval_ctx);

  LoadControlServer lc;
  EebusError err = LoadControlServerConstruct(&lc, entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  // measurement_id = 0 is a fake Measurement ID, as there is no Electrical Connection server defined,
  // it can't provide any meaningful. But KEO requires this to be set
  LoadControlLimitDescriptionDataType new_limit_desc = {
      .limit_type      = &(LoadControlLimitTypeType){kLoadControlLimitTypeTypeSignDependentAbsValueLimit},
      .limit_category  = &(LoadControlCategoryType){kLoadControlCategoryTypeObligation},
      .limit_direction = &self->energy_direction,
      .measurement_id  = &(MeasurementIdType){(MeasurementIdType)0},
      .unit            = &(UnitOfMeasurementType){kUnitOfMeasurementTypeW},
      .scope_type      = &(ScopeTypeType){kScopeTypeTypeActivePowerLimit},
  };

  LoadControlLimitIdType limit_id = 0;

  err = LoadControlServerAddLimitDescription(&lc, &new_limit_desc, &limit_id);
  if (err != kEebusErrorOk) {
    return err;
  }

  LoadControlLimitDataType limit_data = {
      .value               = &(ScaledNumberType){0},
      .is_limit_changeable = &(bool){true},
      .is_limit_active     = &(bool){false},
  };

  LoadControlServerUpdateLimitWithId(&lc, &limit_data, limit_id);
  return kEebusErrorOk;
}

static const DeviceConfigurationKeyValueDataType* CsLpFindKeyValue(
    const DeviceConfigurationKeyValueListDataType* data,
    const DeviceConfigurationCommon* dc_common,
    DeviceConfigurationKeyNameType key_name
) {
  for (size_t i = 0; i < data->device_configuration_key_value_data_size; ++i) {
    const DeviceConfigurationKeyValueDataType* const kv = data->device_configuration_key_value_data[i];
    if ((kv == NULL) || (kv->key_id == NULL) || (kv->value == NULL)) {
      continue;
    }

    const DeviceConfigurationKeyValueDescriptionDataType* const desc
        = DeviceConfigurationCommonGetKeyValueDescriptionWithKeyId(dc_common, *kv->key_id);
    if ((desc == NULL) || (desc->key_name == NULL) || (*desc->key_name != key_name)) {
      continue;
    }

    return kv;
  }

  return NULL;
}

static bool CsLpDeviceConfigParseMessage(
    CsLpUseCase* self,
    const Message* msg,
    FeatureLocalObject* feature,
    const char** ski_out,
    MsgCounterType* msg_cnt_out,
    const DeviceConfigurationKeyValueListDataType** data_out,
    DeviceConfigurationServer* dc_out
) {
  if (msg == NULL || msg->request_header == NULL || msg->request_header->msg_cnt == NULL || msg->cmd == NULL
      || msg->cmd->data_choice == NULL || msg->device_remote == NULL) {
    return false;
  }

  *ski_out     = DEVICE_REMOTE_GET_SKI(msg->device_remote);
  *msg_cnt_out = *msg->request_header->msg_cnt;

  if (self->cs_lpc_approver == NULL) {
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, *ski_out, *msg_cnt_out);
    return false;
  }

  if (msg->cmd->data_choice_type_id != kFunctionTypeDeviceConfigurationKeyValueListData) {
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, *ski_out, *msg_cnt_out);
    return false;
  }

  *data_out = (const DeviceConfigurationKeyValueListDataType*)msg->cmd->data_choice;

  if (DeviceConfigurationServerConstruct(dc_out, USE_CASE(self)->local_entity) != kEebusErrorOk) {
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, *ski_out, *msg_cnt_out);
    return false;
  }

  return true;
}

static void CsLpcFailsafeValueCallback(const Message* msg, void* ctx) {
  CsLpWriteApprovalCtx* const approval_ctx = (CsLpWriteApprovalCtx*)ctx;
  CsLpUseCase* const self                  = approval_ctx->use_case;
  FeatureLocalObject* const feature        = approval_ctx->feature;

  const char* ski;
  MsgCounterType msg_cnt;
  const DeviceConfigurationKeyValueListDataType* data;
  DeviceConfigurationServer dc = {0};
  if (!CsLpDeviceConfigParseMessage(self, msg, feature, &ski, &msg_cnt, &data, &dc)) {
    return;
  }

  const DeviceConfigurationKeyValueDataType* const kv
      = CsLpFindKeyValue(data, &dc.device_cfg_common, self->failsafe_power_limit_key);
  if ((kv == NULL) || (kv->value == NULL) || (kv->value->scaled_number == NULL)) {
    // This write doesn't touch our key: cast our approval vote so the sibling
    // failsafe duration callback's vote isn't stuck waiting on us forever.
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, ski, msg_cnt);
    return;
  }

  ScaledValue failsafe_sv;
  if (ScaledValueInitWithScaledNumber(&failsafe_sv, kv->value->scaled_number) != kEebusErrorOk) {
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, ski, msg_cnt);
    return;
  }

  if (CS_LP_WRITE_APPROVAL_CONTAINER_FIND(CS_LP_PENDING_APPROVAL_CONTAINER(self), ski, msg_cnt) == NULL) {
    CS_LP_WRITE_APPROVAL_CONTAINER_ADD(CS_LP_PENDING_APPROVAL_CONTAINER(self), ski, msg_cnt, feature);
  }

  CS_LPC_APPROVER_ON_FAILSAFE_VALUE_APPROVAL_REQUESTED(self->cs_lpc_approver, ski, msg_cnt, &failsafe_sv);
}

static void CsLpcFailsafeDurationCallback(const Message* msg, void* ctx) {
  CsLpWriteApprovalCtx* const approval_ctx = (CsLpWriteApprovalCtx*)ctx;
  CsLpUseCase* const self                  = approval_ctx->use_case;
  FeatureLocalObject* const feature        = approval_ctx->feature;

  const char* ski;
  MsgCounterType msg_cnt;
  const DeviceConfigurationKeyValueListDataType* data;
  DeviceConfigurationServer dc = {0};
  if (!CsLpDeviceConfigParseMessage(self, msg, feature, &ski, &msg_cnt, &data, &dc)) {
    return;
  }

  const DeviceConfigurationKeyValueDataType* const kv
      = CsLpFindKeyValue(data, &dc.device_cfg_common, kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum);
  if ((kv == NULL) || (kv->value == NULL) || (kv->value->duration == NULL)) {
    // This write doesn't touch our key: cast our approval vote so the sibling
    // failsafe value callback's vote isn't stuck waiting on us forever.
    FEATURE_LOCAL_TRY_APPROVE_WRITE(feature, ski, msg_cnt);
    return;
  }

  if (CS_LP_WRITE_APPROVAL_CONTAINER_FIND(CS_LP_PENDING_APPROVAL_CONTAINER(self), ski, msg_cnt) == NULL) {
    CS_LP_WRITE_APPROVAL_CONTAINER_ADD(CS_LP_PENDING_APPROVAL_CONTAINER(self), ski, msg_cnt, feature);
  }

  CS_LPC_APPROVER_ON_FAILSAFE_DURATION_APPROVAL_REQUESTED(self->cs_lpc_approver, ski, msg_cnt, kv->value->duration);
}

EebusError AddDeviceConfigurationFeature(CsLpUseCase* self, EntityLocalObject* entity) {
  FeatureLocalObject* const fl
      = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeDeviceConfiguration, kRoleTypeServer);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceConfigurationKeyValueDescriptionListData, true, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceConfigurationKeyValueListData, true, true);
  self->failsafe_value_approval_ctx.use_case = self;
  self->failsafe_value_approval_ctx.feature  = fl;
  FEATURE_LOCAL_ADD_WRITE_APPROVAL_CALLBACK(fl, CsLpcFailsafeValueCallback, &self->failsafe_value_approval_ctx);
  FEATURE_LOCAL_ADD_WRITE_APPROVAL_CALLBACK(fl, CsLpcFailsafeDurationCallback, &self->failsafe_value_approval_ctx);
  FEATURE_LOCAL_SET_WRITE_EXPIRY_CALLBACK(fl, CsLpWriteExpiryCallback, &self->failsafe_value_approval_ctx);

  DeviceConfigurationServer dcs;
  EebusError err = DeviceConfigurationServerConstruct(&dcs, entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  const DeviceConfigurationKeyValueDescriptionDataType failsafe_consumption_description = {
      .key_name   = &self->failsafe_power_limit_key,
      .value_type = &(DeviceConfigurationKeyValueTypeType){kDeviceConfigurationKeyValueTypeTypeScaledNumber},
      .unit       = &(UnitOfMeasurementType){kUnitOfMeasurementTypeW},
  };

  DeviceConfigurationServerAddKeyValueDescription(&dcs, &failsafe_consumption_description);

  // Only add if it doesn't exist yet
  const DeviceConfigurationKeyValueDescriptionDataType filter = {
      .key_name = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum},
  };

  EebusDataListMatchIterator it = {0};
  DeviceConfigurationCommonKeyValueDescriptionMatchFirst(&dcs.device_cfg_common, &filter, &it);
  if (EebusDataListMatchIteratorIsDone(&it)) {
    DeviceConfigurationKeyValueDescriptionDataType failsafe_duration_min_description = {
        .key_name   = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum},
        .value_type = &(DeviceConfigurationKeyValueTypeType){kDeviceConfigurationKeyValueTypeTypeDuration},
    };

    DeviceConfigurationServerAddKeyValueDescription(&dcs, &failsafe_duration_min_description);
  }

  DeviceConfigurationKeyValueDataType failsafe_power_limit = {
     .value = &(DeviceConfigurationKeyValueValueType) {
       .scaled_number = &(ScaledNumberType){.number = &(int64_t){0}, .scale = NULL},
     },
 
     .is_value_changeable = &(bool){true},
   };

  DeviceConfigurationKeyValueDescriptionDataType failsafe_power_descripton = {
      .key_name = &self->failsafe_power_limit_key,
  };

  DeviceConfigurationServerUpdateKeyValueWithFilter(&dcs, &failsafe_power_limit, NULL, &failsafe_power_descripton);

  const DeviceConfigurationKeyValueDataType failsafe_duration_minimum = {
     .value = &(DeviceConfigurationKeyValueValueType) {
       .duration = &(DurationType){0},
     },
 
     .is_value_changeable = &(bool){true},
   };

  DeviceConfigurationKeyValueDescriptionDataType failsafe_duration_description = {
      .key_name = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum},
  };

  return DeviceConfigurationServerUpdateKeyValueWithFilter(
      &dcs,
      &failsafe_duration_minimum,
      NULL,
      &failsafe_duration_description
  );
}

EebusError AddDeviceDiagnosisFeature(CsLpUseCase* self, EntityLocalObject* entity) {
  UNUSED(self);
  FeatureLocalObject* const fl
      = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeDeviceDiagnosis, kRoleTypeServer);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceDiagnosisHeartbeatData, true, false);
  if (fl == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EebusError AddElectricalConnection(CsLpUseCase* self, EntityLocalObject* entity) {
  CsLpUseCase* const cs_lp = CS_LP_USE_CASE(self);

  FeatureLocalObject* const fl
      = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeElectricalConnection, kRoleTypeServer);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeElectricalConnectionCharacteristicListData, true, false);

  ElectricalConnectionServer ecs;
  EebusError err = ElectricalConnectionServerConstruct(&ecs, entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  const ElectricalConnectionCharacteristicDataType new_characteristic = {
      .electrical_connection_id = &(ElectricalConnectionIdType){cs_lp->electrical_connection_id},
      .parameter_id             = &(ElectricalConnectionParameterIdType){0},
      .characteristic_context   = &kEccContextEntity,
      .characteristic_type      = &self->nominal_max_characteristic,
      .unit                     = &(UnitOfMeasurementType){kUnitOfMeasurementTypeW},
  };

  return ElectricalConnectionServerAddCharacteristic(&ecs, &new_characteristic);
}

EebusError AddFeatures(CsLpUseCase* self, EntityLocalObject* entity) {
  // Client features
  ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeDeviceDiagnosis, kRoleTypeClient);

  // Server features
  EebusError err = AddLoadControlFeature(self, entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  err = AddDeviceConfigurationFeature(self, entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  err = AddDeviceDiagnosisFeature(self, entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  return AddElectricalConnection(self, entity);
}

EebusError CsLpUseCaseConstruct(
    CsLpUseCase* self,
    EnergyDirectionType energy_direction,
    const UseCaseInfo* cs_lp_use_case_info,
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType electrical_connection_id,
    CsLpListenerObject* cs_lp_listener
) {
  UseCaseConstruct(USE_CASE(self), cs_lp_use_case_info, local_entity, CsLpHandleEvent);
  // Override "virtual functions table"
  USE_CASE_INTERFACE(self) = &lp_use_case_methods;

  self->energy_direction           = energy_direction;
  self->failsafe_power_limit_key   = (DeviceConfigurationKeyNameType)0;
  self->electrical_connection_id   = electrical_connection_id;
  self->nominal_max_characteristic = (ElectricalConnectionCharacteristicTypeType)0;
  self->cs_lp_listener             = cs_lp_listener;
  self->remote_eg_entity_addr      = NULL;
  self->heartbeat_diag_client      = NULL;
  self->heartbeat_keo_workaround   = false;
  self->cs_lpc_approver            = NULL;
  self->pend                       = CsLpWriteApprovalContainerCreate();
  if (self->pend == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  if (energy_direction == kEnergyDirectionTypeConsume) {
    self->failsafe_power_limit_key = kDeviceConfigurationKeyNameTypeFailsafeConsumptionActivePowerLimit;
  } else {
    self->failsafe_power_limit_key = kDeviceConfigurationKeyNameTypeFailsafeProductionActivePowerLimit;
  }

  const DeviceTypeType* const device_type = DEVICE_GET_DEVICE_TYPE(DEVICE_OBJECT(USE_CASE(self)->local_device));

  if (self->energy_direction == kEnergyDirectionTypeConsume) {
    // According to LPC V1.0 2.2, lines 400ff:
    // - a HEMS provides contractual consumption nominal max
    // - any other devices provides power consumption nominal max
    if ((device_type == NULL) || (*device_type == kDeviceTypeTypeEnergyManagementSystem)) {
      self->nominal_max_characteristic = kElectricalConnectionCharacteristicTypeTypeContractualConsumptionNominalMax;
    } else {
      self->nominal_max_characteristic = kElectricalConnectionCharacteristicTypeTypePowerConsumptionNominalMax;
    }
  } else {
    // According to LPP V1.0 2.2, lines 420ff:
    // - a HEMS provides contractual production nominal max
    // - any other devices provides power production nominal max
    if ((device_type == NULL) || (*device_type == kDeviceTypeTypeEnergyManagementSystem)) {
      self->nominal_max_characteristic = kElectricalConnectionCharacteristicTypeTypeContractualProductionNominalMax;
    } else {
      self->nominal_max_characteristic = kElectricalConnectionCharacteristicTypeTypePowerProductionNominalMax;
    }
  }

  return AddFeatures(self, local_entity);
}

CsLpUseCaseObject* CsLpUseCaseCreate(
    EnergyDirectionType energy_direction,
    const UseCaseInfo* use_case_info,
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType electrical_connection_id,
    CsLpListenerObject* cs_lp_listener
) {
  CsLpUseCase* cs_lp_use_case = EEBUS_MALLOC(sizeof(*cs_lp_use_case));
  if (cs_lp_use_case == NULL) {
    return NULL;
  }

  const EebusError err = CsLpUseCaseConstruct(
      cs_lp_use_case,
      energy_direction,
      use_case_info,
      local_entity,
      electrical_connection_id,
      cs_lp_listener
  );

  if (err != kEebusErrorOk) {
    CsLpUseCaseDelete(CS_LP_USE_CASE_OBJECT(cs_lp_use_case));
    return NULL;
  }

  return CS_LP_USE_CASE_OBJECT(cs_lp_use_case);
}

bool CsLpIsLimitValid(double limit, int32_t duration) {
  return (limit >= 0.0) && (duration >= 0);
}

bool CsLpIsFailsafeValueValid(double value) {
  return value >= 0.0;
}

bool CsLpIsFailsafeDurationValid(int32_t duration) {
  static const int32_t kFailsafeDurationMinSeconds = 2 * 60 * 60;
  static const int32_t kFailsafeDurationMaxSeconds = 24 * 60 * 60;

  return (duration >= kFailsafeDurationMinSeconds) && (duration <= kFailsafeDurationMaxSeconds);
}

void CsLpUseCaseDestruct(UseCaseObject* self) {
  CsLpUseCase* cs_lp = CS_LP_USE_CASE(self);

  EntityAddressDelete(cs_lp->remote_eg_entity_addr);
  cs_lp->remote_eg_entity_addr = NULL;

  RemoveDeviceDiagnosisClient(cs_lp);

  CsLpWriteApprovalContainerDelete(CS_LP_PENDING_APPROVAL_CONTAINER(cs_lp));

  UseCaseDestruct(self);
}
