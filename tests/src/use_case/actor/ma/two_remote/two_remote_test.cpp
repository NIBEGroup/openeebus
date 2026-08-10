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
 * @brief Two-remote device test: verifies that MA-MPC correctly routes
 * measurement callbacks from two simultaneously-connected remote devices
 * (one HP and one EV charger) without cross-contamination.
 */

#include "src/use_case/actor/ma/mpc/ma_mpc.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/ma_mpc_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/message_buffer.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/use_case/matchers.h"

// HP inc files — reuse existing ma_mpc_test namespace
#include "tests/src/use_case/actor/ma/mpc/receive/discovery_request.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/discovery_response.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/electrical_connection_description_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/electrical_connection_parameter_description_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/measurement_constraints_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/measurement_description_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/measurement_notify_power.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/measurement_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/receive/use_case_request.inc"
#include "tests/src/use_case/actor/ma/mpc/send/discovery_read.inc"
#include "tests/src/use_case/actor/ma/mpc/send/discovery_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/send/electrical_connection_description_read.inc"
#include "tests/src/use_case/actor/ma/mpc/send/electrical_connection_parameter_description_read.inc"
#include "tests/src/use_case/actor/ma/mpc/send/electrical_connection_subscription_call.inc"
#include "tests/src/use_case/actor/ma/mpc/send/measurement_constraints_read.inc"
#include "tests/src/use_case/actor/ma/mpc/send/measurement_description_read.inc"
#include "tests/src/use_case/actor/ma/mpc/send/measurement_read.inc"
#include "tests/src/use_case/actor/ma/mpc/send/measurement_subscription_call.inc"
#include "tests/src/use_case/actor/ma/mpc/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/ma/mpc/send/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/ma/mpc/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/ma/mpc/send/use_case_data_reply.inc"

// EV inc files — two_remote_test namespace, EV_123456789 device address
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_discovery_request.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_discovery_response.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_electrical_connection_description_reply.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_electrical_connection_parameter_description_reply.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_measurement_constraints_reply.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_measurement_description_reply.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_measurement_notify_power.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_measurement_reply.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_node_management_subscription_request.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_use_case_reply.inc"
#include "tests/src/use_case/actor/ma/two_remote/receive/ev_use_case_request.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_discovery_read.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_discovery_reply.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_electrical_connection_description_read.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_electrical_connection_parameter_description_read.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_electrical_connection_subscription_call.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_measurement_constraints_read.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_measurement_description_read.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_measurement_read.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_measurement_subscription_call.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_node_management_subscription_call.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_use_case_data_read.inc"
#include "tests/src/use_case/actor/ma/two_remote/send/ev_use_case_data_reply.inc"

using testing::_;
using testing::Args;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

MATCHER_P(EntityAddressDeviceEq, expected_device, "") {
  return arg != nullptr && std::string(arg->device) == expected_device;
}

namespace two_remote_test {

class TwoRemoteTestFixture : public ::testing::Test {
 public:
  static constexpr uint32_t kHeartbeatTimeout = 60;
  static constexpr char kHpSki[]              = "0123456789abcdefedcb0123456789abcdefedcb";
  static constexpr char kEvSki[]              = "fedcba9876543210fedcba9876543210fedcba98";

 protected:
  std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> hp_writer_{nullptr, DataWriterMockDelete};
  std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> ev_writer_{nullptr, DataWriterMockDelete};
  std::unique_ptr<DeviceLocalObject, decltype(&DeviceLocalDelete)>  device_local_{nullptr, DeviceLocalDelete};
  DataReaderObject* hp_reader_{nullptr};
  DataReaderObject* ev_reader_{nullptr};

  std::unique_ptr<MaMpcListenerMock, decltype(&MaMpcListenerMockDelete)> listener_mock_{
      nullptr, MaMpcListenerMockDelete};
  std::unique_ptr<MaMpcUseCaseObject, decltype(&MaMpcUseCaseDelete)> use_case_{nullptr, MaMpcUseCaseDelete};

  void SetUp() override;
  void TearDown() override;

  void ExpectHpSend(const char* expected_json);
  void ExpectEvSend(const char* expected_json);
  void HandleHpMessage(const char* msg_string);
  void HandleEvMessage(const char* msg_string);
  void RunHpDiscovery();
  void RunEvDiscovery();

 private:
  static constexpr NetworkManagementFeatureSetType kFeatureSet = kNetworkManagementFeatureSetTypeSmart;

