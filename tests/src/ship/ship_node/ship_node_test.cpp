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

#include <atomic>
#include <iostream>
#include <memory>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/ship_connection/ship_connection_internal.h"
#include "src/ship/ship_node/ship_node_internal.h"
#include "tests/src/memory_leak.inc"

typedef struct TestShipConnection TestShipConnection;

struct TestShipConnection {
  ShipConnectionObject obj;
  InfoProviderObject* info_provider;
  const char* remote_ski;
};

static std::atomic<int> close_connection_count{0};
static std::atomic<int> destruct_count{0};
static std::atomic<int> stop_count{0};
static std::atomic<int> disconnected_count{0};

static void TestShipConnectionDestruct(DataWriterObject* self) {
  UNUSED(self);
  destruct_count++;
}

static void TestShipConnectionClose(ShipConnectionObject* self, bool safe, int32_t code, const char* reason) {
  UNUSED(safe);
  UNUSED(code);
  UNUSED(reason);
  close_connection_count++;
  TestShipConnection* const connection = reinterpret_cast<TestShipConnection*>(self);
  INFO_PROVIDER_HANDLE_CONNECTION_CLOSED(connection->info_provider, self, true);
}

static void TestShipConnectionStop(ShipConnectionObject* self) {
  UNUSED(self);
  stop_count++;
}

static const char* TestShipConnectionGetRemoteSki(ShipConnectionObject* self) {
  return reinterpret_cast<TestShipConnection*>(self)->remote_ski;
}

static ShipConnectionObject* TestShipConnectionCreate(InfoProviderObject* info_provider, const char* remote_ski) {
  static ShipConnectionInterface interface_ = {};
  interface_.data_writer_interface.destruct = TestShipConnectionDestruct;
  interface_.stop                           = TestShipConnectionStop;
  interface_.close_connection               = TestShipConnectionClose;
  interface_.get_remote_ski                 = TestShipConnectionGetRemoteSki;

  TestShipConnection* const connection = static_cast<TestShipConnection*>(EEBUS_MALLOC(sizeof(TestShipConnection)));
  connection->obj.interface_           = &interface_;
  connection->info_provider            = info_provider;
  connection->remote_ski               = remote_ski;
  return SHIP_CONNECTION_OBJECT(connection);
}

static void TestShipNodeReaderOnRemoteSkiDisconnected(ShipNodeReaderObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  disconnected_count++;
}

static void TestShipNodeReaderOnRemoteServicesUpdate(ShipNodeReaderObject* self, const Vector* entries) {
  UNUSED(self);
  UNUSED(entries);
}

static ShipNodeReaderObject* TestShipNodeReaderCreate(void) {
  static ShipNodeReaderInterface interface_ = {};
  interface_.on_remote_ski_disconnected     = TestShipNodeReaderOnRemoteSkiDisconnected;
  interface_.on_remote_services_update      = TestShipNodeReaderOnRemoteServicesUpdate;

  static ShipNodeReaderObject reader = {};
  reader.interface_                  = &interface_;
  return &reader;
}

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

  ShipNodeReaderObject* const ship_node_reader = TestShipNodeReaderCreate();

  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> ship_node{
      ShipNodeCreate(
          "test_ski",
          "client",
          device_info.get(),
          "ship_node_test_service",
          6677,
          nullptr,
          ship_node_reader,
          nullptr
      ),
      &ShipNodeDelete
  };

  // Start the ship node
  SHIP_NODE_START(ship_node.get());

  ShipNode* const internal = SHIP_NODE(ship_node.get());
  ShipConnectionObject* const connection
      = TestShipConnectionCreate(INFO_PROVIDER_OBJECT(ship_node.get()), "remote_ski");
  internal->ship_connection = connection;

  SHIP_NODE_CANCEL_PAIRING_WITH_SKI(ship_node.get(), "remote_ski");

  // Allow the connection loop to process the cancellation message.
  EebusThreadSleep(1);

  if (close_connection_count.load() != 1) {
    std::cout << "Expected one close request, got " << close_connection_count.load() << "\n";
    return -1;
  }

  if (stop_count.load() != 1) {
    std::cout << "Expected one connection stop, got " << stop_count.load() << "\n";
    return -1;
  }

  if (disconnected_count.load() != 1) {
    std::cout << "Expected one disconnect notification, got " << disconnected_count.load() << "\n";
    return -1;
  }

  if (destruct_count.load() != 1) {
    std::cout << "Expected one connection destruction, got " << destruct_count.load() << "\n";
    return -1;
  }

  if (internal->ship_connection != nullptr) {
    std::cout << "ShipNode retained the connection after its close callback\n";
    return -1;
  }

  // Stop the ship node
  SHIP_NODE_STOP(ship_node.get());

  std::cout << "Done!\n";
}
