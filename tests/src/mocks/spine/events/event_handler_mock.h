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
 * @brief Event handler mock
 */

#ifndef TESTS_SRC_MOCKS_SPINE_EVENTS_EVENT_HANDLER_MOCK_H_
#define TESTS_SRC_MOCKS_SPINE_EVENTS_EVENT_HANDLER_MOCK_H_

#include "src/spine/api/events_manager_interface.h"
#include "tests/src/mocks/mock_inst.h"

class EventHandlerInterface {
 public:
  virtual ~EventHandlerInterface()                            = default;
  virtual void Handle(const EventPayload* payload, void* ctx) = 0;
};

class EventHandlerMock : public EventHandlerInterface {
 public:
  EventHandlerMock()           = default;
  ~EventHandlerMock() override = default;
  MOCK_METHOD(void, Handle, (const EventPayload* payload, void* ctx), (override));
};

class EventHandlerMockInst : public MockInst<EventHandlerMock> {
 public:
  explicit EventHandlerMockInst(EventsManagerObject* events_manager);
  ~EventHandlerMockInst();

 private:
  EventsManagerObject* events_manager_;
};

#endif  // TESTS_SRC_MOCKS_SPINE_EVENTS_EVENT_HANDLER_MOCK_H_
