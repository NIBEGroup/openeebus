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
 * @brief CEM OHPCF Listener implementation
 */

#include <stdio.h>

#include "examples/hems/cem_ohpcf_listener.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/use_case/model/ohpcf_types.h"

typedef struct CemOhpcfListener CemOhpcfListener;

struct CemOhpcfListener {
  /** Implements the CEM OHPCF Listener Interface */
  CemOhpcfListenerObject obj;

  /* Pointer to the HEMS instance */
  HemsObject* hems;
};

#define CEM_OHPCF_LISTENER(obj) ((CemOhpcfListener*)(obj))

static void Destruct(CemOhpcfListenerObject* self);
static void OnRemoteCompressorAdded(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteCompressorRemoved(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);
static void OnAnnounce(
    CemOhpcfListenerObject* self,
    const OptionalPowerConsumption* optional_power_consumption,
    const EntityAddressType* entity_addr
);
static void OnStateReport(
    CemOhpcfListenerObject* self,
    CompressorOhpcfState state,
    const EebusDuration* start_time,
    const EntityAddressType* entity_addr
);
static void OnClearProcess(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);

static const CemOhpcfListenerInterface cem_ohpcf_listener_methods = {
    .destruct                     = Destruct,
    .on_remote_compressor_added   = OnRemoteCompressorAdded,
    .on_remote_compressor_removed = OnRemoteCompressorRemoved,
    .on_announce                  = OnAnnounce,
    .on_state_report              = OnStateReport,
    .on_clear_process             = OnClearProcess,
};

static EebusError CemOhpcfListenerConstruct(CemOhpcfListener* self, HemsObject* hems);

EebusError CemOhpcfListenerConstruct(CemOhpcfListener* self, HemsObject* hems) {
  // Override "virtual functions table"
  CEM_OHPCF_LISTENER_INTERFACE(self) = &cem_ohpcf_listener_methods;

  self->hems = hems;

  return kEebusErrorOk;
}

CemOhpcfListenerObject* CemOhpcfListenerCreate(HemsObject* hems) {
  CemOhpcfListener* const cem_ohpcf_listener = (CemOhpcfListener*)EEBUS_MALLOC(sizeof(CemOhpcfListener));
  if (cem_ohpcf_listener == NULL) {
    return NULL;
  }

  if (CemOhpcfListenerConstruct(cem_ohpcf_listener, hems) != kEebusErrorOk) {
    CemOhpcfListenerDelete(CEM_OHPCF_LISTENER_OBJECT(cem_ohpcf_listener));
    return NULL;
  }

  return CEM_OHPCF_LISTENER_OBJECT(cem_ohpcf_listener);
}

void Destruct(CemOhpcfListenerObject* self) {
  UNUSED(self);

  // Nothing to be deallocated yet
}

void OnRemoteCompressorAdded(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr) {
  CemOhpcfListener* const cem_ohpcf_listener = CEM_OHPCF_LISTENER(self);

  HemsSetCemOhpcfRemoteEntity(cem_ohpcf_listener->hems, entity_addr);
}

void OnRemoteCompressorRemoved(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(entity_addr);

  CemOhpcfListener* const cem_ohpcf_listener = CEM_OHPCF_LISTENER(self);

  // Currently only single remote entity is supported,
  // so just clear the remote entity address
  HemsSetCemOhpcfRemoteEntity(cem_ohpcf_listener->hems, NULL);
}

void OnAnnounce(
    CemOhpcfListenerObject* self,
    const OptionalPowerConsumption* optional_power_consumption,
    const EntityAddressType* entity_addr
) {
  UNUSED(self);
  UNUSED(entity_addr);

  printf("Received announce with optional power consumption:\n");
  OptionalPowerConsumptionPrint(optional_power_consumption);
  printf("\n");
}

void OnStateReport(
    CemOhpcfListenerObject* self,
    CompressorOhpcfState state,
    const EebusDuration* start_time,
    const EntityAddressType* entity_addr
) {
  UNUSED(self);
  UNUSED(entity_addr);

  const char* const state_name = CompressorOhpcfStateGetName(state);
  if (state_name == NULL) {
    printf("Unknown state report: %d\n", state);
    return;
  }

  printf("Received state report: %s", state_name);
  if (start_time != NULL) {
    EebusDurationPrint(", start time: %s\n", start_time);
  } else {
    printf("\n");
  }
}

void OnClearProcess(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(self);
  UNUSED(entity_addr);

  printf("Received clear process\n");
}
