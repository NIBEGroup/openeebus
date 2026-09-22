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

#include "src/use_case/actor/eg/lpc/eg_lpc.h"

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
#include "tests/src/use_case/actor/eg/lpc/receive/device_configuration_description_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/device_configuration_key_value_list_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/device_configuration_key_value_list_reply_ref_27.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/device_configuration_key_value_list_reply_ref_28.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/device_diagnosis_heartbeat_request.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/device_diagnosis_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/discovery_request.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/discovery_response.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/electrical_connection_characteristic_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/electrical_connection_characteristic_reply_ref_29.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/heartbeat_notify.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/limits_description_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/limits_notify_no_duration.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/limits_notify_no_duration_after_delete.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/limits_notify_with_duration.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/limits_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/limits_reply_ref_26.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_11.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_18.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_19.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_24.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_25.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_6.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/result_data_msg_cnt_ref_9.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/receive/use_case_request.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_binding_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_description_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_key_value_list_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_key_value_list_read_2.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_key_value_list_read_3.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_configuration_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_diagnosis_heartbeat_notify.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_diagnosis_heartbeat_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/send/device_diagnosis_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/discovery_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/discovery_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/send/electrical_connection_characteristic_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/electrical_connection_characteristic_read_2.inc"
#include "tests/src/use_case/actor/eg/lpc/send/electrical_connection_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/failsafe_duration_write.inc"
#include "tests/src/use_case/actor/eg/lpc/send/failsafe_power_limit_write.inc"
#include "tests/src/use_case/actor/eg/lpc/send/limits_write_delete_duration.inc"
#include "tests/src/use_case/actor/eg/lpc/send/limits_write_with_duration.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_binding_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_limit_description_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_limit_list_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_limit_list_read_2.inc"
#include "tests/src/use_case/actor/eg/lpc/send/load_control_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/eg/lpc/send/result_data_msg_cnt_ref_28.inc"
#include "tests/src/use_case/actor/eg/lpc/send/result_data_msg_cnt_ref_31.inc"
#include "tests/src/use_case/actor/eg/lpc/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/eg/lpc/send/use_case_data_reply.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

namespace eg_lpc_test {

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

class EgLpcTestFixture : public UseCaseTestFixture {
 public:
  EgLpcTestFixture() : UseCaseTestFixture("HEMS", "HEMS", "123456789") {};

  void SetUpUseCase() override {
    uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeGridGuard,
        entity_ids,
        ARRAY_SIZE(entity_ids),
        kHeartbeatTimeout
    );

    eg_lpc_listener_mock_.reset(EgLpListenerMockCreate());
    use_case_.reset(EgLpcUseCaseCreate(entity, EG_LP_LISTENER_OBJECT(eg_lpc_listener_mock_.get())));

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

    ExpectSendMessage(send::discovery_read);
  };

  void TearDownUseCase() override {
    EXPECT_CALL(*eg_lpc_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    use_case_.reset();
    eg_lpc_listener_mock_.reset();
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
    EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnHeartbeatReceive(_, 1)).WillOnce(Return());
    HandleMessage(receive::heartbeat_notify);
    EXPECT_TRUE(EgLpcIsHeartbeatWithinDuration(use_case_.get(), remote_entity_addr));
  }

  void VerifyHeartbeatStopStart() {
    ExpectSendHeartbeat(send::device_diagnosis_heartbeat_notify);
    EgLpcStartHeartbeat(use_case_.get());
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }

    EgLpcStopHeartbeat(use_case_.get());
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }
  }

 protected:
  std::unique_ptr<EgLpListenerMock, decltype(&EgLpListenerMockDelete)> eg_lpc_listener_mock_{
      nullptr,
      EgLpListenerMockDelete
  };

  std::unique_ptr<EgLpUseCaseObject, decltype(&EgLpUseCaseDelete)> use_case_{nullptr, EgLpUseCaseDelete};
};

