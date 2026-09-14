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
 * @brief Compressor Ohpcf Listener mock implementation
 */

#include "compressor_ohpcf_listener_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/use_case/api/compressor_ohpcf_listener_interface.h"

static void Destruct(CompressorOhpcfListenerObject* self);
static void OnRemoteCemAdded(CompressorOhpcfListenerObject* self);
static void OnRemoteCemRemoved(CompressorOhpcfListenerObject* self);
static void OnScheduleOptionalPowerConsumption(CompressorOhpcfListenerObject* self, const EebusDuration* start_time);
static void OnStop(CompressorOhpcfListenerObject* self);
static void OnPause(CompressorOhpcfListenerObject* self);
static void OnResume(CompressorOhpcfListenerObject* self);

static const CompressorOhpcfListenerInterface compressor_ohpcf_listener_methods = {
    .destruct                               = Destruct,
    .on_remote_cem_added                    = OnRemoteCemAdded,
    .on_remote_cem_removed                  = OnRemoteCemRemoved,
    .on_schedule_optional_power_consumption = OnScheduleOptionalPowerConsumption,
    .on_stop                                = OnStop,
    .on_pause                               = OnPause,
    .on_resume                              = OnResume,
};

static EebusError CompressorOhpcfListenerMockConstruct(CompressorOhpcfListenerMock* self);

EebusError CompressorOhpcfListenerMockConstruct(CompressorOhpcfListenerMock* self) {
  // Override "virtual functions table"
  COMPRESSOR_OHPCF_LISTENER_INTERFACE(self) = &compressor_ohpcf_listener_methods;

  self->gmock = new CompressorOhpcfListenerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

CompressorOhpcfListenerMock* CompressorOhpcfListenerMockCreate(void) {
  CompressorOhpcfListenerMock* const mock
      = (CompressorOhpcfListenerMock*)EEBUS_MALLOC(sizeof(CompressorOhpcfListenerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (CompressorOhpcfListenerMockConstruct(mock) != kEebusErrorOk) {
    CompressorOhpcfListenerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(CompressorOhpcfListenerObject* self) {
  CompressorOhpcfListenerMock* const mock = COMPRESSOR_OHPCF_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteCemAdded(CompressorOhpcfListenerObject* self) {
  CompressorOhpcfListenerMock* const mock = COMPRESSOR_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnRemoteCemAdded(self);
}

void OnRemoteCemRemoved(CompressorOhpcfListenerObject* self) {
  CompressorOhpcfListenerMock* const mock = COMPRESSOR_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnRemoteCemRemoved(self);
}

void OnScheduleOptionalPowerConsumption(CompressorOhpcfListenerObject* self, const EebusDuration* start_time) {
  CompressorOhpcfListenerMock* const mock = COMPRESSOR_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnScheduleOptionalPowerConsumption(self, start_time);
}

void OnStop(CompressorOhpcfListenerObject* self) {
  CompressorOhpcfListenerMock* const mock = COMPRESSOR_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnStop(self);
}

void OnPause(CompressorOhpcfListenerObject* self) {
  CompressorOhpcfListenerMock* const mock = COMPRESSOR_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnPause(self);
}

void OnResume(CompressorOhpcfListenerObject* self) {
  CompressorOhpcfListenerMock* const mock = COMPRESSOR_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnResume(self);
}
