/*
 * Copyright 2026 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Battery EEBUS example: a Battery entity exposes CS LPC, CS LPP, and MU MPC.
 */

#include "examples/battery/battery.h"

#include <inttypes.h>
#include <stdio.h>

#include "src/cli/eebus_cli.h"
#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/service/service/eebus_service.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/cs/lpc/cs_lpc.h"
#include "src/use_case/actor/cs/lpp/cs_lpp.h"
#include "src/use_case/actor/mu/mpc/mu_mpc.h"

typedef struct Battery Battery;

struct Battery {
  BatteryObject obj;
  EebusServiceConfig* cfg;
  EebusServiceObject* service;
  CsLpListenerObject* lpc_listener;
  CsLpUseCaseObject* lpc;
  CsLpListenerObject* lpp_listener;
  CsLpUseCaseObject* lpp;
  MuMpcListenerObject* mpc_listener;
  MuMpcUseCaseObject* mpc;
  EebusCliObject* cli;
};

typedef struct BatteryLpListener {
  CsLpListenerObject obj;
  const char* name;
} BatteryLpListener;

typedef struct BatteryMpcListener {
  MuMpcListenerObject obj;
} BatteryMpcListener;

#define BATTERY(obj) ((Battery*)(obj))
#define BATTERY_LP_LISTENER(obj) ((BatteryLpListener*)(obj))

static const uint32_t kHeartbeatTimeoutSeconds = 60;
static const ElectricalConnectionIdType kElectricalConnectionId = 0;

static void BatteryDestruct(ServiceReaderObject* self);
static void OnRemoteSkiConnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
static void OnRemoteSkiDisconnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
static void OnRemoteServicesUpdate(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries);
static void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* ship_id);
static void OnShipStateUpdate(ServiceReaderObject* self, const char* ski, SmeState state);
static bool IsWaitingForTrustAllowed(const ServiceReaderObject* self, const char* ski);

static const ServiceReaderInterface battery_service_reader_methods = {
    .destruct                     = BatteryDestruct,
    .on_remote_ski_connected      = OnRemoteSkiConnected,
    .on_remote_ski_disconnected   = OnRemoteSkiDisconnected,
    .on_remote_services_update    = OnRemoteServicesUpdate,
    .on_ship_id_update            = OnShipIdUpdate,
    .on_ship_state_update         = OnShipStateUpdate,
    .is_waiting_for_trust_allowed = IsWaitingForTrustAllowed,
};

static void LpListenerDestruct(CsLpListenerObject* self) {
  UNUSED(self);
}

static void OnRemoteEgAdded(CsLpListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(entity_addr);
  printf("%s Remote EG added\n", BATTERY_LP_LISTENER(self)->name);
}

static void OnRemoteEgRemoved(CsLpListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(entity_addr);
  printf("%s Remote EG removed\n", BATTERY_LP_LISTENER(self)->name);
}

static void OnPowerLimitReceive(
    CsLpListenerObject* self,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
) {
  const char* const name = BATTERY_LP_LISTENER(self)->name;
  printf("%s Power Limit received ", name);
  ScaledValuePrint("%sW, ", power_limit);
  EebusDurationPrint("duration = %s, ", duration);
  printf("active = %s\n", is_active ? "true" : "false");
}

static void OnFailsafePowerLimitReceive(CsLpListenerObject* self, const ScaledValue* power_limit) {
  printf("%s Failsafe Active Power Limit received: ", BATTERY_LP_LISTENER(self)->name);
  ScaledValuePrint("%sW\n", power_limit);
}

static void OnFailsafeDurationReceive(CsLpListenerObject* self, const DurationType* duration) {
  printf("%s Failsafe Duration Minimum received: ", BATTERY_LP_LISTENER(self)->name);
  EebusDurationPrint("%s\n", duration);
}

static void OnHeartbeatReceive(CsLpListenerObject* self, uint64_t heartbeat_counter) {
  printf("%s Heartbeat received, counter = %" PRIu64 "\n", BATTERY_LP_LISTENER(self)->name, heartbeat_counter);
}

static const CsLpListenerInterface battery_lp_listener_methods = {
    .destruct                        = LpListenerDestruct,
    .on_remote_eg_added              = OnRemoteEgAdded,
    .on_remote_eg_removed            = OnRemoteEgRemoved,
    .on_power_limit_receive          = OnPowerLimitReceive,
    .on_failsafe_power_limit_receive = OnFailsafePowerLimitReceive,
    .on_failsafe_duration_receive    = OnFailsafeDurationReceive,
    .on_heartbeat_receive            = OnHeartbeatReceive,
};

