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
 * @brief EEBUS EV Charger Service implementation
 *
 * Exposes a single kEntityTypeTypeEVSE entity with CS-LPC, CS-LPP, and MU-MPC
 * use cases. Acts as the CS (ControllableSystem) counterpart to a HEMS.
 */

#include "examples/ev_charger/evsrv.h"

#include <stdio.h>
#include <string.h>

#include "src/common/array_util.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"

#include "examples/ev_charger/cs_lpc_listener.h"
#include "examples/ev_charger/cs_lpp_listener.h"
#include "examples/ev_charger/mu_mpc_listener.h"
#include "src/cli/eebus_cli.h"
#include "src/common/eebus_arguments.h"
#include "src/service/api/service_reader_interface.h"
#include "src/service/service/eebus_service.h"
#include "src/ship/tls_certificate/tls_certificate.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/cs/lpc/cs_lpc.h"
#include "src/use_case/actor/cs/lpp/cs_lpp.h"
#include "src/use_case/actor/mu/mpc/mu_mpc.h"

static const int8_t        kScaleDefault                  = -2;
static const uint32_t      kHeartbeatTimeoutSeconds        = 60;
static const ElectricalConnectionIdType kEvsrvElectricalConnectionId = 0;

typedef struct Evsrv Evsrv;

struct Evsrv {
  ServiceReaderObject service_reader;

  EebusServiceConfig*  cfg;
  EebusServiceObject*  service;
  CsLpListenerObject*  cs_lpc_listener;
  CsLpUseCaseObject*   cs_lpc;
  CsLpListenerObject*  cs_lpp_listener;
  CsLpUseCaseObject*   cs_lpp;
  MuMpcListenerObject* mu_mpc_listener;
  MuMpcUseCaseObject*  mu_mpc;
  EebusCliObject*      cli;
};

#define EVSRV(obj) ((Evsrv*)(obj))

static void Destruct(ServiceReaderObject* self);
static void OnRemoteSkiConnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
static void OnRemoteSkiDisconnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
static void OnRemoteServicesUpdate(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries);
static void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* ship_id);
static void OnShipStateUpdate(ServiceReaderObject* self, const char* ski, SmeState state);
static bool IsWaitingForTrustAllowed(const ServiceReaderObject* self, const char* ski);

static const ServiceReaderInterface evsrv_methods = {
    .destruct                     = Destruct,
    .on_remote_ski_connected      = OnRemoteSkiConnected,
    .on_remote_ski_disconnected   = OnRemoteSkiDisconnected,
    .on_remote_services_update    = OnRemoteServicesUpdate,
    .on_ship_id_update            = OnShipIdUpdate,
    .on_ship_state_update         = OnShipStateUpdate,
    .is_waiting_for_trust_allowed = IsWaitingForTrustAllowed,
};

static EebusError EvsrvConstruct(Evsrv* self);
static EebusError EvsrvStart(Evsrv* self, int32_t port, const char* role, TlsCertificateObject* tls_certificate);
static EebusError AddLpc(Evsrv* self, EntityLocalObject* entity_local);
static EebusError AddLpp(Evsrv* self, EntityLocalObject* entity_local);
static EebusError AddMpc(Evsrv* self, EntityLocalObject* entity_local);
static EebusError AddEvseEntity(Evsrv* self, DeviceLocalObject* device_local);

