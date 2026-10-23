/*
 * Copyright 2026 NIBE AB
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
 * @brief Two-remote CEM-OHPCF test: verifies that the compressor state is tracked per
 * remote compressor and that the pause/resume/stop commands are only sent to a compressor
 * whose own state allows them, when two heat pumps are connected simultaneously.
 */

#include <gtest/gtest.h>

#include <deque>
#include <memory>
#include <string>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/cem_ohpcf_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/message_buffer.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/actor/cem/ohpcf/cem_ohpcf.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/use_case/matchers.h"

// Messages of the single remote CEM-OHPCF test, used for both heat pumps
#include "tests/src/use_case/actor/cem/ohpcf/receive/discovery_request.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/discovery_response.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_10.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_6.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_announce_notify_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_announce_notify_2.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_clear_process_notify_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_pause_notify.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_read_reply.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_report_state_completed_notify.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_report_state_running_notify_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_report_state_scheduled_notify_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/use_case_request.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/discovery_read.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/discovery_reply.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/result_data_msg_cnt_ref_27.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_binding_call.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_data_read.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_schedule_write_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_subscription_call.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/use_case_data_reply.inc"

// Writes whose message counter differs from the single remote test
#include "tests/src/use_case/actor/cem/ohpcf/two_remote/send/sem_ps_pause_write_msg_cnt_11.inc"
#include "tests/src/use_case/actor/cem/ohpcf/two_remote/send/sem_ps_resume_write_msg_cnt_12.inc"
#include "tests/src/use_case/actor/cem/ohpcf/two_remote/send/sem_ps_stop_write_msg_cnt_10.inc"

using testing::_;
using testing::Args;
using testing::Return;

MATCHER_P(EntityAddressEq, expected, "") {
  return EntityAddressCompare(arg, expected);
}

namespace two_remote_cem_ohpcf_test {

enum class HeatPump { kHp1, kHp2 };

class TwoRemoteCemOhpcfTestFixture : public ::testing::Test {
 public:
  static constexpr uint32_t kHeartbeatTimeout = 60;
  static constexpr char kHp1Ski[]             = "0123456789abcdefedcb0123456789abcdefedcb";
  static constexpr char kHp2Ski[]             = "fedcba9876543210fedcba9876543210fedcba98";

  // The second heat pump sends and receives the same messages as the first one,
  // only with its own device address
  static constexpr char kHp1DeviceAddress[] = "d:_n:HeatPump_123456789";
  static constexpr char kHp2DeviceAddress[] = "d:_n:HeatPump2_123456789";

  // Both heat pumps expose their compressor as entity [3, 1]
  static constexpr uint32_t kCompressorEntityId           = 3;
  static constexpr uint32_t kCompressorSubEntityId        = 1;
  static constexpr const uint32_t* kCompressorEntityIds[] = {&kCompressorEntityId, &kCompressorSubEntityId};
  static constexpr EntityAddressType kHp1CompressorAddr   = {kHp1DeviceAddress, kCompressorEntityIds, 2};
  static constexpr EntityAddressType kHp2CompressorAddr   = {kHp2DeviceAddress, kCompressorEntityIds, 2};

 protected:
  std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> hp1_writer_{nullptr, DataWriterMockDelete};
  std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> hp2_writer_{nullptr, DataWriterMockDelete};
  std::unique_ptr<DeviceLocalObject, decltype(&DeviceLocalDelete)> device_local_{nullptr, DeviceLocalDelete};
  DataReaderObject* hp1_reader_{nullptr};
  DataReaderObject* hp2_reader_{nullptr};

  std::unique_ptr<CemOhpcfListenerMock, decltype(&CemOhpcfListenerMockDelete)> listener_mock_{
      nullptr,
      CemOhpcfListenerMockDelete
  };
  std::unique_ptr<CemOhpcfUseCaseObject, decltype(&CemOhpcfUseCaseDelete)> use_case_{nullptr, CemOhpcfUseCaseDelete};

  void SetUp() override;
  void TearDown() override;

  const char* MessageFor(HeatPump hp, const char* msg);
  void ExpectSend(HeatPump hp, const char* expected_json);
  void ExpectNoSend(HeatPump hp);
  void HandleMessage(HeatPump hp, const char* msg_string);
  void RunDiscovery(HeatPump hp);

