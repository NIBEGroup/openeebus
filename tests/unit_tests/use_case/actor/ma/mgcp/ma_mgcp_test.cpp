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

#include "src/use_case/actor/ma/mgcp/ma_mgcp.h"

#include <gtest/gtest.h>

#include <map>
#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/ma_mgcp_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/device_configuration_description_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/device_configuration_key_value_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/discovery_request.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/discovery_response.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/electrical_connection_description_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/electrical_connection_parameter_description_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_constraints_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_description_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_notify_current.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_notify_energy.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_notify_energy_feed_in_only.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_notify_frequency.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_notify_mixed.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_notify_power.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_notify_power_total_only.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_notify_voltage.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/measurement_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/node_management_subscription_request.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/result_data_msg_cnt_ref_11.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/result_data_msg_cnt_ref_5.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/result_data_msg_cnt_ref_8.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/use_case_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/receive/use_case_request.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/device_configuration_description_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/device_configuration_key_value_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/device_configuration_subscription_call.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/discovery_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/discovery_reply.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/electrical_connection_description_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/electrical_connection_parameter_description_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/electrical_connection_subscription_call.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/measurement_constraints_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/measurement_description_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/measurement_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/measurement_subscription_call.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/node_management_subscription_call.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/result_data_msg_cnt_ref_3.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/use_case_data_read.inc"
#include "tests/unit_tests/use_case/actor/ma/mgcp/send/use_case_data_reply.inc"
#include "tests/unit_tests/use_case/use_case_test_fixture.h"

using testing::_;
using testing::Return;

namespace ma_mgcp_test {

class MaMgcpTestFixture : public UseCaseTestFixture {
 public:
  MaMgcpTestFixture() : UseCaseTestFixture("HEMS", "HEMS", "123456789") {};
  void SetUpUseCase() override {
    uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeCEM,
        entity_ids,
        ARRAY_SIZE(entity_ids),
        kHeartbeatTimeout
    );

    ma_mgcp_listener_mock_.reset(MaMgcpListenerMockCreate());
    use_case_.reset(MaMgcpUseCaseCreate(entity, MA_MGCP_LISTENER_OBJECT(ma_mgcp_listener_mock_.get())));

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);
    ExpectSendMessage(send::discovery_read);
  };

  void TearDownUseCase() override {
    EXPECT_CALL(*ma_mgcp_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    use_case_.reset();
    ma_mgcp_listener_mock_.reset();
  };

  void ExpectMeasurementsReceive(
      MaMgcpListenerGMock* mock,
      const std::map<GcpMeasurementNameId, ScaledValue>& expected_measurements
  ) {
    for (const auto& [name_id, scaled_value] : expected_measurements) {
      const int64_t value{scaled_value.value};
      const int8_t scale{scaled_value.scale};
      EXPECT_CALL(*mock, OnMeasurementReceive(_, name_id, ScaledValueEq(value, scale), _)).WillOnce(Return());
    }
  }

 protected:
  std::unique_ptr<MaMgcpListenerMock, decltype(&MaMgcpListenerMockDelete)> ma_mgcp_listener_mock_{
      nullptr,
      MaMgcpListenerMockDelete
  };

  std::unique_ptr<MaMgcpUseCaseObject, decltype(&MaMgcpUseCaseDelete)> use_case_{nullptr, MaMgcpUseCaseDelete};
};

