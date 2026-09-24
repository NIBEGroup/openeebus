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
#include "src/ship/ship_node/ship_node.h"

#include <iostream>
#include <memory>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/ship_connection/ship_connection_internal.h"
#include "src/ship/ship_node/ship_node_internal.h"
#include "tests/src/memory_leak.inc"

ShipConnectionObject* ShipConnectionCreate(
    InfoProviderObject* info_provider,
    ShipRole role,
    const char* local_ship_id,
    const char* remote_ski,
    const char* remote_ship_id
) {
  UNUSED(info_provider);
  UNUSED(role);
  UNUSED(local_ship_id);
  UNUSED(remote_ski);
  UNUSED(remote_ship_id);
  // Connection will not be started within test
  return nullptr;
}

int main() {
  std::cout << "ShipNode start-stop test\n";

  // Create the ship node
  std::unique_ptr<EebusDeviceInfo, decltype(&EebusDeviceInfoDelete)> device_info{
      EebusDeviceInfoCreate("type", "brand", "model", "serial", "ship_id", "device_adress"),
      &EebusDeviceInfoDelete
  };

  if (device_info == nullptr) {
    std::cout << "Failed to create Device Info\n";
    return -1;
  }

  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> ship_node{
      ShipNodeCreate(
          "test_ski",
          "client",
          device_info.get(),
          "ship_node_test_service",
          6677,
          nullptr,
          nullptr,
          nullptr,
          kEebusTrustModePreTrust
      ),
      &ShipNodeDelete
  };

  // Start the ship node
  SHIP_NODE_START(ship_node.get());

  // Wait for 1 second
  EebusThreadSleep(1);

  // Stop the ship node
  SHIP_NODE_STOP(ship_node.get());

  std::cout << "Done!\n";
}
