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

#include "src/use_case/actor/eg/lpp/eg_lpp.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/eg_lp_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/use_case/actor/eg/lpp/receive/device_configuration_description_reply.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/device_configuration_key_value_list_reply.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/device_configuration_key_value_list_reply_ref_26.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/device_configuration_key_value_list_reply_ref_27.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/device_diagnosis_heartbeat_request.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/device_diagnosis_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/discovery_request.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/discovery_response.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/electrical_connection_characteristic_reply.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/electrical_connection_characteristic_reply_ref_28.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/heartbeat_notify.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/limits_description_reply.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/limits_reply.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/limits_reply_ref_25.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_11.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_22.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_23.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_24.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_6.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/result_data_msg_cnt_ref_9.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/eg/lpp/receive/use_case_request.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_configuration_binding_call.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_configuration_description_read.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_configuration_key_value_list_read.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_configuration_key_value_list_read_2.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_configuration_key_value_list_read_3.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_configuration_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_diagnosis_heartbeat_notify.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_diagnosis_heartbeat_read.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/eg/lpp/send/device_diagnosis_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpp/send/discovery_read.inc"
#include "tests/src/use_case/actor/eg/lpp/send/discovery_reply.inc"
#include "tests/src/use_case/actor/eg/lpp/send/electrical_connection_characteristic_read.inc"
#include "tests/src/use_case/actor/eg/lpp/send/electrical_connection_characteristic_read_2.inc"
#include "tests/src/use_case/actor/eg/lpp/send/electrical_connection_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpp/send/failsafe_duration_write.inc"
#include "tests/src/use_case/actor/eg/lpp/send/failsafe_power_limit_write.inc"
#include "tests/src/use_case/actor/eg/lpp/send/limits_write.inc"
#include "tests/src/use_case/actor/eg/lpp/send/load_control_binding_call.inc"
#include "tests/src/use_case/actor/eg/lpp/send/load_control_limit_description_read.inc"
#include "tests/src/use_case/actor/eg/lpp/send/load_control_limit_list_read.inc"
#include "tests/src/use_case/actor/eg/lpp/send/load_control_limit_list_read_2.inc"
#include "tests/src/use_case/actor/eg/lpp/send/load_control_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpp/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpp/send/result_data_msg_cnt_ref_28.inc"
#include "tests/src/use_case/actor/eg/lpp/send/result_data_msg_cnt_ref_31.inc"
#include "tests/src/use_case/actor/eg/lpp/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/eg/lpp/send/use_case_data_reply.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

namespace eg_lpp_test {

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

class EgLppTestFixture : public UseCaseTestFixture {
 public:
  EgLppTestFixture() : UseCaseTestFixture("HEMS", "HEMS", "123456789") {};

  void SetUpUseCase() override {
    uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeGridGuard,
        entity_ids,
        ARRAY_SIZE(entity_ids),
        kHeartbeatTimeout
    );

    eg_lpp_listener_mock_.reset(EgLpListenerMockCreate());
    use_case_.reset(EgLppUseCaseCreate(entity, EG_LP_LISTENER_OBJECT(eg_lpp_listener_mock_.get())));

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

    ExpectSendMessage(send::discovery_read);
  };

  void TearDownUseCase() override {
    EXPECT_CALL(*eg_lpp_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    use_case_.reset();
    eg_lpp_listener_mock_.reset();
  };

  void ExpectSendHeartbeat(const char* expected_json) {
    if (IsLogMessagesEnabled()) {
      EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, HeartbeatMsgEq(expected_json), _))
          .WillOnce(WithArgs<1, 2>(Invoke(LogMessageSend)));
    } else {
      EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, HeartbeatMsgEq(expected_json), _)).WillOnce(Return());
    }
  }

  void VerifyHeartbeat(const EntityAddressType* remote_entity_addr) {
    EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnHeartbeatReceive(_, 1)).WillOnce(Return());
    HandleMessage(receive::heartbeat_notify);
    EXPECT_TRUE(EgLppIsHeartbeatWithinDuration(use_case_.get(), remote_entity_addr));
  }

  void VerifyHeartbeatStopStart() {
    ExpectSendHeartbeat(send::device_diagnosis_heartbeat_notify);
    EgLppStartHeartbeat(use_case_.get());
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }

    EgLppStopHeartbeat(use_case_.get());
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }
  }

 protected:
  std::unique_ptr<EgLpListenerMock, decltype(&EgLpListenerMockDelete)> eg_lpp_listener_mock_{
      nullptr,
      EgLpListenerMockDelete
  };

  std::unique_ptr<EgLpUseCaseObject, decltype(&EgLpUseCaseDelete)> use_case_{nullptr, EgLpUseCaseDelete};
};

