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
 * @brief Two-remote EG-LPC test: verifies that EG-LPC write commands are routed
 * to the correct DataWriter when two remote CS devices (HP and EV) are connected
 * simultaneously, and that HP connectivity is unaffected after EV disconnect.
 */

#include "src/use_case/actor/eg/lpc/eg_lpc.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/eg_lp_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/message_buffer.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/model/load_limit_types.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/use_case/matchers.h"

// HP inc files — reuse existing eg_lpc_test namespace
#include "tests/src/use_case/actor/eg/lpc/receive/device_diagnosis_heartbeat_request.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/device_diagnosis_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/discovery_request.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/discovery_response.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/limits_description_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/limits_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_11.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_6.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_9.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/use_case_request.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_binding_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_description_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_diagnosis_heartbeat_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_diagnosis_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/discovery_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/discovery_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/send/electrical_connection_characteristic_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/electrical_connection_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/limits_write_with_duration.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_binding_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_limit_description_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_limit_list_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/result_data_msg_cnt_ref_28.inc"
#include "tests/src/use_case/actor/eg/lpc/send/result_data_msg_cnt_ref_31.inc"
#include "tests/src/use_case/actor/eg/lpc/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/use_case_data_reply.inc"

// EV inc files — two_remote_eg_lpc_test namespace, EV_123456789 device address
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_device_diagnosis_heartbeat_request.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_device_diagnosis_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_discovery_request.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_discovery_response.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_limits_description_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_limits_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_node_management_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_result_data_msg_cnt_ref_11.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_result_data_msg_cnt_ref_6.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_result_data_msg_cnt_ref_9.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_use_case_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/receive/ev_use_case_request.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_device_configuration_binding_call.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_device_configuration_description_read.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_device_configuration_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_device_diagnosis_heartbeat_read.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_device_diagnosis_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_discovery_read.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_discovery_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_electrical_connection_characteristic_read.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_electrical_connection_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_limits_write_with_duration.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_load_control_binding_call.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_load_control_limit_description_read.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_load_control_limit_list_read.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_load_control_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_node_management_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_result_data_msg_cnt_ref_28.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_result_data_msg_cnt_ref_31.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_use_case_data_read.inc"
#include "tests/src/use_case/actor/eg/lpc/two_remote/send/ev_use_case_data_reply.inc"

using testing::_;
using testing::Args;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

namespace two_remote_eg_lpc_test {

class TwoRemoteEgLpcTestFixture : public ::testing::Test {
 public:
  static constexpr uint32_t kHeartbeatTimeout = 60;
  static constexpr char     kHpSki[]          = "0123456789abcdefedcb0123456789abcdefedcb";
  static constexpr char     kEvSki[]          = "fedcba9876543210fedcba9876543210fedcba98";

  // Remote entity addresses — entity [1] is the CS entity
  static constexpr uint32_t        kRemoteEntityId        = 1;
  static constexpr const uint32_t* kRemoteEntityIds[]     = {&kRemoteEntityId};
  static const EntityAddressType   kHpEntityAddr;
  static const EntityAddressType   kEvEntityAddr;

 protected:
  std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> hp_writer_{nullptr, DataWriterMockDelete};
  std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> ev_writer_{nullptr, DataWriterMockDelete};
  std::unique_ptr<DeviceLocalObject, decltype(&DeviceLocalDelete)>  device_local_{nullptr, DeviceLocalDelete};
  DataReaderObject* hp_reader_{nullptr};
  DataReaderObject* ev_reader_{nullptr};

  std::unique_ptr<EgLpListenerMock, decltype(&EgLpListenerMockDelete)> listener_mock_{
      nullptr, EgLpListenerMockDelete};
  std::unique_ptr<EgLpUseCaseObject, decltype(&EgLpUseCaseDelete)> use_case_{nullptr, EgLpUseCaseDelete};

  void SetUp() override;
  void TearDown() override;

  void ExpectHpSend(const char* expected_json);
  void ExpectEvSend(const char* expected_json);
  void ExpectHpHeartbeat(const char* expected_json);
  void ExpectEvHeartbeat(const char* expected_json);
  void HandleHpMessage(const char* msg_string);
  void HandleEvMessage(const char* msg_string);
  void RunHpEgLpcDiscovery();
  void RunEvEgLpcDiscovery();

