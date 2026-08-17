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
 * @brief Device Local implementation
 */

#include "device.h"
#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_mutex/eebus_mutex.h"
#include "src/common/eebus_queue/eebus_queue.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/common/num_ptr.h"
#include "src/common/string_lut.h"
#include "src/common/string_util.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/binding/binding_manager.h"
#include "src/spine/device/data_reader.h"
#include "src/spine/device/device_remote.h"
#include "src/spine/device/sender.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/events/events.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/heartbeat/heartbeat_manager.h"
#include "src/spine/model/cmd.h"
#include "src/spine/model/device_classification_types.h"
#include "src/spine/model/node_management_types.h"
#include "src/spine/model/specification_version.h"
#include "src/spine/node_management/node_management.h"
#include "src/spine/subscription/subscription_manager.h"

/** Set DEVICE_LOCAL_DEBUG 1 to enable debug prints */
#ifndef DEVICE_LOCAL_DEBUG
#define DEVICE_LOCAL_DEBUG 0
#endif

/** mDNS debug printf(), enabled whith MDNS_DEBUG = 1 */
#if DEVICE_LOCAL_DEBUG
#define DEVICE_LOCAL_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define DEVICE_LOCAL_DEBUG_PRINTF(fmt, ...)
#endif  // DEVICE_LOCAL_DEBUG

enum DeviceLocalQueueMsgType {
  kDeviceLocalQueueMsgTypeDataReceived,
  kDeviceLocalQueueMsgTypeTimerTick,
  kDeviceLocalQueueMsgTypeCancel,
};

typedef enum DeviceLocalQueueMsgType DeviceLocalQueueMsgType;

typedef struct DeviceLocalQueueMessage DeviceLocalQueueMessage;

struct DeviceLocalQueueMessage {
  DeviceLocalQueueMsgType type;
  MessageBuffer msg_buf;
  DeviceRemoteObject* remote_device;
};

typedef struct DeviceLocal DeviceLocal;

struct DeviceLocal {
  /** Inherit Device */
  Device obj;

  Vector entities;
  EventsManagerObject* events_manager;
  SubscriptionManagerObject* subscription_manager;
  BindingManagerObject* binding_manager;
  NodeManagementObject* node_management;
  StringLut remote_devices;

  bool cancel;
  EebusQueueObject* msg_queue;
  EebusThreadObject* thread;
  EebusTimerObject* timer;
  EebusMutexObject* mutex;
};

#define DEVICE_LOCAL(obj) ((DeviceLocal*)(obj))

static void Destruct(DeviceObject* self);
static EebusError Start(DeviceLocalObject* self);
static void Stop(DeviceLocalObject* self);
static DataReaderObject* SetupRemoteDevice(DeviceLocalObject* self, const char* ski, DataWriterObject* writer);
static void AddRemoteDeviceForSki(DeviceLocalObject* self, const char* ski, DeviceRemoteObject* remote_device);
static EebusError
RequestDeviceRemoteDetailedDiscoveryData(DeviceLocalObject* self, const DeviceRemoteObject* remote_device);
static void RemoveRemoteDeviceConnection(DeviceLocalObject* self, const char* ski);
static void RemoveRemoteDevice(DeviceLocalObject* self, const char* ski);
static DeviceRemoteObject* GetRemoteDeviceWithAddress(const DeviceLocalObject* self, const char* device_addr);
static DeviceRemoteObject* GetRemoteDeviceWithSki(const DeviceLocalObject* self, const char* ski);
static void AddEntity(DeviceLocalObject* self, EntityLocalObject* entity);
static void RemoveEntity(DeviceLocalObject* self, EntityLocalObject* entity);
static EntityLocalObject*
GetEntity(const DeviceLocalObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size);
static EntityLocalObject* GetEntityWithType(const DeviceLocalObject* self, EntityTypeType entity_type);
static const Vector* GetEntities(const DeviceLocalObject* self);
static FeatureLocalObject* GetFeatureWithAddress(const DeviceLocalObject* self, const FeatureAddressType* feature_addr);
static EebusError HandleMessage(DeviceLocalObject* self, MessageBuffer* msg, DeviceRemoteObject* remote_device);
static NodeManagementObject* GetNodeManagement(const DeviceLocalObject* self);
static BindingManagerObject* GetBindingManager(const DeviceLocalObject* self);
static SubscriptionManagerObject* GetSubscriptionManager(const DeviceLocalObject* self);
static EventsManagerObject* GetEventsManager(const DeviceLocalObject* self);
static void
NotifySubscribers(const DeviceLocalObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd);
static NodeManagementDetailedDiscoveryDeviceInformationType* CreateInformation(const DeviceLocalObject* self);
static void Lock(DeviceLocalObject* self);
static void Unlock(DeviceLocalObject* self);

