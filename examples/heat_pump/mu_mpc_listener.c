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
 * @brief MU MPC Listener implementation
 */

#include <stdio.h>

#include "examples/heat_pump/mu_mpc_listener.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"

typedef struct MuMpcListener MuMpcListener;

struct MuMpcListener {
  /** Implements the MU MPC Listener Interface */
  MuMpcListenerObject obj;
};

#define MU_MPC_LISTENER(obj) ((MuMpcListener*)(obj))

static void Destruct(MuMpcListenerObject* self);
static void OnRemoteMaAdded(MuMpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteMaRemoved(MuMpcListenerObject* self, const EntityAddressType* entity_addr);

static const MuMpcListenerInterface mu_mpc_listener_methods = {
    .destruct             = Destruct,
    .on_remote_ma_added   = OnRemoteMaAdded,
    .on_remote_ma_removed = OnRemoteMaRemoved,
};

static EebusError MuMpcListenerConstruct(MuMpcListener* self);

EebusError MuMpcListenerConstruct(MuMpcListener* self) {
  MU_MPC_LISTENER_INTERFACE(self) = &mu_mpc_listener_methods;
  return kEebusErrorOk;
}

MuMpcListenerObject* MuMpcListenerCreate(void) {
  MuMpcListener* const mu_mpc_listener = (MuMpcListener*)EEBUS_MALLOC(sizeof(MuMpcListener));
  if (mu_mpc_listener == NULL) {
    return NULL;
  }

  if (MuMpcListenerConstruct(mu_mpc_listener) != kEebusErrorOk) {
    MuMpcListenerDelete(MU_MPC_LISTENER_OBJECT(mu_mpc_listener));
    return NULL;
  }

  return MU_MPC_LISTENER_OBJECT(mu_mpc_listener);
}

void Destruct(MuMpcListenerObject* self) {
  UNUSED(self);
}

void OnRemoteMaAdded(MuMpcListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(self);
  UNUSED(entity_addr);

  printf("MU MPC Remote MA added\n");
}

void OnRemoteMaRemoved(MuMpcListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(self);
  UNUSED(entity_addr);

  printf("MU MPC Remote MA removed\n");
}
