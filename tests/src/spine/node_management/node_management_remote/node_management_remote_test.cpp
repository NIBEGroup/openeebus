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

#include <string_view>

#include <gtest/gtest.h>

#include "mocks/spine/device/device_local_mock.h"
#include "mocks/spine/device/device_remote_mock.h"
#include "mocks/spine/entity/entity_remote_mock.h"
#include "src/common/array_util.h"
#include "src/spine/events/events.h"
#include "src/spine/model/function_types.h"
#include "src/spine/node_management/node_management_remote.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/mocks/spine/events/event_handler_mock.h"
#include "tests/src/spine/function/filter_test_data.h"
#include "tests/src/spine/function/function_data_test_data.h"
#include "tests/src/spine/function_data.h"
#include "tests/src/spine/node_management/node_management_remote/use_case_data_less_cases.inc"
#include "tests/src/spine/node_management/node_management_remote/use_case_data_more_cases.inc"
#include "tests/src/spine/node_management/node_management_remote/use_case_data_no_entity_addr.inc"

using std::literals::string_view_literals::operator""sv;

using testing::_;
using testing::Return;

MATCHER_P3(IsUseCaseEventPayload, actor, use_case_name_id, change_type, "") {
  if (arg == nullptr) {
    return false;
  }

  if (arg->event_type != kEventTypeUseCaseChange) {
    return false;
  }

  if (arg->change_type != change_type) {
    return false;
  }

  const auto* use_case_filter = arg->use_case_filter;
  if (arg->use_case_filter == nullptr) {
    return false;
  }

  return (use_case_filter->actor == actor) && (use_case_filter->use_case_name_id == use_case_name_id);
}

MATCHER(IsDeviceUpdatePayload, "check device update payload type") {
  bool match = true;

  match = match && (arg->event_type == kEventTypeDeviceChange);
  match = match && (arg->change_type == kElementChangeUpdate);
  match = match && (arg->function_type == kFunctionTypeNodeManagementUseCaseData);
  return match;
}

//-------------------------------------------------------------------------------------------//
//
// NodeManagementRemoteUpdateData() test
//
//-------------------------------------------------------------------------------------------//
struct NodeManagementRemoteUpdateDataInput {
  std::string_view description = ""sv;
  std::string_view data_txt{};
  std::string_view new_data_txt{};
  std::vector<UseCaseFilterType> use_cases_added{};
  std::vector<UseCaseFilterType> use_cases_removed{};
};

std::ostream& operator<<(std::ostream& os, NodeManagementRemoteUpdateDataInput test_input) {
  return os << test_input.description;
}

class NodeManagementRemoteUpdateDataTests : public ::testing::TestWithParam<NodeManagementRemoteUpdateDataInput> {
 protected:
  static constexpr uint32_t kEntityId{0};
  static constexpr const uint32_t* const kEntityIds[1]{&kEntityId};

  static constexpr EntityAddressType kEntityAddr{
      .device      = "HeatPump_123456789",
      .entity      = kEntityIds,
      .entity_size = ARRAY_SIZE(kEntityIds),

  };

  std::unique_ptr<DeviceRemoteMock, decltype(&DeviceRemoteMockDelete)> device_remote_mock_{
      nullptr,
      DeviceRemoteMockDelete
  };

  std::unique_ptr<DeviceLocalMock, decltype(&DeviceLocalMockDelete)> device_local_mock_{nullptr, DeviceLocalMockDelete};

  std::unique_ptr<EventsManagerObject, decltype(&EventsManagerDelete)> events_manager_{nullptr, EventsManagerDelete};

  std::unique_ptr<EntityRemoteMock, decltype(&EntityRemoteMockDelete)> entity_remote_mock_{
      nullptr,
      EntityRemoteMockDelete
  };

  std::unique_ptr<NodeManagementRemoteObject, decltype(&NodeManagementRemoteDelete)> node_management_remote_{
      nullptr,
      NodeManagementRemoteDelete
  };