static const DeviceLocalInterface device_local_methods = {
    .device_interface = {
        .destruct                = Destruct,
        .get_address             = DeviceGetAddress,
        .get_device_type         = DeviceGetDeviceType,
        .get_feature_set         = DeviceGetFeatureSet,
        .create_destination_data = DeviceCreateDestinationData,
    },

    .start                                  = Start,
    .stop                                   = Stop,
    .setup_remote_device                    = SetupRemoteDevice,
    .add_remote_device_for_ski              = AddRemoteDeviceForSki,
    .request_remote_detailed_discovery_data = RequestDeviceRemoteDetailedDiscoveryData,
    .remove_remote_device_connection        = RemoveRemoteDeviceConnection,
    .remove_remote_device                   = RemoveRemoteDevice,
    .get_remote_device_with_address         = GetRemoteDeviceWithAddress,
    .get_remote_device_with_ski             = GetRemoteDeviceWithSki,
    .add_entity                             = AddEntity,
    .remove_entity                          = RemoveEntity,
    .get_entity                             = GetEntity,
    .get_entity_with_type                   = GetEntityWithType,
    .get_entities                           = GetEntities,
    .get_feature_with_address               = GetFeatureWithAddress,
    .handle_message                         = HandleMessage,
    .get_node_management                    = GetNodeManagement,
    .get_binding_manager                    = GetBindingManager,
    .get_subscription_manager               = GetSubscriptionManager,
    .get_events_manager                     = GetEventsManager,
    .notify_subscribers                     = NotifySubscribers,
    .create_information                     = CreateInformation,
    .lock                                   = Lock,
    .unlock                                 = Unlock,
};

static void DeviceLocalConstruct(
    DeviceLocal* self,
    const EebusDeviceInfo* device_info,
    const NetworkManagementFeatureSetType* feature_set
);
static void DeviceLocalQueueMsgDeallocator(void* msg);
static void DeviceLocalTick(DeviceLocalObject* self);
static void* DeviceLocalLoop(void* parameters);
static EebusError DeviceLocalTryStart(DeviceLocal* self);
static EebusError RequestDeviceRemoteUseCaseData(DeviceLocalObject* self, const DeviceRemoteObject* remote_device);
static void OnUseCaseDataReadReply(
    const ReplyMessage* reply_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
);
static void OnDetailedDiscoveryReadReply(
    const ReplyMessage* reply_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
);
static void RemoteDeviceDeleter(void* dr);
static void
NotifySubscribersOfEntity(DeviceLocal* self, EntityLocalObject* entity, NetworkManagementStateChangeType state);
static EebusError
ProcessDatagram(DeviceLocalObject* self, const DatagramType* datagram, DeviceRemoteObject* remote_device);

void AddDeviceInformation(DeviceLocal* self, const EebusDeviceInfo* device_info) {
  const EntityTypeType entity_type = kEntityTypeTypeDeviceInformation;
  EntityLocalObject* const entity  = EntityLocalCreate(
      DEVICE_LOCAL_OBJECT(self),
      entity_type,
      (const uint32_t[]){DEVICE_INFORMATION_ENTITY_ID},
      1,
      0
  );

  const uint32_t nm_id  = ENTITY_GET_NEXT_FEATURE_ID(ENTITY_OBJECT(entity));
  self->node_management = NodeManagementCreate(nm_id, entity);
  ENTITY_LOCAL_ADD_FEATURE(entity, FEATURE_LOCAL_OBJECT(self->node_management));

  const uint32_t dc_id = ENTITY_GET_NEXT_FEATURE_ID(ENTITY_OBJECT(entity));
  FeatureLocalObject* const fl
      = FeatureLocalCreate(dc_id, entity, kFeatureTypeTypeDeviceClassification, kRoleTypeServer);

  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceClassificationManufacturerData, true, false);

  DeviceClassificationManufacturerDataType manufacturer_data = {
      .brand_name    = (char*)device_info->brand,
      .vendor_name   = (char*)device_info->brand,
      .device_name   = (char*)device_info->model,
      .device_code   = (char*)device_info->ship_id,
      .serial_number = (char*)device_info->serial_num,
  };

  FEATURE_LOCAL_SET_DATA(fl, kFunctionTypeDeviceClassificationManufacturerData, &manufacturer_data);

  ENTITY_LOCAL_ADD_FEATURE(entity, fl);

  VectorPushBack(&self->entities, entity);
}

