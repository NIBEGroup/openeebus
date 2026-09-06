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
 * @brief EEBUS Home Energy Manager Service implementation
 */

#include "examples/hems/hems.h"

#include <stdio.h>
#include <string.h>

#include "src/common/array_util.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/entity_address_list.h"

#include "examples/hems/eg_lpc_listener.h"
#include "examples/hems/eg_lpp_listener.h"
#include "examples/hems/ma_mgcp_listener.h"
#include "examples/hems/ma_mpc_listener.h"
#include "src/cli/eebus_cli.h"
#include "src/common/eebus_arguments.h"
#include "src/service/api/service_reader_interface.h"
#include "src/service/service/eebus_service.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/eg/lpc/eg_lpc.h"
#include "src/use_case/actor/eg/lpp/eg_lpp.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp.h"
#include "src/use_case/actor/ma/mpc/ma_mpc.h"

/** EEBUS Home Energy Manager Service type definition */
typedef struct Hems Hems;

/** EEBUS Home Energy Manager Service type definition */
struct Hems {
  /** "Inherit" Service Reader */
  ServiceReaderObject service_reader;

  EebusServiceConfig* cfg;
  EebusServiceObject* service;
  EgLpUseCaseObject* eg_lpc;
  EgLpListenerObject* eg_lpc_listener;
  EgLpUseCaseObject* eg_lpp;
  EgLpListenerObject* eg_lpp_listener;
  MaMpcUseCaseObject* ma_mpc;
  MaMpcListenerObject* ma_mpc_listener;
  MaMgcpUseCaseObject* ma_mgcp;
  MaMgcpListenerObject* ma_mgcp_listener;
  EebusCliObject* cli;

  EntityAddressList eg_lpc_remotes;
  EntityAddressList eg_lpp_remotes;
  EntityAddressList ma_mpc_remotes;
  EntityAddressList ma_mgcp_remotes;
};

#define HEMS(obj) ((Hems*)(obj))

static const uint32_t kHeartbeatTimeoutSeconds = 60;

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

static EebusError HemsConstruct(Hems* self);
static EebusError AddMaMgcp(Hems* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local);