  void SetUp() override {
    events_manager_.reset(EventsManagerCreate());
    device_local_mock_.reset(DeviceLocalMockCreate());
    EXPECT_CALL(*device_local_mock_->gmock, GetEventsManager(_)).WillRepeatedly(Return(events_manager_.get()));

    device_remote_mock_.reset(DeviceRemoteMockCreate());
    EXPECT_CALL(*device_remote_mock_->gmock, GetLocalDevice(_))
        .WillRepeatedly(Return(DEVICE_LOCAL_OBJECT(device_local_mock_.get())));

    EXPECT_CALL(*device_remote_mock_->gmock, GetSki(_))
        .WillRepeatedly(Return("0123456789abcdefedcb0123456789abcdefedcb"));

    entity_remote_mock_.reset(EntityRemoteMockCreate());
    EXPECT_CALL(*entity_remote_mock_->gmock, GetAddress(_)).WillOnce(Return(&kEntityAddr));
    EXPECT_CALL(*entity_remote_mock_->gmock, GetDevice(_))
        .WillRepeatedly(::testing::Return(DEVICE_REMOTE_OBJECT(device_remote_mock_.get())));

    EXPECT_CALL(*device_remote_mock_->gmock, GetEntity(_, _, _))
        .WillRepeatedly(Return(ENTITY_REMOTE_OBJECT(entity_remote_mock_.get())));

    node_management_remote_.reset(NodeManagementRemoteCreate(0, ENTITY_REMOTE_OBJECT(entity_remote_mock_.get())));
  }

  void TearDown() override {
    EXPECT_CALL(*entity_remote_mock_->gmock, Destruct(ENTITY_OBJECT(entity_remote_mock_.get())));
    EXPECT_CALL(*device_remote_mock_->gmock, Destruct(DEVICE_OBJECT(device_remote_mock_.get())));
    EXPECT_CALL(*device_local_mock_->gmock, Destruct(DEVICE_OBJECT(device_local_mock_.get())));

    node_management_remote_.reset();
    entity_remote_mock_.reset();
    device_remote_mock_.reset();
    device_local_mock_.reset();
    events_manager_.reset();

    CheckForMemoryLeaks();
  }

  EebusError SetUseCaseData(std::string_view use_case_data_txt) {
    std::unique_ptr<FunctionData, decltype(&FunctionDataDelete)> function_data{
        FunctionDataTestDataParse(kFunctionTypeNodeManagementUseCaseData, use_case_data_txt)
    };

    if ((!use_case_data_txt.empty()) && (function_data == nullptr)) {
      return kEebusErrorInit;
    }

    const void* const data = (function_data != nullptr) ? function_data->data : nullptr;
    FeatureRemoteObject* const fr{FEATURE_REMOTE_OBJECT(node_management_remote_.get())};

    return FEATURE_REMOTE_UPDATE_DATA(fr, kFunctionTypeNodeManagementUseCaseData, data, nullptr, nullptr, true);
  }
};

TEST_P(NodeManagementRemoteUpdateDataTests, NodeManagementRemoteUpdateDataTests) {
  ASSERT_EQ(SetUseCaseData(GetParam().data_txt), kEebusErrorOk) << "Wrong Use Case Data input!";

  EventHandlerMockInst event_handler_mock_inst(events_manager_.get());

  EXPECT_CALL(*event_handler_mock_inst, Handle(IsDeviceUpdatePayload(), _));

  // Expect events for added/removed use cases
  for (const UseCaseFilterType& use_case : GetParam().use_cases_added) {
    EXPECT_CALL(
        *event_handler_mock_inst,
        Handle(IsUseCaseEventPayload(use_case.actor, use_case.use_case_name_id, kElementChangeAdd), _)
    );
  }

  for (const UseCaseFilterType& use_case : GetParam().use_cases_removed) {
    EXPECT_CALL(
        *event_handler_mock_inst,
        Handle(IsUseCaseEventPayload(use_case.actor, use_case.use_case_name_id, kElementChangeRemove), _)
    );
  }

  ASSERT_EQ(SetUseCaseData(GetParam().new_data_txt), kEebusErrorOk) << "Wrong New Use Case Data input!";
}