static EebusError EvsrvConstruct(Evsrv* self) {
  SERVICE_READER_INTERFACE(self) = &evsrv_methods;

  self->cfg              = NULL;
  self->service          = NULL;
  self->cs_lpc_listener  = NULL;
  self->cs_lpc           = NULL;
  self->cs_lpp_listener  = NULL;
  self->cs_lpp           = NULL;
  self->mu_mpc_listener  = NULL;
  self->mu_mpc           = NULL;
  self->cli              = NULL;

  self->cli = EebusCliCreate();
  if (self->cli == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

static EebusError AddLpc(Evsrv* self, EntityLocalObject* entity_local) {
  self->cs_lpc_listener = CsLpcListenerCreate();
  if (self->cs_lpc_listener == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  self->cs_lpc = CsLpcUseCaseCreate(entity_local, kEvsrvElectricalConnectionId, self->cs_lpc_listener);
  if (self->cs_lpc == NULL) {
    CsLpcListenerDelete(self->cs_lpc_listener);
    self->cs_lpc_listener = NULL;
    return kEebusErrorInit;
  }

  EEBUS_CLI_SET_CS_LPC(self->cli, self->cs_lpc);
  return kEebusErrorOk;
}

static EebusError AddLpp(Evsrv* self, EntityLocalObject* entity_local) {
  self->cs_lpp_listener = CsLppListenerCreate();
  if (self->cs_lpp_listener == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  self->cs_lpp = CsLppUseCaseCreate(entity_local, kEvsrvElectricalConnectionId, self->cs_lpp_listener);
  if (self->cs_lpp == NULL) {
    CsLppListenerDelete(self->cs_lpp_listener);
    self->cs_lpp_listener = NULL;
    return kEebusErrorInit;
  }

  EEBUS_CLI_SET_CS_LPP(self->cli, self->cs_lpp);
  return kEebusErrorOk;
}

static EebusError AddMpc(Evsrv* self, EntityLocalObject* entity_local) {
  static const MuMpcMeasurementConfig measurement_default_cfg = {
      .value_source = kMeasurementValueSourceTypeMeasuredValue,
  };

  static const MuMpcConfig cfg = {
      .power_cfg = {
          .power_total_cfg = {.value_source = kMeasurementValueSourceTypeMeasuredValue},
          .power_phase_a_cfg = &measurement_default_cfg,
          .power_phase_b_cfg = &measurement_default_cfg,
          .power_phase_c_cfg = &measurement_default_cfg,
      },
  };

  self->mu_mpc_listener = MuMpcListenerCreate();
  if (self->mu_mpc_listener == NULL) {
    return kEebusErrorInit;
  }

  self->mu_mpc = MuMpcUseCaseCreate(entity_local, kEvsrvElectricalConnectionId, &cfg, self->mu_mpc_listener);
  if (self->mu_mpc == NULL) {
    MuMpcListenerDelete(self->mu_mpc_listener);
    self->mu_mpc_listener = NULL;
    return kEebusErrorInit;
  }

  EebusError err = EvsrvSetPowerTotal(EVSRV_OBJECT(self), 0);
  if (err != kEebusErrorOk) {
    return err;
  }

  EEBUS_CLI_SET_MU_MPC(self->cli, self->mu_mpc);
  return kEebusErrorOk;
}

static EebusError AddEvseEntity(Evsrv* self, DeviceLocalObject* device_local) {
  uint32_t entity_ids[1] = {(uint32_t)VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local))};

  EntityLocalObject* const entity = EntityLocalCreate(
      device_local,
      kEntityTypeTypeEVSE,
      entity_ids,
      ARRAY_SIZE(entity_ids),
      kHeartbeatTimeoutSeconds
  );

  if (entity == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  EebusError err = AddLpc(self, entity);
  if (err != kEebusErrorOk) {
    EntityLocalDelete(entity);
    return err;
  }

  err = AddLpp(self, entity);
  if (err != kEebusErrorOk) {
    EntityLocalDelete(entity);
    return err;
  }

  err = AddMpc(self, entity);
  if (err != kEebusErrorOk) {
    EntityLocalDelete(entity);
    return err;
  }

  DEVICE_LOCAL_ADD_ENTITY(device_local, entity);
  return kEebusErrorOk;
}

static EebusError EvsrvStart(Evsrv* self, int32_t port, const char* role, TlsCertificateObject* tls_certificate) {
  if (tls_certificate == NULL) {
    return kEebusErrorInputArgument;
  }

  self->cfg = EebusServiceConfigCreate("OpenEEBUS", "OpenEEBUS", "EV", "123456789", "ElectricVehicleSystem", port);
  if (self->cfg == NULL) {
    return kEebusErrorInit;
  }

  EebusServiceConfigSetAlternateIdentifier(self->cfg, "OpenEEBUS-EV-123456789");

  self->service = EebusServiceCreate(self->cfg, role, tls_certificate, SERVICE_READER_OBJECT(self));
  if (self->service == NULL) {
    EebusServiceConfigDelete(self->cfg);
    self->cfg = NULL;
    return kEebusErrorInit;
  }

  printf("Starting with SKI = %s\n", EEBUS_SERVICE_GET_LOCAL_SKI(self->service));

  DeviceLocalObject* const device_local = EEBUS_SERVICE_GET_LOCAL_DEVICE(self->service);

  if (AddEvseEntity(self, device_local) != kEebusErrorOk) {
    return kEebusErrorOther;
  }

  EEBUS_SERVICE_START(self->service);
  return kEebusErrorOk;
}

EvsrvObject* EvsrvOpen(int32_t port, const char* role, TlsCertificateObject* tls_certificate) {
  Evsrv* const evsrv = (Evsrv*)EEBUS_MALLOC(sizeof(Evsrv));
  if (evsrv == NULL) {
    return NULL;
  }

  if (EvsrvConstruct(evsrv) != kEebusErrorOk) {
    EvsrvClose(EVSRV_OBJECT(evsrv));
    return NULL;
  }

  if (EvsrvStart(evsrv, port, role, tls_certificate) != kEebusErrorOk) {
    EvsrvClose(EVSRV_OBJECT(evsrv));
    return NULL;
  }

  return EVSRV_OBJECT(evsrv);
}

void Destruct(ServiceReaderObject* self) {
  Evsrv* const evsrv = EVSRV(self);

  if (evsrv->service != NULL) {
    EEBUS_SERVICE_STOP(evsrv->service);
    EebusServiceDelete(evsrv->service);
    evsrv->service = NULL;
  }

  EebusCliDelete(evsrv->cli);
  evsrv->cli = NULL;

  UseCaseDelete(USE_CASE_OBJECT(evsrv->mu_mpc));
  evsrv->mu_mpc = NULL;

  MuMpcListenerDelete(evsrv->mu_mpc_listener);
  evsrv->mu_mpc_listener = NULL;

  UseCaseDelete(USE_CASE_OBJECT(evsrv->cs_lpp));
  evsrv->cs_lpp = NULL;

  CsLppListenerDelete(evsrv->cs_lpp_listener);
  evsrv->cs_lpp_listener = NULL;

  UseCaseDelete(USE_CASE_OBJECT(evsrv->cs_lpc));
  evsrv->cs_lpc = NULL;

  CsLpcListenerDelete(evsrv->cs_lpc_listener);
  evsrv->cs_lpc_listener = NULL;

  EebusServiceConfigDelete(evsrv->cfg);
  evsrv->cfg = NULL;
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
}

void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* ship_id) {
  UNUSED(self);
  printf("Ship ID update for SKI %s: %s\n", ski, ship_id);
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

void EvsrvRegisterRemoteSki(EvsrvObject* self, const char* ski) {
  EEBUS_SERVICE_REGISTER_REMOTE_SKI(EVSRV(self)->service, ski, true);
}

void EvsrvUnregisterRemoteSki(EvsrvObject* self, const char* ski) {
  EEBUS_SERVICE_UNREGISTER_REMOTE_SKI(EVSRV(self)->service, ski);
}

EebusError EvsrvSetPowerTotal(EvsrvObject* self, int32_t power_total) {
  Evsrv* const evsrv = EVSRV(self);
  if (evsrv->mu_mpc == NULL) {
    return kEebusErrorOther;
  }
  const ScaledValue power_val = {.value = power_total, .scale = kScaleDefault};
  EebusError err = MuMpcSetMeasurementDataCache(evsrv->mu_mpc, kMpcPowerTotal, &power_val, NULL, NULL);
  if (err != kEebusErrorOk) {
    return err;
  }
  return MuMpcUpdate(evsrv->mu_mpc);
}

void EvsrvHandleCmd(EvsrvObject* self, char* cmd) {
  EEBUS_CLI_HANDLE_CMD(EVSRV(self)->cli, cmd);
}