void DeviceLocalConstruct(
    DeviceLocal* self,
    const EebusDeviceInfo* device_info,
    const NetworkManagementFeatureSetType* feature_set
) {
  DeviceConstruct(DEVICE(self), device_info->address, device_info->type, feature_set);
  // Override "virtual functions table"
  DEVICE_LOCAL_INTERFACE(self) = &device_local_methods;

  VectorConstruct(&self->entities);
  self->events_manager       = EventsManagerCreate();
  self->subscription_manager = SubscriptionManagerCreate(DEVICE_LOCAL_OBJECT(self));
  self->binding_manager      = BindingManagerCreate(DEVICE_LOCAL_OBJECT(self));
  self->node_management      = NULL;
  StringLutInit(&self->remote_devices);
  self->cancel    = false;
  self->msg_queue = NULL;
  self->thread    = NULL;
  self->timer     = NULL;

  static const size_t kQueueMaxMsg = 15;

  self->msg_queue = EebusQueueCreate(kQueueMaxMsg, sizeof(DeviceLocalQueueMessage), DeviceLocalQueueMsgDeallocator);

  self->mutex = EebusMutexCreateRecursive();

  AddDeviceInformation(self, device_info);
}

DeviceLocalObject*
DeviceLocalCreate(const EebusDeviceInfo* device_info, const NetworkManagementFeatureSetType* feature_set) {
  DeviceLocal* const device_local = (DeviceLocal*)EEBUS_MALLOC(sizeof(DeviceLocal));

  DeviceLocalConstruct(device_local, device_info, feature_set);

  return DEVICE_LOCAL_OBJECT(device_local);
}

void DeviceLocalQueueMsgDeallocator(void* msg) {
  if (msg == NULL) {
    return;
  }

  DeviceLocalQueueMessage* queue_msg = (DeviceLocalQueueMessage*)msg;

  if (queue_msg->type != kDeviceLocalQueueMsgTypeDataReceived) {
    return;
  }

  MessageBufferRelease(&queue_msg->msg_buf);
}

void Destruct(DeviceObject* self) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  EebusQueueDelete(dl->msg_queue);
  dl->msg_queue = NULL;

  StringLutRelease(&dl->remote_devices);

  EebusMutexDelete(dl->mutex);
  dl->mutex = NULL;

  // Node Management instance will be deleted by device info entity
  dl->node_management = NULL;

  BindingManagerDelete(dl->binding_manager);
  dl->binding_manager = NULL;

  SubscriptionManagerDelete(dl->subscription_manager);
  dl->subscription_manager = NULL;

  for (size_t i = 0; i < VectorGetSize(&dl->entities); ++i) {
    EntityLocalObject* const entity = (EntityLocalObject*)VectorGetElement(&dl->entities, i);
    EntityLocalDelete(entity);
  }

  VectorDestruct(&dl->entities);

  EventsManagerDelete(dl->events_manager);
  dl->events_manager = NULL;

  DeviceDestruct(DEVICE_OBJECT(self));
}

EventsManagerObject* GetEventsManager(const DeviceLocalObject* self) {
  return DEVICE_LOCAL(self)->events_manager;
}

void DeviceLocalTick(DeviceLocalObject* self) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  for (size_t i = 0; i < VectorGetSize(&dl->entities); ++i) {
    EntityLocalObject* const entity = (EntityLocalObject*)VectorGetElement(&dl->entities, i);

    ENTITY_LOCAL_TICK(entity);

    HeartbeatManagerObject* hbm = ENTITY_LOCAL_GET_HEARTBEAT_MANAGER(entity);
    if (hbm != NULL) {
      HEARTBEAT_MANAGER_TICK(hbm);
    }
  }
}

EebusError HandleQueueMessage(DeviceLocalObject* self) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  DeviceLocalQueueMessage queue_msg;
  EebusError err = EEBUS_QUEUE_RECEIVE(dl->msg_queue, &queue_msg, kTimeoutInfinite);

  if (err != kEebusErrorOk) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), error receiving the message from queue\n", __func__);
    return err;
  }

  if (queue_msg.type == kDeviceLocalQueueMsgTypeDataReceived) {
    DatagramType* const datagram = DatagramParse((const char*)queue_msg.msg_buf.data);

    EEBUS_MUTEX_LOCK(dl->mutex);
    err = ProcessDatagram(self, datagram, queue_msg.remote_device);
    EEBUS_MUTEX_UNLOCK(dl->mutex);

    DatagramDelete(datagram);
    MessageBufferRelease(&queue_msg.msg_buf);
  } else if (queue_msg.type == kDeviceLocalQueueMsgTypeTimerTick) {
    EEBUS_MUTEX_LOCK(dl->mutex);
    DeviceLocalTick(self);
    err = kEebusErrorOk;
    EEBUS_MUTEX_UNLOCK(dl->mutex);
  } else if (queue_msg.type == kDeviceLocalQueueMsgTypeCancel) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), cancelled\n", __func__);
    err = kEebusErrorOk;
  } else {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), invalid queue message type\n", __func__);
    err = kEebusErrorNotSupported;
  }

  return err;
}

void* DeviceLocalLoop(void* parameters) {
  DeviceLocal* const dl = (DeviceLocal*)parameters;

  while (!dl->cancel) {
    HandleQueueMessage(DEVICE_LOCAL_OBJECT(dl));
  }

  return NULL;
}

