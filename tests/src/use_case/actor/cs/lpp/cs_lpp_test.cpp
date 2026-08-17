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

#include "src/use_case/actor/cs/lpp/cs_lpp.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/cs_lp_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/use_case/actor/cs/lpp/receive/device_configuration_binding_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/device_configuration_description_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/device_configuration_key_value_list_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/device_configuration_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/device_diagnosis_heartbeat_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/device_diagnosis_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/discovery_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/discovery_response.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/electrical_connection_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/failsafe_duration_write.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/failsafe_invalid_long_duration_write.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/failsafe_invalid_short_duration_write.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/failsafe_negative_power_limit_write.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/failsafe_power_limit_write.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/heartbeat_notify.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/limits_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/limits_write.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/limits_write_delete_duration.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/load_control_binding_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/load_control_description_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/load_control_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/negative_limits_write.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/cs/lpp/receive/use_case_request.inc"
#include "tests/src/use_case/actor/cs/lpp/send/device_configuration_description_reply.inc"
#include "tests/src/use_case/actor/cs/lpp/send/device_configuration_key_value_list_reply.inc"
#include "tests/src/use_case/actor/cs/lpp/send/device_diagnosis_heartbeat_notify.inc"
#include "tests/src/use_case/actor/cs/lpp/send/device_diagnosis_heartbeat_notify_second.inc"
#include "tests/src/use_case/actor/cs/lpp/send/device_diagnosis_heartbeat_read.inc"
#include "tests/src/use_case/actor/cs/lpp/send/device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/cs/lpp/send/discovery_read.inc"
#include "tests/src/use_case/actor/cs/lpp/send/discovery_reply.inc"
#include "tests/src/use_case/actor/cs/lpp/send/electrical_connection_characteristic_notify.inc"
#include "tests/src/use_case/actor/cs/lpp/send/failsafe_duration_notify.inc"
#include "tests/src/use_case/actor/cs/lpp/send/failsafe_power_limit_notify.inc"
#include "tests/src/use_case/actor/cs/lpp/send/limits_notify.inc"
#include "tests/src/use_case/actor/cs/lpp/send/limits_notify_no_duration.inc"
#include "tests/src/use_case/actor/cs/lpp/send/limits_reply.inc"
#include "tests/src/use_case/actor/cs/lpp/send/load_control_description_reply.inc"
#include "tests/src/use_case/actor/cs/lpp/send/load_control_subscription_call.inc"
#include "tests/src/use_case/actor/cs/lpp/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_11.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_12.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_14.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_15.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_20.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_21.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_22.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_23.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_24.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_25.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_26.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_27.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/cs/lpp/send/result_data_msg_cnt_ref_9.inc"
#include "tests/src/use_case/actor/cs/lpp/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/cs/lpp/send/use_case_data_reply.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

namespace cs_lpp_test {

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

class CsLppTestFixture : public UseCaseTestFixture {
 public:
  CsLppTestFixture() : UseCaseTestFixture("HeatPump", "HeatPump", "123456789") {};

  void SetUpUseCase() override {
    uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeInverter,
        entity_ids,
        ARRAY_SIZE(entity_ids),
        kHeartbeatTimeout
    );

    cs_lpp_listener_mock_.reset(CsLpListenerMockCreate());
    use_case_.reset(CsLppUseCaseCreate(entity, 0, CS_LP_LISTENER_OBJECT(cs_lpp_listener_mock_.get())));
    const ScaledValue initial_limit = {4200, 0};
    CsLppSetActiveProductionPowerLimit(use_case_.get(), &initial_limit, false, true);

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

    ExpectSendMessage(send::discovery_read);
  };

