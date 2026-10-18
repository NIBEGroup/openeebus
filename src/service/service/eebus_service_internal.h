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
#ifndef SRC_SERVICE_SERVICE_EEBUS_SERVICE_INTERNAL_H_
#define SRC_SERVICE_SERVICE_EEBUS_SERVICE_INTERNAL_H_

#include <stdbool.h>

#include "src/common/eebus_device_info.h"
#include "src/service/api/eebus_service_interface.h"
#include "src/service/api/service_reader_interface.h"
#include "src/ship/api/ship_node_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/spine/api/device_local_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EebusService EebusService;

struct EebusService {
  /** Implements the Service Interface */
  EebusServiceObject obj;

  ServiceDetails* local_service_details;
  EebusDeviceInfo* device_info;
  ShipNodeObject* ship_node;
  DeviceLocalObject* spine_local_device;
  const TlsCertificateObject* tls_certificate;
  ServiceReaderObject* service_reader;
  bool is_pairing_possible;
  char* ship_qr_code_string;
};

#define EEBUS_SERVICE(obj) ((EebusService*)(obj))

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // SRC_SERVICE_SERVICE_EEBUS_SERVICE_INTERNAL_H_
