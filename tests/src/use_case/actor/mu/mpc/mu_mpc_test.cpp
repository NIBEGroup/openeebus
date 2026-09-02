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

#include "src/use_case/actor/mu/mpc/mu_mpc.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/mu_mpc_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/use_case/actor/mu/mpc/receive/discovery_request.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/discovery_response.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/electrical_connection_parameter_description_request.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/electrical_connection_request.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/electrical_connection_subscription_request.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/measurement_constraints_request.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/measurement_description_request.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/measurement_subscription_request.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/use_case_reply.inc"
#include "tests/src/use_case/actor/mu/mpc/receive/use_case_request.inc"
#include "tests/src/use_case/actor/mu/mpc/send/discovery_read.inc"
#include "tests/src/use_case/actor/mu/mpc/send/discovery_reply.inc"
#include "tests/src/use_case/actor/mu/mpc/send/electrical_connection_description_reply.inc"
#include "tests/src/use_case/actor/mu/mpc/send/electrical_connection_parameter_description_reply.inc"
#include "tests/src/use_case/actor/mu/mpc/send/measurement_constraints_reply.inc"
#include "tests/src/use_case/actor/mu/mpc/send/measurement_description_reply.inc"
#include "tests/src/use_case/actor/mu/mpc/send/measurement_notify_current.inc"
#include "tests/src/use_case/actor/mu/mpc/send/measurement_notify_energy.inc"
#include "tests/src/use_case/actor/mu/mpc/send/measurement_notify_frequency.inc"
#include "tests/src/use_case/actor/mu/mpc/send/measurement_notify_power.inc"
#include "tests/src/use_case/actor/mu/mpc/send/measurement_notify_voltage.inc"
#include "tests/src/use_case/actor/mu/mpc/send/node_management_subscription_call.inc"
#include "tests/src/use_case/actor/mu/mpc/send/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/mu/mpc/send/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/mu/mpc/send/result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/mu/mpc/send/use_case_data_read.inc"
#include "tests/src/use_case/actor/mu/mpc/send/use_case_data_reply.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

using testing::_;
using testing::Return;

namespace mu_mpc_test {

class MuMpcTestFixture : public UseCaseTestFixture {
 public:
  MuMpcTestFixture() : UseCaseTestFixture("HeatPump", "HeatPump", "123456789") {};
  void SetUpUseCase() override {
    uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeHeatPumpAppliance,
        entity_ids,
        ARRAY_SIZE(entity_ids),
        kHeartbeatTimeout
    );

    static constexpr MuMpcMeasurementConfig measurement_default_cfg{
        .value_source = kMeasurementValueSourceTypeMeasuredValue,
    };

    static constexpr MuMpcMonitorEnergyConfig energy_cfg{
        .energy_production_cfg  = &measurement_default_cfg,
        .energy_consumption_cfg = &measurement_default_cfg,
    };

    static constexpr MuMpcMonitorCurrentConfig current_cfg{
        .current_phase_a_cfg = &measurement_default_cfg,
        .current_phase_b_cfg = &measurement_default_cfg,
        .current_phase_c_cfg = &measurement_default_cfg,
    };

    static constexpr MuMpcMonitorVoltageConfig voltage_cfg{
        .voltage_phase_a_cfg  = &measurement_default_cfg,
        .voltage_phase_b_cfg  = &measurement_default_cfg,
        .voltage_phase_c_cfg  = &measurement_default_cfg,
        .voltage_phase_ab_cfg = &measurement_default_cfg,
        .voltage_phase_bc_cfg = &measurement_default_cfg,
        .voltage_phase_ac_cfg = &measurement_default_cfg,
    };

    static constexpr MuMpcMonitorFrequencyConfig frequency_cfg{
        .frequency_cfg = measurement_default_cfg,
    };

