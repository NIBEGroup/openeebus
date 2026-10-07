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

#include "src/use_case/actor/cs/lpc/cs_lpc.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/cs_lp_listener_mock.h"
#include "mocks/use_case/api/cs_lpc_approver_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/model/error_types.h"
#include "src/spine/model/result_types.h"
#include "tests/src/json.h"
#include "tests/src/use_case/actor/cs/lpc/receive/device_configuration_binding_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/device_configuration_description_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/device_configuration_key_value_list_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/device_configuration_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/device_diagnosis_heartbeat_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/device_diagnosis_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/discovery_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/discovery_response.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/electrical_connection_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/failsafe_duration_write.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/failsafe_invalid_long_duration_write.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/failsafe_invalid_short_duration_write.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/failsafe_negative_power_limit_write.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/failsafe_power_limit_write.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/failsafe_value_and_duration_write.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/heartbeat_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/limits_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/limits_write.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/limits_write_delete_duration.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/limits_write_multi_entry.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/load_control_binding_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/load_control_description_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/load_control_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/negative_limits_write.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/use_case_request.inc"
#include "tests/src/use_case/actor/cs/lpc/send/device_configuration_description_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/send/device_configuration_key_value_list_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/send/device_diagnosis_heartbeat_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/send/device_diagnosis_heartbeat_notify_second.inc"
#include "tests/src/use_case/actor/cs/lpc/send/device_diagnosis_heartbeat_read.inc"
#include "tests/src/use_case/actor/cs/lpc/send/device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/send/discovery_read.inc"
#include "tests/src/use_case/actor/cs/lpc/send/discovery_read_retry.inc"
#include "tests/src/use_case/actor/cs/lpc/send/discovery_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/send/electrical_connection_characteristic_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/send/failsafe_duration_local_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/send/failsafe_duration_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/send/failsafe_power_limit_local_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/send/failsafe_power_limit_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/send/limits_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/send/limits_notify_no_duration.inc"
#include "tests/src/use_case/actor/cs/lpc/send/limits_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/send/load_control_description_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/send/load_control_subscription_call.inc"
#include "tests/src/use_case/actor/cs/lpc/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_11.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_12.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_14.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_15.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_20.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_21.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_22.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_23.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_24.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_25.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_26.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_27.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/cs/lpc/send/result_data_msg_cnt_ref_9.inc"
#include "tests/src/use_case/actor/cs/lpc/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/cs/lpc/send/use_case_data_reply.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

namespace cs_lpc_test {

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

class CsLpcTestFixture : public UseCaseTestFixture {
 public:
  CsLpcTestFixture() : UseCaseTestFixture("HeatPump", "HeatPump", "123456789") {};

  void SetUpUseCase() override {
    uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeCompressor,
        entity_ids,
        ARRAY_SIZE(entity_ids),
        kHeartbeatTimeout
    );

    cs_lpc_listener_mock_.reset(CsLpListenerMockCreate());
    use_case_.reset(CsLpcUseCaseCreate(entity, 0, CS_LP_LISTENER_OBJECT(cs_lpc_listener_mock_.get())));
    const ScaledValue limit{4200, 0};
    CsLpcSetActiveConsumptionPowerLimit(use_case_.get(), &limit, false, true);

    cs_lpc_approver_mock_.reset(CsLpcApproverMockCreate());
    CsLpSetWriteApprover(use_case_.get(), CS_LPC_APPROVER_OBJECT(cs_lpc_approver_mock_.get()));