void DeviceLocal1sTickCallback(void* ctx) {
  DeviceLocal* const dl = (DeviceLocal*)ctx;

  DeviceLocalQueueMessage queue_msg = {.type = kDeviceLocalQueueMsgTypeTimerTick};
  EEBUS_QUEUE_SEND(dl->msg_queue, &queue_msg, kTimeoutInfinite);
}

EebusError DeviceLocalTryStart(DeviceLocal* self) {
  if (self->msg_queue == NULL) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), initialising write queue failed\n", __func__);
    return kEebusErrorMemory;
  }

  // 10 KB: datagram processing runs use-case event handlers synchronously,
  // which build and serialize reply datagrams via recursive cJSON calls -
  // 4 KB overflows the stack (observed on ESP32 while processing
  // nodeManagementUseCaseData and sending the DeviceDiagnosis subscription)
  self->thread = EebusThreadCreate(DeviceLocalLoop, self, 10 * 1024);
  if (self->thread == NULL) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), start thread failed\n", __func__);
    return kEebusErrorThread;
  }

  // Create timer
  self->timer = EebusTimerCreate(DeviceLocal1sTickCallback, self);
  if (self->timer == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  EEBUS_TIMER_START(self->timer, 1000, true);

  return kEebusErrorOk;
}

EebusError Start(DeviceLocalObject* self) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  EebusError ret = DeviceLocalTryStart(dl);
  if (ret != kEebusErrorOk) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), start SPINE Device Local failed\n", __func__);
    Stop(self);
  }

  return ret;
}

static void Stop(DeviceLocalObject* self) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  if (dl->timer != NULL) {
    EEBUS_TIMER_STOP(dl->timer);
    EebusTimerDelete(dl->timer);
    dl->timer = NULL;
  }

  if (dl->thread != NULL) {
    dl->cancel = true;

    DeviceLocalQueueMessage queue_msg = {.type = kDeviceLocalQueueMsgTypeCancel};
    EEBUS_QUEUE_SEND(dl->msg_queue, &queue_msg, kTimeoutInfinite);

    EEBUS_THREAD_JOIN(dl->thread);
    EebusThreadDelete(dl->thread);
    dl->thread = NULL;
  }

  EEBUS_QUEUE_CLEAR(dl->msg_queue);
}

EebusError RequestDeviceRemoteUseCaseData(DeviceLocalObject* self, const DeviceRemoteObject* remote_device) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  NodeManagementObject* const nm = dl->node_management;

  const char* const addr     = DEVICE_GET_ADDRESS(DEVICE_OBJECT(remote_device));
  const char* const ski      = DEVICE_REMOTE_GET_SKI(remote_device);
  SenderObject* const sender = DEVICE_REMOTE_GET_SENDER(remote_device);
  return RequestUseCaseData(nm, addr, ski, sender, OnUseCaseDataReadReply, self);
}

void OnUseCaseDataReadReply(
    const ReplyMessage* reply_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
) {
  UNUSED(remote_feature_addr);

  DeviceLocalObject* const dl = (DeviceLocalObject*)ctx;

  if (DEVICE_LOCAL(dl)->cancel) {
    return;
  }

  if (err == kEebusErrorOk) {
    return;
  }

  DeviceRemoteObject* const remote_device = DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_SKI(dl, reply_msg->ski);
  if (remote_device == NULL) {
    return;
  }

  RequestDeviceRemoteUseCaseData(dl, remote_device);
}

EebusError RequestDeviceRemoteDetailedDiscoveryData(DeviceLocalObject* self, const DeviceRemoteObject* remote_device) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  NodeManagementObject* const nm = dl->node_management;

  const char* const addr     = DEVICE_GET_ADDRESS(DEVICE_OBJECT(remote_device));
  const char* const ski      = DEVICE_REMOTE_GET_SKI(remote_device);
  SenderObject* const sender = DEVICE_REMOTE_GET_SENDER(remote_device);
  return RequestDetailedDiscovery(nm, addr, ski, sender, OnDetailedDiscoveryReadReply, self);
}

void OnDetailedDiscoveryReadReply(
    const ReplyMessage* reply_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
) {
  UNUSED(remote_feature_addr);

  DeviceLocalObject* const dlo = (DeviceLocalObject*)ctx;

  DeviceLocal* const dl = DEVICE_LOCAL(dlo);

  if (dl->cancel) {
    return;
  }

  DeviceRemoteObject* const dr = DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_SKI(dlo, reply_msg->ski);
  if (dr == NULL) {
    return;
  }

  if ((err != kEebusErrorOk) || (reply_msg->function_data == NULL)) {
    RequestDeviceRemoteDetailedDiscoveryData(dlo, dr);
    return;
  }

  FeatureAddressType addr = *FEATURE_GET_ADDRESS(FEATURE_OBJECT(reply_msg->feature_remote));
  if (addr.device == NULL) {
    addr.device = DEVICE_GET_ADDRESS(DEVICE_OBJECT(dr));
  }

  FEATURE_LOCAL_SUBSCRIBE_TO_REMOTE(FEATURE_LOCAL_OBJECT(dl->node_management), &addr);

  RequestDeviceRemoteUseCaseData(dlo, dr);
}