TEST_F(MaMgcpTestFixture, MaMgcpTest) {
  // 1. Receive the detailed discovery request and send the response
  ExpectSendMessage(send::discovery_reply);
  HandleMessage(receive::discovery_request);

  // 2. Receive the detailed discovery response and send subscriptions + use case read
  EXPECT_CALL(*ma_mgcp_listener_mock_->gmock, OnRemoteEntityConnect(_, _)).WillOnce(Return());
  ExpectSendMessage(send::node_management_subscription_call);
  ExpectSendMessage(send::use_case_data_read);
  HandleMessage(receive::discovery_response);

  // 3. Receive the Node Management subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_3);
  HandleMessage(receive::node_management_subscription_request);

  // 4. Receive the use case discovery request and send the reply
  ExpectSendMessage(send::use_case_data_reply);
  HandleMessage(receive::use_case_request);

  // 5. Receive the result with message counter reference 3
  HandleMessage(receive::result_data_msg_cnt_ref_3);

  // 6. Receive the Use Case reply and send electrical connection + measurement + device configuration subscriptions and
  // reads
  ExpectSendMessage(send::electrical_connection_subscription_call);
  ExpectSendMessage(send::electrical_connection_description_read);
  ExpectSendMessage(send::electrical_connection_parameter_description_read);
  ExpectSendMessage(send::measurement_subscription_call);
  ExpectSendMessage(send::measurement_description_read);
  ExpectSendMessage(send::measurement_constraints_read);
  ExpectSendMessage(send::device_configuration_subscription_call);
  ExpectSendMessage(send::device_configuration_description_read);
  HandleMessage(receive::use_case_reply);

  // 7. Receive the result with message counter reference 5
  HandleMessage(receive::result_data_msg_cnt_ref_5);

  // 8. Receive the electrical connection description reply
  HandleMessage(receive::electrical_connection_description_reply);

  // 9. Receive the electrical connection parameter description reply
  HandleMessage(receive::electrical_connection_parameter_description_reply);

  // 10. Receive the result with message counter reference 8
  HandleMessage(receive::result_data_msg_cnt_ref_8);

  // 11. Receive the measurement description reply and send the measurement read
  ExpectSendMessage(send::measurement_read);
  HandleMessage(receive::measurement_description_reply);

  // 12. Receive the measurement constraints reply
  HandleMessage(receive::measurement_constraints_reply);

  // 13. Receive the device configuration subscription result
  HandleMessage(receive::result_data_msg_cnt_ref_11);

  // 14. Receive the device configuration description reply and send the key value read
  ExpectSendMessage(send::device_configuration_key_value_read);
  HandleMessage(receive::device_configuration_description_reply);

  // 15. Receive the measurement reply (power_total)
  EXPECT_CALL(*ma_mgcp_listener_mock_->gmock, OnMeasurementReceive(_, kGcpPowerTotal, ScaledValueEq(33000, -1), _))
      .WillOnce(Return());

  HandleMessage(receive::measurement_reply);

  // 16. Receive the device configuration key value reply
  EXPECT_CALL(*ma_mgcp_listener_mock_->gmock, OnPvCurtailmentLimitFactorReceive(_, ScaledValueEq(75, 0), _))
      .WillOnce(Return());

  HandleMessage(receive::device_configuration_key_value_reply);

  // 17. Receive the measurement notify (power_total)
  const std::map<GcpMeasurementNameId, ScaledValue> expected_power{
      {kGcpPowerTotal, {.value = 5000, .scale = 0}},
  };

  ExpectMeasurementsReceive(ma_mgcp_listener_mock_->gmock, expected_power);
  HandleMessage(receive::measurement_notify_power);

  // 18. Receive the measurement notify (energy)
  const std::map<GcpMeasurementNameId, ScaledValue> expected_energy{
      {  kGcpEnergyFeedIn, {.value = 200000, .scale = 0}},
      {kGcpEnergyConsumed, {.value = 800000, .scale = 0}},
  };

  ExpectMeasurementsReceive(ma_mgcp_listener_mock_->gmock, expected_energy);
  HandleMessage(receive::measurement_notify_energy);

  // 19. Receive the measurement notify (current — all three phases)
  const std::map<GcpMeasurementNameId, ScaledValue> expected_current{
      {kGcpCurrentPhaseA, {.value = 15, .scale = -1}},
      {kGcpCurrentPhaseB, {.value = 12, .scale = -1}},
      {kGcpCurrentPhaseC, {.value = 16, .scale = -1}},
  };

  ExpectMeasurementsReceive(ma_mgcp_listener_mock_->gmock, expected_current);
  HandleMessage(receive::measurement_notify_current);

  // 20. Receive the measurement notify (voltage — all six phase combinations, scenario 6)
  const std::map<GcpMeasurementNameId, ScaledValue> expected_voltage{
      { kGcpVoltagePhaseA, {.value = 2310, .scale = -1}},
      { kGcpVoltagePhaseB, {.value = 2295, .scale = -1}},
      { kGcpVoltagePhaseC, {.value = 2320, .scale = -1}},
      {kGcpVoltagePhaseAb, {.value = 3998, .scale = -1}},
      {kGcpVoltagePhaseBc, {.value = 4012, .scale = -1}},
      {kGcpVoltagePhaseAc, {.value = 3986, .scale = -1}},
  };

  ExpectMeasurementsReceive(ma_mgcp_listener_mock_->gmock, expected_voltage);
  HandleMessage(receive::measurement_notify_voltage);

  // 21. Receive the measurement notify (frequency)
  const std::map<GcpMeasurementNameId, ScaledValue> expected_frequency{
      {kGcpFrequency, {.value = 500, .scale = -1}},
  };

  ExpectMeasurementsReceive(ma_mgcp_listener_mock_->gmock, expected_frequency);
  HandleMessage(receive::measurement_notify_frequency);

  // 22. Receive a partial notify with power total only — tests single-measurement partial
  //     and overwrites the kGcpPowerTotal initially received in the measurement reply (step 15)
  EXPECT_CALL(*ma_mgcp_listener_mock_->gmock, OnMeasurementReceive(_, kGcpPowerTotal, ScaledValueEq(5500, 0), _))
      .WillOnce(Return());
  HandleMessage(receive::measurement_notify_power_total_only);

  // 23. Receive a cross-scenario notify — tests that one notify can carry measurements from
  //     different scenarios; also overwrites kGcpPowerTotal, kGcpCurrentPhaseB,
  //     kGcpVoltagePhaseAb and kGcpFrequency set in earlier notifies
  const std::map<GcpMeasurementNameId, ScaledValue> expected_mixed{
      {    kGcpPowerTotal,  {.value = 6200, .scale = 0}},
      { kGcpCurrentPhaseB,   {.value = 13, .scale = -1}},
      {kGcpVoltagePhaseAb, {.value = 4020, .scale = -1}},
      {     kGcpFrequency, {.value = 4998, .scale = -2}},
  };

  ExpectMeasurementsReceive(ma_mgcp_listener_mock_->gmock, expected_mixed);
  HandleMessage(receive::measurement_notify_mixed);

  // 24. Receive a partial notify with energy feed-in only — tests single-measurement partial
  //     from a different scenario and overwrites kGcpEnergyFeedIn set in step 18
  EXPECT_CALL(*ma_mgcp_listener_mock_->gmock, OnMeasurementReceive(_, kGcpEnergyFeedIn, ScaledValueEq(250000, 0), _))
      .WillOnce(Return());
  HandleMessage(receive::measurement_notify_energy_feed_in_only);

  // 25. Get all 13 measurements via GetMeasurementData() and verify the final stored values
  //     after all notifies (overwritten values reflect the latest notify that touched each ID).
  //     Also verify the PV curtailment limit factor from step 16.
  ScaledValue value{0};
  static constexpr uint32_t remote_entity_id{1};

  static constexpr const uint32_t* const remote_entity_ids[]{&remote_entity_id};

  const EntityAddressType remote_entity_addr
      = {"d:_n:GridConnectionPoint_123456789", remote_entity_ids, ARRAY_SIZE(remote_entity_ids)};

  const std::map<GcpMeasurementNameId, ScaledValue> expected_final{
      {    kGcpPowerTotal,   {.value = 6200, .scale = 0}}, // step 23 overwrites step 22
      {  kGcpEnergyFeedIn, {.value = 250000, .scale = 0}}, // step 24 overwrites step 18
      {kGcpEnergyConsumed, {.value = 800000, .scale = 0}}, // step 18
      { kGcpCurrentPhaseA,    {.value = 15, .scale = -1}}, // step 19
      { kGcpCurrentPhaseB,    {.value = 13, .scale = -1}}, // step 23 overwrites step 19
      { kGcpCurrentPhaseC,    {.value = 16, .scale = -1}}, // step 19
      { kGcpVoltagePhaseA,  {.value = 2310, .scale = -1}}, // step 20
      { kGcpVoltagePhaseB,  {.value = 2295, .scale = -1}}, // step 20
      { kGcpVoltagePhaseC,  {.value = 2320, .scale = -1}}, // step 20
      {kGcpVoltagePhaseAb,  {.value = 4020, .scale = -1}}, // step 23 overwrites step 20
      {kGcpVoltagePhaseBc,  {.value = 4012, .scale = -1}}, // step 20
      {kGcpVoltagePhaseAc,  {.value = 3986, .scale = -1}}, // step 20
      {     kGcpFrequency,  {.value = 4998, .scale = -2}}, // step 23 overwrites step 21
  };

  for (const auto& [name_id, scaled_value] : expected_final) {
    EXPECT_EQ(MaMgcpGetMeasurementData(use_case_.get(), name_id, &remote_entity_addr, &value), kEebusErrorOk);
    EXPECT_THAT(&value, ScaledValueEq(scaled_value.value, scaled_value.scale));
  }

  EXPECT_EQ(MaMgcpGetPvCurtailmentLimitFactor(use_case_.get(), &remote_entity_addr, &value), kEebusErrorOk);
  EXPECT_THAT(&value, ScaledValueEq(75, 0));

  // 26. Expect the remote entity disconnect event while tearing down the use case
  EXPECT_CALL(*ma_mgcp_listener_mock_->gmock, OnRemoteEntityDisconnect(_, _));
}

}  // namespace ma_mgcp_test
