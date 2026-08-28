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
#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/model.h"
#include "src/spine/model/result_types.h"
#include "src/spine/model/usecase_information_types.h"
#include "src/use_case/actor/cs/cs_lp_events.h"
#include "src/use_case/actor/cs/cs_lp_internal.h"
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

static void CsLpLoadControlNegativeLimitWriteCallback(const Message* msg, void* ctx) {
    CsLpUseCase* const self = (CsLpUseCase*)ctx;

    if (msg == NULL || msg->request_header == NULL || msg->request_header->msg_cnt == NULL || msg->cmd == NULL
        || msg->cmd->data_choice == NULL || msg->device_remote == NULL) {
        return;
    }

    FeatureLocalObject* const fl = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(
        USE_CASE(self)->local_entity,
        kFeatureTypeTypeLoadControl,
        kRoleTypeServer
    );

    if (fl == NULL) {
        return;
    }

    const char* const ski        = DEVICE_REMOTE_GET_SKI(msg->device_remote);
    const MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

    // Ignore if it's not a Load Control Limit List Data write
    if (msg->cmd->data_choice_type_id != kFunctionTypeLoadControlLimitListData) {
        FEATURE_LOCAL_TRY_APPROVE_WRITE(fl, ski, msg_cnt);
        return;
    }

    const LoadControlLimitListDataType* const limit_list = (const LoadControlLimitListDataType*)msg->cmd->data_choice;

    for (size_t i = 0; i < limit_list->load_control_limit_data_size; ++i) {
        const LoadControlLimitDataType* const limit = limit_list->load_control_limit_data[i];
        const ScaledNumberType* const value         = LoadControlLimitGetValue(limit);

        if ((value == NULL) || (value->number == NULL)) {
            continue;
        }

        if (*value->number < 0) {
            const ErrorType err = {
                .error_number = kErrorNumberTypeCommandRejected,
                .description  = "Negative limit values are not allowed",
            };
            FEATURE_LOCAL_DENY_WRITE(fl, ski, msg_cnt, &err);
            return;
        }
    }

    FEATURE_LOCAL_TRY_APPROVE_WRITE(fl, ski, msg_cnt);
}

EebusError AddLoadControlFeature(CsLpUseCase* self, EntityLocalObject* entity) {
    FeatureLocalObject* const fl
        = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeLoadControl, kRoleTypeServer);
    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeLoadControlLimitDescriptionListData, true, false);
    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeLoadControlLimitListData, true, true);
    FEATURE_LOCAL_ADD_WRITE_APPROVAL_CALLBACK(fl, CsLpLoadControlNegativeLimitWriteCallback, self);

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

static void CsLpFailsafeActivePowerLimitWriteCallback(const Message* msg, void* ctx) {
    CsLpUseCase* const self = (CsLpUseCase*)ctx;

    if (msg == NULL || msg->request_header == NULL || msg->request_header->msg_cnt == NULL || msg->cmd == NULL
        || msg->cmd->data_choice == NULL || msg->device_remote == NULL) {
        return;
    }

    FeatureLocalObject* const fl = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(
        USE_CASE(self)->local_entity,
        kFeatureTypeTypeDeviceConfiguration,
        kRoleTypeServer
    );

    if (fl == NULL) {
        return;
    }

    const char* const ski        = DEVICE_REMOTE_GET_SKI(msg->device_remote);
    const MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

    if (msg->cmd->data_choice_type_id != kFunctionTypeDeviceConfigurationKeyValueListData) {
        FEATURE_LOCAL_TRY_APPROVE_WRITE(fl, ski, msg_cnt);
        return;
    }

    const DeviceConfigurationKeyValueListDataType* const data
        = (const DeviceConfigurationKeyValueListDataType*)msg->cmd->data_choice;

    DeviceConfigurationServer dc = {0};
    if (DeviceConfigurationServerConstruct(&dc, USE_CASE(self)->local_entity) != kEebusErrorOk) {
        const ErrorType err = {
            .error_number = kErrorNumberTypeCommandRejected,
            .description  = "Internal error: command rejected",
        };
        FEATURE_LOCAL_DENY_WRITE(fl, ski, msg_cnt, &err);
        return;
    }

    const DeviceConfigurationKeyValueDataType* const kv
        = CsLpFindKeyValue(data, &dc.device_cfg_common, self->failsafe_power_limit_key);

    if ((kv != NULL) && (kv->value->scaled_number != NULL) && (kv->value->scaled_number->number != NULL)
        && (*kv->value->scaled_number->number < 0)) {
        const ErrorType err = {
            .error_number = kErrorNumberTypeCommandRejected,
            .description  = "Negative failsafe power limit values are not allowed",
        };
        FEATURE_LOCAL_DENY_WRITE(fl, ski, msg_cnt, &err);
        return;
    }

    FEATURE_LOCAL_TRY_APPROVE_WRITE(fl, ski, msg_cnt);
}

