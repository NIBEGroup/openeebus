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
 * @brief Service implementation
 */

#include <string.h>

#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/service/api/service_reader_interface.h"
#include "src/service/service/eebus_service.h"
#include "src/ship/api/ship_node_interface.h"
#include "src/ship/ship_node/ship_node.h"
#include "src/ship/tls_certificate/tls_certificate.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/device/device_local.h"
#include "src/spine/entity/entity_local.h"

/** Set EEBUS_SERVICE_DEBUG 1 to enable debug prints */
#ifndef EEBUS_SERVICE_DEBUG
#define EEBUS_SERVICE_DEBUG 0
#endif

#if EEBUS_SERVICE_DEBUG
#define EEBUS_SERVICE_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define EEBUS_SERVICE_DEBUG_PRINTF(fmt, ...)
#endif

typedef struct EebusService EebusService;

struct EebusService {
  /** Implements the Service Interface */
  EebusServiceObject obj;

  ServiceDetails* local_service_details;
  EebusDeviceInfo* device_info;
  ShipNodeObject* ship_node;
  DeviceLocalObject* spine_local_device;
  const TlsCertificateObject* tls_certificate;
  ServiceReaderObject* service_reader;
  bool is_pairing_possible;
};

#define EEBUS_SERVICE(obj) ((EebusService*)(obj))

static void Destruct(ShipNodeReaderObject* self);
static void OnRemoteSkiConnected(ShipNodeReaderObject* self, const char* ski);
static void OnRemoteSkiDisconnected(ShipNodeReaderObject* self, const char* ski);
static DataReaderObject* SetupRemoteDevice(ShipNodeReaderObject* self, const char* ski, DataWriterObject* data_writer);
static void OnRemoteServicesUpdate(ShipNodeReaderObject* self, const Vector* entries);
static void OnShipIdUpdate(ShipNodeReaderObject* self, const char* ski, const char* ship_id);
static void OnHandleShipStateUpdate(ShipNodeReaderObject* self, const char* ski, SmeState state);
static bool IsWaitingForTrustAllowed(ShipNodeReaderObject* self, const char* ski);
static void Start(EebusServiceObject* self);
static void Stop(EebusServiceObject* self);
static const ServiceDetails* GetLocalService(const EebusServiceObject* self);
static DeviceLocalObject* GetLocalDevice(const EebusServiceObject* self);
static const ConnectionState* GetConnectionStateWithSki(const EebusServiceObject* self, const char* ski);
static const ServiceDetails* GetRemoteServiceDetailsWithSki(const EebusServiceObject* self, const char* ski);
static void RegisterRemoteSki(EebusServiceObject* self, const char* ski, bool enable);
static void UnregisterRemoteSki(EebusServiceObject* self, const char* ski);
static void CancelPairingWithSki(EebusServiceObject* self, const char* ski);
static void SetPairingPossible(EebusServiceObject* self, bool is_pairing_possible);
static const char* GetLocalSki(EebusServiceObject* self);

static const EebusServiceInterface service_methods = {
    .ship_node_reader_interface = {
        .destruct                     = Destruct,
        .on_remote_ski_connected      = OnRemoteSkiConnected,
        .on_remote_ski_disconnected   = OnRemoteSkiDisconnected,
        .setup_remote_device          = SetupRemoteDevice,
        .on_remote_services_update    = OnRemoteServicesUpdate,
        .on_ship_id_update            = OnShipIdUpdate,
        .on_ship_state_update         = OnHandleShipStateUpdate,
        .is_waiting_for_trust_allowed = IsWaitingForTrustAllowed,
    },

    .start                               = Start,
    .stop                                = Stop,
    .get_local_service                   = GetLocalService,
    .get_local_device                    = GetLocalDevice,
    .get_connection_state_with_ski       = GetConnectionStateWithSki,
    .get_remote_service_details_with_ski = GetRemoteServiceDetailsWithSki,
    .register_remote_ski                 = RegisterRemoteSki,
    .unregister_remote_ski               = UnregisterRemoteSki,
    .cancel_pairing_with_ski             = CancelPairingWithSki,
    .set_pairing_possible                = SetPairingPossible,
    .get_local_ski                       = GetLocalSki,
};

static EebusError ServiceConstruct(
    EebusService* self,
    const EebusServiceConfig* cfg,
    const char* role,
    const TlsCertificateObject* tls_certificate,
    ServiceReaderObject* service_reader
);