TEST_F(EgLpcTestFixture, EgLpcTest) {
  const EntityAddressType* remote_entity_addr = nullptr;

  // 1. Receive the detailed discovery request and send the response
  ExpectSendMessage(send::discovery_reply);
  HandleMessage(receive::discovery_request);

  // 2. Receive the detailed discovery response and send subscription call + use case read
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnRemoteCsAdded(_, _)).WillOnce(testing::SaveArg<1>(&remote_entity_addr));
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

  // 15. Receive the Load Control Limit reply
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnPowerLimitReceive(_, ScaledValueEq(4200, 0), _, false));
  HandleMessage(receive::limits_reply);

  // 16. Receive a notify without timePeriod — OnPowerLimitReceive must be called with null duration
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnPowerLimitReceive(_, ScaledValueEq(4500, 0), testing::IsNull(), true));
  HandleMessage(receive::limits_notify_no_duration);

  // 17. Write the active power limit with duration PT1H2M3S
  ExpectSendMessage(send::limits_write_with_duration);
  LoadLimit limit_with_duration{};
  limit_with_duration.value.value      = 3000;
  limit_with_duration.value.scale      = 0;
  limit_with_duration.duration.hours   = 1;
  limit_with_duration.duration.minutes = 2;
  limit_with_duration.duration.seconds = 3;
  limit_with_duration.is_active        = true;
  EXPECT_EQ(
      EgLpcSetActiveConsumptionPowerLimit(use_case_.get(), remote_entity_addr, &limit_with_duration, nullptr, nullptr),
      kEebusErrorOk
  );

  // 18. Receive CS notify confirming the write — OnPowerLimitReceive must be called with duration
  EXPECT_CALL(
      *eg_lpc_listener_mock_->gmock,
      OnPowerLimitReceive(_, ScaledValueEq(3000, 0), DurationTypeEq(1, 2, 3), true)
  );
  HandleMessage(receive::limits_notify_with_duration);

  // 19. Receive CS result ack for EG write with duration
  HandleMessage(receive::result_data_msg_cnt_ref_18);

  // 20. Write the active power limit deleting the duration using filter delete
  ExpectSendMessage(send::limits_write_delete_duration);
  LoadLimit limit_delete_duration{
      .value           = {.value = 3000, .scale = 0},
      .is_active       = true,
      .delete_duration = true
  };

  EXPECT_EQ(
      EgLpcSetActiveConsumptionPowerLimit(
          use_case_.get(),
          remote_entity_addr,
          &limit_delete_duration,
          nullptr,
          nullptr
      ),
      kEebusErrorOk
  );

  // 21. Receive CS notify confirming timePeriod deletion — OnPowerLimitReceive must be called with null duration
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnPowerLimitReceive(_, ScaledValueEq(3000, 0), testing::IsNull(), true));
  HandleMessage(receive::limits_notify_no_duration_after_delete);

  // 22. Receive CS result ack for EG delete write
  HandleMessage(receive::result_data_msg_cnt_ref_19);

  // 23. Receive the electrical connection characteristic reply — OnPowerConsumptionNominalMaxReceive
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnPowerNominalMaxReceive(_, ScaledValueEq(11000, 0)));
  HandleMessage(receive::electrical_connection_characteristic_reply);

  // 24. Verify that receiving a CS heartbeat NOTIFY triggers OnHeartbeatReceive and IsHeartbeatWithinDuration
  VerifyHeartbeat(remote_entity_addr);

  // 25. Verify that starting the heartbeat sends NOTIFYs and stopping suppresses them
  VerifyHeartbeatStopStart();

  // 27. Receive DC description reply → EG sends DC key value list READ
  ExpectSendMessage(send::device_configuration_key_value_list_read);
  HandleMessage(receive::device_configuration_description_reply);

  // 28. Receive DC key value list reply → OnFailsafePowerLimitReceive + OnFailsafeDurationReceive
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnFailsafePowerLimitReceive(_, ScaledValueEq(500, 0)));
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnFailsafeDurationReceive(_, DurationTypeEq(2, 0, 0)));
  HandleMessage(receive::device_configuration_key_value_list_reply);

  // 29. Get the active power consumption limit
  LoadLimit active_limit{};
  EXPECT_EQ(EgLpcGetActiveConsumptionPowerLimit(use_case_.get(), remote_entity_addr, &active_limit), kEebusErrorOk);
  EXPECT_THAT(&active_limit.value, ScaledValueEq(3000, 0));

  // 30. Get the power consumption nominal max
  ScaledValue nominal_max{};
  EXPECT_EQ(EgLpcGetPowerConsumptionNominalMax(use_case_.get(), remote_entity_addr, &nominal_max), kEebusErrorOk);
  EXPECT_THAT(&nominal_max, ScaledValueEq(11000, 0));

  // 31. Get the failsafe consumption active power limit
  ScaledValue failsafe_power{};
  EXPECT_EQ(
      EgLpcGetFailsafeConsumptionActivePowerLimit(use_case_.get(), remote_entity_addr, &failsafe_power),
      kEebusErrorOk
  );
  EXPECT_THAT(&failsafe_power, ScaledValueEq(500, 0));

  // 32. Get the failsafe duration minimum
  DurationType failsafe_duration{};
  EXPECT_EQ(EgLpcGetFailsafeDurationMinimum(use_case_.get(), remote_entity_addr, &failsafe_duration), kEebusErrorOk);
  EXPECT_THAT(&failsafe_duration, DurationTypeEq(2, 0, 0));

  // 33. Set the failsafe consumption active power limit → sends WRITE to CS at msgCounter 24
  ExpectSendMessage(send::failsafe_power_limit_write);
  const ScaledValue new_power_limit{600, 0};
  EXPECT_EQ(
      EgLpcSetFailsafeConsumptionActivePowerLimit(
          use_case_.get(),
          remote_entity_addr,
          &new_power_limit,
          nullptr,
          nullptr
      ),
      kEebusErrorOk
  );

  // 34. Receive the result ACK for the failsafe power limit write
  HandleMessage(receive::result_data_msg_cnt_ref_24);

  // 35. Set the failsafe duration minimum → sends WRITE to CS at msgCounter 25
  ExpectSendMessage(send::failsafe_duration_write);
  const EebusDuration new_duration{.hours = 3};
  EXPECT_EQ(
      EgLpcSetFailsafeDurationMinimum(use_case_.get(), remote_entity_addr, &new_duration, nullptr, nullptr),
      kEebusErrorOk
  );

  // 36. Receive the result ACK for the failsafe duration write
  HandleMessage(receive::result_data_msg_cnt_ref_25);

  // 38. Explicitly read active consumption power limit → EG sends READ at msgCounter 26
  ExpectSendMessage(send::load_control_limit_list_read_2);
  EXPECT_EQ(EgLpcReadActiveConsumptionPowerLimit(use_case_.get(), remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  // 39. Receive the reply → OnPowerLimitReceive with updated values (3000W active, no duration)
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnPowerLimitReceive(_, ScaledValueEq(3000, 0), testing::IsNull(), true));
  HandleMessage(receive::limits_reply_ref_26);

  // 40. Explicitly read failsafe consumption active power limit → EG sends READ at msgCounter 27
  ExpectSendMessage(send::device_configuration_key_value_list_read_2);
  EXPECT_EQ(
      EgLpcReadFailsafeConsumptionActivePowerLimit(use_case_.get(), remote_entity_addr, nullptr, nullptr),
      kEebusErrorOk
  );

  // 41. Receive the reply → OnFailsafePowerLimitReceive with updated value (600W)
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnFailsafePowerLimitReceive(_, ScaledValueEq(600, 0)));
  HandleMessage(receive::device_configuration_key_value_list_reply_ref_27);

  // 42. Explicitly read failsafe duration minimum → EG sends READ at msgCounter 28
  ExpectSendMessage(send::device_configuration_key_value_list_read_3);
  EXPECT_EQ(EgLpcReadFailsafeDurationMinimum(use_case_.get(), remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  // 43. Receive the reply → OnFailsafeDurationReceive with updated value (3h)
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnFailsafeDurationReceive(_, DurationTypeEq(3, 0, 0)));
  HandleMessage(receive::device_configuration_key_value_list_reply_ref_28);

  // 44. Explicitly read power consumption nominal max → EG sends READ at msgCounter 29
  ExpectSendMessage(send::electrical_connection_characteristic_read_2);
  EXPECT_EQ(EgLpcReadPowerConsumptionNominalMax(use_case_.get(), remote_entity_addr, nullptr, nullptr), kEebusErrorOk);

  // 45. Receive the reply → OnPowerNominalMaxReceive with updated value (12000W)
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnPowerNominalMaxReceive(_, ScaledValueEq(12000, 0)));
  HandleMessage(receive::electrical_connection_characteristic_reply_ref_29);

  // 46. Expect the remote entity disconnect event while tearing down the use case
  EXPECT_CALL(*eg_lpc_listener_mock_->gmock, OnRemoteCsRemoved(_, _));
}

}  // namespace eg_lpc_test
