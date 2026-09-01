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
 * @brief Subscription Manager interface declarations
 */

#ifndef SRC_SPINE_API_SUBSCRIPTION_MANAGER_INTERFACE_H_
#define SRC_SPINE_API_SUBSCRIPTION_MANAGER_INTERFACE_H_

#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/common/vector.h"
#include "src/spine/api/device_remote_interface.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/model/feature_types.h"
#include "src/spine/model/subscription_management_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Subscription Manager Interface
 * (Subscription Manager "virtual functions table" declaration)
 */
typedef struct SubscriptionManagerInterface SubscriptionManagerInterface;

/**
 * @brief Subscription Manager Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct SubscriptionManagerObject SubscriptionManagerObject;

/**
 * @brief SubscriptionManager Interface Structure
 */
struct SubscriptionManagerInterface {
    void (*destruct)(SubscriptionManagerObject* self);
    EebusError (*add_subscription)(
        SubscriptionManagerObject* self,
        DeviceRemoteObject* remote_device,
        const SubscriptionManagementRequestCallType* data
    );
    EebusError (*remove_subscription)(
        SubscriptionManagerObject* self,
        DeviceRemoteObject* remote_device,
        const SubscriptionManagementDeleteCallType* data
    );
    void (*remove_device_subscriptions)(SubscriptionManagerObject* self, DeviceRemoteObject* remote_device);
    void (*remove_entity_subscriptions)(SubscriptionManagerObject* self, EntityRemoteObject* remote_entity);
    void (*publish)(const SubscriptionManagerObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd);
    NodeManagementSubscriptionDataType* (*create_subscription_data)(
        const SubscriptionManagerObject* self,
        DeviceRemoteObject* remote_device
    );
};

/**
 * @brief Subscription Manager Object Structure
 */
struct SubscriptionManagerObject {
    const SubscriptionManagerInterface* interface_;
};

/**
 * @brief Subscription Manager pointer typecast
 */
#define SUBSCRIPTION_MANAGER_OBJECT(obj) ((SubscriptionManagerObject*)(obj))

/**
 * @brief Subscription Manager Interface class pointer typecast
 */
#define SUBSCRIPTION_MANAGER_INTERFACE(obj) (SUBSCRIPTION_MANAGER_OBJECT(obj)->interface_)

/**
 * @brief Subscription Manager Destruct caller definition
 */
#define SUBSCRIPTION_MANAGER_DESTRUCT(obj) (SUBSCRIPTION_MANAGER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Subscription Manager Add Subscription caller definition
 */
#define SUBSCRIPTION_MANAGER_ADD_SUBSCRIPTION(obj, remote_device, data) \
    (SUBSCRIPTION_MANAGER_INTERFACE(obj)->add_subscription(obj, remote_device, data))

/**
 * @brief Subscription Manager Remove Subscription caller definition
 */
#define SUBSCRIPTION_MANAGER_REMOVE_SUBSCRIPTION(obj, remote_device, data) \
    (SUBSCRIPTION_MANAGER_INTERFACE(obj)->remove_subscription(obj, remote_device, data))

/**
 * @brief Subscription Manager Remove Device Subscriptions caller definition
 */
#define SUBSCRIPTION_MANAGER_REMOVE_DEVICE_SUBSCRIPTIONS(obj, remote_device) \
    (SUBSCRIPTION_MANAGER_INTERFACE(obj)->remove_device_subscriptions(obj, remote_device))

/**
 * @brief Subscription Manager Remove Entity Subscriptions caller definition
 */
#define SUBSCRIPTION_MANAGER_REMOVE_ENTITY_SUBSCRIPTIONS(obj, remote_entity) \
    (SUBSCRIPTION_MANAGER_INTERFACE(obj)->remove_entity_subscriptions(obj, remote_entity))

/**
 * @brief Subscription Manager Publish caller definition
 */
#define SUBSCRIPTION_MANAGER_PUBLISH(obj, feature_addr, cmd) \
    (SUBSCRIPTION_MANAGER_INTERFACE(obj)->publish(obj, feature_addr, cmd))

/**
 * @brief Subscription Manager Create Subscription Data caller definition
 */
#define SUBSCRIPTION_MANAGER_CREATE_SUBSCRIPTION_DATA(obj, remote_device) \
    (SUBSCRIPTION_MANAGER_INTERFACE(obj)->create_subscription_data(obj, remote_device))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_SUBSCRIPTION_MANAGER_INTERFACE_H_
