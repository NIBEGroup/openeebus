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
 * @brief Heartbeat Manager implementation
 */

#include <stdint.h>
#include <time.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/heartbeat_manager_interface.h"
#include "src/spine/model/absolute_or_relative_time.h"
#include "src/spine/model/device_diagnosis_types.h"
#include "src/spine/model/model.h"

typedef struct HeartbeatManager HeartbeatManager;

struct HeartbeatManager {
    /** Implements the Heartbeat Manager Interface */
    HeartbeatManagerObject obj;

    EntityLocalObject* local_entity;
    FeatureLocalObject* local_feature;
    uint64_t heartbeat_num;
    uint32_t tick_cnt;
    // Period (in ticks/seconds) between two heartbeat transmissions.
    // Note: this is not the remote heartbeat timeout (the deadline by which the
    // remote expects a heartbeat); the application should choose
    // heartbeat period <= remote heartbeat timeout.
    uint32_t heartbeat_period;

    bool running;
};

#define HEARTBEAT_MANAGER(obj) ((HeartbeatManager*)(obj))

static void Destruct(HeartbeatManagerObject* self);
static bool IsHeartbeatRunning(const HeartbeatManagerObject* self);
static void SetLocalFeature(HeartbeatManagerObject* self, EntityLocalObject* entity, FeatureLocalObject* feature);
static void Tick(HeartbeatManagerObject* self);
static EebusError Start(HeartbeatManagerObject* self);
static void Stop(HeartbeatManagerObject* self);

static const HeartbeatManagerInterface heartbeat_manager_methods = {
    .destruct             = Destruct,
    .is_heartbeat_running = IsHeartbeatRunning,
    .set_local_feature    = SetLocalFeature,
    .tick                 = Tick,
    .start                = Start,
    .stop                 = Stop,
};

static void HeartbeatManagerConstruct(HeartbeatManager* self, EntityLocalObject* local_entity, uint32_t period);
static void UpdateHeartbeatData(HeartbeatManager* self);

void HeartbeatManagerConstruct(HeartbeatManager* self, EntityLocalObject* local_entity, uint32_t period) {
    // Override "virtual functions table"
    HEARTBEAT_MANAGER_INTERFACE(self) = &heartbeat_manager_methods;

    self->local_entity     = local_entity;
    self->local_feature    = NULL;
    self->heartbeat_num    = 0;
    self->tick_cnt         = period;
    self->heartbeat_period = period;
    self->running          = false;
}

HeartbeatManagerObject* HeartbeatManagerCreate(EntityLocalObject* local_entity, uint32_t period) {
    HeartbeatManager* const heartbeat_manager = (HeartbeatManager*)EEBUS_MALLOC(sizeof(HeartbeatManager));

    HeartbeatManagerConstruct(heartbeat_manager, local_entity, period);

    return HEARTBEAT_MANAGER_OBJECT(heartbeat_manager);
}

void Destruct(HeartbeatManagerObject* self) {
    Stop(self);
}

bool IsHeartbeatRunning(const HeartbeatManagerObject* self) {
    HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);
    return hm->running;
}

void SetLocalFeature(HeartbeatManagerObject* self, EntityLocalObject* entity, FeatureLocalObject* feature) {
    HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);

    if (entity == NULL || feature == NULL) {
        return;
    }

    FeatureObject* const fr = FEATURE_OBJECT(feature);
    if ((FEATURE_GET_TYPE(fr) != kFeatureTypeTypeDeviceDiagnosis) || (FEATURE_GET_ROLE(fr) != kRoleTypeServer)) {
        return;
    }

    if (FEATURE_GET_FUNCTION_OPERATIONS(fr, kFunctionTypeDeviceDiagnosisHeartbeatData) == NULL) {
        return;
    }

    hm->local_entity  = entity;
    hm->local_feature = feature;

    UpdateHeartbeatData(hm);
    Start(self);
}

void Tick(HeartbeatManagerObject* self) {
    HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);

    if (hm->heartbeat_period == 0) {
        return;
    }

    if ((hm->running) && (hm->tick_cnt > 0)) {
        hm->tick_cnt--;
    }

    if (hm->tick_cnt == 0) {
        hm->heartbeat_num++;
        UpdateHeartbeatData(hm);
        // On period elapse, reset the heartbeat counter
        hm->tick_cnt = hm->heartbeat_period;
    }
}

void UpdateHeartbeatData(HeartbeatManager* self) {
    DeviceDiagnosisHeartbeatDataType heartbeat_data = {
        .timestamp         = &ABSOLUTE_OR_RELATIVE_TIME_NOW,
        .heartbeat_counter = &self->heartbeat_num,
        // The SPINE heartbeatTimeout wire field declares how often we send;
        // it is filled with the configured heartbeat period.
        // TODO: consider a separate parameter for the declared heartbeatTimeout
        // wire value so the send period and the declared timeout can differ
        .heartbeat_timeout = &(DurationType){.seconds = self->heartbeat_period},
    };

    FEATURE_LOCAL_SET_DATA(self->local_feature, kFunctionTypeDeviceDiagnosisHeartbeatData, &heartbeat_data);
}

EebusError Start(HeartbeatManagerObject* self) {
    HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);

    hm->running = true;

    return kEebusErrorOk;
}

void Stop(HeartbeatManagerObject* self) {
    HeartbeatManager* const hm = HEARTBEAT_MANAGER(self);
    if (IsHeartbeatRunning(self)) {
        hm->running = false;
    }
}
