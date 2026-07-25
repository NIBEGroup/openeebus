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
 * @brief Subscription Manager implementation
 */

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/feature_link.h"
#include "src/spine/api/feature_link_container.h"
#include "src/spine/api/subscription_manager_interface.h"
#include "src/spine/events/events.h"
#include "src/spine/model/node_management_types.h"

typedef struct SubscriptionManager SubscriptionManager;

struct SubscriptionManager {
  /** Implements the Subscription Manager Interface */
  SubscriptionManagerObject obj;

  DeviceLocalObject* local_device;
  uint64_t subscription_num;
  FeatureLinkContainer subscription_entries;
};

#define SUBSCRIPTION_MANAGER(obj) ((SubscriptionManager*)(obj))

static void Destruct(SubscriptionManagerObject* self);
static EebusError AddSubscription(
    SubscriptionManagerObject* self,
    DeviceRemoteObject* remote_device,
    const SubscriptionManagementRequestCallType* data
);
static EebusError RemoveSubscription(
    SubscriptionManagerObject* self,
    DeviceRemoteObject* remote_device,
    const SubscriptionManagementDeleteCallType* data
);
static void RemoveDeviceSubscriptions(SubscriptionManagerObject* self, DeviceRemoteObject* remote_device);
static void RemoveEntitySubscriptions(SubscriptionManagerObject* self, EntityRemoteObject* remote_entity);
static void Publish(const SubscriptionManagerObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd);
static NodeManagementSubscriptionDataType*
CreateSubscriptionData(const SubscriptionManagerObject* self, DeviceRemoteObject* remote_device);

static const SubscriptionManagerInterface subscription_manager_methods = {
    .destruct                    = Destruct,
    .add_subscription            = AddSubscription,
    .remove_subscription         = RemoveSubscription,
    .remove_device_subscriptions = RemoveDeviceSubscriptions,
    .remove_entity_subscriptions = RemoveEntitySubscriptions,
    .publish                     = Publish,
    .create_subscription_data    = CreateSubscriptionData,
};

static void SubscriptionManagerConstruct(SubscriptionManager* self, DeviceLocalObject* local_device);
static uint64_t GetNextSubscriptionId(SubscriptionManager* self);
static SubscriptionManagementEntryDataType* CreateSubscriptionEntryData(const FeatureLink* subscription);

void SubscriptionManagerConstruct(SubscriptionManager* self, DeviceLocalObject* local_device) {
  // Override "virtual functions table"
  SUBSCRIPTION_MANAGER_INTERFACE(self) = &subscription_manager_methods;

  self->local_device     = local_device;
  self->subscription_num = 0;
  FeatureLinkContainerConstruct(&self->subscription_entries);
}

SubscriptionManagerObject* SubscriptionManagerCreate(DeviceLocalObject* local_device) {
  SubscriptionManager* const subscription_manager = (SubscriptionManager*)EEBUS_MALLOC(sizeof(SubscriptionManager));

  SubscriptionManagerConstruct(subscription_manager, local_device);

  return SUBSCRIPTION_MANAGER_OBJECT(subscription_manager);
}

void Destruct(SubscriptionManagerObject* self) {
  SubscriptionManager* const sm = SUBSCRIPTION_MANAGER(self);

  FeatureLinkContainerDestruct(&sm->subscription_entries);
}

uint64_t GetNextSubscriptionId(SubscriptionManager* self) {
  return self->subscription_num++;
}

EebusError AddSubscription(
    SubscriptionManagerObject* self,
    DeviceRemoteObject* remote_device,
    const SubscriptionManagementRequestCallType* data
) {
  SubscriptionManager* const sm = SUBSCRIPTION_MANAGER(self);

  FeatureLocalObject* const server_feature
      = DEVICE_LOCAL_GET_FEATURE_WITH_ADDRESS(sm->local_device, data->server_address);

  FeatureRemoteObject* const client_feature
      = DEVICE_REMOTE_GET_FEATURE_WITH_ADDRESS(remote_device, data->client_address);

  if (data->server_feature_type != NULL) {
    if (!FeatureParametersMatch(FEATURE(server_feature), kRoleTypeServer, *data->server_feature_type)) {
      return kEebusErrorNoChange;
    }

    if (!FeatureParametersMatch(FEATURE(client_feature), kRoleTypeClient, *data->server_feature_type)) {
      return kEebusErrorNoChange;
    }
  }

  if (FeatureLinkContainerFind(&sm->subscription_entries, data->server_address, data->client_address) != NULL) {
    return kEebusErrorNoChange;
  }

  FeatureLinkContainerAdd(&sm->subscription_entries, GetNextSubscriptionId(sm), server_feature, client_feature);

  const EventPayload payload = {
      .ski           = DEVICE_REMOTE_GET_SKI(remote_device),
      .event_type    = kEventTypeSubscriptionChange,
      .change_type   = kElementChangeAdd,
      .device        = remote_device,
      .entity        = FEATURE_REMOTE_GET_ENTITY(client_feature),
      .feature       = client_feature,
      .local_feature = server_feature,
      .function_data = data,
      .function_type = kFunctionTypeSubscriptionManagementRequestCall,
  };

  EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(sm->local_device), &payload);
  return kEebusErrorOk;
}

