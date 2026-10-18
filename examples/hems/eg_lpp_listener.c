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
 * @brief EG LPP Listener implementation
 */

#include <inttypes.h>
#include <math.h>
#include <stdio.h>

#include "examples/hems/eg_lpp_listener.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/eg_lp_listener_interface.h"

typedef struct EgLppListener EgLppListener;

struct EgLppListener {
  /** Implements the CS LP Listener Interface */
  EgLpListenerObject obj;

  /* Pointer to the HEMS instance */
  HemsObject* hems;
};

#define EG_LP_LISTENER(obj) ((EgLppListener*)(obj))

static void Destruct(EgLpListenerObject* self);
static void OnRemoteCsAdded(EgLpListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteCsRemoved(EgLpListenerObject* self, const EntityAddressType* entity_addr);
static void OnPowerLimitReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
);
static void OnFailsafePowerLimitReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
);
static void
OnFailsafeDurationReceive(EgLpListenerObject* self, const EntityAddressType* entity_addr, const DurationType* duration);
static void
OnHeartbeatReceive(EgLpListenerObject* self, const EntityAddressType* entity_addr, uint64_t heartbeat_counter);
static void OnPowerNominalMaxReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
);

static const EgLpListenerInterface eg_lpp_listener_methods = {
    .destruct                        = Destruct,
    .on_remote_cs_added              = OnRemoteCsAdded,
    .on_remote_cs_removed            = OnRemoteCsRemoved,
    .on_power_limit_receive          = OnPowerLimitReceive,
    .on_failsafe_power_limit_receive = OnFailsafePowerLimitReceive,
    .on_failsafe_duration_receive    = OnFailsafeDurationReceive,
    .on_heartbeat_receive            = OnHeartbeatReceive,
    .on_power_nominal_max_receive    = OnPowerNominalMaxReceive,
};

static void EgLppListenerConstruct(EgLppListener* self, HemsObject* hems);

void EgLppListenerConstruct(EgLppListener* self, HemsObject* hems) {
  // Override "virtual functions table"
  EG_LP_LISTENER_INTERFACE(self) = &eg_lpp_listener_methods;

  self->hems = hems;
}

EgLpListenerObject* EgLppListenerCreate(HemsObject* hems) {
  EgLppListener* const eg_lpp_listener = (EgLppListener*)EEBUS_MALLOC(sizeof(EgLppListener));
  if (eg_lpp_listener == NULL) {
    return NULL;
  }

  EgLppListenerConstruct(eg_lpp_listener, hems);

  return EG_LP_LISTENER_OBJECT(eg_lpp_listener);
}

void Destruct(EgLpListenerObject* self) {
  UNUSED(self);
  // Nothing to be deallocated yet
}

void OnRemoteCsAdded(EgLpListenerObject* self, const EntityAddressType* entity_addr) {
  EgLppListener* const lpp_listener = EG_LP_LISTENER(self);

  HemsSetEgLppRemoteEntity(lpp_listener->hems, entity_addr);
}

void OnRemoteCsRemoved(EgLpListenerObject* self, const EntityAddressType* entity_addr) {
  EgLppListener* const lpp_listener = EG_LP_LISTENER(self);

  HemsRemoveEgLppRemoteEntity(lpp_listener->hems, entity_addr);
}

void OnPowerLimitReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit,
    const EebusDuration* duration,
    bool is_active
) {
  UNUSED(self);
  UNUSED(entity_addr);

  ScaledValuePrint("EG LPP Power Limit received %sW, ", power_limit);
  EebusDurationPrint("duration = %s, ", duration);
  printf("active = %s\n", is_active ? "true" : "false");
}

void OnFailsafePowerLimitReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
) {
  UNUSED(self);
  UNUSED(entity_addr);

  ScaledValuePrint("EG LPP Failsafe Active Power Limit received:  %sW\n", power_limit);
}

void OnFailsafeDurationReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const DurationType* duration
) {
  UNUSED(self);
  UNUSED(entity_addr);

  EebusDurationPrint("EG LPP Failsafe Duration Minimum received: %s\n", duration);
}

void OnHeartbeatReceive(EgLpListenerObject* self, const EntityAddressType* entity_addr, uint64_t heartbeat_counter) {
  UNUSED(self);
  UNUSED(entity_addr);

  printf("EG LPP Heartbeat received, counter = %" PRIu64 "\n", heartbeat_counter);
}

void OnPowerNominalMaxReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
) {
  UNUSED(self);
  UNUSED(entity_addr);

  ScaledValuePrint("EG LPP Power Nominal Max received: %sW\n", power_limit);
}