  void TearDownUseCase() override {
    EXPECT_CALL(*cs_lpp_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    use_case_.reset();
    cs_lpp_listener_mock_.reset();
  };

  void ExpectSendHeartbeat(const char* expected_json) {
    if (IsLogMessagesEnabled()) {
      EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, HeartbeatMsgEq(expected_json), _))
          .WillOnce(WithArgs<1, 2>(Invoke(LogMessageSend)));
    } else {
      EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, HeartbeatMsgEq(expected_json), _)).WillOnce(Return());
    }
  }

  void SetUpRemoteConnection() {
    // 1. Receive the detailed discovery request and send the response
    ExpectSendMessage(send::discovery_reply);
    HandleMessage(receive::discovery_request);

    // 2. Receive the detailed discovery response and send subscription call + use case read
    ExpectSendMessage(send::node_management_subscription_call);
    ExpectSendMessage(send::use_case_data_read);
    HandleMessage(receive::discovery_response);

    // 3. Receive the Node Management subscription request and send result
    ExpectSendMessage(send::result_data_msg_cnt_ref_3);
    HandleMessage(receive::node_management_subscription_request);

    // 4. Receive the result with message counter reference 3
    HandleMessage(receive::result_data_msg_cnt_ref_3);

    // 5. Receive the Use Case reply and send LoadControl subscription + heartbeat read
    ExpectSendMessage(send::load_control_subscription_call);
    ExpectSendMessage(send::device_diagnosis_heartbeat_read);
    EXPECT_CALL(*cs_lpp_listener_mock_->gmock, OnRemoteEgAdded(_, _)).WillOnce(Return());
    HandleMessage(receive::use_case_reply);

    // 6. Receive the result with message counter reference 5
    HandleMessage(receive::result_data_msg_cnt_ref_5);

    // 7. Receive the use case discovery request and send the reply
    ExpectSendMessage(send::use_case_data_reply);
    HandleMessage(receive::use_case_request);

    // 8. Receive the load control subscription request and send result
    ExpectSendMessage(send::result_data_msg_cnt_ref_8);
    HandleMessage(receive::load_control_subscription_request);

    // 9. Receive the load control binding request and send result
    ExpectSendMessage(send::result_data_msg_cnt_ref_9);
    HandleMessage(receive::load_control_binding_request);

    // 10. Receive the load control description read request and send the reply
    ExpectSendMessage(send::load_control_description_reply);
    HandleMessage(receive::load_control_description_request);

    // 11. Receive the device configuration subscription request and send result
    ExpectSendMessage(send::result_data_msg_cnt_ref_11);
    HandleMessage(receive::device_configuration_subscription_request);

    // 12. Receive the device configuration binding request and send result
    ExpectSendMessage(send::result_data_msg_cnt_ref_12);
    HandleMessage(receive::device_configuration_binding_request);

    // 13. Receive the device configuration description request and send the reply
    ExpectSendMessage(send::device_configuration_description_reply);
    HandleMessage(receive::device_configuration_description_request);

    // 14. Receive the Device Diagnosis subscription request and send result
    ExpectSendMessage(send::result_data_msg_cnt_ref_14);
    HandleMessage(receive::device_diagnosis_subscription_request);

    // 15. Receive the Electrical Connection subscription request and send result
    ExpectSendMessage(send::result_data_msg_cnt_ref_15);
    HandleMessage(receive::electrical_connection_subscription_request);

    // 16. Receive the Heartbeat subscription request and send the reply
    ExpectSendHeartbeat(send::device_diagnosis_heartbeat_reply);
    HandleMessage(receive::device_diagnosis_heartbeat_request);

    // 17. Receive the Heartbeat reply
    HandleMessage(receive::device_diagnosis_heartbeat_reply);

    // 18. Receive the Limits request and send the reply
    ExpectSendMessage(send::limits_reply);
    HandleMessage(receive::limits_request);

    // 19. Receive the Device Configuration Key Value List request and send the reply
    ExpectSendMessage(send::device_configuration_key_value_list_reply);
    HandleMessage(receive::device_configuration_key_value_list_request);

    // 20. Wait for the Heartbeat notify been sent
    ExpectSendHeartbeat(send::device_diagnosis_heartbeat_notify);
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }
  }

  void VerifyActivePowerLimitWriteValid() {
    ExpectSendMessage(send::limits_notify);
    ExpectSendMessage(send::result_data_msg_cnt_ref_20);

    EXPECT_CALL(
        *cs_lpp_listener_mock_->gmock,
        OnPowerLimitReceive(_, ScaledValueEq(100, 0), DurationTypeEq(1, 2, 3), true)
    );

    HandleMessage(receive::limits_write);

    LoadLimit limit{{0}};
    EXPECT_EQ(CsLppGetActiveProductionPowerLimit(use_case_.get(), &limit), kEebusErrorOk);
    EXPECT_THAT(&limit.value, ScaledValueEq(100, 0));
  }

  void VerifyActivePowerLimitWriteInvalid() {
    ExpectSendMessage(send::result_data_msg_cnt_ref_21);
    HandleMessage(receive::negative_limits_write);

    LoadLimit limit{{0}};
    EXPECT_EQ(CsLppGetActiveProductionPowerLimit(use_case_.get(), &limit), kEebusErrorOk);
    EXPECT_THAT(&limit.value, ScaledValueEq(100, 0));
  }

  void VerifyFailsafePowerLimitWriteValid() {
    ExpectSendMessage(send::failsafe_power_limit_notify);
    ExpectSendMessage(send::result_data_msg_cnt_ref_22);
    EXPECT_CALL(*cs_lpp_listener_mock_->gmock, OnFailsafePowerLimitReceive(_, ScaledValueEq(14, 1)));

    HandleMessage(receive::failsafe_power_limit_write);

    ScaledValue failsafe_limit{0};
    bool is_changeable{false};
    EXPECT_EQ(
        CsLppGetFailsafeProductionActivePowerLimit(use_case_.get(), &failsafe_limit, &is_changeable),
        kEebusErrorOk
    );
    EXPECT_THAT(&failsafe_limit, ScaledValueEq(14, 1));
  }

  void VerifyFailsafePowerLimitWriteInvalid() {
    ExpectSendMessage(send::result_data_msg_cnt_ref_23);

    HandleMessage(receive::failsafe_negative_power_limit_write);

    ScaledValue failsafe_limit{0};
    bool is_changeable{false};
    EXPECT_EQ(
        CsLppGetFailsafeProductionActivePowerLimit(use_case_.get(), &failsafe_limit, &is_changeable),
        kEebusErrorOk
    );
    EXPECT_THAT(&failsafe_limit, ScaledValueEq(14, 1));
  }

  void VerifyFailsafeDurationWriteValid() {
    ExpectSendMessage(send::failsafe_duration_notify);
    ExpectSendMessage(send::result_data_msg_cnt_ref_24);
    EXPECT_CALL(*cs_lpp_listener_mock_->gmock, OnFailsafeDurationReceive(_, DurationTypeEq(2, 2, 5)));

    HandleMessage(receive::failsafe_duration_write);

    DurationType failsafe_duration{0};
    bool is_changeable{false};
    EXPECT_EQ(CsLppGetFailsafeDurationMinimum(use_case_.get(), &failsafe_duration, &is_changeable), kEebusErrorOk);
    EXPECT_THAT(&failsafe_duration, DurationTypeEq(2, 2, 5));
  }

  void VerifyFailsafeDurationWriteInvalid(const char* datagram, const char* expected_result_msg) {
    ExpectSendMessage(expected_result_msg);
    HandleMessage(datagram);

    DurationType failsafe_duration{0};
    bool is_changeable{false};
    EXPECT_EQ(CsLppGetFailsafeDurationMinimum(use_case_.get(), &failsafe_duration, &is_changeable), kEebusErrorOk);
    EXPECT_THAT(&failsafe_duration, DurationTypeEq(2, 2, 5));
  }

  void VerifyProductionNominalMax() {
    ExpectSendMessage(send::electrical_connection_characteristic_notify);

    const ScaledValue production_nominal_max_set{700, 1};
    CsLppSetProductionNominalMax(use_case_.get(), &production_nominal_max_set);

    ScaledValue production_nominal_max_get{0, 0};
    EXPECT_EQ(CsLppGetProductionNominalMax(use_case_.get(), &production_nominal_max_get), kEebusErrorOk);
    EXPECT_THAT(&production_nominal_max_get, ScaledValueEq(700, 1));
  }

  void VerifyActivePowerLimitWriteNullDuration() {
    ExpectSendMessage(send::limits_notify_no_duration);
    ExpectSendMessage(send::result_data_msg_cnt_ref_27);

    EXPECT_CALL(*cs_lpp_listener_mock_->gmock, OnPowerLimitReceive(_, ScaledValueEq(200, 0), testing::IsNull(), true));

    HandleMessage(receive::limits_write_delete_duration);

    LoadLimit limit{};
    EXPECT_EQ(CsLppGetActiveProductionPowerLimit(use_case_.get(), &limit), kEebusErrorOk);
    EXPECT_THAT(&limit.value, ScaledValueEq(200, 0));
    EXPECT_TRUE(limit.delete_duration);
  }

  void VerifyHeartbeat() {
    EXPECT_CALL(*cs_lpp_listener_mock_->gmock, OnHeartbeatReceive(_, _)).WillOnce(Return());
    HandleMessage(receive::heartbeat_notify);
    EXPECT_TRUE(CsLppIsHeartbeatWithinDuration(use_case_.get()));
  }

  void VerifyHeartbeatStopStart() {
    CsLppStopHeartbeat(use_case_.get());
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }

    ExpectSendHeartbeat(send::device_diagnosis_heartbeat_notify_second);
    CsLppStartHeartbeat(use_case_.get());
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }
  }

 protected:
  std::unique_ptr<CsLpListenerMock, decltype(&CsLpListenerMockDelete)> cs_lpp_listener_mock_{
      nullptr,
      CsLpListenerMockDelete
  };

  std::unique_ptr<CsLpUseCaseObject, decltype(&CsLpUseCaseDelete)> use_case_{nullptr, CsLpUseCaseDelete};
};

