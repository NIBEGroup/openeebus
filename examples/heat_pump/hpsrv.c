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
 * @brief EEBUS Heat Pump Service implementation
 */

#include "hpsrv.h"

#include <stdio.h>
#include <string.h>

#include "src/common/array_util.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"

#include "examples/heat_pump/cs_lpc_listener.h"
#include "examples/heat_pump/cs_lpp_listener.h"
#include "examples/heat_pump/mu_mpc_listener.h"
#include "src/cli/eebus_cli.h"
#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/service/api/service_reader_interface.h"
#include "src/service/service/eebus_service.h"
#include "src/ship/tls_certificate/tls_certificate.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/cs/lpc/cs_lpc.h"
#include "src/use_case/actor/cs/lpp/cs_lpp.h"
#include "src/use_case/actor/gcp/mgcp/gcp_mgcp.h"
#include "src/use_case/actor/mu/mpc/mu_mpc.h"

static const int8_t kScaleDefault = -2;  // Default scale for measurements

/** EEBUS Heat Pump Service type definition */
typedef struct Hpsrv Hpsrv;

/** EEBUS Heat Pump Service type definition */
struct Hpsrv {
    /** "Inherit" Service Reader */
    ServiceReaderObject service_reader;

    EebusServiceConfig* cfg;
    EebusServiceObject* service;
    CsLpListenerObject* cs_lpc_listener;
    CsLpUseCaseObject* cs_lpc;
    CsLpListenerObject* cs_lpp_listener;
    CsLpUseCaseObject* cs_lpp;
    MuMpcListenerObject* mu_mpc_listener;
    MuMpcUseCaseObject* mu_mpc;
    GcpMgcpUseCaseObject* gcp_mgcp;
    EebusCliObject* cli;
};

#define HPSRV(obj) ((Hpsrv*)(obj))

typedef struct MpcData MpcData;

struct MpcData {
    MuMpcMeasurementNameId name;
    int32_t value;
};

static const uint32_t kHeartbeatTimeoutSeconds = 60;

static const ElectricalConnectionIdType kHpsrvElectricalConnectionId = 0;

static void Destruct(ServiceReaderObject* self);
static void OnRemoteSkiConnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
static void OnRemoteSkiDisconnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
static void OnRemoteServicesUpdate(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries);
static void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* shipd_id);
static void OnShipStateUpdate(ServiceReaderObject* self, const char* ski, SmeState state);
static bool IsWaitingForTrustAllowed(const ServiceReaderObject* self, const char* ski);

static const ServiceReaderInterface hpsrv_methods = {
    .destruct                     = Destruct,
    .on_remote_ski_connected      = OnRemoteSkiConnected,
    .on_remote_ski_disconnected   = OnRemoteSkiDisconnected,
    .on_remote_services_update    = OnRemoteServicesUpdate,
    .on_ship_id_update            = OnShipIdUpdate,
    .on_ship_state_update         = OnShipStateUpdate,
    .is_waiting_for_trust_allowed = IsWaitingForTrustAllowed,
};

static EebusError HpsrvConstruct(Hpsrv* self);
static EebusError HpsrvStart(Hpsrv* self, int32_t port, const char* role, TlsCertificateObject* tls_certificate);

static EebusError AddLpc(Hpsrv* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local);
static EebusError AddLpp(Hpsrv* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local);
static EebusError AddMpc(Hpsrv* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local);
static EebusError AddGcpMgcp(Hpsrv* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local);

static EebusError AddHeatPumpApplianceEntity(
    Hpsrv* self,
    DeviceLocalObject* device_local,
    const uint32_t* entity_ids,
    size_t entity_id_size
);
static EebusError
AddInverterEntity(Hpsrv* self, DeviceLocalObject* device_local, const uint32_t* entity_ids, size_t entity_id_size);
static EebusError AddGridConnectionPointEntity(
    Hpsrv* self,
    DeviceLocalObject* device_local,
    const uint32_t* entity_ids,
    size_t entity_id_size
);

static EebusError SetMpcData(Hpsrv* self, const MpcData* mpc_data, size_t mpc_data_size);

