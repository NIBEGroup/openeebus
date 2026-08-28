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
 * @brief EG LPC Listener implementation
 */

#include <inttypes.h>
#include <math.h>
#include <stdio.h>

#include "examples/hems/eg_lpc_listener.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/eg_lp_listener_interface.h"

typedef struct EgLpcListener EgLpcListener;

struct EgLpcListener {
    /** Implements the CS LP Listener Interface */
    EgLpListenerObject obj;

    /* Pointer to the HEMS instance */
    HemsObject* hems;
};

#define EG_LP_LISTENER(obj) ((EgLpcListener*)(obj))

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

static const EgLpListenerInterface eg_lpc_listener_methods = {
    .destruct                        = Destruct,
    .on_remote_cs_added              = OnRemoteCsAdded,
    .on_remote_cs_removed            = OnRemoteCsRemoved,
    .on_power_limit_receive          = OnPowerLimitReceive,
    .on_failsafe_power_limit_receive = OnFailsafePowerLimitReceive,
    .on_failsafe_duration_receive    = OnFailsafeDurationReceive,
    .on_heartbeat_receive            = OnHeartbeatReceive,
    .on_power_nominal_max_receive    = OnPowerNominalMaxReceive,
};

static void EgLpcListenerConstruct(EgLpcListener* self, HemsObject* hems);

void EgLpcListenerConstruct(EgLpcListener* self, HemsObject* hems) {
    // Override "virtual functions table"
    EG_LP_LISTENER_INTERFACE(self) = &eg_lpc_listener_methods;

    self->hems = hems;
}

EgLpListenerObject* EgLpcListenerCreate(HemsObject* hems) {
    EgLpcListener* const eg_lpc_listener = (EgLpcListener*)EEBUS_MALLOC(sizeof(EgLpcListener));
    if (eg_lpc_listener == NULL) {
        return NULL;
    }

    EgLpcListenerConstruct(eg_lpc_listener, hems);

    return EG_LP_LISTENER_OBJECT(eg_lpc_listener);
}

void Destruct(EgLpListenerObject* self) {
    UNUSED(self);
    // Nothing to be deallocated yet
}

void OnRemoteCsAdded(EgLpListenerObject* self, const EntityAddressType* entity_addr) {
    EgLpcListener* const lpc_listener = EG_LP_LISTENER(self);

    HemsSetEgLpcRemoteEntity(lpc_listener->hems, entity_addr);
}

void OnRemoteCsRemoved(EgLpListenerObject* self, const EntityAddressType* entity_addr) {
    UNUSED(entity_addr);

    EgLpcListener* const lpc_listener = EG_LP_LISTENER(self);

    // Currently only single remote entity is supported,
    // so just clear the remote entity address
    HemsSetEgLpcRemoteEntity(lpc_listener->hems, NULL);
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

    ScaledValuePrint("EG LPC Power Limit received %sW, ", power_limit);
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

    ScaledValuePrint("EG LPC Failsafe Active Power Limit received:  %sW\n", power_limit);
}

void OnFailsafeDurationReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const DurationType* duration
) {
    UNUSED(self);
    UNUSED(entity_addr);

    EebusDurationPrint("EG LPC Failsafe Duration Minimum received: %s\n", duration);
}

void OnHeartbeatReceive(EgLpListenerObject* self, const EntityAddressType* entity_addr, uint64_t heartbeat_counter) {
    UNUSED(self);
    UNUSED(entity_addr);

    printf("EG LPC Heartbeat received, counter = %" PRIu64 "\n", heartbeat_counter);
}

void OnPowerNominalMaxReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
) {
    UNUSED(self);
    UNUSED(entity_addr);

    ScaledValuePrint("EG LPC Power Nominal Max received: %sW\n", power_limit);
}
