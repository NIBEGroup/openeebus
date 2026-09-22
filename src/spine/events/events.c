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

#include "src/spine/events/events.h"

#include "src/common/eebus_malloc.h"
#include "src/common/vector.h"

typedef struct EventHandlerInfo EventHandlerInfo;

struct EventHandlerInfo {
  EventHandlerLevel level;
  EventHandler handler;
  void* ctx;
};

typedef struct EventsManager EventsManager;

struct EventsManager {
  EventsManagerObject obj;

  Vector handlers;
};

#define EVENTS_MANAGER(obj) ((EventsManager*)(obj))

static void Destruct(EventsManagerObject* self);
static EebusError Subscribe(EventsManagerObject* self, EventHandlerLevel level, EventHandler handler, void* ctx);
static EebusError Unsubscribe(EventsManagerObject* self, EventHandlerLevel level, EventHandler handler, void* ctx);
static void Publish(EventsManagerObject* self, const EventPayload* payload);

static const EventsManagerInterface events_manager_methods = {
    .destruct    = Destruct,
    .subscribe   = Subscribe,
    .unsubscribe = Unsubscribe,
    .publish     = Publish,
};

EventHandlerInfo* EventHandlerInfoCreate(EventHandlerLevel level, EventHandler handler, void* ctx) {
  EventHandlerInfo* info = EEBUS_MALLOC(sizeof(*info));
  if (info == NULL) {
    return NULL;
  }

  info->level   = level;
  info->handler = handler;
  info->ctx     = ctx;
  return info;
}

void EventHandlerInfoDelete(EventHandlerInfo* info) { EEBUS_FREE(info); }

const EventHandlerInfo*
EventHandlerFind(const EventsManager* self, EventHandlerLevel level, EventHandler handler, void* ctx) {
  for (size_t i = 0; i < VectorGetSize(&self->handlers); ++i) {
    EventHandlerInfo* info = VectorGetElement(&self->handlers, i);
    if ((info->level == level) && (info->handler == handler) && (info->ctx == ctx)) {
      return info;
    }
  }

  return NULL;
}

void EventsManagerConstruct(EventsManager* self) {
  EVENTS_MANAGER_INTERFACE(self) = &events_manager_methods;
  VectorConstruct(&self->handlers);
}

EventsManagerObject* EventsManagerCreate(void) {
  EventsManager* const events_manager = EEBUS_MALLOC(sizeof(*events_manager));
  if (events_manager == NULL) {
    return NULL;
  }

  EventsManagerConstruct(events_manager);
  return EVENTS_MANAGER_OBJECT(events_manager);
}

void Destruct(EventsManagerObject* self) {
  EventsManager* const events_manager = EVENTS_MANAGER(self);
  for (size_t i = 0; i < VectorGetSize(&events_manager->handlers); ++i) {
    EventHandlerInfoDelete(VectorGetElement(&events_manager->handlers, i));
  }
  VectorDestruct(&events_manager->handlers);
}

EebusError Subscribe(EventsManagerObject* self, EventHandlerLevel level, EventHandler handler, void* ctx) {
  EventsManager* const events_manager = EVENTS_MANAGER(self);
  if (EventHandlerFind(events_manager, level, handler, ctx) != NULL) {
    return kEebusErrorOk;
  }

  EventHandlerInfo* const new_handler_info = EventHandlerInfoCreate(level, handler, ctx);
  if (new_handler_info == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  VectorPushBack(&events_manager->handlers, new_handler_info);
  return kEebusErrorOk;
}

EebusError Unsubscribe(EventsManagerObject* self, EventHandlerLevel level, EventHandler handler, void* ctx) {
  EventsManager* const events_manager = EVENTS_MANAGER(self);
  const EventHandlerInfo* const info  = EventHandlerFind(events_manager, level, handler, ctx);
  if (info == NULL) {
    return kEebusErrorNoChange;
  }

  VectorRemove(&events_manager->handlers, (void*)info);
  EventHandlerInfoDelete((void*)info);
  if (VectorGetSize(&events_manager->handlers) == 0) {
    VectorClear(&events_manager->handlers);
  }

  return kEebusErrorOk;
}

void Publish(EventsManagerObject* self, const EventPayload* payload) {
  if (payload == NULL) {
    return;
  }

  EventsManager* const events_manager = EVENTS_MANAGER(self);
  for (size_t i = 0; i < VectorGetSize(&events_manager->handlers); ++i) {
    EventHandlerInfo* info = VectorGetElement(&events_manager->handlers, i);
    info->handler(payload, info->ctx);
  }
}