 private:
  static constexpr NetworkManagementFeatureSetType kFeatureSet = kNetworkManagementFeatureSetTypeSmart;

  std::unique_ptr<EebusDeviceInfo, decltype(&EebusDeviceInfoDelete)> device_info_{nullptr, EebusDeviceInfoDelete};

  // Keeps the second heat pump's messages alive as long as the expectations referring to them
  std::deque<std::string> hp2_messages_;
};

void TwoRemoteCemOhpcfTestFixture::SetUp() {
  device_info_.reset(EebusDeviceInfoCreate("HEMS", "HEMS", "TestBrand", "TestModel", "123456789", "TestShipId"));
  hp1_writer_.reset(DataWriterMockCreate());
  hp2_writer_.reset(DataWriterMockCreate());

  device_local_.reset(DeviceLocalCreate(device_info_.get(), &kFeatureSet));

  uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};
  EntityLocalObject* const entity = EntityLocalCreate(
      device_local_.get(),
      kEntityTypeTypeCEM,
      entity_ids,
      ARRAY_SIZE(entity_ids),
      kHeartbeatTimeout
  );

  listener_mock_.reset(CemOhpcfListenerMockCreate());
  use_case_.reset(CemOhpcfUseCaseCreate(entity, CEM_OHPCF_LISTENER_OBJECT(listener_mock_.get())));
  DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

  ExpectSend(HeatPump::kHp1, cem_ohpcf_test::send::discovery_read);
  hp1_reader_ = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(device_local_.get(), kHp1Ski, DATA_WRITER_OBJECT(hp1_writer_.get()));

  ExpectSend(HeatPump::kHp2, cem_ohpcf_test::send::discovery_read);
  hp2_reader_ = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(device_local_.get(), kHp2Ski, DATA_WRITER_OBJECT(hp2_writer_.get()));
}