EebusError ServiceConstruct(
    EebusService* self,
    const EebusServiceConfig* cfg,
    const char* role,
    const TlsCertificateObject* tls_certificate,
    ServiceReaderObject* service_reader
) {
  // Override "virtual functions table"
  EEBUS_SERVICE_INTERFACE(self) = &service_methods;

  self->local_service_details = NULL;
  self->device_info           = NULL;
  self->ship_node             = NULL;
  self->spine_local_device    = NULL;
  self->tls_certificate       = NULL;
  self->service_reader        = NULL;

  const char* const type    = EebusServiceConfigGetDeviceType(cfg);
  const char* const ship_id = EebusServiceConfigGetShipId(cfg);
  const bool auto_accept    = EebusServiceConfigGetRegisterAutoAccept(cfg);
  const char* const ski     = TLS_CERTIFICATE_GET_SKI(tls_certificate);

  self->local_service_details = ServiceDetailsCreate(ski, ship_id, type, auto_accept);
  if (self->local_service_details == NULL) {
    return kEebusErrorInit;
  }

  const char* const vendor = EebusServiceConfigGetVendorCode(cfg);
  const char* const brand  = EebusServiceConfigGetDeviceBrand(cfg);
  const char* const model  = EebusServiceConfigGetDeviceModel(cfg);
  const char* const serial = EebusServiceConfigGetDeviceSerialNumber(cfg);

  self->device_info = EebusDeviceInfoCreate(type, vendor, brand, model, serial, ship_id);
  if (self->device_info == NULL) {
    return kEebusErrorInit;
  }

  const NetworkManagementFeatureSetType feature_set = EebusServiceConfigGetFeatureSet(cfg);

  // Create the local SPINE device
  self->spine_local_device = DeviceLocalCreate(self->device_info, &feature_set);
  if (self->spine_local_device == NULL) {
    return kEebusErrorInit;
  }

  const char* const service_name = EebusServiceConfigGetMdnsServiceName(cfg);
  const int32_t port             = EebusServiceConfigGetPort(cfg);

  self->tls_certificate = tls_certificate;

  // Create the Ship Node
  self->ship_node = ShipNodeCreate(
      ski,
      role,
      self->device_info,
      service_name,
      port,
      tls_certificate,
      SHIP_NODE_READER_OBJECT(self),
      self->local_service_details
  );

  if (self->ship_node == NULL) {
    return kEebusErrorInit;
  }

  self->service_reader      = service_reader;
  self->is_pairing_possible = false;
  return kEebusErrorOk;
}

EebusServiceObject* EebusServiceCreate(
    const EebusServiceConfig* cfg,
    const char* role,
    const TlsCertificateObject* tls_certificate,
    ServiceReaderObject* service_reader
) {
  EebusService* const service = (EebusService*)EEBUS_MALLOC(sizeof(EebusService));
  if (service == NULL) {
    return NULL;
  }

  if (ServiceConstruct(service, cfg, role, tls_certificate, service_reader) != kEebusErrorOk) {
    EebusServiceDelete(EEBUS_SERVICE_OBJECT(service));
    return NULL;
  }

  return EEBUS_SERVICE_OBJECT(service);
}

void Destruct(ShipNodeReaderObject* self) {
  EebusService* const service = EEBUS_SERVICE(self);
  EEBUS_SERVICE_DEBUG_PRINTF("EebusService::%s(): begin\n", __func__);

  // Note: Service shall not own Service Reader therefore it is not released here

  ShipNodeDelete(service->ship_node);
  service->ship_node = NULL;

  TlsCertificateDelete((TlsCertificateObject*)service->tls_certificate);
  service->tls_certificate = NULL;

  if (service->spine_local_device != NULL) {
    DeviceLocalDelete(service->spine_local_device);
    service->spine_local_device = NULL;
  }

  ServiceDetailsDelete(service->local_service_details);
  service->local_service_details = NULL;

  EebusDeviceInfoDelete(service->device_info);
  service->device_info = NULL;

  EEBUS_SERVICE_DEBUG_PRINTF("EebusService::%s(): end\n", __func__);
}

void OnRemoteSkiConnected(ShipNodeReaderObject* self, const char* ski) {
  EebusService* const service = EEBUS_SERVICE(self);
  SERVICE_READER_ON_REMOTE_SKI_CONNECTED(service->service_reader, EEBUS_SERVICE_OBJECT(self), ski);
}