static EebusError HpsrvConstruct(Hpsrv* self) {
    // Override "virtual functions table"
    SERVICE_READER_INTERFACE(self) = &hpsrv_methods;

    self->cfg             = NULL;
    self->service         = NULL;
    self->cs_lpc_listener = NULL;
    self->cs_lpc          = NULL;
    self->cs_lpp_listener = NULL;
    self->cs_lpp          = NULL;
    self->mu_mpc_listener = NULL;
    self->mu_mpc          = NULL;
    self->gcp_mgcp        = NULL;
    self->cli             = NULL;

    self->cli = EebusCliCreate();
    if (self->cli == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

static EebusError AddLpc(Hpsrv* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local) {
    UNUSED(device_local);

    self->cs_lpc_listener = CsLpcListenerCreate();
    if (self->cs_lpc_listener == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    self->cs_lpc = CsLpcUseCaseCreate(entity_local, kHpsrvElectricalConnectionId, self->cs_lpc_listener);
    if (self->cs_lpc == NULL) {
        CsLpcListenerDelete(self->cs_lpc_listener);
        self->cs_lpc_listener = NULL;
        return kEebusErrorInit;
    }

    EEBUS_CLI_SET_CS_LPC(self->cli, self->cs_lpc);
    return kEebusErrorOk;
}

static EebusError AddLpp(Hpsrv* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local) {
    UNUSED(device_local);

    self->cs_lpp_listener = CsLppListenerCreate();
    if (self->cs_lpp_listener == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    self->cs_lpp = CsLppUseCaseCreate(entity_local, kHpsrvElectricalConnectionId, self->cs_lpp_listener);
    if (self->cs_lpp == NULL) {
        CsLppListenerDelete(self->cs_lpp_listener);
        self->cs_lpp_listener = NULL;
        return kEebusErrorInit;
    }

    EEBUS_CLI_SET_CS_LPP(self->cli, self->cs_lpp);
    return kEebusErrorOk;
}

static EebusError AddMpc(Hpsrv* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local) {
    UNUSED(device_local);

    static const MuMpcMeasurementConfig measurement_default_cfg = {
        .value_source = kMeasurementValueSourceTypeMeasuredValue,
    };

    static const MuMpcMonitorEnergyConfig energy_cfg = {
        .energy_production_cfg  = &measurement_default_cfg,
        .energy_consumption_cfg = &measurement_default_cfg,
    };

    static const MuMpcMonitorCurrentConfig current_cfg = {
        .current_phase_a_cfg = &measurement_default_cfg,
        .current_phase_b_cfg = &measurement_default_cfg,
        .current_phase_c_cfg = &measurement_default_cfg,
    };

    static const MuMpcMonitorVoltageConfig voltage_cfg = {
        .voltage_phase_a_cfg  = &measurement_default_cfg,
        .voltage_phase_b_cfg  = &measurement_default_cfg,
        .voltage_phase_c_cfg  = &measurement_default_cfg,
        .voltage_phase_ab_cfg = &measurement_default_cfg,
        .voltage_phase_bc_cfg = &measurement_default_cfg,
        .voltage_phase_ac_cfg = &measurement_default_cfg,
    };

    static const MuMpcMonitorFrequencyConfig frequency_cfg = {
        .frequency_cfg = {.value_source = kMeasurementValueSourceTypeMeasuredValue},
    };

    static const MuMpcConfig cfg = {
    .power_cfg = {
        .power_total_cfg   = {.value_source = kMeasurementValueSourceTypeMeasuredValue},
        .power_phase_a_cfg = &measurement_default_cfg,
        .power_phase_b_cfg = &measurement_default_cfg,
        .power_phase_c_cfg = &measurement_default_cfg,
    },

    .energy_cfg    = &energy_cfg,
    .current_cfg   = &current_cfg,
    .voltage_cfg   = &voltage_cfg,
    .frequency_cfg = &frequency_cfg
  };

    self->mu_mpc_listener = MuMpcListenerCreate();
    if (self->mu_mpc_listener == NULL) {
        return kEebusErrorInit;
    }

    self->mu_mpc = MuMpcUseCaseCreate(entity_local, kHpsrvElectricalConnectionId, &cfg, self->mu_mpc_listener);
    if (self->mu_mpc == NULL) {
        MuMpcListenerDelete(self->mu_mpc_listener);
        self->mu_mpc_listener = NULL;
        return kEebusErrorInit;
    }

    EebusError err = HpsrvSetPowerTotal(HPSRV_OBJECT(self), 0);
    if (err != kEebusErrorOk) {
        return err;
    }

    EEBUS_CLI_SET_MU_MPC(self->cli, self->mu_mpc);
    return kEebusErrorOk;
}

static EebusError AddHeatPumpApplianceEntity(
    Hpsrv* self,
    DeviceLocalObject* device_local,
    const uint32_t* entity_ids,
    size_t entity_id_size
) {
    EntityLocalObject* const entity = EntityLocalCreate(
        device_local,
        kEntityTypeTypeHeatPumpAppliance,
        entity_ids,
        entity_id_size,
        kHeartbeatTimeoutSeconds
    );

    if (entity == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    EebusError err = AddLpc(self, device_local, entity);
    if (err != kEebusErrorOk) {
        EntityLocalDelete(entity);
        return err;
    }

    err = AddMpc(self, device_local, entity);
    if (err != kEebusErrorOk) {
        EntityLocalDelete(entity);
        return err;
    }

    DEVICE_LOCAL_ADD_ENTITY(device_local, entity);
    return kEebusErrorOk;
}

EebusError
AddInverterEntity(Hpsrv* self, DeviceLocalObject* device_local, const uint32_t* entity_ids, size_t entity_id_size) {
    EntityLocalObject* const entity = EntityLocalCreate(
        device_local,
        kEntityTypeTypeInverter,
        entity_ids,
        entity_id_size,
        kHeartbeatTimeoutSeconds
    );

    if (entity == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    const EebusError err = AddLpp(self, device_local, entity);
    if (err != kEebusErrorOk) {
        EntityLocalDelete(entity);
        return err;
    }

    DEVICE_LOCAL_ADD_ENTITY(device_local, entity);
    return kEebusErrorOk;
}

static EebusError AddGcpMgcp(Hpsrv* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local) {
    UNUSED(device_local);

    static const GcpMgcpMeasurementConfig measurement_default_cfg = {
        .value_source = kMeasurementValueSourceTypeMeasuredValue,
    };

    static const GcpMgcpMonitorEnergyConfig energy_cfg = {
        .energy_feed_in_cfg  = &measurement_default_cfg,
        .energy_consumed_cfg = &measurement_default_cfg,
    };

    static const GcpMgcpMonitorCurrentConfig current_cfg = {
        .current_phase_a_cfg = &measurement_default_cfg,
        .current_phase_b_cfg = &measurement_default_cfg,
        .current_phase_c_cfg = &measurement_default_cfg,
    };

    static const GcpMgcpMonitorVoltageConfig voltage_cfg = {
        .voltage_phase_a_cfg  = &measurement_default_cfg,
        .voltage_phase_b_cfg  = &measurement_default_cfg,
        .voltage_phase_c_cfg  = &measurement_default_cfg,
        .voltage_phase_ab_cfg = &measurement_default_cfg,
        .voltage_phase_bc_cfg = &measurement_default_cfg,
        .voltage_phase_ac_cfg = &measurement_default_cfg,
    };

    static const GcpMgcpMonitorFrequencyConfig frequency_cfg = {
        .frequency_cfg = {.value_source = kMeasurementValueSourceTypeMeasuredValue},
    };

    static const GcpMgcpPvCurtailmentConfig pv_curtailment_cfg = {0};

    static const GcpMgcpConfig cfg = {
      .pv_curtailment_cfg = &pv_curtailment_cfg,
      .power_cfg          = {
          .phases          = kElectricalConnectionPhaseNameTypeAbc,
          .power_total_cfg = {.value_source = kMeasurementValueSourceTypeMeasuredValue},
      },
      .energy_cfg         = &energy_cfg,
      .current_cfg        = &current_cfg,
      .voltage_cfg        = &voltage_cfg,
      .frequency_cfg      = &frequency_cfg,
  };

    self->gcp_mgcp = GcpMgcpUseCaseCreate(entity_local, kHpsrvElectricalConnectionId, &cfg);
    if (self->gcp_mgcp == NULL) {
        return kEebusErrorInit;
    }

    const ScaledValue zero_power = {.value = 0, .scale = kScaleDefault};
    EebusError err = GcpMgcpSetMeasurementDataCache(self->gcp_mgcp, kGcpPowerTotal, &zero_power, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    err = GcpMgcpUpdate(self->gcp_mgcp);
    if (err != kEebusErrorOk) {
        return err;
    }

    EEBUS_CLI_SET_GCP_MGCP(self->cli, self->gcp_mgcp);
    return kEebusErrorOk;
}

static EebusError AddGridConnectionPointEntity(
    Hpsrv* self,
    DeviceLocalObject* device_local,
    const uint32_t* entity_ids,
    size_t entity_id_size
) {
    EntityLocalObject* const entity = EntityLocalCreate(
        device_local,
        kEntityTypeTypeGridConnectionPointOfPremises,
        entity_ids,
        entity_id_size,
        kHeartbeatTimeoutSeconds
    );

    if (entity == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    if (AddGcpMgcp(self, device_local, entity) != kEebusErrorOk) {
        EntityLocalDelete(entity);
        return kEebusErrorInit;
    }

    DEVICE_LOCAL_ADD_ENTITY(device_local, entity);
    return kEebusErrorOk;
}

static EebusError HpsrvStart(Hpsrv* hpsrv, int32_t port, const char* role, TlsCertificateObject* tls_certificate) {
    if (tls_certificate == NULL) {
        return kEebusErrorInputArgument;
    }

    hpsrv->cfg = EebusServiceConfigCreate("NIBE", "NIBE", "HeatPump", "123456789", "HeatGenerationSystem", port);
    if (hpsrv->cfg == NULL) {
        return kEebusErrorOther;
    }

    EebusServiceConfigSetAlternateIdentifier(hpsrv->cfg, "NIBE-HeatPump-123456789");

    hpsrv->service = EebusServiceCreate(hpsrv->cfg, role, tls_certificate, SERVICE_READER_OBJECT(hpsrv));
    if (hpsrv->service == NULL) {
        return kEebusErrorInit;
    }

    printf("Starting with SKI = %s\n", EEBUS_SERVICE_GET_LOCAL_SKI(hpsrv->service));

    // Add entities to SPINE Device Local
    DeviceLocalObject* const device_local = EEBUS_SERVICE_GET_LOCAL_DEVICE(hpsrv->service);

    uint32_t entity_ids[1] = {VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local))};

    if (AddHeatPumpApplianceEntity(hpsrv, device_local, entity_ids, ARRAY_SIZE(entity_ids)) != kEebusErrorOk) {
        return kEebusErrorOther;
    }

    uint32_t inverter_entity_ids[1] = {VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local))};
    if (AddInverterEntity(hpsrv, device_local, inverter_entity_ids, ARRAY_SIZE(inverter_entity_ids)) != kEebusErrorOk) {
        return kEebusErrorOther;
    }

    uint32_t gcp_entity_ids[1] = {VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local))};
    if (AddGridConnectionPointEntity(hpsrv, device_local, gcp_entity_ids, ARRAY_SIZE(gcp_entity_ids))
        != kEebusErrorOk) {
        return kEebusErrorOther;
    }

    EEBUS_SERVICE_START(hpsrv->service);

    return kEebusErrorOk;
}

HpsrvObject* HpsrvOpen(int32_t port, const char* role, TlsCertificateObject* tls_certificate) {
    Hpsrv* const hpsrv = (Hpsrv*)EEBUS_MALLOC(sizeof(Hpsrv));
    if (hpsrv == NULL) {
        return NULL;
    }

    EebusError err = HpsrvConstruct(hpsrv);
    if (err != kEebusErrorOk) {
        HpsrvClose(HPSRV_OBJECT(hpsrv));
        return NULL;
    }

    if (HpsrvStart(hpsrv, port, role, tls_certificate) != kEebusErrorOk) {
        HpsrvClose(HPSRV_OBJECT(hpsrv));
        return NULL;
    }

    return HPSRV_OBJECT(hpsrv);
}

void Destruct(ServiceReaderObject* self) {
    Hpsrv* const hpsrv = HPSRV(self);

    EebusCliDelete(hpsrv->cli);
    hpsrv->cli = NULL;

    if (hpsrv->service != NULL) {
        EEBUS_SERVICE_STOP(hpsrv->service);
        EebusServiceDelete(hpsrv->service);
        hpsrv->service = NULL;
    }

    UseCaseDelete(USE_CASE_OBJECT(hpsrv->mu_mpc));
    hpsrv->mu_mpc = NULL;

    MuMpcListenerDelete(hpsrv->mu_mpc_listener);
    hpsrv->mu_mpc_listener = NULL;

    UseCaseDelete(USE_CASE_OBJECT(hpsrv->cs_lpp));
    hpsrv->cs_lpp = NULL;

    CsLppListenerDelete(hpsrv->cs_lpp_listener);
    hpsrv->cs_lpp_listener = NULL;

    UseCaseDelete(USE_CASE_OBJECT(hpsrv->cs_lpc));
    hpsrv->cs_lpc = NULL;

    CsLpcListenerDelete(hpsrv->cs_lpc_listener);
    hpsrv->cs_lpc_listener = NULL;

    UseCaseDelete(USE_CASE_OBJECT(hpsrv->gcp_mgcp));
    hpsrv->gcp_mgcp = NULL;

    EebusServiceConfigDelete(hpsrv->cfg);
    hpsrv->cfg = NULL;
}

void OnRemoteSkiConnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski) {
    UNUSED(self);
    UNUSED(service);

    printf("Remote SKI connected: %s\n", ski);
}

void OnRemoteSkiDisconnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski) {
    UNUSED(self);
    UNUSED(service);

    printf("Remote SKI disconnected: %s\n", ski);
}