INSTANTIATE_TEST_SUITE_P(
    NodeManagementRemoteUpdateDataTests,
    NodeManagementRemoteUpdateDataTests,
    ::testing::Values(
        NodeManagementRemoteUpdateDataInput{
            .description       = "Test no cases added",
            .data_txt          = {},
            .new_data_txt      = {},
            .use_cases_added   = {},
            .use_cases_removed = {},
},
        NodeManagementRemoteUpdateDataInput{
            .description  = "Many cases added",
            .data_txt     = {},
            .new_data_txt = use_case_data_more_cases,
            .use_cases_added
            = {{kUseCaseActorTypeControllableSystem, kUseCaseNameTypeLimitationOfPowerConsumption},
               {kUseCaseActorTypeCompressor,
                kUseCaseNameTypeOptimizationOfSelfConsumptionByHeatPumpCompressorFlexibility},
               {kUseCaseActorTypeMonitoredUnit, kUseCaseNameTypeMonitoringOfPowerConsumption},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeMonitoringOfDhwTemperature},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeConfigurationOfDhwTemperature},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeConfigurationOfDhwSystemFunction},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeMonitoringOfDhwSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeConfigurationOfRoomHeatingTemperature},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeConfigurationOfRoomHeatingSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeMonitoringOfRoomHeatingSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeVisualizationOfHeatingAreaName},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeMonitoringOfRoomTemperature},
               {kUseCaseActorTypeHeatingCircuit, kUseCaseNameTypeVisualizationOfHeatingAreaName},
               {kUseCaseActorTypeHeatingZone, kUseCaseNameTypeVisualizationOfHeatingAreaName},
               {kUseCaseActorTypeOutdoorTemperatureSensor, kUseCaseNameTypeMonitoringOfOutdoorTemperature},
               {kUseCaseActorTypeMonitoringAppliance, kUseCaseNameTypeMonitoringOfGridConnectionPoint},
               {kUseCaseActorTypeVisualizationAppliance, kUseCaseNameTypeVisualizationOfAggregatedBatteryData},
               {kUseCaseActorTypeVisualizationAppliance, kUseCaseNameTypeVisualizationOfAggregatedPhotovoltaicData}},
            .use_cases_removed = {},
        },
        NodeManagementRemoteUpdateDataInput{
            .description     = "Many use cases removed",
            .data_txt        = use_case_data_more_cases,
            .new_data_txt    = {},
            .use_cases_added = {},
            .use_cases_removed
            = {{kUseCaseActorTypeControllableSystem, kUseCaseNameTypeLimitationOfPowerConsumption},
               {kUseCaseActorTypeCompressor,
                kUseCaseNameTypeOptimizationOfSelfConsumptionByHeatPumpCompressorFlexibility},
               {kUseCaseActorTypeMonitoredUnit, kUseCaseNameTypeMonitoringOfPowerConsumption},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeMonitoringOfDhwTemperature},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeConfigurationOfDhwTemperature},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeConfigurationOfDhwSystemFunction},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeMonitoringOfDhwSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeConfigurationOfRoomHeatingTemperature},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeConfigurationOfRoomHeatingSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeMonitoringOfRoomHeatingSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeVisualizationOfHeatingAreaName},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeMonitoringOfRoomTemperature},
               {kUseCaseActorTypeHeatingCircuit, kUseCaseNameTypeVisualizationOfHeatingAreaName},
               {kUseCaseActorTypeHeatingZone, kUseCaseNameTypeVisualizationOfHeatingAreaName},
               {kUseCaseActorTypeOutdoorTemperatureSensor, kUseCaseNameTypeMonitoringOfOutdoorTemperature},
               {kUseCaseActorTypeMonitoringAppliance, kUseCaseNameTypeMonitoringOfGridConnectionPoint},
               {kUseCaseActorTypeVisualizationAppliance, kUseCaseNameTypeVisualizationOfAggregatedBatteryData},
               {kUseCaseActorTypeVisualizationAppliance, kUseCaseNameTypeVisualizationOfAggregatedPhotovoltaicData}},
        },
        NodeManagementRemoteUpdateDataInput{
            .description  = "Several use cases added",
            .data_txt     = use_case_data_less_cases,
            .new_data_txt = use_case_data_more_cases,
            .use_cases_added
            = {{kUseCaseActorTypeMonitoredUnit, kUseCaseNameTypeMonitoringOfPowerConsumption},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeConfigurationOfDhwSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeMonitoringOfRoomHeatingSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeVisualizationOfHeatingAreaName}},
            .use_cases_removed = {}
        },
        NodeManagementRemoteUpdateDataInput{
            .description     = "Several use cases removed",
            .data_txt        = use_case_data_more_cases,
            .new_data_txt    = use_case_data_less_cases,
            .use_cases_added = {},
            .use_cases_removed
            = {{kUseCaseActorTypeMonitoredUnit, kUseCaseNameTypeMonitoringOfPowerConsumption},
               {kUseCaseActorTypeDHWCircuit, kUseCaseNameTypeConfigurationOfDhwSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeMonitoringOfRoomHeatingSystemFunction},
               {kUseCaseActorTypeHVACRoom, kUseCaseNameTypeVisualizationOfHeatingAreaName}}
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// NodeManagementRemoteUpdateData() null entity address test
// When address has no entity, the event must be published for each entity on the remote device
//
//-------------------------------------------------------------------------------------------//

class NodeManagementRemoteNullEntityAddrTests : public ::testing::TestWithParam<NodeManagementRemoteUpdateDataInput> {
 protected:
  static constexpr uint32_t kEntityId{0};
  static constexpr const uint32_t* const kEntityIds[1]{&kEntityId};

  static constexpr EntityAddressType kEntityAddr{
      .device      = "HeatPump_123456789",
      .entity      = kEntityIds,
      .entity_size = ARRAY_SIZE(kEntityIds),
  };

  std::unique_ptr<DeviceRemoteMock, decltype(&DeviceRemoteMockDelete)> device_remote_mock_{
      nullptr,
      DeviceRemoteMockDelete
  };

  std::unique_ptr<DeviceLocalMock, decltype(&DeviceLocalMockDelete)> device_local_mock_{nullptr, DeviceLocalMockDelete};

  std::unique_ptr<EventsManagerObject, decltype(&EventsManagerDelete)> events_manager_{nullptr, EventsManagerDelete};

  std::unique_ptr<EntityRemoteMock, decltype(&EntityRemoteMockDelete)> nm_entity_remote_mock_{
      nullptr,
      EntityRemoteMockDelete
  };

  std::unique_ptr<EntityRemoteMock, decltype(&EntityRemoteMockDelete)> entity_remote_mock_1_{
      nullptr,
      EntityRemoteMockDelete
  };

  std::unique_ptr<EntityRemoteMock, decltype(&EntityRemoteMockDelete)> entity_remote_mock_2_{
      nullptr,
      EntityRemoteMockDelete
  };

  std::unique_ptr<NodeManagementRemoteObject, decltype(&NodeManagementRemoteDelete)> node_management_remote_{
      nullptr,
      NodeManagementRemoteDelete
  };

  Vector entities_vector_{};

  void SetUp() override {
    events_manager_.reset(EventsManagerCreate());
    device_local_mock_.reset(DeviceLocalMockCreate());
    EXPECT_CALL(*device_local_mock_->gmock, GetEventsManager(_)).WillRepeatedly(Return(events_manager_.get()));

    device_remote_mock_.reset(DeviceRemoteMockCreate());
    EXPECT_CALL(*device_remote_mock_->gmock, GetLocalDevice(_))
        .WillRepeatedly(Return(DEVICE_LOCAL_OBJECT(device_local_mock_.get())));
    EXPECT_CALL(*device_remote_mock_->gmock, GetSki(_))
        .WillRepeatedly(Return("0123456789abcdefedcb0123456789abcdefedcb"));

    nm_entity_remote_mock_.reset(EntityRemoteMockCreate());
    EXPECT_CALL(*nm_entity_remote_mock_->gmock, GetAddress(_)).WillOnce(Return(&kEntityAddr));
    EXPECT_CALL(*nm_entity_remote_mock_->gmock, GetDevice(_))
        .WillRepeatedly(Return(DEVICE_REMOTE_OBJECT(device_remote_mock_.get())));

    entity_remote_mock_1_.reset(EntityRemoteMockCreate());
    entity_remote_mock_2_.reset(EntityRemoteMockCreate());

    VectorConstruct(&entities_vector_);
    VectorPushBack(&entities_vector_, ENTITY_REMOTE_OBJECT(entity_remote_mock_1_.get()));
    VectorPushBack(&entities_vector_, ENTITY_REMOTE_OBJECT(entity_remote_mock_2_.get()));

    EXPECT_CALL(*device_remote_mock_->gmock, GetEntities(_)).WillRepeatedly(Return(&entities_vector_));

    node_management_remote_.reset(NodeManagementRemoteCreate(0, ENTITY_REMOTE_OBJECT(nm_entity_remote_mock_.get())));
  }

  void TearDown() override {
    EXPECT_CALL(*nm_entity_remote_mock_->gmock, Destruct(ENTITY_OBJECT(nm_entity_remote_mock_.get())));
    EXPECT_CALL(*entity_remote_mock_1_->gmock, Destruct(ENTITY_OBJECT(entity_remote_mock_1_.get())));
    EXPECT_CALL(*entity_remote_mock_2_->gmock, Destruct(ENTITY_OBJECT(entity_remote_mock_2_.get())));
    EXPECT_CALL(*device_remote_mock_->gmock, Destruct(DEVICE_OBJECT(device_remote_mock_.get())));
    EXPECT_CALL(*device_local_mock_->gmock, Destruct(DEVICE_OBJECT(device_local_mock_.get())));

    node_management_remote_.reset();
    VectorDestruct(&entities_vector_);
    nm_entity_remote_mock_.reset();
    entity_remote_mock_1_.reset();
    entity_remote_mock_2_.reset();
    device_remote_mock_.reset();
    device_local_mock_.reset();
    events_manager_.reset();

    CheckForMemoryLeaks();
  }

  EebusError SetUseCaseData(std::string_view use_case_data_txt) {
    std::unique_ptr<FunctionData, decltype(&FunctionDataDelete)> function_data{
        FunctionDataTestDataParse(kFunctionTypeNodeManagementUseCaseData, use_case_data_txt)
    };

    if ((!use_case_data_txt.empty()) && (function_data == nullptr)) {
      return kEebusErrorInit;
    }

    const void* const data = (function_data != nullptr) ? function_data->data : nullptr;
    FeatureRemoteObject* const fr{FEATURE_REMOTE_OBJECT(node_management_remote_.get())};

    return FEATURE_REMOTE_UPDATE_DATA(fr, kFunctionTypeNodeManagementUseCaseData, data, nullptr, nullptr, true);
  }
};

TEST_P(NodeManagementRemoteNullEntityAddrTests, NullEntityAddrPublishesForEachEntity) {
  ASSERT_EQ(SetUseCaseData(GetParam().data_txt), kEebusErrorOk) << "Wrong Use Case Data input!";

  EventHandlerMockInst event_handler_mock_inst(events_manager_.get());

  EXPECT_CALL(*event_handler_mock_inst, Handle(IsDeviceUpdatePayload(), _));

  for (const UseCaseFilterType& use_case : GetParam().use_cases_added) {
    EXPECT_CALL(
        *event_handler_mock_inst,
        Handle(IsUseCaseEventPayload(use_case.actor, use_case.use_case_name_id, kElementChangeAdd), _)
    )
        .Times(2);
  }

  for (const UseCaseFilterType& use_case : GetParam().use_cases_removed) {
    EXPECT_CALL(
        *event_handler_mock_inst,
        Handle(IsUseCaseEventPayload(use_case.actor, use_case.use_case_name_id, kElementChangeRemove), _)
    )
        .Times(2);
  }

  ASSERT_EQ(SetUseCaseData(GetParam().new_data_txt), kEebusErrorOk) << "Wrong New Use Case Data input!";
}

INSTANTIATE_TEST_SUITE_P(
    NodeManagementRemoteNullEntityAddrTests,
    NodeManagementRemoteNullEntityAddrTests,
    ::testing::Values(
        NodeManagementRemoteUpdateDataInput{
            .description  = "Null entity addr: use cases added, event published for each entity",
            .data_txt     = {},
            .new_data_txt = use_case_data_no_entity_addr,
            .use_cases_added
            = {{kUseCaseActorTypeControllableSystem, kUseCaseNameTypeLimitationOfPowerConsumption},
                             {kUseCaseActorTypeMonitoredUnit, kUseCaseNameTypeMonitoringOfPowerConsumption}},
            .use_cases_removed = {},
},
        NodeManagementRemoteUpdateDataInput{
            .description     = "Null entity addr: use cases removed, event published for each entity",
            .data_txt        = use_case_data_no_entity_addr,
            .new_data_txt    = {},
            .use_cases_added = {},
            .use_cases_removed
            = {{kUseCaseActorTypeControllableSystem, kUseCaseNameTypeLimitationOfPowerConsumption},
               {kUseCaseActorTypeMonitoredUnit, kUseCaseNameTypeMonitoringOfPowerConsumption}},
        }
    )
);