    static constexpr MuMpcConfig cfg {
      .power_cfg = {
          .power_total_cfg   = measurement_default_cfg,
          .power_phase_a_cfg = &measurement_default_cfg,
          .power_phase_b_cfg = &measurement_default_cfg,
          .power_phase_c_cfg = &measurement_default_cfg,
      },

      .energy_cfg    = &energy_cfg,
      .current_cfg   = &current_cfg,
      .voltage_cfg   = &voltage_cfg,
      .frequency_cfg = &frequency_cfg
    };

    mu_mpc_listener_mock_.reset(MuMpcListenerMockCreate());
    use_case_.reset(MuMpcUseCaseCreate(entity, 0, &cfg, MU_MPC_LISTENER_OBJECT(mu_mpc_listener_mock_.get())));

    static constexpr ScaledValue power_total{1000, 0};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcPowerTotal, &power_total, NULL, NULL);

    static constexpr ScaledValue power_phase_a{500, 0};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcPowerPhaseA, &power_phase_a, NULL, NULL);

    static constexpr ScaledValue power_phase_b{1200, 0};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcPowerPhaseB, &power_phase_b, NULL, NULL);

    static constexpr ScaledValue power_phase_c{800, 0};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcPowerPhaseC, &power_phase_c, NULL, NULL);

    static constexpr ScaledValue current_phase_a{33, -1};
    static constexpr EebusDateTime timestamp{
        .date = {.year = 2025, .month = 7, .day = 1},
        .time = {  .hour = 12,   .min = 0, .sec = 0}
    };

    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcCurrentPhaseA, &current_phase_a, &timestamp, NULL);

    static constexpr ScaledValue current_phase_b{20, -1};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcCurrentPhaseB, &current_phase_b, NULL, NULL);

    static constexpr ScaledValue current_phase_c{18, -1};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcCurrentPhaseC, &current_phase_c, NULL, NULL);

    static constexpr ScaledValue voltage_phase_a{23000, -2};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcVoltagePhaseA, &voltage_phase_a, NULL, NULL);

    static constexpr ScaledValue voltage_phase_b{23100, -2};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcVoltagePhaseB, &voltage_phase_b, NULL, NULL);

    static constexpr ScaledValue voltage_phase_c{22900, -2};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcVoltagePhaseC, &voltage_phase_c, NULL, NULL);

    static constexpr ScaledValue voltage_phase_ab{40000, -2};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcVoltagePhaseAb, &voltage_phase_ab, NULL, NULL);

    static constexpr ScaledValue voltage_phase_bc{40100, -2};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcVoltagePhaseBc, &voltage_phase_bc, NULL, NULL);

    static constexpr ScaledValue voltage_phase_ac{39900, -2};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcVoltagePhaseAc, &voltage_phase_ac, NULL, NULL);

    static constexpr ScaledValue energy_consumed{5000, 0};
    static constexpr EebusDateTime start_time{
        .date = {.year = 2025, .month = 9, .day = 1},
        .time = {   .hour = 0,   .min = 0, .sec = 0}
    };

    static constexpr EebusDateTime end_time{
        .date = {.year = 2025, .month = 10, .day = 2},
        .time = {   .hour = 0,    .min = 0, .sec = 0}
    };

    MuMpcSetEnergyConsumedCache(use_case_.get(), &energy_consumed, NULL, NULL, &start_time, &end_time);

    static constexpr ScaledValue energy_produced{2000, 0};
    MuMpcSetEnergyProducedCache(use_case_.get(), &energy_produced, NULL, NULL, &start_time, &end_time);

    static constexpr ScaledValue frequency{50, 0};
    MuMpcSetMeasurementDataCache(use_case_.get(), kMpcFrequency, &frequency, NULL, NULL);

    MuMpcUpdate(use_case_.get());

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

    ExpectSendMessage(send::discovery_read);
  }

  void TearDownUseCase() override {
    EXPECT_CALL(*mu_mpc_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    use_case_.reset();
    mu_mpc_listener_mock_.reset();
  }

 protected:
  std::unique_ptr<MuMpcListenerMock, decltype(&MuMpcListenerMockDelete)> mu_mpc_listener_mock_{
      nullptr,
      MuMpcListenerMockDelete
  };
  std::unique_ptr<MuMpcUseCaseObject, decltype(&MuMpcUseCaseDelete)> use_case_{nullptr, MuMpcUseCaseDelete};
};