void OnRemoteServicesUpdate(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries) {
    UNUSED(self);
    UNUSED(service);
    UNUSED(entries);

    // Optional: print the remote services
}

void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* shipd_id) {
    UNUSED(self);
    printf("Ship ID update for SKI %s: %s\n", ski, shipd_id);
}

void OnShipStateUpdate(ServiceReaderObject* self, const char* ski, SmeState state) {
    UNUSED(self);

    printf("Ship state update for SKI %s: %d\n", ski, state);
}

bool IsWaitingForTrustAllowed(const ServiceReaderObject* self, const char* ski) {
    UNUSED(self);
    UNUSED(ski);

    return true;
}

void HpsrvRegisterRemoteSki(HpsrvObject* self, const char* ski) {
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(HPSRV(self)->service, ski, true);
}

void HpsrvUnregisterRemoteSki(HpsrvObject* self, const char* ski) {
    EEBUS_SERVICE_UNREGISTER_REMOTE_SKI(HPSRV(self)->service, ski);
}

static EebusError SetMpcData(Hpsrv* self, const MpcData* mpc_data, size_t mpc_data_size) {
    EebusError err = kEebusErrorOk;

    for (size_t i = 0; i < mpc_data_size; ++i) {
        const ScaledValue value = {.value = mpc_data[i].value, .scale = kScaleDefault};

        err = MuMpcSetMeasurementDataCache(self->mu_mpc, mpc_data[i].name, &value, NULL, NULL);
        if (err != kEebusErrorOk) {
            return err;
        }
    }

    return kEebusErrorOk;
}

