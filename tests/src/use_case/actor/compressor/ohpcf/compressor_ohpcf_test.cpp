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

#include "src/use_case/actor/compressor/ohpcf/compressor_ohpcf.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/compressor_ohpcf_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/discovery_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/discovery_response.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_binding_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_pause_write_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_read_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_resume_write_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_schedule_write_request_1.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_schedule_write_request_2.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_schedule_write_request_3.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_schedule_write_request_4.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_stop_write_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/sem_ps_subscription_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/receive/use_case_request.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/discovery_read.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/discovery_reply.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/result_data_msg_cnt_ref_14.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/result_data_msg_cnt_ref_15.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/result_data_msg_cnt_ref_16.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/result_data_msg_cnt_ref_17.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/result_data_msg_cnt_ref_21.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/result_data_msg_cnt_ref_22.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/result_data_msg_cnt_ref_9.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_data_announce.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_data_announce_2.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_data_announce_3.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_data_reply.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_notify_invalid.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_notify_paused.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_notify_running.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_notify_schedule_1.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_notify_schedule_2.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_notify_schedule_3.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_notify_schedule_4.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_process_cleared.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_process_cleared_2.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_process_cleared_3.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_state_completed.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_state_completed_3.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_state_running.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_state_running_2.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_state_running_3.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_state_scheduled.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_state_scheduled_2.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/sem_ps_state_scheduled_3.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/compressor/ohpcf/send/use_case_data_reply.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

using testing::_;
using testing::Return;

namespace compressor_ohpcf_test {

class CompressorOhpcfTestFixture : public UseCaseTestFixture {
 public:
  CompressorOhpcfTestFixture() : UseCaseTestFixture("HeatGenerationSystem", "HeatPump", "123456789") {};
  void SetUpUseCase() override {
    uint32_t heat_pump_entity_ids[1]
        = {static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const heat_pump_entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeHeatPumpAppliance,
        heat_pump_entity_ids,
        ARRAY_SIZE(heat_pump_entity_ids),
        kHeartbeatTimeout
    );

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), heat_pump_entity);

    uint32_t ohpcf_entity_ids[2] = {heat_pump_entity_ids[0], 1};

    EntityLocalObject* const compressor_entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeCompressor,
        ohpcf_entity_ids,
        ARRAY_SIZE(ohpcf_entity_ids),
        kHeartbeatTimeout
    );

    cp_ohpcf_listener_mock_.reset(CompressorOhpcfListenerMockCreate());
    use_case_.reset(
        CompressorOhpcfUseCaseCreate(compressor_entity, COMPRESSOR_OHPCF_LISTENER_OBJECT(cp_ohpcf_listener_mock_.get()))
    );

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), compressor_entity);
    ExpectSendMessage(send::discovery_read);
  };

  void TearDownUseCase() override {
    EXPECT_CALL(*cp_ohpcf_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    use_case_.reset();
    cp_ohpcf_listener_mock_.reset();
  };

 protected:
  std::unique_ptr<CompressorOhpcfListenerMock, decltype(&CompressorOhpcfListenerMockDelete)> cp_ohpcf_listener_mock_{
      nullptr,
      CompressorOhpcfListenerMockDelete
  };

  std::unique_ptr<CompressorOhpcfUseCaseObject, decltype(&CompressorOhpcfUseCaseDelete)> use_case_{
      nullptr,
      CompressorOhpcfUseCaseDelete
  };
};

