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
#include "src/service/service/eebus_service.h"

#include <gtest/gtest.h>

#include "mocks/service/api/service_reader_mock.h"
#include "mocks/ship/ship_node/ship_node_mock.h"
#include "mocks/ship/tls_certificate/tls_certificate_mock.h"
#include "mocks/spine/device/device_local_mock.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/common/vector.h"
#include "src/ship/ship_node/ship_node.h"
#include "src/spine/device/device_local.h"
#include "src/spine/model/entity_types.h"
#include "src/spine/model/feature_types.h"

using namespace std::literals;

using testing::_;
using testing::Return;

class EebusServiceTestSuite : public testing::Test {
 public:
  void SetUp() override;
  void TearDown() override;

 protected:
  EebusServiceConfig* configuration_;
  EebusServiceObject* service_;
  ServiceReaderMock* service_reader_mock_;
  TlsCertificateMock* tls_certificate_mock_;
};

static DeviceLocalMock* device_local_mock;

DeviceLocalObject*
DeviceLocalCreate(const EebusDeviceInfo* device_info, const NetworkManagementFeatureSetType* feature_set) {
  UNUSED(device_info);
  UNUSED(feature_set);
  return DEVICE_LOCAL_OBJECT(device_local_mock);
}

static ShipNodeMock* ship_node_mock;

ShipNodeObject* ShipNodeCreate(
    const char* ski,
    const char* role,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    const TlsCertificateObject* ssl_cert,
    ShipNodeReaderObject* ship_node_reader,
    ServiceDetails* local_service_details
) {
  UNUSED(ski);
  UNUSED(role);
  UNUSED(device_info);
  UNUSED(service_name);
  UNUSED(port);
  UNUSED(ssl_cert);
  UNUSED(ship_node_reader);
  UNUSED(local_service_details);
  return SHIP_NODE_OBJECT(ship_node_mock);
}

void EebusServiceTestSuite::SetUp() {
  configuration_ = EebusServiceConfigCreate("vendor", "brand", "model", "serial", "EnergyManagementSystem", 4729);

  tls_certificate_mock_ = TlsCertificateMockCreate();
  service_reader_mock_  = ServiceReaderMockCreate();
  device_local_mock     = DeviceLocalMockCreate();
  ship_node_mock        = ShipNodeMockCreate();

  TlsCertificateObject* const tls_cert = TLS_CERTIFICATE_OBJECT(tls_certificate_mock_);
  EXPECT_CALL(*tls_certificate_mock_->gmock, GetSki(tls_cert)).WillOnce(Return("test-ski"));

  service_ = EebusServiceCreate(
      configuration_,
      "client",
      TLS_CERTIFICATE_OBJECT(tls_certificate_mock_),
      SERVICE_READER_OBJECT(service_reader_mock_)
  );
}

void EebusServiceTestSuite::TearDown() {
  EXPECT_CALL(*ship_node_mock->gmock, Destruct(INFO_PROVIDER_OBJECT(ship_node_mock)));
  EXPECT_CALL(*device_local_mock->gmock, Destruct(DEVICE_OBJECT(device_local_mock)));
  EXPECT_CALL(*tls_certificate_mock_->gmock, Destruct(TLS_CERTIFICATE_OBJECT(tls_certificate_mock_)));

  EebusServiceDelete(service_);
  service_ = nullptr;

  EebusServiceConfigDelete(configuration_);

  EXPECT_CALL(*service_reader_mock_->gmock, Destruct(SERVICE_READER_OBJECT(service_reader_mock_)));
  SERVICE_READER_DESTRUCT(SERVICE_READER_OBJECT(service_reader_mock_));
  EEBUS_FREE(service_reader_mock_);
}

