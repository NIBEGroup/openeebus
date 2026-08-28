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
 * @brief Use Case implementation declarations
 */

#ifndef SRC_USE_CASE_USE_CASE_H_
#define SRC_USE_CASE_USE_CASE_H_

#include <stdbool.h>
#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/events/events.h"
#include "src/use_case/api/use_case_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct UseCaseScenario UseCaseScenario;
struct UseCaseScenario {
    UseCaseScenarioSupportType scenario;     // The scenario number
    bool mandatory;                          // If this scenario is mandatory to be supported by the remote entity
    const FeatureTypeType* server_features;  // The server features required for this scenario on the remote entity
    size_t server_features_size;
};

typedef struct UseCaseInfo UseCaseInfo;

struct UseCaseInfo {
    const UseCaseActorType* valid_actor_types;
    size_t valid_actor_types_size;
    const EntityTypeType* valid_entity_types;
    size_t valid_entity_types_size;
    const UseCaseScenario* use_case_scenarios;
    size_t use_case_scenarios_size;

    UseCaseActorType actor;
    UseCaseNameType use_case_name_id;
    SpecificationVersionType version;
    const char* sub_revision;
    bool available;
};

typedef struct UseCase UseCase;

struct UseCase {
    /** Implements the Use Case Interface */
    UseCaseObject obj;

    const UseCaseInfo* info;
    DeviceLocalObject* local_device;
    EntityLocalObject* local_entity;
    EventHandler event_handler;
};

#define USE_CASE(obj) ((UseCase*)(obj))

void UseCaseConstruct(
    UseCase* self,
    const UseCaseInfo* info,
    EntityLocalObject* local_entity,
    EventHandler event_handler
);
void UseCaseDestruct(UseCaseObject* self);

static inline void UseCaseDelete(UseCaseObject* use_case) {
    if (use_case != NULL) {
        USE_CASE_DESTRUCT(USE_CASE_OBJECT(use_case));
        EEBUS_FREE(use_case);
    }
}

bool UseCaseIsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity);
bool UseCaseIsUseCaseCompatible(const UseCaseObject* self, const UseCaseFilterType* use_case_filter);
EntityRemoteObject*
UseCaseGetRemoteEntityWithAddress(const UseCaseObject* self, const EntityAddressType* remote_entity_addr);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_USE_CASE_H_
