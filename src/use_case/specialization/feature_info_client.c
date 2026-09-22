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
 * @brief Feature Info Client implementation
 */

#include "src/use_case/specialization/feature_info_client.h"

EebusError GetLocalAndRemoteFeatures(FeatureInfoClient* self) {
  self->local_feature
      = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(self->local_entity, self->feature_type, self->local_role);
  if (self->local_feature == NULL) {
    self->local_feature
        = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(self->local_entity, kFeatureTypeTypeGeneric, self->local_role);
  }

  if (self->local_feature == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  const DeviceRemoteObject* const remote_device = ENTITY_REMOTE_GET_DEVICE(self->remote_entity);
  const EntityAddressType* const entity_addr    = ENTITY_GET_ADDRESS(ENTITY_OBJECT(self->remote_entity));

  self->remote_feature = DEVICE_REMOTE_GET_FEATURE_WITH_TYPE_AND_ROLE(
      remote_device,
      entity_addr->entity,
      entity_addr->entity_size,
      self->feature_type,
      self->remote_role
  );

  if (self->remote_feature == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  return kEebusErrorOk;
}

EebusError FeatureInfoClientConstruct(
    FeatureInfoClient* self,
    FeatureTypeType feature_type,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
  if ((local_entity == NULL) || (remote_entity == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  self->feature_type  = feature_type;
  self->local_role    = kRoleTypeClient;
  self->remote_role   = kRoleTypeServer;
  self->local_device  = ENTITY_LOCAL_GET_DEVICE(local_entity);
  self->local_entity  = local_entity;
  self->remote_device = ENTITY_REMOTE_GET_DEVICE(remote_entity);
  self->remote_entity = remote_entity;

  return GetLocalAndRemoteFeatures(self);
}

static inline const FeatureAddressType* GetFeatureRemoteAddress(FeatureInfoClient* self) {
  return FEATURE_GET_ADDRESS(FEATURE_OBJECT(self->remote_feature));
}

bool HasSubscription(FeatureInfoClient* self) {
  return FEATURE_LOCAL_HAS_SUBSCRIPTION_TO_REMOTE(self->local_feature, GetFeatureRemoteAddress(self));
}

EebusError Subscribe(FeatureInfoClient* self) {
  return FEATURE_LOCAL_SUBSCRIBE_TO_REMOTE(self->local_feature, GetFeatureRemoteAddress(self));
}

EebusError Unsubscribe(FeatureInfoClient* self) {
  return FEATURE_LOCAL_REMOVE_REMOTE_SUBSCRIPTION(self->local_feature, GetFeatureRemoteAddress(self));
}

bool HasBinding(FeatureInfoClient* self) {
  return FEATURE_LOCAL_HAS_BINDING_TO_REMOTE(self->local_feature, GetFeatureRemoteAddress(self));
}

EebusError Bind(FeatureInfoClient* self) {
  return FEATURE_LOCAL_BIND_TO_REMOTE(self->local_feature, GetFeatureRemoteAddress(self));
}

EebusError Unbind(FeatureInfoClient* self) {
  return FEATURE_LOCAL_REMOVE_REMOTE_BINDING(self->local_feature, GetFeatureRemoteAddress(self));
}
