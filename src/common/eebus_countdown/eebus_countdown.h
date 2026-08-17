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
 * @brief Decrementing tick counter with expiry latch
 */

#ifndef SRC_COMMON_EEBUS_COUNTDOWN_EEBUS_COUNTDOWN_H_
#define SRC_COMMON_EEBUS_COUNTDOWN_EEBUS_COUNTDOWN_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EebusCountdown {
  uint32_t remaining;
  bool expired;
} EebusCountdown;

#define EEBUS_COUNTDOWN(ticks) ((EebusCountdown){.remaining = (ticks), .expired = false})

static inline void EebusCountdownTick(EebusCountdown* self) {
  if (self->remaining > 0) {
    self->remaining--;
  }

  if (self->remaining == 0) {
    self->expired = true;
  }
}

static inline bool EebusCountdownHasExpired(const EebusCountdown* self) {
  return self->expired;
}

static inline uint32_t EebusCountdownGetRemaining(const EebusCountdown* self) {
  return self->remaining;
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_COUNTDOWN_EEBUS_COUNTDOWN_H_