  std::unique_ptr<EebusDeviceInfo, decltype(&EebusDeviceInfoDelete)> device_info_{nullptr, EebusDeviceInfoDelete};
};

void TwoRemoteTestFixture::SetUp() {
  device_info_.reset(EebusDeviceInfoCreate("HEMS", "HEMS", "TestBrand", "TestModel", "123456789", "TestShipId"));
  hp_writer_.reset(DataWriterMockCreate());
  ev_writer_.reset(DataWriterMockCreate());

  device_local_.reset(DeviceLocalCreate(device_info_.get(), &kFeatureSet));

  uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};
  EntityLocalObject* const entity = EntityLocalCreate(
      device_local_.get(), kEntityTypeTypeCEM, entity_ids, ARRAY_SIZE(entity_ids), kHeartbeatTimeout
  );

  listener_mock_.reset(MaMpcListenerMockCreate());
  use_case_.reset(MaMpcUseCaseCreate(entity, MA_MPC_LISTENER_OBJECT(listener_mock_.get())));
  DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

  // Set up HP remote: triggers discovery_read to hp_writer_
  ExpectHpSend(ma_mpc_test::send::discovery_read);
  hp_reader_ = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(
      device_local_.get(), kHpSki, DATA_WRITER_OBJECT(hp_writer_.get())
  );

  // Set up EV remote: triggers discovery_read to ev_writer_
  ExpectEvSend(two_remote_test::send::discovery_read);
  ev_reader_ = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(
      device_local_.get(), kEvSki, DATA_WRITER_OBJECT(ev_writer_.get())
  );
}

void TwoRemoteTestFixture::TearDown() {
  device_local_.reset();

  EXPECT_CALL(*hp_writer_->gmock, Destruct(_)).WillOnce(Return());
  hp_writer_.reset();

  EXPECT_CALL(*ev_writer_->gmock, Destruct(_)).WillOnce(Return());
  ev_writer_.reset();

  EXPECT_CALL(*listener_mock_->gmock, Destruct(_)).WillOnce(Return());
  use_case_.reset();
  listener_mock_.reset();

  device_info_.reset();

  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

void TwoRemoteTestFixture::ExpectHpSend(const char* expected_json) {
  EXPECT_CALL(*hp_writer_->gmock, WriteMessage(_, _, _))
      .With(Args<1, 2>(JsonMsgEq(expected_json)))
      .WillOnce(Return());
}

void TwoRemoteTestFixture::ExpectEvSend(const char* expected_json) {
  EXPECT_CALL(*ev_writer_->gmock, WriteMessage(_, _, _))
      .With(Args<1, 2>(JsonMsgEq(expected_json)))
      .WillOnce(Return());
}

void TwoRemoteTestFixture::HandleHpMessage(const char* msg_string) {
  MessageBuffer msg_buf;
  const char* const s   = JsonUnformat(msg_string);
  uint8_t* const    msg = reinterpret_cast<uint8_t*>(const_cast<char*>(s));
  MessageBufferInitWithDeallocator(&msg_buf, msg, strlen(s) + 1, JsonFree);
  DATA_READER_HANDLE_MESSAGE(hp_reader_, &msg_buf);
  MessageBufferRelease(&msg_buf);
  EXPECT_EQ(HandleQueueMessage(device_local_.get()), kEebusErrorOk);
}

void TwoRemoteTestFixture::HandleEvMessage(const char* msg_string) {
  MessageBuffer msg_buf;
  const char* const s   = JsonUnformat(msg_string);
  uint8_t* const    msg = reinterpret_cast<uint8_t*>(const_cast<char*>(s));
  MessageBufferInitWithDeallocator(&msg_buf, msg, strlen(s) + 1, JsonFree);
  DATA_READER_HANDLE_MESSAGE(ev_reader_, &msg_buf);
  MessageBufferRelease(&msg_buf);
  EXPECT_EQ(HandleQueueMessage(device_local_.get()), kEebusErrorOk);
}

void TwoRemoteTestFixture::RunHpDiscovery() {
  // 1. HP sends discovery request → HEMS replies
  ExpectHpSend(ma_mpc_test::send::discovery_reply);
  HandleHpMessage(ma_mpc_test::receive::discovery_request);

  // 2. HP sends discovery response → HEMS sends subscriptions + use case read
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteMuAdded(_, _)).WillOnce(Return());
  ExpectHpSend(ma_mpc_test::send::node_management_subscription_call);
  ExpectHpSend(ma_mpc_test::send::use_case_data_read);
  HandleHpMessage(ma_mpc_test::receive::discovery_response);

  // 3. HP sends node management subscription request → HEMS replies result
  ExpectHpSend(ma_mpc_test::send::result_data_msg_cnt_ref_3);
  HandleHpMessage(ma_mpc_test::receive::node_management_subscription_request);

  // 4. HP sends use case request → HEMS replies
  ExpectHpSend(ma_mpc_test::send::use_case_data_reply);
  HandleHpMessage(ma_mpc_test::receive::use_case_request);

  // 5. HP sends result ref 3
  HandleHpMessage(ma_mpc_test::receive::result_data_msg_cnt_ref_3);

  // 6. HP sends use case reply → HEMS sends electrical connection + measurement subscriptions/reads
  ExpectHpSend(ma_mpc_test::send::electrical_connection_subscription_call);
  ExpectHpSend(ma_mpc_test::send::electrical_connection_description_read);
  ExpectHpSend(ma_mpc_test::send::electrical_connection_parameter_description_read);
  ExpectHpSend(ma_mpc_test::send::measurement_subscription_call);
  ExpectHpSend(ma_mpc_test::send::measurement_description_read);
  ExpectHpSend(ma_mpc_test::send::measurement_constraints_read);
  HandleHpMessage(ma_mpc_test::receive::use_case_reply);

  // 7. HP sends result ref 5
  HandleHpMessage(ma_mpc_test::receive::result_data_msg_cnt_ref_5);

  // 8. HP sends electrical connection description reply
  HandleHpMessage(ma_mpc_test::receive::electrical_connection_description_reply);

  // 9. HP sends electrical connection parameter description reply
  HandleHpMessage(ma_mpc_test::receive::electrical_connection_parameter_description_reply);

  // 10. HP sends result ref 8
  HandleHpMessage(ma_mpc_test::receive::result_data_msg_cnt_ref_8);

  // 11. HP sends measurement description reply → HEMS requests measurement read
  ExpectHpSend(ma_mpc_test::send::measurement_read);
  HandleHpMessage(ma_mpc_test::receive::measurement_description_reply);

  // 12. HP sends measurement constraints reply
  HandleHpMessage(ma_mpc_test::receive::measurement_constraints_reply);

  // 13. HP sends measurement reply → triggers initial OnMeasurementReceive
  EXPECT_CALL(*listener_mock_->gmock, OnMeasurementReceive(_, kMpcPowerTotal, ScaledValueEq(33000, -1), _))
      .WillOnce(Return());
  HandleHpMessage(ma_mpc_test::receive::measurement_reply);
}