void OnRemoteSkiDisconnected(ShipNodeReaderObject* self, const char* ski) {
  EebusService* const service = EEBUS_SERVICE(self);
  if (service->spine_local_device != NULL) {
    DEVICE_LOCAL_REMOVE_REMOTE_DEVICE_CONNECTION(service->spine_local_device, ski);
  }

  SERVICE_READER_ON_REMOTE_SKI_DISCONNECTED(service->service_reader, EEBUS_SERVICE_OBJECT(self), ski);
}

DataReaderObject* SetupRemoteDevice(ShipNodeReaderObject* self, const char* ski, DataWriterObject* data_writer) {
  EebusService* const service = EEBUS_SERVICE(self);
  return DEVICE_LOCAL_SETUP_REMOTE_DEVICE(service->spine_local_device, ski, data_writer);
}

void OnRemoteServicesUpdate(ShipNodeReaderObject* self, const Vector* entries) {
  EebusService* const service = EEBUS_SERVICE(self);
  SERVICE_READER_ON_REMOTE_SERVICES_UPDATE(service->service_reader, EEBUS_SERVICE_OBJECT(self), entries);
}

void OnShipIdUpdate(ShipNodeReaderObject* self, const char* ski, const char* ship_id) {
  SERVICE_READER_ON_SHIP_ID_UPDATE(EEBUS_SERVICE(self)->service_reader, ski, ship_id);
}

void OnHandleShipStateUpdate(ShipNodeReaderObject* self, const char* ski, SmeState state) {
  SERVICE_READER_ON_SHIP_STATE_UPDATE(EEBUS_SERVICE(self)->service_reader, ski, state);
}

bool IsWaitingForTrustAllowed(ShipNodeReaderObject* self, const char* ski) {
  UNUSED(ski);

  return EEBUS_SERVICE(self)->is_pairing_possible;
}

void Start(EebusServiceObject* self) {
  EebusService* const service = EEBUS_SERVICE(self);
  DEVICE_LOCAL_START(service->spine_local_device);
  SHIP_NODE_START(service->ship_node);
}

void Stop(EebusServiceObject* self) {
  EebusService* const service = EEBUS_SERVICE(self);
  EEBUS_SERVICE_DEBUG_PRINTF("EebusService::%s(): begin\n", __func__);
  // Stop DeviceLocal first so its loop thread is joined before ShipNode
  // frees remote-device objects — prevents dangling-pointer DeviceLocal::ProcessDatagram() crash
  DEVICE_LOCAL_STOP(service->spine_local_device);
  SHIP_NODE_STOP(service->ship_node);
  EEBUS_SERVICE_DEBUG_PRINTF("EebusService::%s(): end\n", __func__);
}

const ServiceDetails* GetLocalService(const EebusServiceObject* self) {
  return EEBUS_SERVICE(self)->local_service_details;
}

DeviceLocalObject* GetLocalDevice(const EebusServiceObject* self) {
  return EEBUS_SERVICE(self)->spine_local_device;
}

const ConnectionState* GetConnectionStateWithSki(const EebusServiceObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  // TODO: Implement method
  return NULL;
}

const ServiceDetails* GetRemoteServiceDetailsWithSki(const EebusServiceObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  // TODO: Implement method
  return NULL;
}

void RegisterRemoteSki(EebusServiceObject* self, const char* ski, bool enable) {
  SHIP_NODE_REGISTER_REMOTE_SKI(EEBUS_SERVICE(self)->ship_node, ski, enable);
}

void UnregisterRemoteSki(EebusServiceObject* self, const char* ski) {
  SHIP_NODE_UNREGISTER_REMOTE_SKI(EEBUS_SERVICE(self)->ship_node, ski);
}

void CancelPairingWithSki(EebusServiceObject* self, const char* ski) {
  SHIP_NODE_CANCEL_PAIRING_WITH_SKI(EEBUS_SERVICE(self)->ship_node, ski);
}

void SetPairingPossible(EebusServiceObject* self, bool is_pairing_possible) {
  EEBUS_SERVICE(self)->is_pairing_possible = is_pairing_possible;
}

const char* GetLocalSki(EebusServiceObject* self) {
  EebusService* const service = EEBUS_SERVICE(self);
  return service->local_service_details->ski;
}
