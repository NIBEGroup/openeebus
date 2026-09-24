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
 * @brief Ma Mgcp Listener implementation
 */

#include <stdio.h>

#include "examples/hems/hems.h"
#include "examples/hems/ma_mgcp_listener.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/ma_mgcp_listener_interface.h"
#include "src/use_case/model/mgcp_types.h"
#include "src/use_case/model/scaled_value.h"

typedef struct MaMgcpListener MaMgcpListener;

struct MaMgcpListener {
  /** Implements the Ma Mgcp Listener Interface */
  MaMgcpListenerObject obj;

  /* Pointer to the HEMS instance */
  HemsObject* hems;
};

#define MA_MGCP_LISTENER(obj) ((MaMgcpListener*)(obj))

static void Destruct(MaMgcpListenerObject* self);
static void OnRemoteEntityConnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteEntityDisconnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr);
static void OnMeasurementReceive(
    MaMgcpListenerObject* self,
    GcpMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
);
static void OnPvCurtailmentLimitFactorReceive(
    MaMgcpListenerObject* self,
    const ScaledValue* value,
    const EntityAddressType* remote_entity_addr
);

static const MaMgcpListenerInterface ma_mgcp_listener_methods = {
    .destruct                               = Destruct,
    .on_remote_entity_connect               = OnRemoteEntityConnect,
    .on_remote_entity_disconnect            = OnRemoteEntityDisconnect,
    .on_measurement_receive                 = OnMeasurementReceive,
    .on_pv_curtailment_limit_factor_receive = OnPvCurtailmentLimitFactorReceive,
};

static EebusError MaMgcpListenerConstruct(MaMgcpListener* self, HemsObject* hems);

EebusError MaMgcpListenerConstruct(MaMgcpListener* self, HemsObject* hems) {
  // Override "virtual functions table"
  MA_MGCP_LISTENER_INTERFACE(self) = &ma_mgcp_listener_methods;

  self->hems = hems;

  return kEebusErrorOk;
}

MaMgcpListenerObject* MaMgcpListenerCreate(HemsObject* hems) {
  MaMgcpListener* const ma_mgcp_listener = (MaMgcpListener*)EEBUS_MALLOC(sizeof(MaMgcpListener));
  if (ma_mgcp_listener == NULL) {
    return NULL;
  }

  if (MaMgcpListenerConstruct(ma_mgcp_listener, hems) != kEebusErrorOk) {
    MaMgcpListenerDelete(MA_MGCP_LISTENER_OBJECT(ma_mgcp_listener));
    return NULL;
  }

  return MA_MGCP_LISTENER_OBJECT(ma_mgcp_listener);
}

void Destruct(MaMgcpListenerObject* self) {
  UNUSED(self);

  // Nothing to be deallocated yet
}

void OnRemoteEntityConnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr) {
  MaMgcpListener* const ma_mgcp_listener = MA_MGCP_LISTENER(self);

  printf("MA MGCP remote entity connected\n");
  HemsSetMaMgcpRemoteEntity(ma_mgcp_listener->hems, entity_addr);
}

void OnRemoteEntityDisconnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr) {
  MaMgcpListener* const ma_mgcp_listener = MA_MGCP_LISTENER(self);

  HemsRemoveMaMgcpRemoteEntity(ma_mgcp_listener->hems, entity_addr);
}

void OnMeasurementReceive(
    MaMgcpListenerObject* self,
    GcpMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
) {
  UNUSED(self);

  const char* name = GcpMgcpMeasurementGetName(name_id);
  if (name == NULL) {
    printf("MA MGCP Measurement received: Unknown Measurement ID %d\n", (int)name_id);
    return;
  }

  printf("MA MGCP Measurement received: %s = ", name);
  ScaledValuePrint("%s,", measurement_value);
  EntityAddressPrint(" from entity: %s\n", remote_entity_addr);
}

void OnPvCurtailmentLimitFactorReceive(
    MaMgcpListenerObject* self,
    const ScaledValue* value,
    const EntityAddressType* remote_entity_addr
) {
  UNUSED(self);

  printf("MA MGCP PV curtailment limit factor received: ");
  ScaledValuePrint("%s,", value);
  EntityAddressPrint(" from entity: %s\n", remote_entity_addr);
}