TEST_F(EgLppTestFixture, EgLppTest) {
  const EntityAddressType* remote_entity_addr = nullptr;

  // 1. Receive the detailed discovery request and send the response
  ExpectSendMessage(send::discovery_reply);
  HandleMessage(receive::discovery_request);

  // 2. Receive the detailed discovery response and send subscription call + use case read
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnRemoteCsAdded(_, _)).WillOnce(testing::SaveArg<1>(&remote_entity_addr));
  ExpectSendMessage(send::node_management_subscription_call);
  ExpectSendMessage(send::use_case_data_read);
  HandleMessage(receive::discovery_response);

  // 3. Receive the result with message counter reference 5
  HandleMessage(receive::result_data_msg_cnt_ref_5);
  // 4. Receive the result with message counter reference 6
  HandleMessage(receive::result_data_msg_cnt_ref_6);
  // 5. Receive the result with message counter reference 8
  HandleMessage(receive::result_data_msg_cnt_ref_8);
  // 6. Receive the result with message counter reference 9
  HandleMessage(receive::result_data_msg_cnt_ref_9);
  // 7. Receive the result with message counter reference 11
  HandleMessage(receive::result_data_msg_cnt_ref_11);

  // 8. Receive the Node Management subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_28);
  HandleMessage(receive::node_management_subscription_request);

  // 9. Receive the use case discovery request and send the reply
  ExpectSendMessage(send::use_case_data_reply);
  HandleMessage(receive::use_case_request);

  // 10. Receive the Use Case reply and send load control + device configuration + device diagnosis +
  // electrical connection subscriptions
  ExpectSendMessage(send::load_control_subscription_call);
  ExpectSendMessage(send::load_control_binding_call);
  ExpectSendMessage(send::load_control_limit_description_read);
  ExpectSendMessage(send::device_configuration_subscription_call);
  ExpectSendMessage(send::device_configuration_binding_call);
  ExpectSendMessage(send::device_configuration_description_read);
  ExpectSendMessage(send::device_diagnosis_subscription_call);
  ExpectSendMessage(send::device_diagnosis_heartbeat_read);
  ExpectSendMessage(send::electrical_connection_subscription_call);
  ExpectSendMessage(send::electrical_connection_characteristic_read);
  HandleMessage(receive::use_case_reply);

  // 11. Receive the Device Diagnosis subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_31);
  HandleMessage(receive::device_diagnosis_subscription_request);

  // 12. Receive the Heartbeat read request and send the reply
  ExpectSendHeartbeat(send::device_diagnosis_heartbeat_reply);
  HandleMessage(receive::device_diagnosis_heartbeat_request);

  // 13. Receive the result with message counter reference 3
  HandleMessage(receive::result_data_msg_cnt_ref_3);

  // 14. Receive the Load Control Limit Description reply and send the read
  ExpectSendMessage(send::load_control_limit_list_read);
  HandleMessage(receive::limits_description_reply);

  // 15. Receive the Load Control Limit reply (limitId 1 = produce, value 10000W)
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnPowerLimitReceive(_, ScaledValueEq(10000, 0), _, false));
  HandleMessage(receive::limits_reply);

  // 16. Receive the electrical connection characteristic reply — OnPowerProductionNominalMaxReceive
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnPowerNominalMaxReceive(_, ScaledValueEq(12000, 0)));
  HandleMessage(receive::electrical_connection_characteristic_reply);

  // 17. Verify that receiving a CS heartbeat NOTIFY triggers OnHeartbeatReceive and IsHeartbeatWithinDuration
  VerifyHeartbeat(remote_entity_addr);

  // 18. Verify that starting the heartbeat sends NOTIFYs and stopping suppresses them
  VerifyHeartbeatStopStart();

  // 20. Receive DC description reply → EG sends DC key value list READ
  ExpectSendMessage(send::device_configuration_key_value_list_read);
  HandleMessage(receive::device_configuration_description_reply);

  // 21. Receive DC key value list reply → OnFailsafePowerLimitReceive + OnFailsafeDurationReceive
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnFailsafePowerLimitReceive(_, ScaledValueEq(500, 0)));
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnFailsafeDurationReceive(_, DurationTypeEq(2, 0, 0)));
  HandleMessage(receive::device_configuration_key_value_list_reply);

  // 22. Get the active production power limit
  LoadLimit active_limit{};
  EXPECT_EQ(EgLppGetActiveProductionPowerLimit(use_case_.get(), remote_entity_addr, &active_limit), kEebusErrorOk);
  EXPECT_THAT(&active_limit.value, ScaledValueEq(10000, 0));

  // 23. Get the power production nominal max
  ScaledValue nominal_max{};
  EXPECT_EQ(EgLppGetPowerProductionNominalMax(use_case_.get(), remote_entity_addr, &nominal_max), kEebusErrorOk);
  EXPECT_THAT(&nominal_max, ScaledValueEq(12000, 0));

  // 24. Get the failsafe production active power limit
  ScaledValue failsafe_power{};
  EXPECT_EQ(
      EgLppGetFailsafeProductionActivePowerLimit(use_case_.get(), remote_entity_addr, &failsafe_power),
      kEebusErrorOk
  );
  EXPECT_THAT(&failsafe_power, ScaledValueEq(500, 0));

  // 25. Get the failsafe duration minimum
  DurationType failsafe_duration{};
  EXPECT_EQ(EgLppGetFailsafeDurationMinimum(use_case_.get(), remote_entity_addr, &failsafe_duration), kEebusErrorOk);
  EXPECT_THAT(&failsafe_duration, DurationTypeEq(2, 0, 0));

  // 26. Set the failsafe production active power limit → sends WRITE to CS at msgCounter 22
  ExpectSendMessage(send::failsafe_power_limit_write);
  const ScaledValue new_power_limit{600, 0};
  EXPECT_EQ(
      EgLppSetFailsafeProductionActivePowerLimit(
          use_case_.get(),
          remote_entity_addr,
          &new_power_limit,
          nullptr,
          nullptr
      ),
      kEebusErrorOk
  );

  // 27. Receive the result ACK for the failsafe power limit write
  HandleMessage(receive::result_data_msg_cnt_ref_22);

  // 28. Set the failsafe duration minimum → sends WRITE to CS at msgCounter 23
  ExpectSendMessage(send::failsafe_duration_write);
  const EebusDuration new_duration{.hours = 3};
  EXPECT_EQ(
      EgLppSetFailsafeDurationMinimum(use_case_.get(), remote_entity_addr, &new_duration, nullptr, nullptr),
      kEebusErrorOk
  );

  // 29. Receive the result ACK for the failsafe duration write
  HandleMessage(receive::result_data_msg_cnt_ref_23);

  // 30. Set the active production power limit → sends WRITE to CS at msgCounter 24
  ExpectSendMessage(send::limits_write);
  static constexpr LoadLimit new_limit = {
      .value     = {.value = 8000, .scale = 0},
      .is_active = true,
  };

  EXPECT_EQ(
      EgLppSetActiveProductionPowerLimit(use_case_.get(), remote_entity_addr, &new_limit, nullptr, nullptr),
      kEebusErrorOk
  );

  // 31. Receive the result ACK for the active power limit write
  HandleMessage(receive::result_data_msg_cnt_ref_24);

  // 33. Explicitly read active production power limit → EG sends READ at msgCounter 25
  ExpectSendMessage(send::load_control_limit_list_read_2);
  EXPECT_EQ(EgLppReadActiveProductionPowerLimit(use_case_.get(), remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  // 34. Receive the reply → OnPowerLimitReceive with updated values (8000W active, no duration)
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnPowerLimitReceive(_, ScaledValueEq(8000, 0), testing::IsNull(), true));
  HandleMessage(receive::limits_reply_ref_25);

  // 35. Explicitly read failsafe production active power limit → EG sends READ at msgCounter 26
  ExpectSendMessage(send::device_configuration_key_value_list_read_2);
  EXPECT_EQ(
      EgLppReadFailsafeProductionActivePowerLimit(use_case_.get(), remote_entity_addr, nullptr, nullptr),
      kEebusErrorOk
  );

  // 36. Receive the reply → OnFailsafePowerLimitReceive with updated value (600W)
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnFailsafePowerLimitReceive(_, ScaledValueEq(600, 0)));
  HandleMessage(receive::device_configuration_key_value_list_reply_ref_26);

  // 37. Explicitly read failsafe duration minimum → EG sends READ at msgCounter 27
  ExpectSendMessage(send::device_configuration_key_value_list_read_3);
  EXPECT_EQ(EgLppReadFailsafeDurationMinimum(use_case_.get(), remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  // 38. Receive the reply → OnFailsafeDurationReceive with updated value (3h)
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnFailsafeDurationReceive(_, DurationTypeEq(3, 0, 0)));
  HandleMessage(receive::device_configuration_key_value_list_reply_ref_27);

  // 39. Explicitly read power production nominal max → EG sends READ at msgCounter 28
  ExpectSendMessage(send::electrical_connection_characteristic_read_2);
  EXPECT_EQ(EgLppReadPowerProductionNominalMax(use_case_.get(), remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  // 40. Receive the reply → OnPowerNominalMaxReceive with updated value (14000W)
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnPowerNominalMaxReceive(_, ScaledValueEq(14000, 0)));
  HandleMessage(receive::electrical_connection_characteristic_reply_ref_28);

  // 41. Expect the remote entity disconnect event while tearing down the use case
  EXPECT_CALL(*eg_lpp_listener_mock_->gmock, OnRemoteCsRemoved(_, _));
}

}  // namespace eg_lpp_test