TEST_F(CompressorOhpcfTestFixture, CompressorOhpcfTest) {
  // 1. Receive the detailed discovery request and send the response
  ExpectSendMessage(send::discovery_reply);
  HandleMessage(receive::discovery_request);

  // 2. Receive the detailed discovery response and send subscriptions + use case read
  ExpectSendMessage(send::node_management_subscription_call);
  ExpectSendMessage(send::use_case_data_read);
  HandleMessage(receive::discovery_response);

  // 3. Receive the Node Management subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_9);
  HandleMessage(receive::node_management_subscription_request);

  // 4. Receive the use case discovery request and send the reply
  ExpectSendMessage(send::use_case_data_reply);
  HandleMessage(receive::use_case_request);

  // 5. Receive the result with message counter reference 3
  HandleMessage(receive::result_data_msg_cnt_ref_3);

  // 6. Receive the Use Case reply
  HandleMessage(receive::use_case_reply);

  // 7. Receive the smart energy management read request and send the response
  ExpectSendMessage(send::sem_ps_data_reply);
  HandleMessage(receive::sem_ps_read_request);

  // 8. Receive the smart energy management subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_14);
  HandleMessage(receive::sem_ps_subscription_request);

  // 9. Receive the smart energy management binding request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_15);
  HandleMessage(receive::sem_ps_binding_request);

  //-------------------------------------------------------------------------------------------//
  // 10. Compressor announces optional power consumption and
  // receives the sem ps schedule write request and send the response.
  // Compressor reports states kCompressorOhpcfStateScheduled, kCompressorOhpcfStateRunning,
  // kCompressorOhpcfStateCompleted and clears the process afterwards.
  //-------------------------------------------------------------------------------------------//
  const OptionalPowerConsumption optional_power_consumption = {
      .max_power_w         = {2000, 0},
      .earliest_start_time = {0},
      .latest_end_time     = {.days = 1},
      .active_duration_min = {.minutes = 3},
      .is_stoppable        = true,
      .is_pausable         = false,
  };

  ExpectSendMessage(send::sem_ps_data_announce);
  CompressorOhpcfAnnounce(use_case_.get(), &optional_power_consumption);

  ExpectSendMessage(send::sem_ps_notify_schedule_1);
  ExpectSendMessage(send::result_data_msg_cnt_ref_16);
  EXPECT_CALL(*cp_ohpcf_listener_mock_->gmock, OnScheduleOptionalPowerConsumption(_, _)).WillOnce(Return());
  HandleMessage(receive::sem_ps_schedule_write_request_1);

  const EebusDuration start_time = {.seconds = 9};
  ExpectSendMessage(send::sem_ps_state_scheduled);
  EXPECT_EQ(CompressorOhpcfReportState(use_case_.get(), kCompressorOhpcfStateScheduled, &start_time), kEebusErrorOk);
  ExpectSendMessage(send::sem_ps_state_running);
  EXPECT_EQ(CompressorOhpcfReportState(use_case_.get(), kCompressorOhpcfStateRunning, NULL), kEebusErrorOk);
  ExpectSendMessage(send::sem_ps_state_completed);
  EXPECT_EQ(CompressorOhpcfReportState(use_case_.get(), kCompressorOhpcfStateCompleted, NULL), kEebusErrorOk);
  ExpectSendMessage(send::sem_ps_process_cleared);
  EXPECT_EQ(CompressorOhpcfClearProcess(use_case_.get()), kEebusErrorOk);

  //-------------------------------------------------------------------------------------------//
  // 11. Compressor announces optional power consumption and receives the schedule write request.
  // Compressor reports states kCompressorOhpcfStateScheduled, kCompressorOhpcfStateRunning.
  // Then receive the pause/resume/stop commands.
  // Finally reports state kCompressorOhpcfStateCompleted and clears the process afterwards.
  //-------------------------------------------------------------------------------------------//
  ExpectSendMessage(send::sem_ps_data_announce_2);
  CompressorOhpcfAnnounce(use_case_.get(), &optional_power_consumption);

  ExpectSendMessage(send::sem_ps_notify_schedule_2);
  ExpectSendMessage(send::result_data_msg_cnt_ref_17);
  EXPECT_CALL(*cp_ohpcf_listener_mock_->gmock, OnScheduleOptionalPowerConsumption(_, _)).WillOnce(Return());
  HandleMessage(receive::sem_ps_schedule_write_request_2);

  ExpectSendMessage(send::sem_ps_state_scheduled_2);
  EXPECT_EQ(CompressorOhpcfReportState(use_case_.get(), kCompressorOhpcfStateScheduled, &start_time), kEebusErrorOk);
  ExpectSendMessage(send::sem_ps_state_running_2);
  EXPECT_EQ(CompressorOhpcfReportState(use_case_.get(), kCompressorOhpcfStateRunning, NULL), kEebusErrorOk);

  ExpectSendMessage(send::sem_ps_notify_paused);
  EXPECT_CALL(*cp_ohpcf_listener_mock_->gmock, OnPause(_)).WillOnce(Return());
  HandleMessage(receive::sem_ps_pause_write_request);

  ExpectSendMessage(send::sem_ps_notify_running);
  EXPECT_CALL(*cp_ohpcf_listener_mock_->gmock, OnResume(_)).WillOnce(Return());
  HandleMessage(receive::sem_ps_resume_write_request);

  ExpectSendMessage(send::sem_ps_notify_invalid);
  EXPECT_CALL(*cp_ohpcf_listener_mock_->gmock, OnStop(_)).WillOnce(Return());
  HandleMessage(receive::sem_ps_stop_write_request);

  ExpectSendMessage(send::sem_ps_process_cleared_2);
  EXPECT_EQ(CompressorOhpcfClearProcess(use_case_.get()), kEebusErrorOk);

  //-------------------------------------------------------------------------------------------//
  // 12. Compressor announces optional power consumption and receives the schedule write request.
  // Then schedule is overwritten before starting.
  // Compressor reports states kCompressorOhpcfStateScheduled, kCompressorOhpcfStateRunning,
  // kCompressorOhpcfStateCompleted.
  // Finally reports state kCompressorOhpcfStateCompleted and clears the process afterwards.
  //-------------------------------------------------------------------------------------------//
  ExpectSendMessage(send::sem_ps_data_announce_3);
  EXPECT_EQ(CompressorOhpcfAnnounce(use_case_.get(), &optional_power_consumption), kEebusErrorOk);

  ExpectSendMessage(send::sem_ps_notify_schedule_3);
  ExpectSendMessage(send::result_data_msg_cnt_ref_21);
  EXPECT_CALL(*cp_ohpcf_listener_mock_->gmock, OnScheduleOptionalPowerConsumption(_, _)).WillOnce(Return());
  HandleMessage(receive::sem_ps_schedule_write_request_3);

  ExpectSendMessage(send::sem_ps_notify_schedule_4);
  ExpectSendMessage(send::result_data_msg_cnt_ref_22);
  EXPECT_CALL(*cp_ohpcf_listener_mock_->gmock, OnScheduleOptionalPowerConsumption(_, _)).WillOnce(Return());
  HandleMessage(receive::sem_ps_schedule_write_request_4);

  ExpectSendMessage(send::sem_ps_state_scheduled_3);
  EXPECT_EQ(CompressorOhpcfReportState(use_case_.get(), kCompressorOhpcfStateScheduled, &start_time), kEebusErrorOk);
  ExpectSendMessage(send::sem_ps_state_running_3);
  EXPECT_EQ(CompressorOhpcfReportState(use_case_.get(), kCompressorOhpcfStateRunning, NULL), kEebusErrorOk);
  ExpectSendMessage(send::sem_ps_state_completed_3);
  EXPECT_EQ(CompressorOhpcfReportState(use_case_.get(), kCompressorOhpcfStateCompleted, NULL), kEebusErrorOk);
  ExpectSendMessage(send::sem_ps_process_cleared_3);
  EXPECT_EQ(CompressorOhpcfClearProcess(use_case_.get()), kEebusErrorOk);
}

}  // namespace compressor_ohpcf_test