    ON_CALL(*cs_lpc_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, _, _, _))
        .WillByDefault(Invoke(this, &CsLpcTestFixture::ApprovePowerLimit));
    ON_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeValueApprovalRequested(_, _, _, _))
        .WillByDefault(Invoke(this, &CsLpcTestFixture::ApproveFailsafeValue));
    ON_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeDurationApprovalRequested(_, _, _, _))
        .WillByDefault(Invoke(this, &CsLpcTestFixture::ApproveFailsafeDuration));

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

    ExpectSendMessage(send::discovery_read);
  };

  void TearDownUseCase() override {
    EXPECT_CALL(*cs_lpc_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    EXPECT_CALL(*cs_lpc_approver_mock_->gmock, Destruct(_)).WillOnce(Return());
    CsLpSetWriteApprover(use_case_.get(), nullptr);
    use_case_.reset();
    cs_lpc_listener_mock_.reset();
    cs_lpc_approver_mock_.reset();
  };

  void ApprovePowerLimit(
      CsLpcApproverObject*,
      const char* ski,
      MsgCounterType msg_cnt,
      const ScaledValue* limit,
      const DurationType* duration,
      bool
  ) {
    double limit_value = 0.0;
    ScaledValueToDouble(limit, &limit_value);
    const int32_t duration_seconds = static_cast<int32_t>(EebusDurationToSeconds(duration));

    if (CsLpIsLimitValid(limit_value, duration_seconds)) {
      CsLpApproveWrite(use_case_.get(), ski, msg_cnt);
      return;
    }

    const ErrorType err{kErrorNumberTypeCommandRejected, "Negative limit values are not allowed"};
    CsLpDenyWrite(use_case_.get(), ski, msg_cnt, &err);
  }

  void ApproveFailsafeValue(CsLpcApproverObject*, const char* ski, MsgCounterType msg_cnt, const ScaledValue* value) {
    double failsafe_value = 0.0;
    ScaledValueToDouble(value, &failsafe_value);

    if (CsLpIsFailsafeValueValid(failsafe_value)) {
      CsLpApproveWrite(use_case_.get(), ski, msg_cnt);
      return;
    }

    const ErrorType err{kErrorNumberTypeCommandRejected, "Negative failsafe power limit values are not allowed"};
    CsLpDenyWrite(use_case_.get(), ski, msg_cnt, &err);
  }

  void
  ApproveFailsafeDuration(CsLpcApproverObject*, const char* ski, MsgCounterType msg_cnt, const DurationType* duration) {
    const int32_t duration_seconds = static_cast<int32_t>(EebusDurationToSeconds(duration));

    if (CsLpIsFailsafeDurationValid(duration_seconds)) {
      CsLpApproveWrite(use_case_.get(), ski, msg_cnt);
      return;
    }

    const ErrorType err{
        kErrorNumberTypeCommandRejected,
        "Invalid failsafe duration minimum value: should be between 2 hours and 24 hours"
    };
    CsLpDenyWrite(use_case_.get(), ski, msg_cnt, &err);
  }

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
    EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnRemoteEgAdded(_, _)).WillOnce(Return());
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
        *cs_lpc_approver_mock_->gmock,
        OnPowerLimitApprovalRequested(_, _, _, ScaledValueEq(100, 0), DurationTypeEq(1, 2, 3), true)
    );
    EXPECT_CALL(
        *cs_lpc_listener_mock_->gmock,
        OnPowerLimitReceive(_, ScaledValueEq(100, 0), DurationTypeEq(1, 2, 3), true)
    );

    HandleMessage(receive::limits_write);

    LoadLimit limit{{0}};
    EXPECT_EQ(CsLpcGetActiveConsumptionPowerLimit(use_case_.get(), &limit), kEebusErrorOk);
    EXPECT_THAT(&limit.value, ScaledValueEq(100, 0));
  }

  // Verify that a negative power limit write is rejected by the approver (via CsLpIsLimitValid)
  // and that the previously stored valid limit is left unchanged.
  void VerifyActivePowerLimitInvalid() {
    ExpectSendMessage(send::result_data_msg_cnt_ref_21);

    EXPECT_CALL(
        *cs_lpc_approver_mock_->gmock,
        OnPowerLimitApprovalRequested(_, _, _, ScaledValueEq(-1, 0), DurationTypeEq(1, 2, 3), true)
    );

    HandleMessage(receive::negative_limits_write);

    LoadLimit limit{{0}};
    EXPECT_EQ(CsLpcGetActiveConsumptionPowerLimit(use_case_.get(), &limit), kEebusErrorOk);
    EXPECT_THAT(&limit.value, ScaledValueEq(100, 0));
  }

  void VerifyFailsafePowerLimitWriteValid() {
    ExpectSendMessage(send::failsafe_power_limit_notify);
    ExpectSendMessage(send::result_data_msg_cnt_ref_22);
    EXPECT_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeValueApprovalRequested(_, _, _, ScaledValueEq(14, 1)));
    EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnFailsafePowerLimitReceive(_, ScaledValueEq(14, 1)));

    HandleMessage(receive::failsafe_power_limit_write);

    ScaledValue failsafe_limit{0};
    bool is_changeable{false};
    EXPECT_EQ(
        CsLpcGetFailsafeConsumptionActivePowerLimit(use_case_.get(), &failsafe_limit, &is_changeable),
        kEebusErrorOk
    );
    EXPECT_THAT(&failsafe_limit, ScaledValueEq(14, 1));
  }

  // Verify that a negative failsafe power limit write is rejected by the approver
  // (via CsLpIsFailsafeValueValid) and that the previously stored valid value is left unchanged.
  void VerifyFailsafePowerLimitInvalid() {
    ExpectSendMessage(send::result_data_msg_cnt_ref_23);
    EXPECT_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeValueApprovalRequested(_, _, _, ScaledValueEq(-14, 1)));

    HandleMessage(receive::failsafe_negative_power_limit_write);

    ScaledValue failsafe_limit{0};
    bool is_changeable{false};
    EXPECT_EQ(
        CsLpcGetFailsafeConsumptionActivePowerLimit(use_case_.get(), &failsafe_limit, &is_changeable),
        kEebusErrorOk
    );
    EXPECT_THAT(&failsafe_limit, ScaledValueEq(14, 1));
  }

  void VerifyFailsafeDurationWriteValid() {
    ExpectSendMessage(send::failsafe_duration_notify);
    ExpectSendMessage(send::result_data_msg_cnt_ref_24);
    EXPECT_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeDurationApprovalRequested(_, _, _, DurationTypeEq(2, 2, 5)));
    EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnFailsafeDurationReceive(_, DurationTypeEq(2, 2, 5)));

    HandleMessage(receive::failsafe_duration_write);

    DurationType failsafe_duration{0};
    bool is_changeable{false};
    EXPECT_EQ(CsLpcGetFailsafeDurationMinimum(use_case_.get(), &failsafe_duration, &is_changeable), kEebusErrorOk);
    EXPECT_THAT(&failsafe_duration, DurationTypeEq(2, 2, 5));
  }

  // Verify that a failsafe duration minimum shorter than 2 hours is rejected by the approver
  // (via CsLpIsFailsafeDurationValid) and that the previously stored valid duration is unchanged.
  void VerifyFailsafeDurationInvalidShort() {
    ExpectSendMessage(send::result_data_msg_cnt_ref_25);
    EXPECT_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeDurationApprovalRequested(_, _, _, DurationTypeEq(1, 2, 5)));

    HandleMessage(receive::failsafe_invalid_short_duration_write);

    DurationType failsafe_duration{0};
    bool is_changeable{false};
    EXPECT_EQ(CsLpcGetFailsafeDurationMinimum(use_case_.get(), &failsafe_duration, &is_changeable), kEebusErrorOk);
    EXPECT_THAT(&failsafe_duration, DurationTypeEq(2, 2, 5));
  }

  // Verify that a failsafe duration minimum longer than 24 hours is rejected by the approver
  // (via CsLpIsFailsafeDurationValid) and that the previously stored valid duration is unchanged.
  void VerifyFailsafeDurationInvalidLong() {
    ExpectSendMessage(send::result_data_msg_cnt_ref_26);
    EXPECT_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeDurationApprovalRequested(_, _, _, DurationTypeEq(25, 2, 5)));

    HandleMessage(receive::failsafe_invalid_long_duration_write);

    DurationType failsafe_duration{0};
    bool is_changeable{false};
    EXPECT_EQ(CsLpcGetFailsafeDurationMinimum(use_case_.get(), &failsafe_duration, &is_changeable), kEebusErrorOk);
    EXPECT_THAT(&failsafe_duration, DurationTypeEq(2, 2, 5));
  }

  void VerifyConsumptionNominalMax() {
    ExpectSendMessage(send::electrical_connection_characteristic_notify);

    const ScaledValue consumption_nominal_max_set{700, 1};
    CsLpcSetConsumptionNominalMax(use_case_.get(), &consumption_nominal_max_set);

    ScaledValue consumption_nominal_max_get{0, 0};
    EXPECT_EQ(CsLpcGetConsumptionNominalMax(use_case_.get(), &consumption_nominal_max_get), kEebusErrorOk);
    EXPECT_THAT(&consumption_nominal_max_get, ScaledValueEq(700, 1));
  }

  void VerifyActivePowerLimitWriteNullDuration() {
    ExpectSendMessage(send::limits_notify_no_duration);
    ExpectSendMessage(send::result_data_msg_cnt_ref_27);

    EXPECT_CALL(
        *cs_lpc_approver_mock_->gmock,
        OnPowerLimitApprovalRequested(_, _, _, ScaledValueEq(200, 0), testing::IsNull(), true)
    );
    EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnPowerLimitReceive(_, ScaledValueEq(200, 0), testing::IsNull(), true));

    HandleMessage(receive::limits_write_delete_duration);

    LoadLimit limit{};
    EXPECT_EQ(CsLpcGetActiveConsumptionPowerLimit(use_case_.get(), &limit), kEebusErrorOk);
    EXPECT_THAT(&limit.value, ScaledValueEq(200, 0));
    EXPECT_TRUE(limit.delete_duration);
  }

  void VerifyHeartbeat() {
    EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnHeartbeatReceive(_, _)).WillOnce(Return());
    HandleMessage(receive::heartbeat_notify);
    EXPECT_TRUE(CsLpcIsHeartbeatWithinDuration(use_case_.get()));
  }

  void VerifyHeartbeatStopStart() {
    CsLpcStopHeartbeat(use_case_.get());
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }

    ExpectSendHeartbeat(send::device_diagnosis_heartbeat_notify_second);
    CsLpcStartHeartbeat(use_case_.get());
    for (size_t i = 0; i < kHeartbeatTimeout; ++i) {
      HandleTick();
    }
  }

  void VerifyLocalSetFailsafePowerLimit() {
    ExpectSendMessage(send::failsafe_power_limit_local_notify);
    const ScaledValue power_limit{2000, 0};
    EXPECT_EQ(CsLpcSetFailsafeConsumptionActivePowerLimit(use_case_.get(), &power_limit, true), kEebusErrorOk);

    ScaledValue result{0, 0};
    bool is_changeable = false;
    EXPECT_EQ(CsLpcGetFailsafeConsumptionActivePowerLimit(use_case_.get(), &result, &is_changeable), kEebusErrorOk);
    EXPECT_THAT(&result, ScaledValueEq(2000, 0));
    EXPECT_TRUE(is_changeable);
  }

  void VerifyLocalSetFailsafeDuration() {
    ExpectSendMessage(send::failsafe_duration_local_notify);
    DurationType duration{};
    duration.hours = 3;
    EXPECT_EQ(CsLpcSetFailsafeDurationMinimum(use_case_.get(), &duration, true), kEebusErrorOk);

    DurationType result{};
    bool is_changeable = false;
    EXPECT_EQ(CsLpcGetFailsafeDurationMinimum(use_case_.get(), &result, &is_changeable), kEebusErrorOk);
    EXPECT_THAT(&result, DurationTypeEq(3, 0, 0));
    EXPECT_TRUE(is_changeable);
  }

 protected:
  std::unique_ptr<CsLpListenerMock, decltype(&CsLpListenerMockDelete)> cs_lpc_listener_mock_{
      nullptr,
      CsLpListenerMockDelete
  };

  std::unique_ptr<CsLpcApproverMock, decltype(&CsLpcApproverMockDelete)> cs_lpc_approver_mock_{
      nullptr,
      CsLpcApproverMockDelete
  };

  std::unique_ptr<CsLpUseCaseObject, decltype(&CsLpUseCaseDelete)> use_case_{nullptr, CsLpUseCaseDelete};
};

