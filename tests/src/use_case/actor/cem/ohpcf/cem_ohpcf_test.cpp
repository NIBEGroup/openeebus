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
 * @brief Currently it is not a regular unit test but more a "sand box"
 * to feed the SPINE Device with specific datagrams and check the outgoing messages printed.
 * @note Remember to enable the message printing in PrintMessage() before getting started
 */

#include "src/use_case/actor/cem/ohpcf/cem_ohpcf.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/cem_ohpcf_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/use_case/actor/cem/ohpcf/receive/discovery_request.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/discovery_response.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_10.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_11.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_12.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_13.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_14.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/result_data_msg_cnt_ref_6.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_announce_notify_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_announce_notify_2.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_clear_process_notify_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_clear_process_notify_2.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_pause_notify.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_read_reply.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_read_reply_ref_15.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_report_state_completed_notify.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_report_state_running_notify_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_report_state_running_notify_2.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_report_state_scheduled_notify_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_report_state_scheduled_notify_2.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/sem_ps_running_notify.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/cem/ohpcf/receive/use_case_request.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/discovery_read.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/discovery_reply.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/result_data_msg_cnt_ref_27.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_binding_call.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_data_read.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_data_read_2.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_pause_write.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_resume_write.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_schedule_write_1.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_schedule_write_2.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_stop_write.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/sem_ps_subscription_call.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/cem/ohpcf/send/use_case_data_reply.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

using testing::_;
using testing::Return;

MATCHER_P(EebusDurationEq, expected, "") {
  if ((arg == nullptr) || (expected == nullptr)) {
    return arg == expected;
  }

  return EebusDurationCompare(arg, expected) == 0;
}

MATCHER_P(OptionalPowerConsumptionEq, expected, "") {
  if ((arg == nullptr) || (expected == nullptr)) {
    return arg == expected;
  }

  bool match = true;

  match = match && (arg->max_power_w.value == expected->max_power_w.value);
  match = match && (arg->max_power_w.scale == expected->max_power_w.scale);
  match = match && (EebusDurationCompare(&arg->earliest_start_time, &expected->earliest_start_time) == 0);
  match = match && (EebusDurationCompare(&arg->latest_end_time, &expected->latest_end_time) == 0);
  match = match && (EebusDurationCompare(&arg->active_duration_min, &expected->active_duration_min) == 0);
  match = match && (arg->is_stoppable == expected->is_stoppable);
  match = match && (arg->is_pausable == expected->is_pausable);

  return match;
}

namespace cem_ohpcf_test {

class CemOhpcfTestFixture : public UseCaseTestFixture {
 public:
  CemOhpcfTestFixture() : UseCaseTestFixture("HEMS", "HEMS", "123456789") {};
  void SetUpUseCase() override {
    // Create the device entities and add it to the SPINE device
    uint32_t entity_ids[1] = {static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeCEM,
        entity_ids,
        ARRAY_SIZE(entity_ids),
        kHeartbeatTimeout
    );

    cem_ohpcf_listener_mock_.reset(CemOhpcfListenerMockCreate());
    use_case_.reset(CemOhpcfUseCaseCreate(entity, CEM_OHPCF_LISTENER_OBJECT(cem_ohpcf_listener_mock_.get())));

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);
    ExpectSendMessage(send::discovery_read);
  };

  void TearDownUseCase() override {
    EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    use_case_.reset();
    cem_ohpcf_listener_mock_.reset();
  };

 protected:
  std::unique_ptr<CemOhpcfListenerMock, decltype(&CemOhpcfListenerMockDelete)> cem_ohpcf_listener_mock_{
      nullptr,
      CemOhpcfListenerMockDelete
  };

  std::unique_ptr<CemOhpcfUseCaseObject, decltype(&CemOhpcfUseCaseDelete)> use_case_{nullptr, CemOhpcfUseCaseDelete};
};

