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
 * @brief Ma Mpc Listener implementation
 */

#include <math.h>
#include <stdio.h>

#include "examples/hems/hems.h"
#include "examples/hems/ma_mpc_listener.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/ma_mpc_listener_interface.h"
#include "src/use_case/model/mpc_types.h"
#include "src/use_case/model/scaled_value.h"

typedef struct MaMpcListener MaMpcListener;

struct MaMpcListener {
    /** Implements the Ma Mpc Listener Interface */
    MaMpcListenerObject obj;

    /* Pointer to the HEMS instance */
    HemsObject* hems;
};

#define MA_MPC_LISTENER(obj) ((MaMpcListener*)(obj))

static void Destruct(MaMpcListenerObject* self);
static void OnRemoteMuAdded(MaMpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteMuRemoved(MaMpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnMeasurementReceive(
    MaMpcListenerObject* self,
    MuMpcMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
);

static const MaMpcListenerInterface ma_mpc_listener_methods = {
    .destruct               = Destruct,
    .on_remote_mu_added     = OnRemoteMuAdded,
    .on_remote_mu_removed   = OnRemoteMuRemoved,
    .on_measurement_receive = OnMeasurementReceive,
};

static EebusError MaMpcListenerConstruct(MaMpcListener* self, HemsObject* hems);

EebusError MaMpcListenerConstruct(MaMpcListener* self, HemsObject* hems) {
    // Override "virtual functions table"
    MA_MPC_LISTENER_INTERFACE(self) = &ma_mpc_listener_methods;

    self->hems = hems;

    return kEebusErrorOk;
}

MaMpcListenerObject* MaMpcListenerCreate(HemsObject* hems) {
    MaMpcListener* const ma_mpc_listener = (MaMpcListener*)EEBUS_MALLOC(sizeof(MaMpcListener));
    if (ma_mpc_listener == NULL) {
        return NULL;
    }

    if (MaMpcListenerConstruct(ma_mpc_listener, hems) != kEebusErrorOk) {
        MaMpcListenerDelete(MA_MPC_LISTENER_OBJECT(ma_mpc_listener));
        return NULL;
    }

    return MA_MPC_LISTENER_OBJECT(ma_mpc_listener);
}

void Destruct(MaMpcListenerObject* self) {
    UNUSED(self);

    // Nothing to be deallocated yet
}

void OnRemoteMuAdded(MaMpcListenerObject* self, const EntityAddressType* entity_addr) {
    MaMpcListener* const ma_mpc_listener = MA_MPC_LISTENER(self);

    HemsAddMaMpcRemoteEntity(ma_mpc_listener->hems, entity_addr);
}

void OnRemoteMuRemoved(MaMpcListenerObject* self, const EntityAddressType* entity_addr) {
    MaMpcListener* const ma_mpc_listener = MA_MPC_LISTENER(self);

    HemsRemoveMaMpcRemoteEntity(ma_mpc_listener->hems, entity_addr);
}

void OnMeasurementReceive(
    MaMpcListenerObject* self,
    MuMpcMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
) {
    UNUSED(self);

    const char* name = MuMpcMeasurementGetName(name_id);
    if (name == NULL) {
        printf("MA MPC Measurement received: Unknown Measurement ID %d\n", (int)name_id);
        return;
    }

    printf("MA MPC Measurement received: %s = ", name);
    ScaledValuePrint("%s,", measurement_value);
    EntityAddressPrint(" from entity: %s\n", remote_entity_addr);
}
