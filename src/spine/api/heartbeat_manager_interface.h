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
 * @brief Heartbeat Manager interface declarations
 */

#ifndef SRC_SPINE_API_HEARTBEAT_MANAGER_INTERFACE_H_
#define SRC_SPINE_API_HEARTBEAT_MANAGER_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Heartbeat Manager Interface
 * (Heartbeat Manager "virtual functions table" declaration)
 */
typedef struct HeartbeatManagerInterface HeartbeatManagerInterface;

/**
 * @brief Heartbeat Manager Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct HeartbeatManagerObject HeartbeatManagerObject;

/**
 * @brief HeartbeatManager Interface Structure
 */
struct HeartbeatManagerInterface {
    void (*destruct)(HeartbeatManagerObject* self);
    bool (*is_heartbeat_running)(const HeartbeatManagerObject* self);
    void (*set_local_feature)(HeartbeatManagerObject* self, EntityLocalObject* entity, FeatureLocalObject* feature);
    void (*tick)(HeartbeatManagerObject* self);
    EebusError (*start)(HeartbeatManagerObject* self);
    void (*stop)(HeartbeatManagerObject* self);
};

/**
 * @brief Heartbeat Manager Object Structure
 */
struct HeartbeatManagerObject {
    const HeartbeatManagerInterface* interface_;
};

/**
 * @brief Heartbeat Manager pointer typecast
 */
#define HEARTBEAT_MANAGER_OBJECT(obj) ((HeartbeatManagerObject*)(obj))

/**
 * @brief Heartbeat Manager Interface class pointer typecast
 */
#define HEARTBEAT_MANAGER_INTERFACE(obj) (HEARTBEAT_MANAGER_OBJECT(obj)->interface_)

/**
 * @brief Heartbeat Manager Destruct caller definition
 */
#define HEARTBEAT_MANAGER_DESTRUCT(obj) (HEARTBEAT_MANAGER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Heartbeat Manager Is Heartbeat Running caller definition
 */
#define HEARTBEAT_MANAGER_IS_HEARTBEAT_RUNNING(obj) (HEARTBEAT_MANAGER_INTERFACE(obj)->is_heartbeat_running(obj))

/**
 * @brief Heartbeat Manager Set Local Feature caller definition
 */
#define HEARTBEAT_MANAGER_SET_LOCAL_FEATURE(obj, entity, feature) \
    (HEARTBEAT_MANAGER_INTERFACE(obj)->set_local_feature(obj, entity, feature))

/**
 * @brief Heartbeat Manager Tick caller definition
 */
#define HEARTBEAT_MANAGER_TICK(obj) (HEARTBEAT_MANAGER_INTERFACE(obj)->tick(obj))

/**
 * @brief Heartbeat Manager Start caller definition
 */
#define HEARTBEAT_MANAGER_START(obj) (HEARTBEAT_MANAGER_INTERFACE(obj)->start(obj))

/**
 * @brief Heartbeat Manager Stop caller definition
 */
#define HEARTBEAT_MANAGER_STOP(obj) (HEARTBEAT_MANAGER_INTERFACE(obj)->stop(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_HEARTBEAT_MANAGER_INTERFACE_H_