void TwoRemoteCemOhpcfTestFixture::TearDown() {
  DEVICE_LOCAL_REMOVE_REMOTE_DEVICE_CONNECTION(device_local_.get(), kHp1Ski);
  DEVICE_LOCAL_REMOVE_REMOTE_DEVICE_CONNECTION(device_local_.get(), kHp2Ski);

  EXPECT_CALL(*listener_mock_->gmock, Destruct(_)).WillOnce(Return());
  use_case_.reset();
  listener_mock_.reset();

  device_local_.reset();

  EXPECT_CALL(*hp1_writer_->gmock, Destruct(_)).WillOnce(Return());
  hp1_writer_.reset();

  EXPECT_CALL(*hp2_writer_->gmock, Destruct(_)).WillOnce(Return());
  hp2_writer_.reset();

  device_info_.reset();
  hp2_messages_.clear();

  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

const char* TwoRemoteCemOhpcfTestFixture::MessageFor(HeatPump hp, const char* msg) {
  if (hp == HeatPump::kHp1) {
    return msg;
  }

  std::string s{msg};
  const std::string from{kHp1DeviceAddress};
  const std::string to{kHp2DeviceAddress};
  for (size_t pos = s.find(from); pos != std::string::npos; pos = s.find(from, pos + to.size())) {
    s.replace(pos, from.size(), to);
  }

  return hp2_messages_.emplace_back(std::move(s)).c_str();
}

void TwoRemoteCemOhpcfTestFixture::ExpectSend(HeatPump hp, const char* expected_json) {
  DataWriterMock* const writer = (hp == HeatPump::kHp1) ? hp1_writer_.get() : hp2_writer_.get();
  EXPECT_CALL(*writer->gmock, WriteMessage(_, _, _))
      .With(Args<1, 2>(JsonMsgEq(MessageFor(hp, expected_json))))
      .WillOnce(Return());
}

void TwoRemoteCemOhpcfTestFixture::ExpectNoSend(HeatPump hp) {
  DataWriterMock* const writer = (hp == HeatPump::kHp1) ? hp1_writer_.get() : hp2_writer_.get();
  EXPECT_CALL(*writer->gmock, WriteMessage(_, _, _)).Times(0);
}

void TwoRemoteCemOhpcfTestFixture::HandleMessage(HeatPump hp, const char* msg_string) {
  DataReaderObject* const reader = (hp == HeatPump::kHp1) ? hp1_reader_ : hp2_reader_;

  MessageBuffer msg_buf;
  const char* const s = JsonUnformat(MessageFor(hp, msg_string));
  uint8_t* const msg  = reinterpret_cast<uint8_t*>(const_cast<char*>(s));
  MessageBufferInitWithDeallocator(&msg_buf, msg, strlen(s) + 1, JsonFree);
  DATA_READER_HANDLE_MESSAGE(reader, &msg_buf);
  MessageBufferRelease(&msg_buf);
  EXPECT_EQ(HandleQueueMessage(device_local_.get()), kEebusErrorOk);
}

// Stand-in for the heat pump: detailed discovery, use case discovery, subscription, binding
// and the initial read, ending with no optional process announced
void TwoRemoteCemOhpcfTestFixture::RunDiscovery(HeatPump hp) {
  const EntityAddressType* const compressor_addr = (hp == HeatPump::kHp1) ? &kHp1CompressorAddr : &kHp2CompressorAddr;

  ExpectSend(hp, cem_ohpcf_test::send::discovery_reply);
  HandleMessage(hp, cem_ohpcf_test::receive::discovery_request);

  ExpectSend(hp, cem_ohpcf_test::send::node_management_subscription_call);
  ExpectSend(hp, cem_ohpcf_test::send::use_case_data_read);
  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCompressorAdded(_, EntityAddressEq(compressor_addr))).WillOnce(Return());
  HandleMessage(hp, cem_ohpcf_test::receive::discovery_response);

  ExpectSend(hp, cem_ohpcf_test::send::result_data_msg_cnt_ref_27);
  HandleMessage(hp, cem_ohpcf_test::receive::node_management_subscription_request);

  HandleMessage(hp, cem_ohpcf_test::receive::result_data_msg_cnt_ref_3);

  ExpectSend(hp, cem_ohpcf_test::send::sem_ps_subscription_call);
  ExpectSend(hp, cem_ohpcf_test::send::sem_ps_binding_call);
  ExpectSend(hp, cem_ohpcf_test::send::sem_ps_data_read);
  HandleMessage(hp, cem_ohpcf_test::receive::use_case_reply);

  ExpectSend(hp, cem_ohpcf_test::send::use_case_data_reply);
  HandleMessage(hp, cem_ohpcf_test::receive::use_case_request);

  EXPECT_CALL(*listener_mock_->gmock, OnClearProcess(_, EntityAddressEq(compressor_addr))).WillOnce(Return());
  HandleMessage(hp, cem_ohpcf_test::receive::sem_ps_read_reply);

  HandleMessage(hp, cem_ohpcf_test::receive::result_data_msg_cnt_ref_5);
  HandleMessage(hp, cem_ohpcf_test::receive::result_data_msg_cnt_ref_6);
}