EebusError HpsrvSetPowerTotal(HpsrvObject* self, int32_t power_total) {
    Hpsrv* const hpsrv = HPSRV(self);

    const ScaledValue power_total_val = {.value = power_total, .scale = kScaleDefault};
    EebusError err = MuMpcSetMeasurementDataCache(hpsrv->mu_mpc, kMpcPowerTotal, &power_total_val, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    return MuMpcUpdate(hpsrv->mu_mpc);
}

EebusError
HpsrvSetPowerPerPhase(HpsrvObject* self, int32_t power_phase_a, int32_t power_phase_b, int32_t power_phase_c) {
    Hpsrv* const hpsrv = HPSRV(self);

    const MpcData power_phase_data[] = {
        {kMpcPowerPhaseA, power_phase_a},
        {kMpcPowerPhaseB, power_phase_b},
        {kMpcPowerPhaseC, power_phase_c},
    };

    EebusError err = SetMpcData(hpsrv, power_phase_data, ARRAY_SIZE(power_phase_data));
    if (err != kEebusErrorOk) {
        return err;
    }

    return MuMpcUpdate(hpsrv->mu_mpc);
}

EebusError HpsrvSetEnergyConsumed(HpsrvObject* self, int32_t energy_consumed) {
    Hpsrv* const hpsrv = HPSRV(self);

    const ScaledValue energy_consumed_val = {.value = energy_consumed, .scale = kScaleDefault};
    EebusError err = MuMpcSetEnergyConsumedCache(hpsrv->mu_mpc, &energy_consumed_val, NULL, NULL, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    return MuMpcUpdate(hpsrv->mu_mpc);
}

EebusError HpsrvSetEnergyProduced(HpsrvObject* self, int32_t energy_produced) {
    Hpsrv* const hpsrv = HPSRV(self);

    const ScaledValue energy_produced_val = {.value = energy_produced, .scale = kScaleDefault};
    EebusError err = MuMpcSetEnergyProducedCache(hpsrv->mu_mpc, &energy_produced_val, NULL, NULL, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    return MuMpcUpdate(hpsrv->mu_mpc);
}

EebusError HpsrvSetAcCurrentPerPhase(
    HpsrvObject* self,
    int32_t current_phase_a,
    int32_t current_phase_b,
    int32_t current_phase_c
) {
    Hpsrv* const hpsrv = HPSRV(self);

    const MpcData current_phase_data[] = {
        {kMpcCurrentPhaseA, current_phase_a},
        {kMpcCurrentPhaseB, current_phase_b},
        {kMpcCurrentPhaseC, current_phase_c},
    };

    EebusError err = SetMpcData(hpsrv, current_phase_data, ARRAY_SIZE(current_phase_data));
    if (err != kEebusErrorOk) {
        return err;
    }

    return MuMpcUpdate(hpsrv->mu_mpc);
}

EebusError HpsrvSetVoltagePerPhase(
    HpsrvObject* self,
    int32_t voltage_phase_a,
    int32_t voltage_phase_b,
    int32_t voltage_phase_c,
    int32_t voltage_phase_ab,
    int32_t voltage_phase_bc,
    int32_t voltage_phase_ac
) {
    Hpsrv* const hpsrv = HPSRV(self);

    const MpcData voltage_phase_data[] = {
        { kMpcVoltagePhaseA,  voltage_phase_a},
        { kMpcVoltagePhaseB,  voltage_phase_b},
        { kMpcVoltagePhaseC,  voltage_phase_c},
        {kMpcVoltagePhaseAb, voltage_phase_ab},
        {kMpcVoltagePhaseBc, voltage_phase_bc},
        {kMpcVoltagePhaseAc, voltage_phase_ac},
    };

    EebusError err = SetMpcData(hpsrv, voltage_phase_data, ARRAY_SIZE(voltage_phase_data));
    if (err != kEebusErrorOk) {
        return err;
    }

    return MuMpcUpdate(hpsrv->mu_mpc);
}

EebusError HpsrvSetAcFrequency(HpsrvObject* self, int32_t ac_frequency) {
    Hpsrv* const hpsrv = HPSRV(self);

    const ScaledValue frequency_val = {.value = ac_frequency, .scale = kScaleDefault};
    EebusError err = MuMpcSetMeasurementDataCache(hpsrv->mu_mpc, kMpcFrequency, &frequency_val, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    return MuMpcUpdate(hpsrv->mu_mpc);
}

EebusError HpsrvSetGcpMgcpPowerTotal(HpsrvObject* self, int32_t power_total) {
    Hpsrv* const hpsrv = HPSRV(self);

    const ScaledValue value = {.value = power_total, .scale = kScaleDefault};
    EebusError err          = GcpMgcpSetMeasurementDataCache(hpsrv->gcp_mgcp, kGcpPowerTotal, &value, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    return GcpMgcpUpdate(hpsrv->gcp_mgcp);
}

EebusError HpsrvSetGcpMgcpEnergyFeedIn(HpsrvObject* self, int32_t energy_feed_in) {
    Hpsrv* const hpsrv = HPSRV(self);

    const ScaledValue value = {.value = energy_feed_in, .scale = kScaleDefault};
    EebusError err          = GcpMgcpSetEnergyFeedInCache(hpsrv->gcp_mgcp, &value, NULL, NULL, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    return GcpMgcpUpdate(hpsrv->gcp_mgcp);
}

EebusError HpsrvSetGcpMgcpEnergyConsumed(HpsrvObject* self, int32_t energy_consumed) {
    Hpsrv* const hpsrv = HPSRV(self);

    const ScaledValue value = {.value = energy_consumed, .scale = kScaleDefault};
    EebusError err          = GcpMgcpSetEnergyConsumedCache(hpsrv->gcp_mgcp, &value, NULL, NULL, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    return GcpMgcpUpdate(hpsrv->gcp_mgcp);
}

EebusError HpsrvSetGcpMgcpCurrentPerPhase(
    HpsrvObject* self,
    int32_t current_phase_a,
    int32_t current_phase_b,
    int32_t current_phase_c
) {
    Hpsrv* const hpsrv = HPSRV(self);

    const GcpMeasurementNameId names[] = {kGcpCurrentPhaseA, kGcpCurrentPhaseB, kGcpCurrentPhaseC};
    const int32_t values[]             = {current_phase_a, current_phase_b, current_phase_c};

    for (size_t i = 0; i < ARRAY_SIZE(names); ++i) {
        const ScaledValue sv = {.value = values[i], .scale = kScaleDefault};
        EebusError err       = GcpMgcpSetMeasurementDataCache(hpsrv->gcp_mgcp, names[i], &sv, NULL, NULL);
        if (err != kEebusErrorOk) {
            return err;
        }
    }

    return GcpMgcpUpdate(hpsrv->gcp_mgcp);
}

EebusError HpsrvSetGcpMgcpVoltagePerPhase(
    HpsrvObject* self,
    int32_t voltage_phase_a,
    int32_t voltage_phase_b,
    int32_t voltage_phase_c,
    int32_t voltage_phase_ab,
    int32_t voltage_phase_bc,
    int32_t voltage_phase_ac
) {
    Hpsrv* const hpsrv = HPSRV(self);

    const GcpMeasurementNameId names[] = {
        kGcpVoltagePhaseA,
        kGcpVoltagePhaseB,
        kGcpVoltagePhaseC,
        kGcpVoltagePhaseAb,
        kGcpVoltagePhaseBc,
        kGcpVoltagePhaseAc,
    };
    const int32_t values[] = {
        voltage_phase_a,
        voltage_phase_b,
        voltage_phase_c,
        voltage_phase_ab,
        voltage_phase_bc,
        voltage_phase_ac,
    };

    for (size_t i = 0; i < ARRAY_SIZE(names); ++i) {
        const ScaledValue sv = {.value = values[i], .scale = kScaleDefault};
        EebusError err       = GcpMgcpSetMeasurementDataCache(hpsrv->gcp_mgcp, names[i], &sv, NULL, NULL);
        if (err != kEebusErrorOk) {
            return err;
        }
    }

    return GcpMgcpUpdate(hpsrv->gcp_mgcp);
}

EebusError HpsrvSetGcpMgcpFrequency(HpsrvObject* self, int32_t frequency) {
    Hpsrv* const hpsrv = HPSRV(self);

    const ScaledValue value = {.value = frequency, .scale = kScaleDefault};
    EebusError err          = GcpMgcpSetMeasurementDataCache(hpsrv->gcp_mgcp, kGcpFrequency, &value, NULL, NULL);
    if (err != kEebusErrorOk) {
        return err;
    }

    return GcpMgcpUpdate(hpsrv->gcp_mgcp);
}

EebusError HpsrvSetGcpMgcpPvCurtailmentLimitFactor(HpsrvObject* self, const ScaledValue* value) {
    Hpsrv* const hpsrv = HPSRV(self);

    return GcpMgcpSetPvCurtailmentLimitFactor(hpsrv->gcp_mgcp, value);
}

void HpsrvHandleCmd(HpsrvObject* self, char* cmd) {
    Hpsrv* const hpsrv = HPSRV(self);
    EEBUS_CLI_HANDLE_CMD(hpsrv->cli, cmd);
}