void TwoRemoteTestFixture::RunEvDiscovery() {
  // 1. EV sends discovery request → HEMS replies
  ExpectEvSend(two_remote_test::send::discovery_reply);
  HandleEvMessage(two_remote_test::receive::discovery_request);

  // 2. EV sends discovery response → HEMS sends subscriptions + use case read
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteMuAdded(_, _)).WillOnce(Return());
  ExpectEvSend(two_remote_test::send::node_management_subscription_call);
  ExpectEvSend(two_remote_test::send::use_case_data_read);
  HandleEvMessage(two_remote_test::receive::discovery_response);

  // 3. EV sends node management subscription request → HEMS replies result
  ExpectEvSend(two_remote_test::send::result_data_msg_cnt_ref_3);
  HandleEvMessage(two_remote_test::receive::node_management_subscription_request);

  // 4. EV sends use case request → HEMS replies
  ExpectEvSend(two_remote_test::send::use_case_data_reply);
  HandleEvMessage(two_remote_test::receive::use_case_request);

  // 5. EV sends result ref 3
  HandleEvMessage(two_remote_test::receive::result_data_msg_cnt_ref_3);

  // 6. EV sends use case reply → HEMS sends electrical connection + measurement subscriptions/reads
  ExpectEvSend(two_remote_test::send::electrical_connection_subscription_call);
  ExpectEvSend(two_remote_test::send::electrical_connection_description_read);
  ExpectEvSend(two_remote_test::send::electrical_connection_parameter_description_read);
  ExpectEvSend(two_remote_test::send::measurement_subscription_call);
  ExpectEvSend(two_remote_test::send::measurement_description_read);
  ExpectEvSend(two_remote_test::send::measurement_constraints_read);
  HandleEvMessage(two_remote_test::receive::use_case_reply);

  // 7. EV sends result ref 5
  HandleEvMessage(two_remote_test::receive::result_data_msg_cnt_ref_5);

  // 8. EV sends electrical connection description reply
  HandleEvMessage(two_remote_test::receive::electrical_connection_description_reply);

  // 9. EV sends electrical connection parameter description reply
  HandleEvMessage(two_remote_test::receive::electrical_connection_parameter_description_reply);

  // 10. EV sends result ref 8
  HandleEvMessage(two_remote_test::receive::result_data_msg_cnt_ref_8);

  // 11. EV sends measurement description reply → HEMS requests measurement read
  ExpectEvSend(two_remote_test::send::measurement_read);
  HandleEvMessage(two_remote_test::receive::measurement_description_reply);

  // 12. EV sends measurement constraints reply
  HandleEvMessage(two_remote_test::receive::measurement_constraints_reply);

  // 13. EV sends measurement reply → triggers initial OnMeasurementReceive
  EXPECT_CALL(*listener_mock_->gmock, OnMeasurementReceive(_, kMpcPowerTotal, ScaledValueEq(33000, -1), _))
      .WillOnce(Return());
  HandleEvMessage(two_remote_test::receive::measurement_reply);
}