DataReaderObject* SetupRemoteDevice(DeviceLocalObject* self, const char* ski, DataWriterObject* writer) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  SenderObject* const sender = SenderCreate(writer);
  DeviceRemoteObject* dr     = DeviceRemoteCreate(DEVICE_LOCAL_OBJECT(dl), ski, sender);
  EEBUS_MUTEX_LOCK(dl->mutex);
  AddRemoteDeviceForSki(self, ski, dr);

  // Request Detailed Discovery Data
  RequestDeviceRemoteDetailedDiscoveryData(self, dr);

  // TODO: Add error handling
  // If the request returned an error, it should be retried until it does not
  EEBUS_MUTEX_UNLOCK(dl->mutex);
  return DEVICE_REMOTE_GET_DATA_READER(dr);
}

void RemoteDeviceDeleter(void* dr) {
  DeviceRemoteDelete((DeviceRemoteObject*)dr);
}

void AddRemoteDeviceForSki(DeviceLocalObject* self, const char* ski, DeviceRemoteObject* remote_device) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);
  StringLutInsert(&dl->remote_devices, ski, remote_device, RemoteDeviceDeleter);
}

void RemoveRemoteDeviceConnection(DeviceLocalObject* self, const char* ski) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);
  EEBUS_MUTEX_LOCK(dl->mutex);
  DeviceRemoteObject* const remote_device = DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_SKI(self, ski);

  // We get the events for any disconnection, even for cases where SHIP
  // closed a connection and therefor it never reached SPINE
  if (remote_device == NULL) {
    EEBUS_MUTEX_UNLOCK(dl->mutex);
    return;
  }

  DEVICE_LOCAL_REMOVE_REMOTE_DEVICE(self, ski);

  // inform about the disconnection
  const EventPayload payload = {
      .ski         = ski,
      .event_type  = kEventTypeDeviceChange,
      .change_type = kElementChangeRemove,
      .device      = remote_device,
  };

  EVENTS_PUBLISH(dl->events_manager, &payload);
  EEBUS_MUTEX_UNLOCK(dl->mutex);
}

void RemoveRemoteDevice(DeviceLocalObject* self, const char* ski) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  DeviceRemoteObject* const remote_device = DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_SKI(self, ski);
  if (remote_device == NULL) {
    return;
  }

  // Remove all subscriptions for this device
  SubscriptionManagerObject* const sm = DEVICE_LOCAL_GET_SUBSCRIPTION_MANAGER(self);
  SUBSCRIPTION_MANAGER_REMOVE_DEVICE_SUBSCRIPTIONS(sm, remote_device);

  // Remove all bindings for this device
  BindingManagerObject* bm = DEVICE_LOCAL_GET_BINDING_MANAGER(self);
  BINDING_MANAGER_REMOVE_DEVICE_BINDINGS(bm, remote_device);

  const DeviceAddressType remote_device_addr = {
      .device = (char*)DEVICE_GET_ADDRESS(DEVICE_OBJECT(remote_device)),
  };

  // Remove all data caches for this device
  for (size_t i = 0; i < VectorGetSize(&dl->entities); ++i) {
    EntityLocalObject* const entity = VectorGetElement(&dl->entities, i);
    const Vector* const features    = ENTITY_LOCAL_GET_FEATURES(entity);
    for (size_t j = 0; j < VectorGetSize(features); ++j) {
      FeatureLocalObject* const fl = VectorGetElement(features, j);
      // TODO: Add feature cache cleaning:
      // feature.CleanWriteApprovalCaches(ski);
      FEATURE_LOCAL_CLEAN_REMOTE_DEVICE_CACHES(fl, &remote_device_addr, ski);
    }
  }

  StringLutRemove(&dl->remote_devices, ski);
}

DeviceRemoteObject* GetRemoteDeviceWithAddress(const DeviceLocalObject* self, const char* device_addr) {
  const DeviceLocal* const dl = DEVICE_LOCAL(self);

  if (device_addr == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < StringLutGetSize(&dl->remote_devices); ++i) {
    DeviceRemoteObject* const dr = (DeviceRemoteObject*)StringLutGetElementValue(&dl->remote_devices, i);

    const char* const addr = DEVICE_GET_ADDRESS(DEVICE_OBJECT(dr));
    if ((addr != NULL) && (strcmp(addr, device_addr) == 0)) {
      return dr;
    }
  }

  return NULL;
}