TEST_F(TwoRemoteCemOhpcfTestFixture, CompressorStateIsTrackedPerCompressor) {
  RunDiscovery(HeatPump::kHp1);
  RunDiscovery(HeatPump::kHp2);

  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp1CompressorAddr), kCompressorOhpcfStateUndefined);
  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp2CompressorAddr), kCompressorOhpcfStateUndefined);

  // HP1 announces an optional process, HP2 has none yet
  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, EntityAddressEq(&kHp1CompressorAddr)))
      .WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnStateReport(_, kCompressorOhpcfStateAnnounced, nullptr, EntityAddressEq(&kHp1CompressorAddr))
  )
      .WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_announce_notify_1);

  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp1CompressorAddr), kCompressorOhpcfStateAnnounced);
  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp2CompressorAddr), kCompressorOhpcfStateUndefined);

  // HP2 announces an optional process
  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, EntityAddressEq(&kHp2CompressorAddr)))
      .WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnStateReport(_, kCompressorOhpcfStateAnnounced, nullptr, EntityAddressEq(&kHp2CompressorAddr))
  )
      .WillOnce(Return());
  HandleMessage(HeatPump::kHp2, cem_ohpcf_test::receive::sem_ps_announce_notify_2);

  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp1CompressorAddr), kCompressorOhpcfStateAnnounced);
  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp2CompressorAddr), kCompressorOhpcfStateAnnounced);

  // CEM schedules HP1, HP1 reports it as scheduled
  static constexpr EebusDuration start_time = {.seconds = 10};
  ExpectSend(HeatPump::kHp1, cem_ohpcf_test::send::sem_ps_schedule_write_1);
  EXPECT_EQ(
      CemOhpcfScheduleOptionalPowerConsumption(use_case_.get(), &kHp1CompressorAddr, &start_time, nullptr, nullptr),
      kEebusErrorOk
  );

  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, EntityAddressEq(&kHp1CompressorAddr)))
      .WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnStateReport(_, kCompressorOhpcfStateScheduled, _, EntityAddressEq(&kHp1CompressorAddr))
  )
      .WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_report_state_scheduled_notify_1);
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::result_data_msg_cnt_ref_10);

  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp1CompressorAddr), kCompressorOhpcfStateScheduled);
  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp2CompressorAddr), kCompressorOhpcfStateAnnounced);

  // HP1 starts running
  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, EntityAddressEq(&kHp1CompressorAddr)))
      .WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnStateReport(_, kCompressorOhpcfStateRunning, nullptr, EntityAddressEq(&kHp1CompressorAddr))
  )
      .WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_report_state_running_notify_1);

  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp1CompressorAddr), kCompressorOhpcfStateRunning);
  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp2CompressorAddr), kCompressorOhpcfStateAnnounced);

  // HP1 completes its process
  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, EntityAddressEq(&kHp1CompressorAddr)))
      .WillOnce(Return());
  EXPECT_CALL(
      *listener_mock_->gmock,
      OnStateReport(_, kCompressorOhpcfStateCompleted, nullptr, EntityAddressEq(&kHp1CompressorAddr))
  )
      .WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_report_state_completed_notify);

  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp1CompressorAddr), kCompressorOhpcfStateCompleted);
  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp2CompressorAddr), kCompressorOhpcfStateAnnounced);

  // HP1 clears the process
  EXPECT_CALL(*listener_mock_->gmock, OnClearProcess(_, EntityAddressEq(&kHp1CompressorAddr))).WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_clear_process_notify_1);

  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp1CompressorAddr), kCompressorOhpcfStateUndefined);
  EXPECT_EQ(CemOhpcfGetCompressorState(use_case_.get(), &kHp2CompressorAddr), kCompressorOhpcfStateAnnounced);

  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCompressorRemoved(_, _)).Times(2);
}

TEST_F(TwoRemoteCemOhpcfTestFixture, PauseIsOnlySentToARunningCompressor) {
  RunDiscovery(HeatPump::kHp1);
  RunDiscovery(HeatPump::kHp2);

  // HP1 and HP2 announce an optional process
  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, _)).Times(2);
  EXPECT_CALL(*listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateAnnounced, nullptr, _)).Times(2);
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_announce_notify_1);
  HandleMessage(HeatPump::kHp2, cem_ohpcf_test::receive::sem_ps_announce_notify_2);

  // CEM schedules HP1, HP1 reports it as scheduled and then as running
  static constexpr EebusDuration start_time = {.seconds = 10};
  ExpectSend(HeatPump::kHp1, cem_ohpcf_test::send::sem_ps_schedule_write_1);
  EXPECT_EQ(
      CemOhpcfScheduleOptionalPowerConsumption(use_case_.get(), &kHp1CompressorAddr, &start_time, nullptr, nullptr),
      kEebusErrorOk
  );

  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, _)).WillOnce(Return());
  EXPECT_CALL(*listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateScheduled, _, _)).WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_report_state_scheduled_notify_1);
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::result_data_msg_cnt_ref_10);

  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, _)).WillOnce(Return());
  EXPECT_CALL(*listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateRunning, nullptr, _)).WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_report_state_running_notify_1);

  // HP2 is only announced, so it cannot be paused
  ExpectNoSend(HeatPump::kHp2);
  EXPECT_EQ(CemOhpcfWritePauseCommand(use_case_.get(), &kHp2CompressorAddr, nullptr, nullptr), kEebusErrorNoChange);

  // HP1 is running, so it can be paused
  ExpectSend(HeatPump::kHp1, send::sem_ps_pause_write_msg_cnt_11);
  EXPECT_EQ(CemOhpcfWritePauseCommand(use_case_.get(), &kHp1CompressorAddr, nullptr, nullptr), kEebusErrorOk);

  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCompressorRemoved(_, _)).Times(2);
}

