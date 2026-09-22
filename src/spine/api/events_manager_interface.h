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
 * @brief Events Manager interface declarations
 */

#ifndef SRC_SPINE_API_EVENTS_MANAGER_INTERFACE_H_
#define SRC_SPINE_API_EVENTS_MANAGER_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/spine/api/events.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EventsManagerInterface EventsManagerInterface;
typedef struct EventsManagerObject EventsManagerObject;

struct EventsManagerInterface {
  void (*destruct)(EventsManagerObject* self);
  EebusError (*subscribe)(EventsManagerObject* self, EventHandlerLevel level, EventHandler handler, void* ctx);
  EebusError (*unsubscribe)(EventsManagerObject* self, EventHandlerLevel level, EventHandler handler, void* ctx);
  void (*publish)(EventsManagerObject* self, const EventPayload* payload);
};

struct EventsManagerObject {
  const EventsManagerInterface* interface_;
};

#define EVENTS_MANAGER_OBJECT(obj) ((EventsManagerObject*)(obj))
#define EVENTS_MANAGER_INTERFACE(obj) (EVENTS_MANAGER_OBJECT(obj)->interface_)

#define EVENTS_DESTRUCT(obj) (EVENTS_MANAGER_INTERFACE(obj)->destruct(obj))
#define EVENTS_SUBSCRIBE(obj, level, handler, ctx) (EVENTS_MANAGER_INTERFACE(obj)->subscribe(obj, level, handler, ctx))
#define EVENTS_UNSUBSCRIBE(obj, level, handler, ctx) \
  (EVENTS_MANAGER_INTERFACE(obj)->unsubscribe(obj, level, handler, ctx))
#define EVENTS_PUBLISH(obj, payload) (EVENTS_MANAGER_INTERFACE(obj)->publish(obj, payload))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_EVENTS_MANAGER_INTERFACE_H_
