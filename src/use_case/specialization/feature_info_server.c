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
 * @brief Feature Info Server implementation
 */

#include "src/use_case/specialization/feature_info_server.h"

#include "src/common/eebus_errors.h"

EebusError
FeatureInfoServerConstruct(FeatureInfoServer* self, FeatureTypeType feature_type, EntityLocalObject* local_entity) {
    if (local_entity == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    self->feature_type = feature_type;
    self->local_role   = kRoleTypeServer;
    self->local_device = ENTITY_LOCAL_GET_DEVICE(local_entity);
    self->local_entity = local_entity;

    self->local_feature
        = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(self->local_entity, self->feature_type, self->local_role);
    if (self->local_feature == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    return kEebusErrorOk;
}