EebusError RemoveSubscription(
    SubscriptionManagerObject* self,
    DeviceRemoteObject* remote_device,
    const SubscriptionManagementDeleteCallType* data
) {
  SubscriptionManager* const sm = SUBSCRIPTION_MANAGER(self);

  // According to the spec 7.4.4
  // a. The absence of "subscriptionDelete.clientAddress.device" SHALL be treated as if it was
  //    present and set to the sender's "device" address part.
  // b. The absence of "subscriptionDelete.serverAddress.device" SHALL be treated as if it was
  //    present and set to the recipient's "device" address part.

  FeatureAddressType client_addr = *data->client_address;
  if (client_addr.device == NULL) {
    client_addr.device = DEVICE_GET_ADDRESS(DEVICE_OBJECT(remote_device));
  }

  FeatureAddressType server_addr = *data->server_address;
  if (server_addr.device == NULL) {
    server_addr.device = DEVICE_GET_ADDRESS(DEVICE_OBJECT(sm->local_device));
  }

  FeatureRemoteObject* const client_feature = DEVICE_REMOTE_GET_FEATURE_WITH_ADDRESS(remote_device, &client_addr);
  if (client_feature == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  FeatureLocalObject* const server_feature = DEVICE_LOCAL_GET_FEATURE_WITH_ADDRESS(sm->local_device, &server_addr);
  if (server_feature == NULL) {
    return kEebusErrorNoChange;
  }

  FeatureLink* const subscription = FeatureLinkContainerFind(&sm->subscription_entries, &server_addr, &client_addr);
  if (subscription == NULL) {
    return kEebusErrorNoChange;
  }

  FeatureLinkContainerRemove(&sm->subscription_entries, subscription);

  const EventPayload payload = {
      .ski           = DEVICE_REMOTE_GET_SKI(remote_device),
      .event_type    = kEventTypeSubscriptionChange,
      .change_type   = kElementChangeRemove,
      .device        = remote_device,
      .entity        = FEATURE_REMOTE_GET_ENTITY(client_feature),
      .feature       = client_feature,
      .local_feature = server_feature,
      .function_data = data,
      .function_type = kFunctionTypeSubscriptionManagementDeleteCall,
  };

  EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(sm->local_device), &payload);
  return kEebusErrorOk;
}

void RemoveDeviceSubscriptions(SubscriptionManagerObject* self, DeviceRemoteObject* remote_device) {
  if (remote_device == NULL) {
    return;
  }

  const Vector* const entities = DEVICE_REMOTE_GET_ENTITIES(remote_device);
  for (size_t i = 0; i < VectorGetSize(entities); ++i) {
    EntityRemoteObject* const entity = (EntityRemoteObject*)VectorGetElement(entities, i);
    SUBSCRIPTION_MANAGER_REMOVE_ENTITY_SUBSCRIPTIONS(self, entity);
  }
}