static CsLpListenerObject* BatteryLpListenerCreate(const char* name) {
  BatteryLpListener* const listener = EEBUS_MALLOC(sizeof(*listener));
  if (listener == NULL) {
    return NULL;
  }

  CS_LP_LISTENER_INTERFACE(listener) = &battery_lp_listener_methods;
  listener->name                     = name;
  return CS_LP_LISTENER_OBJECT(listener);
}

static void MpcListenerDestruct(MuMpcListenerObject* self) {
  UNUSED(self);
}

static void OnRemoteMaAdded(MuMpcListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(self);
  UNUSED(entity_addr);
  printf("MU MPC Remote MA added\n");
}

static void OnRemoteMaRemoved(MuMpcListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(self);
  UNUSED(entity_addr);
  printf("MU MPC Remote MA removed\n");
}

static const MuMpcListenerInterface battery_mpc_listener_methods = {
    .destruct             = MpcListenerDestruct,
    .on_remote_ma_added   = OnRemoteMaAdded,
    .on_remote_ma_removed = OnRemoteMaRemoved,
};

static MuMpcListenerObject* BatteryMpcListenerCreate(void) {
  BatteryMpcListener* const listener = EEBUS_MALLOC(sizeof(*listener));
  if (listener == NULL) {
    return NULL;
  }

  MU_MPC_LISTENER_INTERFACE(listener) = &battery_mpc_listener_methods;
  return MU_MPC_LISTENER_OBJECT(listener);
}

static void DeleteLpListener(CsLpListenerObject* listener) {
  if (listener != NULL) {
    CS_LP_LISTENER_DESTRUCT(listener);
    EEBUS_FREE(listener);
  }
}

static void DeleteMpcListener(MuMpcListenerObject* listener) {
  if (listener != NULL) {
    MU_MPC_LISTENER_DESTRUCT(listener);
    EEBUS_FREE(listener);
  }
}

static EebusError AddUseCases(Battery* self, DeviceLocalObject* device_local) {
  const uint32_t entity_id[] = {VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local))};
  EntityLocalObject* const entity = EntityLocalCreate(
      device_local, kEntityTypeTypeBattery, entity_id, ARRAY_SIZE(entity_id), kHeartbeatTimeoutSeconds);
  if (entity == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  self->lpc_listener = BatteryLpListenerCreate("CS LPC");
  self->lpp_listener = BatteryLpListenerCreate("CS LPP");
  self->mpc_listener = BatteryMpcListenerCreate();
  if ((self->lpc_listener == NULL) || (self->lpp_listener == NULL) || (self->mpc_listener == NULL)) {
    EntityLocalDelete(entity);
    return kEebusErrorMemoryAllocate;
  }

  self->lpc = CsLpcUseCaseCreate(entity, kElectricalConnectionId, self->lpc_listener);
  self->lpp = CsLppUseCaseCreate(entity, kElectricalConnectionId, self->lpp_listener);

  static const MuMpcMeasurementConfig measured = {.value_source = kMeasurementValueSourceTypeMeasuredValue};
  static const MuMpcMonitorEnergyConfig energy_cfg = {
      .energy_production_cfg = &measured,
      .energy_consumption_cfg = &measured,
  };
  static const MuMpcConfig mpc_cfg = {
      .power_cfg = {
          .power_total_cfg = {.value_source = kMeasurementValueSourceTypeMeasuredValue},
          .power_phase_a_cfg = &measured,
          .power_phase_b_cfg = &measured,
          .power_phase_c_cfg = &measured,
      },
      .energy_cfg = &energy_cfg,
      .current_cfg = NULL,
      .voltage_cfg = NULL,
      .frequency_cfg = NULL,
  };
  self->mpc = MuMpcUseCaseCreate(entity, kElectricalConnectionId, &mpc_cfg, self->mpc_listener);

  if ((self->lpc == NULL) || (self->lpp == NULL) || (self->mpc == NULL)) {
    EntityLocalDelete(entity);
    return kEebusErrorInit;
  }

  EEBUS_CLI_SET_CS_LPC(self->cli, self->lpc);
  EEBUS_CLI_SET_CS_LPP(self->cli, self->lpp);
  EEBUS_CLI_SET_MU_MPC(self->cli, self->mpc);
  DEVICE_LOCAL_ADD_ENTITY(device_local, entity);
  return kEebusErrorOk;
}

