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
 * @brief Compressor OHPCF Listener implementation
 */

#include <stdio.h>

#include "examples/heat_pump/compressor_ohpcf_listener.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

typedef struct CompressorOhpcfListener CompressorOhpcfListener;

struct CompressorOhpcfListener {
  /** Implements the Compressor OHPCF Listener Interface */
  CompressorOhpcfListenerObject obj;
};

#define COMPRESSOR_OHPCF_LISTENER(obj) ((CompressorOhpcfListener*)(obj))

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

static EebusError CompressorOhpcfListenerConstruct(CompressorOhpcfListener* self);

EebusError CompressorOhpcfListenerConstruct(CompressorOhpcfListener* self) {
  // Override "virtual functions table"
  COMPRESSOR_OHPCF_LISTENER_INTERFACE(self) = &compressor_ohpcf_listener_methods;

  return kEebusErrorOk;
}

CompressorOhpcfListenerObject* CompressorOhpcfListenerCreate(void) {
  CompressorOhpcfListener* const compressor_ohpcf_listener
      = (CompressorOhpcfListener*)EEBUS_MALLOC(sizeof(CompressorOhpcfListener));

  if (CompressorOhpcfListenerConstruct(compressor_ohpcf_listener) != kEebusErrorOk) {
    CompressorOhpcfListenerDelete(COMPRESSOR_OHPCF_LISTENER_OBJECT(compressor_ohpcf_listener));
    return NULL;
  }

  return COMPRESSOR_OHPCF_LISTENER_OBJECT(compressor_ohpcf_listener);
}

void Destruct(CompressorOhpcfListenerObject* self) {
  UNUSED(self);

  // Nothing to be deallocated yet
}

void OnRemoteCemAdded(CompressorOhpcfListenerObject* self) {
  UNUSED(self);
  printf("Compressor OHPCF remote CEM added\n");
}

void OnRemoteCemRemoved(CompressorOhpcfListenerObject* self) {
  UNUSED(self);
  printf("Compressor OHPCF remote CEM removed\n");
}

void OnScheduleOptionalPowerConsumption(CompressorOhpcfListenerObject* self, const EebusDuration* start_time) {
  UNUSED(self);

  EebusDurationPrint("Compressor OHPCF scheduled optional power consumption at: %s\n", start_time);
}

void OnStop(CompressorOhpcfListenerObject* self) {
  UNUSED(self);
  printf("Compressor OHPCF received STOP command\n");
}

void OnPause(CompressorOhpcfListenerObject* self) {
  UNUSED(self);
  printf("Compressor OHPCF received PAUSE command\n");
}

void OnResume(CompressorOhpcfListenerObject* self) {
  UNUSED(self);
  printf("Compressor OHPCF received RESUME command\n");
}
