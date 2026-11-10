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

#include <memory>

#include <gtest/gtest.h>

#include "src/spine/events/events.h"

struct EventHandlerContext {
  size_t call_count;
  const EventPayload* payload;
};

static void CountEvent(const EventPayload* payload, void* ctx) {
  EventHandlerContext* const handler_ctx = static_cast<EventHandlerContext*>(ctx);
  ++handler_ctx->call_count;
  handler_ctx->payload = payload;
}

TEST(EventsManagerTests, PublishOnlyNotifiesOwnSubscribers) {
  std::unique_ptr<EventsManagerObject, decltype(&EventsManagerDelete)> first{
      EventsManagerCreate(),
      EventsManagerDelete
  };
  std::unique_ptr<EventsManagerObject, decltype(&EventsManagerDelete)> second{
      EventsManagerCreate(),
      EventsManagerDelete
  };

  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);

  EventHandlerContext first_ctx{};
  EventHandlerContext second_ctx{};
  ASSERT_EQ(EVENTS_SUBSCRIBE(first.get(), kEventHandlerLevelApplication, CountEvent, &first_ctx), kEebusErrorOk);
  ASSERT_EQ(EVENTS_SUBSCRIBE(second.get(), kEventHandlerLevelApplication, CountEvent, &second_ctx), kEebusErrorOk);

  const EventPayload first_payload = {
      .ski         = "first",
      .event_type  = kEventTypeDeviceChange,
      .change_type = kElementChangeAdd,
  };
  EVENTS_PUBLISH(first.get(), &first_payload);

  EXPECT_EQ(first_ctx.call_count, 1);
  EXPECT_EQ(first_ctx.payload, &first_payload);
  EXPECT_EQ(second_ctx.call_count, 0);
  EXPECT_EQ(second_ctx.payload, nullptr);

  const EventPayload second_payload = {
      .ski         = "second",
      .event_type  = kEventTypeDeviceChange,
      .change_type = kElementChangeAdd,
  };
  EVENTS_PUBLISH(second.get(), &second_payload);

  EXPECT_EQ(first_ctx.call_count, 1);
  EXPECT_EQ(second_ctx.call_count, 1);
  EXPECT_EQ(second_ctx.payload, &second_payload);
}