TEST_F(CsLppTestFixture, CsLppTest) {
  // 1-20. Set up the remote connection by processing the incoming messages in the right order
  SetUpRemoteConnection();

  // 21. Verify that the valid power limit write is processed correctly and updates the active power limit value
  VerifyActivePowerLimitWriteValid();

  // 22. Verify that the negative power limit write was rejected and did not update the active power limit value
  VerifyActivePowerLimitWriteInvalid();

  // 23. Verify that the valid failsafe power limit write is processed correctly
  VerifyFailsafePowerLimitWriteValid();

  // 24. Verify the negative power limit write was rejected and did not update the failsafe power limit value
  VerifyFailsafePowerLimitWriteInvalid();

  // 25. Verify that the valid failsafe duration write is processed correctly
  VerifyFailsafeDurationWriteValid();

  // 26. Verify that the too short failsafe duration write is rejected and does not update the failsafe duration
  VerifyFailsafeDurationWriteInvalid(receive::failsafe_invalid_short_duration_write, send::result_data_msg_cnt_ref_25);

  // 27. Verify that the too long failsafe duration write is rejected and does not update the failsafe duration
  VerifyFailsafeDurationWriteInvalid(receive::failsafe_invalid_long_duration_write, send::result_data_msg_cnt_ref_26);

  // 28. Verify that the production nominal max can be set and read back correctly
  VerifyProductionNominalMax();

  // 29. Verify that the Heartbeat message is received and processed correctly
  VerifyHeartbeat();

  // 30. Verify that a write with an empty timePeriod triggers OnPowerLimitReceive with null duration
  VerifyActivePowerLimitWriteNullDuration();

  // 31. Verify that stopping the heartbeat suppresses further NOTIFYs and re-starting resumes them
  VerifyHeartbeatStopStart();

  // 32. Expect the remote EG disconnect event while tearing down the use case
  EXPECT_CALL(*cs_lpp_listener_mock_->gmock, OnRemoteEgRemoved(_, _));
}

}  // namespace cs_lpp_test