void RemoveEntitySubscriptions(SubscriptionManagerObject* self, EntityRemoteObject* remote_entity) {
  SubscriptionManager* const sm = SUBSCRIPTION_MANAGER(self);

  if (remote_entity == NULL) {
    return;
  }

  for (size_t i = 0; i < FeatureLinkContainerGetSize(&sm->subscription_entries);) {
    FeatureLink* const subscription = FeatureLinkContainerGetElement(&sm->subscription_entries, i);
    if (FeatureLinkRemoteEntityMatch(subscription, remote_entity)) {
      const FeatureAddressType* const server_addr = FeatureLinkGetServerAddr(subscription);
      const FeatureAddressType* const client_addr = FeatureLinkGetClientAddr(subscription);

      DeviceRemoteObject* const dr = ENTITY_REMOTE_GET_DEVICE(remote_entity);

      EventPayload payload = {
          .ski           = DEVICE_REMOTE_GET_SKI(dr),
          .event_type    = kEventTypeSubscriptionChange,
          .change_type   = kElementChangeRemove,
          .device        = ENTITY_REMOTE_GET_DEVICE(remote_entity),
          .entity        = remote_entity,
          .feature       = ENTITY_REMOTE_GET_FEATURE_WITH_ID(remote_entity, client_addr->feature),
          .local_feature = DEVICE_LOCAL_GET_FEATURE_WITH_ADDRESS(sm->local_device, server_addr),
      };

      EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(sm->local_device), &payload);
      FeatureLinkContainerRemove(&sm->subscription_entries, subscription);
    } else {
      ++i;
    }
  }
}

Vector* GetFeatureSubscriptions(const SubscriptionManagerObject* self, FeatureAddressType feature_addr) {
  UNUSED(self);
  UNUSED(feature_addr);
  // TODO: Implement method
  return NULL;
}

SubscriptionManagementEntryDataType* CreateSubscriptionEntryData(const FeatureLink* subscription) {
  const FeatureAddressType* const server_addr = FeatureLinkGetServerAddr(subscription);
  const FeatureAddressType* const client_addr = FeatureLinkGetClientAddr(subscription);

  return SubscriptionManagementEntryDataCreate((SubscriptionIdType)subscription->id, server_addr, client_addr);
}

EebusError AddSubscriptionEntries(
    const SubscriptionManager* self,
    NodeManagementSubscriptionDataType* subscription_data,
    const DeviceRemoteObject* device_remote
) {
  const size_t n = FeatureLinkContainerGetRemoteDeviceMatchNum(&self->subscription_entries, device_remote);
  if (n == 0) {
    return kEebusErrorOk;
  }

  const size_t buf_size = sizeof(subscription_data->subscription_entry[0]) * n;

  const SubscriptionManagementEntryDataType** const subscription_entry
      = (const SubscriptionManagementEntryDataType**)EEBUS_MALLOC(buf_size);

  subscription_data->subscription_entry = subscription_entry;

  if (subscription_data->subscription_entry == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  subscription_data->subscription_entry_size = 0;
  for (size_t i = 0; i < FeatureLinkContainerGetSize(&self->subscription_entries); ++i) {
    const FeatureLink* const se = FeatureLinkContainerGetElement(&self->subscription_entries, i);
    if (FeatureLinkRemoteDeviceMatch(se, device_remote)) {
      subscription_entry[i] = CreateSubscriptionEntryData(se);
      if (subscription_entry[i] == NULL) {
        return kEebusErrorMemoryAllocate;
      }

      subscription_data->subscription_entry_size++;
    }
  }

  return kEebusErrorOk;
}

void Publish(const SubscriptionManagerObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd) {
  const SubscriptionManager* const sm = SUBSCRIPTION_MANAGER(self);
  for (size_t i = 0; i < FeatureLinkContainerGetSize(&sm->subscription_entries); ++i) {
    const FeatureLink* const subscription       = FeatureLinkContainerGetElement(&sm->subscription_entries, i);
    const FeatureAddressType* const server_addr = FeatureLinkGetServerAddr(subscription);

    if (FeatureAddressCompare(server_addr, feature_addr)) {
      // TODO: Add error handling
      const FeatureRemoteObject* const client_feature = subscription->client_feature;
      const DeviceRemoteObject* const device_remote   = FEATURE_REMOTE_GET_DEVICE(client_feature);

      SenderObject* const sender = DEVICE_REMOTE_GET_SENDER(device_remote);
      SEND_NOTIFY(sender, server_addr, FeatureLinkGetClientAddr(subscription), cmd);
    }
  }
}

NodeManagementSubscriptionDataType*
CreateSubscriptionData(const SubscriptionManagerObject* self, DeviceRemoteObject* remote_device) {
  NodeManagementSubscriptionDataType* const subscription_data = NodeManagementSubscriptionDataCreateEmpty();
  if (subscription_data == NULL) {
    return NULL;
  }

  const SubscriptionManager* const sm = SUBSCRIPTION_MANAGER(self);

  const EebusError err = AddSubscriptionEntries(sm, subscription_data, remote_device);
  if (err != kEebusErrorOk) {
    NodeManagementSubscriptionDataDelete(subscription_data);
    return NULL;
  }

  return subscription_data;
}