TEST_F(EebusServiceTestSuite, eebus_service_service_reader) {
  static constexpr char test_ski[] = "test";

  const Vector* entries = VectorCreate();

  ServiceReaderObject* const sr = SERVICE_READER_OBJECT(service_reader_mock_);
  ServiceReaderGMock& sr_gmock  = *service_reader_mock_->gmock;
  DeviceLocalGMock& dl_gmock    = *device_local_mock->gmock;

  EXPECT_CALL(sr_gmock, OnRemoteServicesUpdate(sr, service_, entries));
  SHIP_NODE_READER_ON_REMOTE_SERVICES_UPDATE(SHIP_NODE_READER_OBJECT(service_), entries);

  EXPECT_CALL(sr_gmock, OnRemoteSkiConnected(sr, service_, test_ski));
  SHIP_NODE_READER_ON_REMOTE_SKI_CONNECTED(SHIP_NODE_READER_OBJECT(service_), test_ski);

  EXPECT_CALL(sr_gmock, OnRemoteSkiDisconnected(sr, service_, test_ski));
  EXPECT_CALL(dl_gmock, RemoveRemoteDeviceConnection(DEVICE_LOCAL_OBJECT(device_local_mock), test_ski));
  SHIP_NODE_READER_ON_REMOTE_SKI_DISCONNECTED(SHIP_NODE_READER_OBJECT(service_), test_ski);

  static constexpr char ship_id[] = "shipid";
  EXPECT_CALL(sr_gmock, OnShipIdUpdate(sr, test_ski, ship_id));
  SHIP_NODE_READER_ON_SHIP_ID_UPDATE(SHIP_NODE_READER_OBJECT(service_), test_ski, ship_id);

  const SmeState state = kSmeStateApproved;
  EXPECT_CALL(sr_gmock, OnShipStateUpdate(sr, test_ski, state));
  SHIP_NODE_READER_ON_SHIP_STATE_UPDATE(SHIP_NODE_READER_OBJECT(service_), test_ski, state);

  EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, true);
  EXPECT_EQ(SHIP_NODE_READER_IS_WAITING_FOR_TRUST_ALLOWED(SHIP_NODE_READER_OBJECT(service_), test_ski), true);

  const ServiceDetails* const service_details = EEBUS_SERVICE_GET_LOCAL_SERVICE(service_);
  EXPECT_STREQ(service_details->ski, "test-ski");
  EXPECT_EQ(service_details->ipv4, nullptr);
  EXPECT_STREQ(service_details->ship_id, "brand-serial");
  EXPECT_STREQ(service_details->device_type, "EnergyManagementSystem");
  EXPECT_EQ(service_details->auto_accept, false);
  EXPECT_EQ(service_details->is_trusted, false);
  EXPECT_EQ(service_details->state, kConnectionStateNone);
}

TEST_F(EebusServiceTestSuite, eebus_service_ship_node) {
  static constexpr char test_ski[] = "test";

  ShipNodeObject* const sn = SHIP_NODE_OBJECT(ship_node_mock);
  ShipNodeGMock& sn_gmock  = *ship_node_mock->gmock;

  EXPECT_CALL(sn_gmock, RegisterRemoteSki(sn, test_ski, true));
  EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, test_ski, true);

  EXPECT_CALL(sn_gmock, CancelPairingWithSki(sn, test_ski));
  EEBUS_SERVICE_CANCEL_PAIRING_WITH_SKI(service_, test_ski);
}

TEST_F(EebusServiceTestSuite, eebus_service_start_stop) {
  EXPECT_CALL(*ship_node_mock->gmock, Start(SHIP_NODE_OBJECT(ship_node_mock)));
  EXPECT_CALL(*device_local_mock->gmock, Start(DEVICE_LOCAL_OBJECT(device_local_mock)));
  EEBUS_SERVICE_START(EEBUS_SERVICE_OBJECT(service_));

  EXPECT_CALL(*ship_node_mock->gmock, Stop(SHIP_NODE_OBJECT(ship_node_mock)));
  EXPECT_CALL(*device_local_mock->gmock, Stop(DEVICE_LOCAL_OBJECT(device_local_mock)));
  EEBUS_SERVICE_STOP(EEBUS_SERVICE_OBJECT(service_));
}

TEST_F(EebusServiceTestSuite, eebus_service_qr_code_string) {
  const char* qr_code = EEBUS_SERVICE_GET_QR_CODE_STRING(service_);
  EXPECT_STREQ(qr_code, "SHIP;SKI:TEST -SKI;ID:brand-serial;BRAND:brand;TYPE:EnergyManagementSystem;MODEL:model;");
}