DeviceRemoteObject* GetRemoteDeviceWithSki(const DeviceLocalObject* self, const char* ski) {
  const DeviceLocal* const dl = DEVICE_LOCAL(self);
  return StringLutFind(&dl->remote_devices, ski);
}

EebusError NodeManagementDetailedDiscoveryDataAddEntityInfo(
    NodeManagementDetailedDiscoveryDataType* discovery_data,
    EntityLocalObject* entity,
    NetworkManagementStateChangeType state
) {
  const NodeManagementDetailedDiscoveryEntityInformationType** const entity_information
      = (const NodeManagementDetailedDiscoveryEntityInformationType**)EEBUS_MALLOC(
          sizeof(discovery_data->entity_information[0])
      );

  discovery_data->entity_information = entity_information;
  if (discovery_data->entity_information == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  discovery_data->entity_information_size = 0;

  entity_information[0] = ENTITY_LOCAL_CREATE_INFORMATION(entity);
  if (entity_information[0] == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  discovery_data->entity_information_size++;

  NetworkManagementEntityDescriptionDataType* const entity_descritpion
      = (NetworkManagementEntityDescriptionDataType*)entity_information[0]->description;

  if (entity_descritpion == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  entity_descritpion->last_state_change = Int32Create(state);
  if (entity_descritpion->last_state_change == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EebusError NodeManagementDetailedDiscoveryDataAddFeatureInfo(
    NodeManagementDetailedDiscoveryDataType* discovery_data,
    EntityLocalObject* entity
) {
  const Vector* const features = ENTITY_LOCAL_GET_FEATURES(entity);
  const size_t features_num    = VectorGetSize(features);

  discovery_data->feature_information_size = 0;
  const NodeManagementDetailedDiscoveryFeatureInformationType** const feature_information
      = (const NodeManagementDetailedDiscoveryFeatureInformationType**)EEBUS_MALLOC(
          sizeof(discovery_data->feature_information[0]) * features_num
      );

  discovery_data->feature_information = feature_information;

  if (discovery_data->feature_information == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  for (size_t i = 0; i < features_num; ++i) {
    const FeatureLocalObject* const fl = (const FeatureLocalObject*)VectorGetElement(features, i);
    feature_information[i]             = FEATURE_LOCAL_CREATE_INFORMATION(fl);
    if (feature_information[i] == NULL) {
      return kEebusErrorMemoryAllocate;
    }

    discovery_data->feature_information_size++;
  }

  return kEebusErrorOk;
}

EebusError CmdAddEntityDetailedDiscoveryData(
    CmdType* cmd,
    const DeviceLocalObject* self,
    EntityLocalObject* entity,
    NetworkManagementStateChangeType state
) {
  cmd->function = Int32Create(kFunctionTypeNodeManagementDetailedDiscoveryData);
  if (CmdAddFilterPartialEmpty(cmd) != kEebusErrorOk) {
    return kEebusErrorMemoryAllocate;
  }

  NodeManagementDetailedDiscoveryDataType* const discovery_data
      = NodeManagementDetailedDiscoveryDataCreate((const SpecificationVersionType*)&specification_version, 1);
  if (discovery_data == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  cmd->data_choice         = discovery_data;
  cmd->data_choice_type_id = kFunctionTypeNodeManagementDetailedDiscoveryData;

  discovery_data->device_information = DEVICE_LOCAL_CREATE_INFORMATION(DEVICE_LOCAL_OBJECT(self));
  if (discovery_data->device_information == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  if (NodeManagementDetailedDiscoveryDataAddEntityInfo(discovery_data, entity, state) != kEebusErrorOk) {
    return kEebusErrorMemoryAllocate;
  }

  if (state == kNetworkManagementStateChangeTypeAdded) {
    if (NodeManagementDetailedDiscoveryDataAddFeatureInfo(discovery_data, entity) != kEebusErrorOk) {
      return kEebusErrorMemoryAllocate;
    }
  }

  return kEebusErrorOk;
}

void NotifySubscribersOfEntity(DeviceLocal* self, EntityLocalObject* entity, NetworkManagementStateChangeType state) {
  CmdType* const cmd = CmdCreateEmpty();
  if (cmd == NULL) {
    return;
  }

  const DeviceLocalObject* const dlo = DEVICE_LOCAL_OBJECT(self);
  if (CmdAddEntityDetailedDiscoveryData(cmd, dlo, entity, state) != kEebusErrorOk) {
    CmdDelete(cmd);
    return;
  }

  const NodeManagementObject* const nm = DEVICE_LOCAL_GET_NODE_MANAGEMENT(dlo);
  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(nm));
  DEVICE_LOCAL_NOTIFY_SUBSCRIBERS(DEVICE_LOCAL_OBJECT(self), addr, cmd);
  CmdDelete(cmd);
}

void AddEntity(DeviceLocalObject* self, EntityLocalObject* entity) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);
  VectorPushBack(&dl->entities, entity);
  NotifySubscribersOfEntity(dl, entity, kNetworkManagementStateChangeTypeAdded);
}

void RemoveEntity(DeviceLocalObject* self, EntityLocalObject* entity) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  ENTITY_LOCAL_REMOVE_ALL_USE_CASE_SUPPORTS(entity);
  ENTITY_LOCAL_REMOVE_ALL_SUBSCRIPTIONS(entity);
  ENTITY_LOCAL_REMOVE_ALL_BINDINGS(entity);

  HeartbeatManagerObject* const hm = ENTITY_LOCAL_GET_HEARTBEAT_MANAGER(entity);
  if (hm != NULL) {
    HEARTBEAT_MANAGER_STOP(hm);
  }

  NotifySubscribersOfEntity(dl, entity, kNetworkManagementStateChangeTypeRemoved);
  VectorRemove(&dl->entities, entity);
  EntityLocalDelete(entity);
}

EntityLocalObject* GetEntity(const DeviceLocalObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size) {
  const DeviceLocal* const dl = DEVICE_LOCAL(self);

  if ((entity_ids == NULL) || (entity_ids_size == 0)) {
    return NULL;
  }

  for (size_t i = 0; i < VectorGetSize(&dl->entities); ++i) {
    EntityLocalObject* const entity = (EntityLocalObject*)VectorGetElement(&dl->entities, i);
    if (EntityAddressMatchIds(ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity)), entity_ids, entity_ids_size)) {
      return entity;
    }
  }

  return NULL;
}

EntityLocalObject* GetEntityWithType(const DeviceLocalObject* self, EntityTypeType entity_type) {
  const DeviceLocal* const dl = DEVICE_LOCAL(self);

  for (size_t i = 0; i < VectorGetSize(&dl->entities); ++i) {
    EntityLocalObject* const entity = (EntityLocalObject*)VectorGetElement(&dl->entities, i);

    const EntityTypeType cur_entity_type = ENTITY_GET_TYPE(ENTITY_OBJECT(entity));
    if (cur_entity_type == entity_type) {
      return entity;
    }
  }

  return NULL;
}

const Vector* GetEntities(const DeviceLocalObject* self) {
  return &DEVICE_LOCAL(self)->entities;
}

FeatureLocalObject* GetFeatureWithAddress(const DeviceLocalObject* self, const FeatureAddressType* feature_addr) {
  const Device* const device = DEVICE(self);

  if (device->address == NULL) {
    EEBUS_ASSERT_ALWAYS();
    return NULL;
  }

  if (feature_addr == NULL) {
    return NULL;
  }

  if (feature_addr->device != NULL) {
    if (strcmp(feature_addr->device, device->address) != 0) {
      return NULL;
    }
  }

  const EntityLocalObject* const entity
      = DEVICE_LOCAL_GET_ENTITY(self, feature_addr->entity, feature_addr->entity_size);

  if (entity == NULL) {
    return NULL;
  }

  return ENTITY_LOCAL_GET_FEATURE_WITH_ID(entity, feature_addr->feature);
}

EebusError ProcessCmd(
    DeviceLocalObject* self,
    const DatagramType* datagram,
    const CmdType* cmd,
    DeviceRemoteObject* remote_device
) {
  const HeaderType* header = datagram->header;

  const FeatureAddressType* const dest_addr = header->dest_addr;
  FeatureLocalObject* const local_feature   = DEVICE_LOCAL_GET_FEATURE_WITH_ADDRESS(self, dest_addr);
  if (local_feature == NULL) {
    return kEebusErrorInputArgumentOutOfRange;
  }

  const FeatureAddressType* src_addr = header->src_addr;

  EntityRemoteObject* const remote_entity
      = DEVICE_REMOTE_GET_ENTITY(remote_device, src_addr->entity, src_addr->entity_size);

  FeatureRemoteObject* const remote_feature = DEVICE_REMOTE_GET_FEATURE_WITH_ADDRESS(remote_device, src_addr);
  if (remote_feature == NULL) {
    return kEebusErrorNoChange;
  }

  if (header->cmd_classifier == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  Message message = {
      .request_header = header,
      .cmd_classifier = *header->cmd_classifier,
      .cmd            = cmd,
      .filter_partial = CmdGetFilterPartial(cmd),
      .filter_delete  = CmdGetFilterDelete(cmd),
      .feature_remote = remote_feature,
      .entity_remote  = remote_entity,
      .device_remote  = remote_device,
  };

  // Check if this is a write with an existing binding and if write is allowed on this feature
  if (message.cmd_classifier == kCommandClassifierTypeWrite) {
    // TODO: Check function operations

    const BindingManagerObject* const bm          = DEVICE_LOCAL_GET_BINDING_MANAGER(self);
    const FeatureAddressType* local_feature_addr  = FEATURE_GET_ADDRESS(FEATURE_OBJECT(local_feature));
    const FeatureAddressType* remote_feature_addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(remote_feature));
    if (!BINDING_MANAGER_HAS_BINDING(bm, local_feature_addr, remote_feature_addr)) {
      return kEebusErrorNoChange;
    }
  }

  const EebusError err = FEATURE_LOCAL_HANDLE_MESSAGE(local_feature, &message);
  if (err != kEebusErrorOk) {
    // TODO: Add error handling
    return err;
  }

  return kEebusErrorOk;
}

EebusError ProcessDatagram(DeviceLocalObject* self, const DatagramType* datagram, DeviceRemoteObject* remote_device) {
  if (datagram == NULL) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), datagram is NULL\n", __func__);
    return kEebusErrorInputArgumentNull;
  }

  if (datagram->header == NULL) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), datagram has no header\n", __func__);
    return kEebusErrorInputArgumentNull;
  }

  if (datagram->header->cmd_classifier == NULL) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), datagram has no cmd_classifier\n", __func__);
    return kEebusErrorInputArgumentNull;
  }

  if ((datagram->payload == NULL) || (datagram->payload->cmd == NULL) || (datagram->payload->cmd_size == 0)) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), datagram has no payload\n", __func__);
    return kEebusErrorInputArgumentNull;
  }

  if (remote_device == NULL) {
    DEVICE_LOCAL_DEBUG_PRINTF("%s(), remote device is NULL\n", __func__);
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;
  for (size_t i = 0; i < datagram->payload->cmd_size; ++i) {
    if (datagram->payload->cmd[i] == NULL) {
      DEVICE_LOCAL_DEBUG_PRINTF("%s(), command is NULL\n", __func__);
      return kEebusErrorInputArgumentNull;
    }

    err = ProcessCmd(self, datagram, datagram->payload->cmd[i], remote_device);
    if (err != kEebusErrorOk) {
      DEVICE_LOCAL_DEBUG_PRINTF("%s(), error processing command: %d\n", __func__, err);
      break;
    }
  }

  if (err == kEebusErrorOk) {
    const bool* const ack_request        = datagram->header->ack_request;
    CommandClassifierType cmd_classifier = *datagram->header->cmd_classifier;

    const bool is_ack_cmd_classifier = (cmd_classifier == kCommandClassifierTypeCall)
                                       || (cmd_classifier == kCommandClassifierTypeReply)
                                       || (cmd_classifier == kCommandClassifierTypeNotify);

    if ((ack_request != NULL) && (*ack_request) && (is_ack_cmd_classifier)) {
      // Return success as defined in SPINE chapter 5.2.4
      SenderObject* const sender = DEVICE_REMOTE_GET_SENDER(remote_device);
      SEND_RESULT_SUCCESS(sender, datagram->header, datagram->header->dest_addr);
    }
  }

  return err;
}