 private:
  static constexpr NetworkManagementFeatureSetType kFeatureSet = kNetworkManagementFeatureSetTypeSmart;

  std::unique_ptr<EebusDeviceInfo, decltype(&EebusDeviceInfoDelete)> device_info_{nullptr, EebusDeviceInfoDelete};
};

const EntityAddressType TwoRemoteEgLpcTestFixture::kHpEntityAddr = {
    "d:_n:HeatPump_123456789",
    TwoRemoteEgLpcTestFixture::kRemoteEntityIds,
    ARRAY_SIZE(TwoRemoteEgLpcTestFixture::kRemoteEntityIds),
};

const EntityAddressType TwoRemoteEgLpcTestFixture::kEvEntityAddr = {
    "d:_n:EV_123456789",
    TwoRemoteEgLpcTestFixture::kRemoteEntityIds,
    ARRAY_SIZE(TwoRemoteEgLpcTestFixture::kRemoteEntityIds),
};

void TwoRemoteEgLpcTestFixture::SetUp() {
  device_info_.reset(EebusDeviceInfoCreate("HEMS", "HEMS", "TestBrand", "TestModel", "123456789", "TestShipId"));
  hp_writer_.reset(DataWriterMockCreate());
  ev_writer_.reset(DataWriterMockCreate());

  device_local_.reset(DeviceLocalCreate(device_info_.get(), &kFeatureSet));

  uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};
  EntityLocalObject* const entity = EntityLocalCreate(
      device_local_.get(), kEntityTypeTypeGridGuard, entity_ids, ARRAY_SIZE(entity_ids), kHeartbeatTimeout
  );

  listener_mock_.reset(EgLpListenerMockCreate());
  use_case_.reset(EgLpcUseCaseCreate(entity, EG_LP_LISTENER_OBJECT(listener_mock_.get())));
  DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

  // Set up HP remote: triggers discovery_read to hp_writer_
  ExpectHpSend(eg_lpc_test::send::discovery_read);
  hp_reader_ = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(
      device_local_.get(), kHpSki, DATA_WRITER_OBJECT(hp_writer_.get())
  );

  // Set up EV remote: triggers discovery_read to ev_writer_
  ExpectEvSend(two_remote_eg_lpc_test::send::discovery_read);
  ev_reader_ = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(
      device_local_.get(), kEvSki, DATA_WRITER_OBJECT(ev_writer_.get())
  );
}

