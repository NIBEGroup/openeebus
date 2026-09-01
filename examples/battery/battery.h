/*
 * Copyright 2026 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef EXAMPLES_BATTERY_BATTERY_H_
#define EXAMPLES_BATTERY_BATTERY_H_

#include <stdint.h>

#include "src/common/eebus_malloc.h"
#include "src/service/api/service_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"

typedef struct BatteryObject BatteryObject;

struct BatteryObject {
  ServiceReaderObject service_reader;
};

#define BATTERY_OBJECT(obj) ((BatteryObject*)(obj))

BatteryObject* BatteryOpen(int32_t port, const char* role, TlsCertificateObject* tls_certificate);
void BatteryRegisterRemoteSki(BatteryObject* self, const char* ski);
void BatteryHandleCmd(BatteryObject* self, char* cmd);

static inline void BatteryClose(BatteryObject* self) {
  if (self != NULL) {
    SERVICE_READER_DESTRUCT(SERVICE_READER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // EXAMPLES_BATTERY_BATTERY_H_