EebusError HandleMessage(DeviceLocalObject* self, MessageBuffer* msg, DeviceRemoteObject* remote_device) {
  DeviceLocal* const dl = DEVICE_LOCAL(self);

  DeviceLocalQueueMessage queue_msg = {
      .type          = kDeviceLocalQueueMsgTypeDataReceived,
      .remote_device = remote_device,
  };

  MessageBufferInit(&queue_msg.msg_buf, NULL, 0);
  MessageBufferMove(msg, &queue_msg.msg_buf);

  return EEBUS_QUEUE_SEND(dl->msg_queue, &queue_msg, kTimeoutInfinite);
}

NodeManagementObject* GetNodeManagement(const DeviceLocalObject* self) {
  return DEVICE_LOCAL(self)->node_management;
}

BindingManagerObject* GetBindingManager(const DeviceLocalObject* self) {
  return DEVICE_LOCAL(self)->binding_manager;
}

SubscriptionManagerObject* GetSubscriptionManager(const DeviceLocalObject* self) {
  return DEVICE_LOCAL(self)->subscription_manager;
}

void NotifySubscribers(const DeviceLocalObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd) {
  const DeviceLocal* dl = DEVICE_LOCAL(self);
  SUBSCRIPTION_MANAGER_PUBLISH(dl->subscription_manager, feature_addr, cmd);
}

NodeManagementDetailedDiscoveryDeviceInformationType* CreateInformation(const DeviceLocalObject* self) {
  const DeviceObject* d = DEVICE_OBJECT(self);

  return NodeManagementDetailedDiscoveryDeviceInformationCreate(
      DEVICE_GET_ADDRESS(d),
      DEVICE_GET_DEVICE_TYPE(d),
      DEVICE_GET_FEATURE_SET(d)
  );
}

void Lock(DeviceLocalObject* self) {
  EEBUS_MUTEX_LOCK(DEVICE_LOCAL(self)->mutex);
}

void Unlock(DeviceLocalObject* self) {
  EEBUS_MUTEX_UNLOCK(DEVICE_LOCAL(self)->mutex);
}