TEST_F(TwoRemoteTestFixture, HpMeasurementRoutedToHpEntity) {
  RunHpDiscovery();
  RunEvDiscovery();

  // HP sends measurement notify → callback fires with HP entity address
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseA, ScaledValueEq(1000, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseB, ScaledValueEq(1100, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseC, ScaledValueEq(1200, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  HandleHpMessage(ma_mpc_test::receive::measurement_notify_power);

  // Teardown: both remotes will fire OnRemoteMuRemoved
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteMuRemoved(_, _)).Times(2);
}

TEST_F(TwoRemoteTestFixture, EvMeasurementRoutedToEvEntity) {
  RunHpDiscovery();
  RunEvDiscovery();

  // EV sends measurement notify → callback fires with EV entity address
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseA, ScaledValueEq(1000, 0), EntityAddressDeviceEq("d:_n:EV_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseB, ScaledValueEq(1100, 0), EntityAddressDeviceEq("d:_n:EV_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseC, ScaledValueEq(1200, 0), EntityAddressDeviceEq("d:_n:EV_123456789"))
  ).WillOnce(Return());
  HandleEvMessage(two_remote_test::receive::measurement_notify_power);

  // Teardown: both remotes will fire OnRemoteMuRemoved
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteMuRemoved(_, _)).Times(2);
}

TEST_F(TwoRemoteTestFixture, BothMeasurementsRoutedIndependently) {
  RunHpDiscovery();
  RunEvDiscovery();

  // HP notify
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseA, ScaledValueEq(1000, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseB, ScaledValueEq(1100, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseC, ScaledValueEq(1200, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  HandleHpMessage(ma_mpc_test::receive::measurement_notify_power);

  // EV notify
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseA, ScaledValueEq(1000, 0), EntityAddressDeviceEq("d:_n:EV_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseB, ScaledValueEq(1100, 0), EntityAddressDeviceEq("d:_n:EV_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseC, ScaledValueEq(1200, 0), EntityAddressDeviceEq("d:_n:EV_123456789"))
  ).WillOnce(Return());
  HandleEvMessage(two_remote_test::receive::measurement_notify_power);

  // Verify stored data is independently accessible per entity address
  static constexpr uint32_t                  remote_entity_id{1};
  static constexpr const uint32_t* const     remote_entity_ids[]{&remote_entity_id};
  const EntityAddressType hp_entity_addr = {"d:_n:HeatPump_123456789", remote_entity_ids, ARRAY_SIZE(remote_entity_ids)};
  const EntityAddressType ev_entity_addr = {"d:_n:EV_123456789",      remote_entity_ids, ARRAY_SIZE(remote_entity_ids)};

  ScaledValue value{};
  EXPECT_EQ(MaMpcGetMeasurementData(use_case_.get(), kMpcPowerPhaseA, &hp_entity_addr, &value), kEebusErrorOk);
  EXPECT_THAT(&value, ScaledValueEq(1000, 0));

  EXPECT_EQ(MaMpcGetMeasurementData(use_case_.get(), kMpcPowerPhaseA, &ev_entity_addr, &value), kEebusErrorOk);
  EXPECT_THAT(&value, ScaledValueEq(1000, 0));

  // Teardown: both remotes will fire OnRemoteMuRemoved
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteMuRemoved(_, _)).Times(2);
}

// Phase 5.4: EV disconnect — HP measurement notify unaffected
TEST_F(TwoRemoteTestFixture, EvDisconnectHpMeasurementUnaffected) {
  RunHpDiscovery();
  RunEvDiscovery();

  // Disconnect EV — fires OnRemoteMuRemoved for EV only
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteMuRemoved(_, EntityAddressDeviceEq("d:_n:EV_123456789")))
      .WillOnce(Return());
  DEVICE_LOCAL_REMOVE_REMOTE_DEVICE_CONNECTION(device_local_.get(), kEvSki);

  // HP measurement notify still routes to HP entity after EV disconnect
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseA, ScaledValueEq(1000, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseB, ScaledValueEq(1100, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnMeasurementReceive(_, kMpcPowerPhaseC, ScaledValueEq(1200, 0), EntityAddressDeviceEq("d:_n:HeatPump_123456789"))
  ).WillOnce(Return());
  HandleHpMessage(ma_mpc_test::receive::measurement_notify_power);

  // Teardown: only HP fires OnRemoteMuRemoved (EV was already disconnected above)
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteMuRemoved(_, _)).Times(1);
}

}  // namespace two_remote_test