TEST_F(MuMpcTestFixture, MuMpcTest) {
  // 1. Check that assigned while SetUpUseCase values are correctly set
  ScaledValue value{0};
  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcPowerTotal, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 1000);
  EXPECT_EQ(value.scale, 0);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcCurrentPhaseA, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 33);
  EXPECT_EQ(value.scale, -1);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcCurrentPhaseB, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 20);
  EXPECT_EQ(value.scale, -1);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcCurrentPhaseC, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 18);
  EXPECT_EQ(value.scale, -1);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcVoltagePhaseA, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 23000);
  EXPECT_EQ(value.scale, -2);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcVoltagePhaseB, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 23100);
  EXPECT_EQ(value.scale, -2);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcVoltagePhaseC, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 22900);
  EXPECT_EQ(value.scale, -2);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcVoltagePhaseAb, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 40000);
  EXPECT_EQ(value.scale, -2);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcVoltagePhaseBc, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 40100);
  EXPECT_EQ(value.scale, -2);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcVoltagePhaseAc, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 39900);
  EXPECT_EQ(value.scale, -2);

  EXPECT_EQ(MuMpcGetMeasurementData(use_case_.get(), kMpcFrequency, &value), kEebusErrorOk);
  EXPECT_EQ(value.value, 50);
  EXPECT_EQ(value.scale, 0);

  // 2. Receive the detailed discovery request and send the response
  ExpectSendMessage(send::discovery_reply);
  HandleMessage(receive::discovery_request);

  // 3. Receive the detailed discovery response and send subscriptions + use case read
  ExpectSendMessage(send::node_management_subscription_call);
  ExpectSendMessage(send::use_case_data_read);
  HandleMessage(receive::discovery_response);

  // 4. Receive the Node Management subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_3);
  HandleMessage(receive::node_management_subscription_request);

  // 5. Receive the use case discovery request and send the reply
  ExpectSendMessage(send::use_case_data_reply);
  HandleMessage(receive::use_case_request);

  // 6. Receive the electrical connection subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_5);
  HandleMessage(receive::electrical_connection_subscription_request);

  // 7. Receive the electrical connection read request and send the response
  ExpectSendMessage(send::electrical_connection_description_reply);
  HandleMessage(receive::electrical_connection_request);

  // 8. Receive the electrical connection parameter description request and send the response
  ExpectSendMessage(send::electrical_connection_parameter_description_reply);
  HandleMessage(receive::electrical_connection_parameter_description_request);

  // 9. Receive the measurement subscription request and send result
  ExpectSendMessage(send::result_data_msg_cnt_ref_8);
  HandleMessage(receive::measurement_subscription_request);

  // 10. Receive the measurement description request and send the response
  ExpectSendMessage(send::measurement_description_reply);
  HandleMessage(receive::measurement_description_request);

  // 11. Receive the measurement constraints request and send the response
  ExpectSendMessage(send::measurement_constraints_reply);
  HandleMessage(receive::measurement_constraints_request);

  // 12. Receive the result with message counter reference 3
  HandleMessage(receive::result_data_msg_cnt_ref_3);

  // 13. Receive the Use Case reply — MA recognized as compatible, OnRemoteMaAdded fires
  EXPECT_CALL(*mu_mpc_listener_mock_->gmock, OnRemoteMaAdded(_, _)).WillOnce(Return());
  HandleMessage(receive::use_case_reply);

  // 14. Expect the remote MA disconnect event while tearing down the use case
  EXPECT_CALL(*mu_mpc_listener_mock_->gmock, OnRemoteMaRemoved(_, _));
}

}  // namespace mu_mpc_test