TEST_F(CemOhpcfTestFixture, CemOhpcfTest) {
  // 1. Receive the detailed discovery request and send the response
  ExpectSendMessage(send::discovery_reply);
  HandleMessage(receive::discovery_request);

  // 2. Receive the detailed discovery response and send subscriptions + use case read
  ExpectSendMessage(send::node_management_subscription_call);
  ExpectSendMessage(send::use_case_data_read);
  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnRemoteCompressorAdded(_, _)).WillOnce(Return());
  HandleMessage(receive::discovery_response);

  // 3. Receive the Node Management subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_27);
  HandleMessage(receive::node_management_subscription_request);

  // 4. Receive the result with message counter reference 3
  HandleMessage(receive::result_data_msg_cnt_ref_3);

  // 5. Receive the Use Case reply and send subscriptions + read
  ExpectSendMessage(send::sem_ps_subscription_call);
  ExpectSendMessage(send::sem_ps_binding_call);
  ExpectSendMessage(send::sem_ps_data_read);
  HandleMessage(receive::use_case_reply);

  // 6. Receive the use case discovery request and send the reply
  ExpectSendMessage(send::use_case_data_reply);
  HandleMessage(receive::use_case_request);

  // 7. Receive the smart energy management read reply
  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnClearProcess(_, _)).WillOnce(Return());
  HandleMessage(receive::sem_ps_read_reply);

  // 8. Receive the result with message counter reference 5
  HandleMessage(receive::result_data_msg_cnt_ref_5);

  // 9. Receive the result with message counter reference 6
  HandleMessage(receive::result_data_msg_cnt_ref_6);

  //-------------------------------------------------------------------------------------------//
  // 10. Compressor announces optional power consumption and
  // receives the schedule write request from Compressor and send the response.
  // Compressor reports states kCemOhpcfStateScheduled, kCemOhpcfStateRunning,
  // kCemOhpcfStateCompleted and clears the process afterwards.
  //-------------------------------------------------------------------------------------------//
  const OptionalPowerConsumption optional_power_consumption = {
      .max_power_w         = {2000, 0},
      .earliest_start_time = {0},
      .latest_end_time     = {.days = 1},
      .active_duration_min = {.minutes = 3},
      .is_stoppable        = true,
      .is_pausable         = false,
  };

  EXPECT_CALL(
      *cem_ohpcf_listener_mock_->gmock,
      OnAnnounce(_, OptionalPowerConsumptionEq(&optional_power_consumption), _)
  )
      .WillOnce(Return());
  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateAnnounced, nullptr, _))
      .WillOnce(Return());
  HandleMessage(receive::sem_ps_announce_notify_1);

  static constexpr uint32_t remote_entity_id     = 3;
  static constexpr uint32_t remote_sub_entity_id = 1;

  static constexpr const uint32_t* const remote_entity_ids[] = {&remote_entity_id, &remote_sub_entity_id};

  static constexpr EntityAddressType remote_entity_addr
      = {"d:_n:HeatPump_123456789", remote_entity_ids, ARRAY_SIZE(remote_entity_ids)};

  static constexpr EebusDuration start_time = {.seconds = 10};

  ExpectSendMessage(send::sem_ps_schedule_write_1);
  EXPECT_EQ(
      CemOhpcfScheduleOptionalPowerConsumption(use_case_.get(), &remote_entity_addr, &start_time, nullptr, nullptr),
      kEebusErrorOk
  );

  static constexpr EebusDuration compressor_start_time = {.seconds = 10};
  EXPECT_CALL(
      *cem_ohpcf_listener_mock_->gmock,
      OnStateReport(_, kCompressorOhpcfStateScheduled, EebusDurationEq(&compressor_start_time), _)
  )
      .WillOnce(Return());

  HandleMessage(receive::sem_ps_report_state_scheduled_notify_1);
  HandleMessage(receive::result_data_msg_cnt_ref_10);

  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateRunning, nullptr, _))
      .WillOnce(Return());
  HandleMessage(receive::sem_ps_report_state_running_notify_1);

  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateCompleted, nullptr, _))
      .WillOnce(Return());

  HandleMessage(receive::sem_ps_report_state_completed_notify);

  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnClearProcess(_, _)).WillOnce(Return());
  HandleMessage(receive::sem_ps_clear_process_notify_1);

  //-------------------------------------------------------------------------------------------//
  // 11. Compressor announces optional power consumption, CEM sends the schedule write request.
  // Compressor reports states kCemOhpcfStateScheduled, kCemOhpcfStateRunning.
  // CEM sends the pause/resume/stop commands.
  // Finally Compressor reports state kCemOhpcfStateCompleted and clears the process afterwards.
  //-------------------------------------------------------------------------------------------//
  const OptionalPowerConsumption optional_power_consumption_2 = {
      .max_power_w         = {3000, 0},
      .earliest_start_time = {.minutes = 5},
      .latest_end_time     = {.days = 1},
      .active_duration_min = {.minutes = 10},
      .is_stoppable        = true,
      .is_pausable         = false,
  };

  EXPECT_CALL(
      *cem_ohpcf_listener_mock_->gmock,
      OnAnnounce(_, OptionalPowerConsumptionEq(&optional_power_consumption_2), _)
  )
      .WillOnce(Return());
  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateAnnounced, nullptr, _))
      .WillOnce(Return());
  HandleMessage(receive::sem_ps_announce_notify_2);

  static constexpr EebusDuration start_time_2 = {.minutes = 5};

  ExpectSendMessage(send::sem_ps_schedule_write_2);
  EXPECT_EQ(
      CemOhpcfScheduleOptionalPowerConsumption(use_case_.get(), &remote_entity_addr, &start_time_2, nullptr, nullptr),
      kEebusErrorOk
  );

  static constexpr EebusDuration compressor_start_time_2 = {.minutes = 5};
  EXPECT_CALL(
      *cem_ohpcf_listener_mock_->gmock,
      OnStateReport(_, kCompressorOhpcfStateScheduled, EebusDurationEq(&compressor_start_time_2), _)
  )
      .WillOnce(Return());

  HandleMessage(receive::sem_ps_report_state_scheduled_notify_2);
  HandleMessage(receive::result_data_msg_cnt_ref_11);

  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateRunning, nullptr, _))
      .WillOnce(Return());
  HandleMessage(receive::sem_ps_report_state_running_notify_2);

  ExpectSendMessage(send::sem_ps_pause_write);
  EXPECT_EQ(CemOhpcfWritePauseCommand(use_case_.get(), &remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStatePaused, nullptr, _))
      .WillOnce(Return());
  HandleMessage(receive::sem_ps_pause_notify);
  HandleMessage(receive::result_data_msg_cnt_ref_12);

  ExpectSendMessage(send::sem_ps_resume_write);
  EXPECT_EQ(CemOhpcfWriteResumeCommand(use_case_.get(), &remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnStateReport(_, kCompressorOhpcfStateRunning, nullptr, _))
      .WillOnce(Return());
  HandleMessage(receive::sem_ps_running_notify);
  HandleMessage(receive::result_data_msg_cnt_ref_13);

  ExpectSendMessage(send::sem_ps_stop_write);
  EXPECT_EQ(CemOhpcfWriteStopCommand(use_case_.get(), &remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnClearProcess(_, _)).WillOnce(Return());
  HandleMessage(receive::sem_ps_clear_process_notify_2);
  HandleMessage(receive::result_data_msg_cnt_ref_14);

  // 12. Explicitly read smart energy management PS data → CEM sends READ at msgCounter 15
  ExpectSendMessage(send::sem_ps_data_read_2);
  EXPECT_EQ(CemOhpcfReadSmartData(use_case_.get(), &remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  // 13. Receive the reply → OnClearProcess fires (nodeRemoteControllable=true, alternativesCount=0)
  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnClearProcess(_, _)).WillOnce(Return());
  HandleMessage(receive::sem_ps_read_reply_ref_15);

  // 14. Expect the remote compressor removed callback while tearing down the use case
  EXPECT_CALL(*cem_ohpcf_listener_mock_->gmock, OnRemoteCompressorRemoved(_, _));
}

}  // namespace cem_ohpcf_test