TEST_F(TwoRemoteCemOhpcfTestFixture, ResumeIsOnlySentToAPausedCompressor) {
  RunDiscovery(HeatPump::kHp1);
  RunDiscovery(HeatPump::kHp2);

  // HP1 and HP2 announce an optional process
  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, _)).Times(2);
  EXPECT_CALL(*listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateAnnounced, nullptr, _)).Times(2);
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_announce_notify_1);
  HandleMessage(HeatPump::kHp2, cem_ohpcf_test::receive::sem_ps_announce_notify_2);

  // CEM schedules HP1, HP1 reports it as scheduled and then as running
  static constexpr EebusDuration start_time = {.seconds = 10};
  ExpectSend(HeatPump::kHp1, cem_ohpcf_test::send::sem_ps_schedule_write_1);
  EXPECT_EQ(
      CemOhpcfScheduleOptionalPowerConsumption(use_case_.get(), &kHp1CompressorAddr, &start_time, nullptr, nullptr),
      kEebusErrorOk
  );

  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, _)).WillOnce(Return());
  EXPECT_CALL(*listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateScheduled, _, _)).WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_report_state_scheduled_notify_1);
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::result_data_msg_cnt_ref_10);

  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, _)).WillOnce(Return());
  EXPECT_CALL(*listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateRunning, nullptr, _)).WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_report_state_running_notify_1);

  // CEM pauses HP1, HP1 reports it as paused
  ExpectSend(HeatPump::kHp1, send::sem_ps_pause_write_msg_cnt_11);
  EXPECT_EQ(CemOhpcfWritePauseCommand(use_case_.get(), &kHp1CompressorAddr, nullptr, nullptr), kEebusErrorOk);

  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, _)).WillOnce(Return());
  EXPECT_CALL(*listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStatePaused, nullptr, _)).WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_pause_notify);

  // HP2 is only announced, so it cannot be resumed
  ExpectNoSend(HeatPump::kHp2);
  EXPECT_EQ(CemOhpcfWriteResumeCommand(use_case_.get(), &kHp2CompressorAddr, nullptr, nullptr), kEebusErrorNoChange);

  // HP1 is paused, so it can be resumed
  ExpectSend(HeatPump::kHp1, send::sem_ps_resume_write_msg_cnt_12);
  EXPECT_EQ(CemOhpcfWriteResumeCommand(use_case_.get(), &kHp1CompressorAddr, nullptr, nullptr), kEebusErrorOk);

  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCompressorRemoved(_, _)).Times(2);
}

TEST_F(TwoRemoteCemOhpcfTestFixture, StopIsOnlySentToACompressorWithAProcess) {
  RunDiscovery(HeatPump::kHp1);
  RunDiscovery(HeatPump::kHp2);

  // HP2 and HP1 announce an optional process
  EXPECT_CALL(*listener_mock_->gmock, OnOptionalPowerConsumptionUpdate(_, _, _)).Times(2);
  EXPECT_CALL(*listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateAnnounced, nullptr, _)).Times(2);
  HandleMessage(HeatPump::kHp2, cem_ohpcf_test::receive::sem_ps_announce_notify_2);
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_announce_notify_1);

  // HP1 withdraws its process
  EXPECT_CALL(*listener_mock_->gmock, OnClearProcess(_, EntityAddressEq(&kHp1CompressorAddr))).WillOnce(Return());
  HandleMessage(HeatPump::kHp1, cem_ohpcf_test::receive::sem_ps_clear_process_notify_1);

  // HP1 has no process anymore, so there is nothing to stop
  ExpectNoSend(HeatPump::kHp1);
  EXPECT_EQ(CemOhpcfWriteStopCommand(use_case_.get(), &kHp1CompressorAddr, nullptr, nullptr), kEebusErrorNoChange);

  // HP2 still has its process, so it can be stopped
  ExpectSend(HeatPump::kHp2, send::sem_ps_stop_write_msg_cnt_10);
  EXPECT_EQ(CemOhpcfWriteStopCommand(use_case_.get(), &kHp2CompressorAddr, nullptr, nullptr), kEebusErrorOk);

  EXPECT_CALL(*listener_mock_->gmock, OnRemoteCompressorRemoved(_, _)).Times(2);
}

}  // namespace two_remote_cem_ohpcf_test