void TwoRemoteEgLpcTestFixture::TearDown() {
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

void TwoRemoteEgLpcTestFixture::ExpectHpSend(const char* expected_json) {
  EXPECT_CALL(*hp_writer_->gmock, WriteMessage(_, _, _))
      .With(Args<1, 2>(JsonMsgEq(expected_json)))
      .WillOnce(Return());
}

void TwoRemoteEgLpcTestFixture::ExpectEvSend(const char* expected_json) {
  EXPECT_CALL(*ev_writer_->gmock, WriteMessage(_, _, _))
      .With(Args<1, 2>(JsonMsgEq(expected_json)))
      .WillOnce(Return());
}

void TwoRemoteEgLpcTestFixture::ExpectHpHeartbeat(const char* expected_json) {
  EXPECT_CALL(*hp_writer_->gmock, WriteMessage(_, HeartbeatMsgEq(expected_json), _)).WillOnce(Return());
}

void TwoRemoteEgLpcTestFixture::ExpectEvHeartbeat(const char* expected_json) {
  EXPECT_CALL(*ev_writer_->gmock, WriteMessage(_, HeartbeatMsgEq(expected_json), _)).WillOnce(Return());
}

void TwoRemoteEgLpcTestFixture::HandleHpMessage(const char* msg_string) {
  MessageBuffer msg_buf;
  const char* const s   = JsonUnformat(msg_string);
  uint8_t* const    msg = reinterpret_cast<uint8_t*>(const_cast<char*>(s));
  MessageBufferInitWithDeallocator(&msg_buf, msg, strlen(s) + 1, JsonFree);
  DATA_READER_HANDLE_MESSAGE(hp_reader_, &msg_buf);
  MessageBufferRelease(&msg_buf);
  EXPECT_EQ(HandleQueueMessage(device_local_.get()), kEebusErrorOk);
}

void TwoRemoteEgLpcTestFixture::HandleEvMessage(const char* msg_string) {
  MessageBuffer msg_buf;
  const char* const s   = JsonUnformat(msg_string);
  uint8_t* const    msg = reinterpret_cast<uint8_t*>(const_cast<char*>(s));
  MessageBufferInitWithDeallocator(&msg_buf, msg, strlen(s) + 1, JsonFree);
  DATA_READER_HANDLE_MESSAGE(ev_reader_, &msg_buf);
  MessageBufferRelease(&msg_buf);
  EXPECT_EQ(HandleQueueMessage(device_local_.get()), kEebusErrorOk);
}

void TwoRemoteEgLpcTestFixture::RunHpEgLpcDiscovery() {
  // 1. HP sends discovery request → HEMS replies
  ExpectHpSend(eg_lpc_test::send::discovery_reply);
  HandleHpMessage(eg_lpc_test::receive::discovery_request);

  // 2. HP sends discovery response → HEMS sends subscriptions + use case read
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCsAdded(_, _)).WillOnce(Return());
  ExpectHpSend(eg_lpc_test::send::node_management_subscription_call);
  ExpectHpSend(eg_lpc_test::send::use_case_data_read);
  HandleHpMessage(eg_lpc_test::receive::discovery_response);

  // 3-7. HP sends results for HEMS subscription messages
  HandleHpMessage(eg_lpc_test::receive::result_data_msg_cnt_ref_5);
  HandleHpMessage(eg_lpc_test::receive::result_data_msg_cnt_ref_6);
  HandleHpMessage(eg_lpc_test::receive::result_data_msg_cnt_ref_8);
  HandleHpMessage(eg_lpc_test::receive::result_data_msg_cnt_ref_9);
  HandleHpMessage(eg_lpc_test::receive::result_data_msg_cnt_ref_11);

  // 8. HP sends node management subscription request → HEMS replies result
  ExpectHpSend(eg_lpc_test::send::result_data_msg_cnt_ref_28);
  HandleHpMessage(eg_lpc_test::receive::node_management_subscription_request);

  // 9. HP sends use case request → HEMS replies
  ExpectHpSend(eg_lpc_test::send::use_case_data_reply);
  HandleHpMessage(eg_lpc_test::receive::use_case_request);

  // 10. HP sends use case reply → HEMS sends load control + device configuration +
  //     device diagnosis + electrical connection subscriptions/reads
  ExpectHpSend(eg_lpc_test::send::load_control_subscription_call);
  ExpectHpSend(eg_lpc_test::send::load_control_binding_call);
  ExpectHpSend(eg_lpc_test::send::load_control_limit_description_read);
  ExpectHpSend(eg_lpc_test::send::device_configuration_subscription_call);
  ExpectHpSend(eg_lpc_test::send::device_configuration_binding_call);
  ExpectHpSend(eg_lpc_test::send::device_configuration_description_read);
  ExpectHpSend(eg_lpc_test::send::device_diagnosis_subscription_call);
  ExpectHpSend(eg_lpc_test::send::device_diagnosis_heartbeat_read);
  ExpectHpSend(eg_lpc_test::send::electrical_connection_subscription_call);
  ExpectHpSend(eg_lpc_test::send::electrical_connection_characteristic_read);
  HandleHpMessage(eg_lpc_test::receive::use_case_reply);

  // 11. HP sends device diagnosis subscription request → HEMS replies result
  ExpectHpSend(eg_lpc_test::send::result_data_msg_cnt_ref_31);
  HandleHpMessage(eg_lpc_test::receive::device_diagnosis_subscription_request);

  // 12. HP sends device diagnosis heartbeat request → HEMS sends heartbeat reply
  ExpectHpHeartbeat(eg_lpc_test::send::device_diagnosis_heartbeat_reply);
  HandleHpMessage(eg_lpc_test::receive::device_diagnosis_heartbeat_request);

  // 13. HP sends result ref 3 (ack for heartbeat reply)
  HandleHpMessage(eg_lpc_test::receive::result_data_msg_cnt_ref_3);

  // 14. HP sends limit description reply → HEMS reads limit list
  ExpectHpSend(eg_lpc_test::send::load_control_limit_list_read);
  HandleHpMessage(eg_lpc_test::receive::limits_description_reply);

  // 15. HP sends limits reply → OnPowerLimitReceive callback
  EXPECT_CALL(*listener_mock_->gmock, OnPowerLimitReceive(_, _, _, _)).WillOnce(Return());
  HandleHpMessage(eg_lpc_test::receive::limits_reply);
}