static void CsLpFailsafeDurationMinimumWriteCallback(const Message* msg, void* ctx) {
    CsLpUseCase* const self = (CsLpUseCase*)ctx;

    if (msg == NULL || msg->request_header == NULL || msg->request_header->msg_cnt == NULL || msg->cmd == NULL
        || msg->cmd->data_choice == NULL || msg->device_remote == NULL) {
        return;
    }

    FeatureLocalObject* const fl = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(
        USE_CASE(self)->local_entity,
        kFeatureTypeTypeDeviceConfiguration,
        kRoleTypeServer
    );

    if (fl == NULL) {
        return;
    }

    const char* const ski        = DEVICE_REMOTE_GET_SKI(msg->device_remote);
    const MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

    // Ignore if it is not a Device Configuration Key Value List Data write
    if (msg->cmd->data_choice_type_id != kFunctionTypeDeviceConfigurationKeyValueListData) {
        FEATURE_LOCAL_TRY_APPROVE_WRITE(fl, ski, msg_cnt);
        return;
    }

    DeviceConfigurationServer dc = {0};
    if (DeviceConfigurationServerConstruct(&dc, USE_CASE(self)->local_entity) != kEebusErrorOk) {
        const ErrorType err = {
            .error_number = kErrorNumberTypeCommandRejected,
            .description  = "Internal error: command rejected",
        };
        FEATURE_LOCAL_DENY_WRITE(fl, ski, msg_cnt, &err);
        return;
    }

    const DeviceConfigurationKeyValueDataType* const kv = CsLpFindKeyValue(
        (const DeviceConfigurationKeyValueListDataType*)msg->cmd->data_choice,
        &dc.device_cfg_common,
        kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum
    );

    if ((kv != NULL) && (kv->value->duration != NULL)) {
        const int64_t total_seconds = EebusDurationToSeconds(kv->value->duration);

        // The failsafe duration minimum should be between 2 hours and 24 hours
        if (total_seconds < (2 * 3600) || total_seconds > (24 * 3600)) {
            const ErrorType err = {
                .error_number = kErrorNumberTypeCommandRejected,
                .description  = "Invalid failsafe duration minimum value: should be between 2 hours and 24 hours",
            };
            FEATURE_LOCAL_DENY_WRITE(fl, ski, msg_cnt, &err);
            return;
        }
    }

    FEATURE_LOCAL_TRY_APPROVE_WRITE(fl, ski, msg_cnt);
}

EebusError AddDeviceConfigurationFeature(CsLpUseCase* self, EntityLocalObject* entity) {
    FeatureLocalObject* const fl
        = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeDeviceConfiguration, kRoleTypeServer);
    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceConfigurationKeyValueDescriptionListData, true, false);
    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceConfigurationKeyValueListData, true, true);
    FEATURE_LOCAL_ADD_WRITE_APPROVAL_CALLBACK(fl, CsLpFailsafeActivePowerLimitWriteCallback, self);
    FEATURE_LOCAL_ADD_WRITE_APPROVAL_CALLBACK(fl, CsLpFailsafeDurationMinimumWriteCallback, self);

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
            self->nominal_max_characteristic
                = kElectricalConnectionCharacteristicTypeTypeContractualConsumptionNominalMax;
        } else {
            self->nominal_max_characteristic = kElectricalConnectionCharacteristicTypeTypePowerConsumptionNominalMax;
        }
    } else {
        // According to LPP V1.0 2.2, lines 420ff:
        // - a HEMS provides contractual production nominal max
        // - any other devices provides power production nominal max
        if ((device_type == NULL) || (*device_type == kDeviceTypeTypeEnergyManagementSystem)) {
            self->nominal_max_characteristic
                = kElectricalConnectionCharacteristicTypeTypeContractualProductionNominalMax;
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

void CsLpUseCaseDestruct(UseCaseObject* self) {
    CsLpUseCase* cs_lp = CS_LP_USE_CASE(self);

    EntityAddressDelete(cs_lp->remote_eg_entity_addr);
    cs_lp->remote_eg_entity_addr = NULL;

    RemoveDeviceDiagnosisClient(cs_lp);

    UseCaseDestruct(self);
}
