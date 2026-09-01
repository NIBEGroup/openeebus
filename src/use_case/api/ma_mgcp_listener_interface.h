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
 * @brief MA MGCP Listener interface declarations
 */

#ifndef SRC_USE_CASE_API_MA_MGCP_LISTENER_INTERFACE_H_
#define SRC_USE_CASE_API_MA_MGCP_LISTENER_INTERFACE_H_

#include "src/spine/model/entity_types.h"
#include "src/use_case/model/mgcp_types.h"
#include "src/use_case/model/scaled_value.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MaMgcpListenerInterface MaMgcpListenerInterface;
typedef struct MaMgcpListenerObject MaMgcpListenerObject;

struct MaMgcpListenerInterface {
    void (*destruct)(MaMgcpListenerObject* self);
    void (*on_remote_entity_connect)(MaMgcpListenerObject* self, const EntityAddressType* entity_addr);
    void (*on_remote_entity_disconnect)(MaMgcpListenerObject* self, const EntityAddressType* entity_addr);
    void (*on_measurement_receive)(
        MaMgcpListenerObject* self,
        GcpMeasurementNameId name_id,
        const ScaledValue* measurement_value,
        const EntityAddressType* remote_entity_addr
    );
    void (*on_pv_curtailment_limit_factor_receive)(
        MaMgcpListenerObject* self,
        const ScaledValue* value,
        const EntityAddressType* remote_entity_addr
    );
};

struct MaMgcpListenerObject {
    const MaMgcpListenerInterface* interface;
};

#define MA_MGCP_LISTENER_OBJECT(obj) ((MaMgcpListenerObject*)(obj))
#define MA_MGCP_LISTENER_INTERFACE(obj) (MA_MGCP_LISTENER_OBJECT(obj)->interface)

#define MA_MGCP_LISTENER_DESTRUCT(obj) (MA_MGCP_LISTENER_INTERFACE(obj)->destruct(obj))

#define MA_MGCP_LISTENER_ON_REMOTE_ENTITY_CONNECT(obj, entity_addr) \
    (MA_MGCP_LISTENER_INTERFACE(obj)->on_remote_entity_connect(obj, entity_addr))

#define MA_MGCP_LISTENER_ON_REMOTE_ENTITY_DISCONNECT(obj, entity_addr) \
    (MA_MGCP_LISTENER_INTERFACE(obj)->on_remote_entity_disconnect(obj, entity_addr))

#define MA_MGCP_LISTENER_ON_MEASUREMENT_RECEIVE(obj, name_id, measurement_value, remote_entity_addr) \
    (MA_MGCP_LISTENER_INTERFACE(obj)->on_measurement_receive(obj, name_id, measurement_value, remote_entity_addr))

#define MA_MGCP_LISTENER_ON_PV_CURTAILMENT_LIMIT_FACTOR_RECEIVE(obj, value, remote_entity_addr) \
    (MA_MGCP_LISTENER_INTERFACE(obj)->on_pv_curtailment_limit_factor_receive(obj, value, remote_entity_addr))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_MA_MGCP_LISTENER_INTERFACE_H_