void TwoRemoteEgLpcTestFixture::RunEvEgLpcDiscovery() {
  // 1. EV sends discovery request → HEMS replies
  ExpectEvSend(two_remote_eg_lpc_test::send::discovery_reply);
  HandleEvMessage(two_remote_eg_lpc_test::receive::discovery_request);

  // 2. EV sends discovery response → HEMS sends subscriptions + use case read
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCsAdded(_, _)).WillOnce(Return());
  ExpectEvSend(two_remote_eg_lpc_test::send::node_management_subscription_call);
  ExpectEvSend(two_remote_eg_lpc_test::send::use_case_data_read);
  HandleEvMessage(two_remote_eg_lpc_test::receive::discovery_response);

  // 3-7. EV sends results for HEMS subscription messages
  HandleEvMessage(two_remote_eg_lpc_test::receive::result_data_msg_cnt_ref_5);
  HandleEvMessage(two_remote_eg_lpc_test::receive::result_data_msg_cnt_ref_6);
  HandleEvMessage(two_remote_eg_lpc_test::receive::result_data_msg_cnt_ref_8);
  HandleEvMessage(two_remote_eg_lpc_test::receive::result_data_msg_cnt_ref_9);
  HandleEvMessage(two_remote_eg_lpc_test::receive::result_data_msg_cnt_ref_11);

  // 8. EV sends node management subscription request → HEMS replies result
  ExpectEvSend(two_remote_eg_lpc_test::send::result_data_msg_cnt_ref_28);
  HandleEvMessage(two_remote_eg_lpc_test::receive::node_management_subscription_request);

  // 9. EV sends use case request → HEMS replies
  ExpectEvSend(two_remote_eg_lpc_test::send::use_case_data_reply);
  HandleEvMessage(two_remote_eg_lpc_test::receive::use_case_request);

  // 10. EV sends use case reply → HEMS sends subscriptions/reads
  ExpectEvSend(two_remote_eg_lpc_test::send::load_control_subscription_call);
  ExpectEvSend(two_remote_eg_lpc_test::send::load_control_binding_call);
  ExpectEvSend(two_remote_eg_lpc_test::send::load_control_limit_description_read);
  ExpectEvSend(two_remote_eg_lpc_test::send::device_configuration_subscription_call);
  ExpectEvSend(two_remote_eg_lpc_test::send::device_configuration_binding_call);
  ExpectEvSend(two_remote_eg_lpc_test::send::device_configuration_description_read);
  ExpectEvSend(two_remote_eg_lpc_test::send::device_diagnosis_subscription_call);
  ExpectEvSend(two_remote_eg_lpc_test::send::device_diagnosis_heartbeat_read);
  ExpectEvSend(two_remote_eg_lpc_test::send::electrical_connection_subscription_call);
  ExpectEvSend(two_remote_eg_lpc_test::send::electrical_connection_characteristic_read);
  HandleEvMessage(two_remote_eg_lpc_test::receive::use_case_reply);

  // 11. EV sends device diagnosis subscription request → HEMS replies result
  ExpectEvSend(two_remote_eg_lpc_test::send::result_data_msg_cnt_ref_31);
  HandleEvMessage(two_remote_eg_lpc_test::receive::device_diagnosis_subscription_request);

  // 12. EV sends device diagnosis heartbeat request → HEMS sends heartbeat reply
  ExpectEvHeartbeat(two_remote_eg_lpc_test::send::device_diagnosis_heartbeat_reply);
  HandleEvMessage(two_remote_eg_lpc_test::receive::device_diagnosis_heartbeat_request);

  // 13. EV sends result ref 3 (ack for heartbeat reply)
  HandleEvMessage(two_remote_eg_lpc_test::receive::result_data_msg_cnt_ref_3);

  // 14. EV sends limit description reply → HEMS reads limit list
  ExpectEvSend(two_remote_eg_lpc_test::send::load_control_limit_list_read);
  HandleEvMessage(two_remote_eg_lpc_test::receive::limits_description_reply);

  // 15. EV sends limits reply → OnPowerLimitReceive callback
  EXPECT_CALL(*listener_mock_->gmock, OnPowerLimitReceive(_, _, _, _)).WillOnce(Return());
  HandleEvMessage(two_remote_eg_lpc_test::receive::limits_reply);
}