static EebusError BatteryStart(Battery* self, int32_t port, const char* role, TlsCertificateObject* tls_certificate) {
  self->cfg = EebusServiceConfigCreate(
      "OpenEEBUS", "OpenEEBUS", "Battery", "123456789", "EnergyManagementSystem", port);
  if (self->cfg == NULL) {
    return kEebusErrorInit;
  }

  EebusServiceConfigSetAlternateIdentifier(self->cfg, "OpenEEBUS-Battery-123456789");
  EebusServiceConfigSetAlternateMdnsServiceName(self->cfg, "OpenEEBUS-Battery-123456789");
  self->service = EebusServiceCreate(self->cfg, role, tls_certificate, SERVICE_READER_OBJECT(self));
  if (self->service == NULL) {
    return kEebusErrorInit;
  }

  printf("Starting with SKI = %s\n", EEBUS_SERVICE_GET_LOCAL_SKI(self->service));
  const EebusError err = AddUseCases(self, EEBUS_SERVICE_GET_LOCAL_DEVICE(self->service));
  if (err != kEebusErrorOk) {
    return err;
  }

  EEBUS_SERVICE_START(self->service);
  return kEebusErrorOk;
}

BatteryObject* BatteryOpen(int32_t port, const char* role, TlsCertificateObject* tls_certificate) {
  if (tls_certificate == NULL) {
    return NULL;
  }

  Battery* const battery = EEBUS_MALLOC(sizeof(*battery));
  if (battery == NULL) {
    return NULL;
  }

  SERVICE_READER_INTERFACE(battery) = &battery_service_reader_methods;
  battery->cfg = NULL;
  battery->service = NULL;
  battery->lpc_listener = NULL;
  battery->lpc = NULL;
  battery->lpp_listener = NULL;
  battery->lpp = NULL;
  battery->mpc_listener = NULL;
  battery->mpc = NULL;
  battery->cli = EebusCliCreate();

  if ((battery->cli == NULL) || (BatteryStart(battery, port, role, tls_certificate) != kEebusErrorOk)) {
    BatteryClose(BATTERY_OBJECT(battery));
    return NULL;
  }

  return BATTERY_OBJECT(battery);
}

static void BatteryDestruct(ServiceReaderObject* self) {
  Battery* const battery = BATTERY(self);

  if (battery->service != NULL) {
    EEBUS_SERVICE_STOP(battery->service);
  }

  UseCaseDelete(USE_CASE_OBJECT(battery->mpc));
  DeleteMpcListener(battery->mpc_listener);
  UseCaseDelete(USE_CASE_OBJECT(battery->lpp));
  DeleteLpListener(battery->lpp_listener);
  UseCaseDelete(USE_CASE_OBJECT(battery->lpc));
  DeleteLpListener(battery->lpc_listener);

  if (battery->service != NULL) {
    EebusServiceDelete(battery->service);
  }

  EebusCliDelete(battery->cli);
  EebusServiceConfigDelete(battery->cfg);
}

static void OnRemoteSkiConnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski) {
  UNUSED(self);
  UNUSED(service);
  printf("Remote SKI connected: %s\n", ski);
}

static void OnRemoteSkiDisconnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski) {
  UNUSED(self);
  UNUSED(service);
  printf("Remote SKI disconnected: %s\n", ski);
}

static void OnRemoteServicesUpdate(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries) {
  UNUSED(self);
  UNUSED(service);
  UNUSED(entries);
}

static void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* ship_id) {
  UNUSED(self);
  printf("Ship ID update for SKI %s: %s\n", ski, ship_id);
}

static void OnShipStateUpdate(ServiceReaderObject* self, const char* ski, SmeState state) {
  UNUSED(self);
  printf("Ship state update for SKI %s: %d\n", ski, state);
}

static bool IsWaitingForTrustAllowed(const ServiceReaderObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  return false;
}

void BatteryRegisterRemoteSki(BatteryObject* self, const char* ski) {
  EEBUS_SERVICE_REGISTER_REMOTE_SKI(BATTERY(self)->service, ski, true);
}

void BatteryHandleCmd(BatteryObject* self, char* cmd) {
  EEBUS_CLI_HANDLE_CMD(BATTERY(self)->cli, cmd);
}