EebusError HemsConstruct(Hems* self) {
  // Override "virtual functions table"
  SERVICE_READER_INTERFACE(self) = &hpsrv_methods;

  self->cfg              = NULL;
  self->service          = NULL;
  self->eg_lpc           = NULL;
  self->eg_lpc_listener  = NULL;
  self->eg_lpp           = NULL;
  self->eg_lpp_listener  = NULL;
  self->ma_mpc           = NULL;
  self->ma_mpc_listener  = NULL;
  self->ma_mgcp          = NULL;
  self->ma_mgcp_listener = NULL;
  self->cli              = NULL;

  self->cli = EebusCliCreate();
  if (self->cli == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  EntityAddressListInit(&self->eg_lpc_remotes);
  EntityAddressListInit(&self->eg_lpp_remotes);
  EntityAddressListInit(&self->ma_mpc_remotes);
  EntityAddressListInit(&self->ma_mgcp_remotes);

  return kEebusErrorOk;
}

EebusError AddEgLpc(Hems* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local) {
  UNUSED(device_local);

  self->eg_lpc_listener = EgLpcListenerCreate(HEMS_OBJECT(self));
  if (self->eg_lpc_listener == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  self->eg_lpc = EgLpcUseCaseCreate(entity_local, self->eg_lpc_listener);
  if (self->eg_lpc == NULL) {
    EgLpcListenerDelete(self->eg_lpc_listener);
    self->eg_lpc_listener = NULL;
    return kEebusErrorInit;
  }

  return kEebusErrorOk;
}

EebusError AddEgLpp(Hems* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local) {
  UNUSED(device_local);

  self->eg_lpp_listener = EgLppListenerCreate(HEMS_OBJECT(self));
  if (self->eg_lpp_listener == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  self->eg_lpp = EgLppUseCaseCreate(entity_local, self->eg_lpp_listener);
  if (self->eg_lpp == NULL) {
    EgLppListenerDelete(self->eg_lpp_listener);
    self->eg_lpp_listener = NULL;
    return kEebusErrorInit;
  }

  return kEebusErrorOk;
}

EebusError AddMaMpc(Hems* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local) {
  UNUSED(device_local);

  self->ma_mpc_listener = MaMpcListenerCreate(HEMS_OBJECT(self));
  if (self->ma_mpc_listener == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  self->ma_mpc = MaMpcUseCaseCreate(entity_local, self->ma_mpc_listener);
  if (self->ma_mpc == NULL) {
    MaMpcListenerDelete(self->ma_mpc_listener);
    self->ma_mpc_listener = NULL;
    return kEebusErrorInit;
  }

  return kEebusErrorOk;
}

EebusError AddMaMgcp(Hems* self, DeviceLocalObject* device_local, EntityLocalObject* entity_local) {
  UNUSED(device_local);

  self->ma_mgcp_listener = MaMgcpListenerCreate(HEMS_OBJECT(self));
  if (self->ma_mgcp_listener == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  self->ma_mgcp = MaMgcpUseCaseCreate(entity_local, self->ma_mgcp_listener);
  if (self->ma_mgcp == NULL) {
    MaMgcpListenerDelete(self->ma_mgcp_listener);
    self->ma_mgcp_listener = NULL;
    return kEebusErrorInit;
  }

  return kEebusErrorOk;
}

EebusError HemsStart(Hems* hems, int32_t port, const char* role, TlsCertificateObject* tls_certificate) {
  if (tls_certificate == NULL) {
    return kEebusErrorInputArgument;
  }

  hems->cfg = EebusServiceConfigCreate("OpenEEBUS", "OpenEEBUS", "HEMS", "123456789", "EnergyManagementSystem", port);
  if (hems->cfg == NULL) {
    return kEebusErrorInit;
  }

  EebusServiceConfigSetAlternateIdentifier(hems->cfg, "OpenEEBUS-HEMS-123456789");

  hems->service = EebusServiceCreate(hems->cfg, role, tls_certificate, SERVICE_READER_OBJECT(hems));
  if (hems->service == NULL) {
    EebusServiceConfigDelete(hems->cfg);
    hems->cfg = NULL;
    return kEebusErrorInit;
  }

  printf("Starting with SKI = %s\n", EEBUS_SERVICE_GET_LOCAL_SKI(hems->service));

  // Add entities to SPINE Device Local
  DeviceLocalObject* const device_local = EEBUS_SERVICE_GET_LOCAL_DEVICE(hems->service);

  uint32_t entity_ids[1] = {VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local))};

  EntityLocalObject* const entity
      = EntityLocalCreate(device_local, kEntityTypeTypeCEM, entity_ids, 1, kHeartbeatTimeoutSeconds);

  EebusError err = AddEgLpc(hems, device_local, entity);
  if (err != kEebusErrorOk) {
    EntityLocalDelete(entity);
    return err;
  }

  err = AddEgLpp(hems, device_local, entity);
  if (err != kEebusErrorOk) {
    EntityLocalDelete(entity);
    return err;
  }

  err = AddMaMpc(hems, device_local, entity);
  if (err != kEebusErrorOk) {
    EntityLocalDelete(entity);
    return err;
  }

  err = AddMaMgcp(hems, device_local, entity);
  if (err != kEebusErrorOk) {
    EntityLocalDelete(entity);
    return err;
  }

  DEVICE_LOCAL_ADD_ENTITY(device_local, entity);
  EEBUS_SERVICE_START(hems->service);

  return kEebusErrorOk;
}

HemsObject* HemsOpen(int32_t port, const char* role, TlsCertificateObject* tls_certificate) {
  Hems* const hems = (Hems*)EEBUS_MALLOC(sizeof(Hems));
  if (hems == NULL) {
    return NULL;
  }

  EebusError err = HemsConstruct(hems);
  if (err != kEebusErrorOk) {
    HemsClose(HEMS_OBJECT(hems));
    return NULL;
  }

  if (HemsStart(hems, port, role, tls_certificate) != kEebusErrorOk) {
    HemsClose(HEMS_OBJECT(hems));
    return NULL;
  }

  return HEMS_OBJECT(hems);
}

void Destruct(ServiceReaderObject* self) {
  Hems* const hems = HEMS(self);

  if (hems->service != NULL) {
    EEBUS_SERVICE_STOP(hems->service);
    EebusServiceDelete(hems->service);
    hems->service = NULL;
  }

  EebusCliDelete(hems->cli);
  hems->cli = NULL;

  EntityAddressListRelease(&hems->eg_lpc_remotes);
  EntityAddressListRelease(&hems->eg_lpp_remotes);
  EntityAddressListRelease(&hems->ma_mpc_remotes);
  EntityAddressListRelease(&hems->ma_mgcp_remotes);

  UseCaseDelete(USE_CASE_OBJECT(hems->ma_mgcp));
  hems->ma_mgcp = NULL;

  MaMgcpListenerDelete(hems->ma_mgcp_listener);
  hems->ma_mgcp_listener = NULL;

  UseCaseDelete(USE_CASE_OBJECT(hems->ma_mpc));
  hems->ma_mpc = NULL;

  MaMpcListenerDelete(hems->ma_mpc_listener);
  hems->ma_mpc_listener = NULL;

  UseCaseDelete(USE_CASE_OBJECT(hems->eg_lpp));
  hems->eg_lpp = NULL;

  EgLppListenerDelete(hems->eg_lpp_listener);
  hems->eg_lpp_listener = NULL;

  UseCaseDelete(USE_CASE_OBJECT(hems->eg_lpc));
  hems->eg_lpc = NULL;

  EgLpcListenerDelete(hems->eg_lpc_listener);
  hems->eg_lpc_listener = NULL;

  EebusServiceConfigDelete(hems->cfg);
  hems->cfg = NULL;
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

void HemsRegisterRemoteSki(HemsObject* self, const char* ski) {
  EEBUS_SERVICE_REGISTER_REMOTE_SKI(HEMS(self)->service, ski, true);
}

void HemsUnregisterRemoteSki(HemsObject* self, const char* ski) {
  EEBUS_SERVICE_UNREGISTER_REMOTE_SKI(HEMS(self)->service, ski);
}

void HemsSetEgLpcRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr) {
  Hems* const hems = HEMS(self);

  if ((hems->cli == NULL) || (entity_addr == NULL)) {
    return;
  }

  EntityAddressListAdd(&hems->eg_lpc_remotes, entity_addr);
  if (EntityAddressListGetSize(&hems->eg_lpc_remotes) == 1) {
    EEBUS_CLI_SET_EG_LPC(hems->cli, hems->eg_lpc, &hems->eg_lpc_remotes);
  }
}

void HemsRemoveEgLpcRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr) {
  Hems* const hems = HEMS(self);

  if (entity_addr == NULL) {
    return;
  }

  EntityAddressListRemove(&hems->eg_lpc_remotes, entity_addr);
  if (EntityAddressListGetSize(&hems->eg_lpc_remotes) == 0) {
    EEBUS_CLI_SET_EG_LPC(hems->cli, NULL, NULL);
  }
}

void HemsSetEgLppRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr) {
  Hems* const hems = HEMS(self);

  if ((hems->cli == NULL) || (entity_addr == NULL)) {
    return;
  }

  EntityAddressListAdd(&hems->eg_lpp_remotes, entity_addr);
  if (EntityAddressListGetSize(&hems->eg_lpp_remotes) == 1) {
    EEBUS_CLI_SET_EG_LPP(hems->cli, hems->eg_lpp, &hems->eg_lpp_remotes);
  }
}

void HemsRemoveEgLppRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr) {
  Hems* const hems = HEMS(self);

  if (entity_addr == NULL) {
    return;
  }

  EntityAddressListRemove(&hems->eg_lpp_remotes, entity_addr);
  if (EntityAddressListGetSize(&hems->eg_lpp_remotes) == 0) {
    EEBUS_CLI_SET_EG_LPP(hems->cli, NULL, NULL);
  }
}

void HemsAddMaMpcRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr) {
  Hems* const hems = HEMS(self);

  if ((hems->cli == NULL) || (entity_addr == NULL)) {
    return;
  }

  // Simplified check to avoid dealing with the subentity
  // (as there is OHPCF with optional MPC attached to subentity)
  if (entity_addr->entity_size == 1) {
    EntityAddressListAdd(&hems->ma_mpc_remotes, entity_addr);
    if (EntityAddressListGetSize(&hems->ma_mpc_remotes) == 1) {
      EEBUS_CLI_SET_MA_MPC(hems->cli, hems->ma_mpc, &hems->ma_mpc_remotes);
    }
  }
}

void HemsRemoveMaMpcRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr) {
  Hems* const hems = HEMS(self);

  if ((hems->cli == NULL) || (entity_addr == NULL)) {
    return;
  }

  if (entity_addr->entity_size == 1) {
    EntityAddressListRemove(&hems->ma_mpc_remotes, entity_addr);
    if (EntityAddressListGetSize(&hems->ma_mpc_remotes) == 0) {
      EEBUS_CLI_SET_MA_MPC(hems->cli, NULL, NULL);
    }
  }
}

void HemsSetMaMgcpRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr) {
  Hems* const hems = HEMS(self);

  if ((hems->cli == NULL) || (entity_addr == NULL)) {
    return;
  }

  EntityAddressListAdd(&hems->ma_mgcp_remotes, entity_addr);
  if (EntityAddressListGetSize(&hems->ma_mgcp_remotes) == 1) {
    EEBUS_CLI_SET_MA_MGCP(hems->cli, hems->ma_mgcp, &hems->ma_mgcp_remotes);
  }
}

void HemsRemoveMaMgcpRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr) {
  Hems* const hems = HEMS(self);

  if (entity_addr == NULL) {
    return;
  }

  EntityAddressListRemove(&hems->ma_mgcp_remotes, entity_addr);
  if (EntityAddressListGetSize(&hems->ma_mgcp_remotes) == 0) {
    EEBUS_CLI_SET_MA_MGCP(hems->cli, NULL, NULL);
  }
}

void HemsHandleCmd(HemsObject* self, char* cmd) {
  Hems* const hems = HEMS(self);

  EEBUS_CLI_HANDLE_CMD(hems->cli, cmd);
}