// Phase 5.3: EG-LPC write to HP entity addr routes to hp_writer_ only
TEST_F(TwoRemoteEgLpcTestFixture, EgLpcWriteRoutedToHpOnly) {
  RunHpEgLpcDiscovery();
  RunEvEgLpcDiscovery();

  // Write to HP entity address → only hp_writer_ should receive it
  ExpectHpSend(eg_lpc_test::send::limits_write_with_duration);

  const LoadLimit limit = {
      .value      = {.value = 3000, .scale = 0},
      .duration   = {.hours = 1, .minutes = 2, .seconds = 3},
      .is_active  = true,
      .delete_duration = false,
  };
  EXPECT_EQ(EgLpcSetActiveConsumptionPowerLimit(use_case_.get(), &kHpEntityAddr, &limit, nullptr, nullptr),
            kEebusErrorOk);

  // Teardown: both remotes fire OnRemoteCsRemoved
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCsRemoved(_, _)).Times(2);
}

// Phase 5.3: EG-LPC write to EV entity addr routes to ev_writer_ only
TEST_F(TwoRemoteEgLpcTestFixture, EgLpcWriteRoutedToEvOnly) {
  RunHpEgLpcDiscovery();
  RunEvEgLpcDiscovery();

  // Write to EV entity address → only ev_writer_ should receive it
  ExpectEvSend(two_remote_eg_lpc_test::send::limits_write_with_duration);

  const LoadLimit limit = {
      .value      = {.value = 3000, .scale = 0},
      .duration   = {.hours = 1, .minutes = 2, .seconds = 3},
      .is_active  = true,
      .delete_duration = false,
  };
  EXPECT_EQ(EgLpcSetActiveConsumptionPowerLimit(use_case_.get(), &kEvEntityAddr, &limit, nullptr, nullptr),
            kEebusErrorOk);

  // Teardown: both remotes fire OnRemoteCsRemoved
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCsRemoved(_, _)).Times(2);
}

// Phase 5.4: EV disconnect — HP EG-LPC write still routes correctly
TEST_F(TwoRemoteEgLpcTestFixture, EvDisconnectHpEgLpcRouting) {
  RunHpEgLpcDiscovery();
  RunEvEgLpcDiscovery();

  // Disconnect EV — fires OnRemoteCsRemoved for EV
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCsRemoved(_, _)).WillOnce(Return());
  DEVICE_LOCAL_REMOVE_REMOTE_DEVICE_CONNECTION(device_local_.get(), kEvSki);

  // HP write still routes to hp_writer_ after EV disconnect
  ExpectHpSend(eg_lpc_test::send::limits_write_with_duration);

  const LoadLimit limit = {
      .value      = {.value = 3000, .scale = 0},
      .duration   = {.hours = 1, .minutes = 2, .seconds = 3},
      .is_active  = true,
      .delete_duration = false,
  };
  EXPECT_EQ(EgLpcSetActiveConsumptionPowerLimit(use_case_.get(), &kHpEntityAddr, &limit, nullptr, nullptr),
            kEebusErrorOk);

  // EV write returns NoChange — EV entity no longer registered
  EXPECT_EQ(EgLpcSetActiveConsumptionPowerLimit(use_case_.get(), &kEvEntityAddr, &limit, nullptr, nullptr),
            kEebusErrorNoChange);

  // Teardown: only HP fires OnRemoteCsRemoved (EV was already removed)
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCsRemoved(_, _)).WillOnce(Return());
}

}  // namespace two_remote_eg_lpc_test
