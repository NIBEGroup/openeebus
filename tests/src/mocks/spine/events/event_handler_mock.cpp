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
 * @brief Event handler mock implementation
 */

#include "src/spine/events/events.h"

#include "tests/src/mocks/spine/events/event_handler_mock.h"

static std::unique_ptr<EventHandlerMock> event_handler_mock{};

static void TestEventHandler(const EventPayload* payload, void* ctx);

void TestEventHandler(const EventPayload* payload, void* ctx) {
  if (event_handler_mock != nullptr) {
    event_handler_mock->Handle(payload, ctx);
  }
}

EventHandlerMockInst::EventHandlerMockInst(EventsManagerObject* events_manager)
    : MockInst(event_handler_mock), events_manager_(events_manager) {
  EVENTS_SUBSCRIBE(events_manager_, kEventHandlerLevelApplication, TestEventHandler, nullptr);
}

EventHandlerMockInst::~EventHandlerMockInst() {
  EVENTS_UNSUBSCRIBE(events_manager_, kEventHandlerLevelApplication, TestEventHandler, nullptr);
}