// Verify that no response within the timeout triggers a re-send of the discovery read request.
TEST_F(CsLpcTestFixture, RetryDetailedDiscoveryOnTimeout) {
  // kDefaultMaxResponseDelayMs = 10000ms -> 10 ticks to expire, +1 tick to fire the callback
  static constexpr size_t kDiscoveryTimeoutTicks = 11;

  // After timeout the pending discovery reply fires with NULL -> expect retry read (msgCounter=2)
  ExpectSendMessage(send::discovery_read_retry);
  for (size_t i = 0; i < kDiscoveryTimeoutTicks; ++i) {
    HandleTick();
  }
}

TEST_F(CsLpcTestFixture, CsLpcTest) {
  // 1-20. Set up the remote connection by processing the incoming messages in the right order
  SetUpRemoteConnection();

  // 21. Verify that the valid power limit write is approved and updates the active power limit value
  VerifyActivePowerLimitWriteValid();

  // 22. Verify that a negative power limit write is denied by the approver and the value is unchanged
  VerifyActivePowerLimitInvalid();

  // 23. Verify that the valid failsafe power limit write is approved
  VerifyFailsafePowerLimitWriteValid();

  // 24. Verify that a negative failsafe power limit write is denied by the approver and the value is unchanged
  VerifyFailsafePowerLimitInvalid();

  // 25. Verify that the valid failsafe duration write is approved
  VerifyFailsafeDurationWriteValid();

  // 26. Verify that a too-short failsafe duration write is denied by the approver and the value is unchanged
  VerifyFailsafeDurationInvalidShort();

  // 27. Verify that a too-long failsafe duration write is denied by the approver and the value is unchanged
  VerifyFailsafeDurationInvalidLong();

  // 28. Verify that the consumption nominal max can be set and read back correctly
  VerifyConsumptionNominalMax();

  // 29. Verify that the Heartbeat message is received and processed correctly
  VerifyHeartbeat();

  // 30. Verify that a write with an empty timePeriod triggers OnPowerLimitReceive with null duration
  VerifyActivePowerLimitWriteNullDuration();

  // 31. Verify that stopping the heartbeat suppresses further NOTIFYs and re-starting resumes them
  VerifyHeartbeatStopStart();

  // 32. Verify the local failsafe power limit setter sends NOTIFY and updates stored state
  VerifyLocalSetFailsafePowerLimit();

  // 33. Verify the local failsafe duration minimum setter sends NOTIFY and updates stored state
  VerifyLocalSetFailsafeDuration();

  // 34. Expect the remote EG disconnect event while tearing down the use case
  EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnRemoteEgRemoved(_, _));
}

