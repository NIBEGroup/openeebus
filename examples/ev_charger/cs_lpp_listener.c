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
 * @brief CS LPP Listener implementation for EV Charger (V2G production limit — log only)
 */

#include <inttypes.h>
#include <math.h>
#include <stdio.h>

#include "examples/ev_charger/cs_lpp_listener.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/cs_lp_listener_interface.h"
#include "src/use_case/model/scaled_value.h"

typedef struct CsLppListener CsLppListener;

struct CsLppListener {
  CsLpListenerObject obj;
};

#define CS_LPP_LISTENER(obj) ((CsLppListener*)(obj))

static void Destruct(CsLpListenerObject* self);
static void OnRemoteEgAdded(CsLpListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteEgRemoved(CsLpListenerObject* self, const EntityAddressType* entity_addr);
static void OnPowerLimitReceive(
    CsLpListenerObject* self,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
);
static void OnFailsafePowerLimitReceive(CsLpListenerObject* self, const ScaledValue* power_limit);
static void OnFailsafeDurationReceive(CsLpListenerObject* self, const DurationType* duration);
static void OnHeartbeatReceive(CsLpListenerObject* self, uint64_t heartbeat_counter);

static const CsLpListenerInterface cs_lpp_listener_methods = {
    .destruct                        = Destruct,
    .on_remote_eg_added              = OnRemoteEgAdded,
    .on_remote_eg_removed            = OnRemoteEgRemoved,
    .on_power_limit_receive          = OnPowerLimitReceive,
    .on_failsafe_power_limit_receive = OnFailsafePowerLimitReceive,
    .on_failsafe_duration_receive    = OnFailsafeDurationReceive,
    .on_heartbeat_receive            = OnHeartbeatReceive,
};

static EebusError CsLppListenerConstruct(CsLppListener* self) {
  CS_LP_LISTENER_INTERFACE(self) = &cs_lpp_listener_methods;
  return kEebusErrorOk;
}

CsLpListenerObject* CsLppListenerCreate(void) {
  CsLppListener* const listener = (CsLppListener*)EEBUS_MALLOC(sizeof(CsLppListener));
  if (listener == NULL) {
    return NULL;
  }
  if (CsLppListenerConstruct(listener) != kEebusErrorOk) {
    CsLppListenerDelete(CS_LP_LISTENER_OBJECT(listener));
    return NULL;
  }
  return CS_LP_LISTENER_OBJECT(listener);
}

void Destruct(CsLpListenerObject* self) { UNUSED(self); }

void OnRemoteEgAdded(CsLpListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(self);
  UNUSED(entity_addr);
  printf("EV CS LPP: Remote EG added\n");
}

void OnRemoteEgRemoved(CsLpListenerObject* self, const EntityAddressType* entity_addr) {
  UNUSED(self);
  UNUSED(entity_addr);
  printf("EV CS LPP: Remote EG removed\n");
}

void OnPowerLimitReceive(
    CsLpListenerObject* self,
    const ScaledValue* power_limit,
    const EebusDuration* duration,
    bool is_active
) {
  UNUSED(self);
  /* V2G production limit — this basic EV charger does not support V2G */
  ScaledValuePrint("EV CS LPP: Production limit received %sW (V2G not supported), ", power_limit);
  EebusDurationPrint("duration = %s, ", duration);
  printf("active = %s\n", is_active ? "true" : "false");
}

void OnFailsafePowerLimitReceive(CsLpListenerObject* self, const ScaledValue* power_limit) {
  UNUSED(self);
  ScaledValuePrint("EV CS LPP: Failsafe power limit received %sW\n", power_limit);
}

void OnFailsafeDurationReceive(CsLpListenerObject* self, const DurationType* duration) {
  UNUSED(self);
  EebusDurationPrint("EV CS LPP: Failsafe duration received %s\n", duration);
}

void OnHeartbeatReceive(CsLpListenerObject* self, uint64_t heartbeat_counter) {
  UNUSED(self);
  printf("EV CS LPP: Heartbeat received, counter = %" PRIu64 "\n", heartbeat_counter);
}