// Regression test: a single write touching both the failsafe value and failsafe duration keys
// requires one approval per key (two independent write-approval callbacks on the same feature).
// An approver that answers asynchronously (i.e. does not call CsLpApproveWrite synchronously
// from within the ON_..._REQUESTED callback) must still be able to approve the second key after
// the first: previously, the pending-approval mapping was removed after the very first approval,
// so the second CsLpApproveWrite call for the same write silently failed with kEebusErrorNoChange
// and the write never applied.
TEST_F(CsLpcTestFixture, ApproveWriteWithTwoKeysRequiresBothApprovals) {
  SetUpRemoteConnection();

  const char* captured_ski        = nullptr;
  MsgCounterType captured_msg_cnt = 0;

  // Defer both approvals instead of approving synchronously, to simulate a real async approver.
  EXPECT_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeValueApprovalRequested(_, _, _, ScaledValueEq(20, 1)))
      .WillOnce(Invoke([&](CsLpcApproverObject*, const char* ski, MsgCounterType msg_cnt, const ScaledValue*) {
        captured_ski     = ski;
        captured_msg_cnt = msg_cnt;
      }));
  EXPECT_CALL(*cs_lpc_approver_mock_->gmock, OnFailsafeDurationApprovalRequested(_, _, _, DurationTypeEq(3, 0, 0)))
      .WillOnce(Return());

  // The exact NOTIFY/RESULT wire content once the write finalizes isn't the point of this test.
  EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, _, _)).Times(testing::AnyNumber());

  HandleMessage(receive::failsafe_value_and_duration_write);
  ASSERT_NE(captured_ski, nullptr);

  // First vote (failsafe value): one of two required approvals. The write must stay pending,
  // and the pending-approval mapping must still exist for the second vote to find it.
  EXPECT_EQ(CsLpApproveWrite(use_case_.get(), captured_ski, captured_msg_cnt), kEebusErrorPending);

  // Second vote (failsafe duration): this is the last required approval, so the write applies now.
  EXPECT_EQ(CsLpApproveWrite(use_case_.get(), captured_ski, captured_msg_cnt), kEebusErrorOk);

  ScaledValue failsafe_limit{0};
  bool is_changeable{false};
  EXPECT_EQ(
      CsLpcGetFailsafeConsumptionActivePowerLimit(use_case_.get(), &failsafe_limit, &is_changeable),
      kEebusErrorOk
  );
  EXPECT_THAT(&failsafe_limit, ScaledValueEq(20, 1));

  DurationType failsafe_duration{0};
  EXPECT_EQ(CsLpcGetFailsafeDurationMinimum(use_case_.get(), &failsafe_duration, &is_changeable), kEebusErrorOk);
  EXPECT_THAT(&failsafe_duration, DurationTypeEq(3, 0, 0));

  EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnRemoteEgRemoved(_, _));
}

TEST_F(CsLpcTestFixture, WriteWithMultipleLimitEntriesIsRejectedWithoutConsultingApprover) {
  SetUpRemoteConnection();

  EXPECT_CALL(*cs_lpc_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, _, _, _)).Times(0);
  EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnPowerLimitReceive(_, _, _, _)).Times(0);

  // The exact RESULT wire content isn't the point of this test.
  EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, _, _)).Times(testing::AnyNumber());

  HandleMessage(receive::limits_write_multi_entry);

  // Neither entry was applied: the known limit keeps its original value, and no new limit_id
  // sneaked into the device's LoadControlLimitListData.
  LoadLimit limit{{0}};
  EXPECT_EQ(CsLpcGetActiveConsumptionPowerLimit(use_case_.get(), &limit), kEebusErrorOk);
  EXPECT_THAT(&limit.value, ScaledValueEq(4200, 0));

  EXPECT_CALL(*cs_lpc_listener_mock_->gmock, OnRemoteEgRemoved(_, _));
}

}  // namespace cs_lpc_test
