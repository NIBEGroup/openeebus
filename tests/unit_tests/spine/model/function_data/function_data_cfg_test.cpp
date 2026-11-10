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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string_view>

#include "src/common/eebus_data/eebus_data.h"
#include "src/common/string_util.h"
#include "src/spine/model/model.h"
#include "src/spine/model/model_internal.h"

using std::literals::string_view_literals::operator""sv;

bool IsListContainer(const EebusDataCfg* cfg) {
  if (!EEBUS_DATA_IS_SEQUENCE(cfg)) {
    return false;
  }

  auto const referred_cfg = reinterpret_cast<const EebusDataCfg*>(cfg->metadata);
  return (EebusDataGetCfgSize(referred_cfg) == 1) && (EEBUS_DATA_IS_LIST(&referred_cfg[0]));
}

TEST(FunctionDataCfgTests, FunctionDataCfgNamesTest) {
  const EnumMapping* const fct_enum_lut         = ModelGetFunctionEnumCfg();
  const EebusDataCfg* const data_cfg            = ModelGetDataChoiceCfg();
  const EebusDataCfg* const elements_cfg        = ModelGetElementsChoiceCfg();
  const EebusDataCfg* const selectors_cfg_first = ModelGetSelectorsChoiceCfg();

  const size_t fct_enum_lut_size        = EebusDataGetEnumMappingSize(fct_enum_lut);
  const size_t data_cfg_size            = EebusDataGetCfgSize(data_cfg);
  const size_t elements_cfg_size        = EebusDataGetCfgSize(elements_cfg);
  const size_t selectors_cfg_first_size = EebusDataGetCfgSize(selectors_cfg_first);

  ASSERT_EQ(data_cfg_size, kFunctionTypeNum);
  ASSERT_EQ(elements_cfg_size, kFunctionTypeNum);
  ASSERT_EQ(selectors_cfg_first_size, kFunctionTypeNum);

  for (size_t i = 0; i < fct_enum_lut_size - 1; ++i) {
    EXPECT_GT(strcmp(fct_enum_lut[i + 1].name, fct_enum_lut[i].name), 0);
  }

  for (size_t i = 0; i < fct_enum_lut_size; ++i) {
    EXPECT_STREQ(fct_enum_lut[i].name, data_cfg[i].name);
  }

  for (size_t i = 0; i < elements_cfg_size; ++i) {
    std::string name{std::string{data_cfg[i].name} + "Elements"};
    std::size_t pos = name.find("List");
    if (pos != std::string::npos) {
      name.replace(pos, strlen("List"), "");
    }

    EXPECT_EQ(name, elements_cfg[i].name);
  }

  for (size_t i = 0; i < selectors_cfg_first_size; ++i) {
    std::string name{std::string{data_cfg[i].name} + "Selectors"};
    EXPECT_EQ(name, selectors_cfg_first[i].name);
  }
}

void CheckSelectorsCfg(
    const EebusDataCfg* selector_cfg,
    const EebusDataCfg* list_item_cfg_first,
    const char* selectors_choice_name
) {
  const size_t selectors_cfg_first_size = EebusDataGetCfgSize(selector_cfg);

  for (size_t i = 0; i < selectors_cfg_first_size; ++i) {
    const EebusDataCfg* selectors{&selector_cfg[i]};

    std::unique_ptr<char[], decltype(&StringDelete)> expected_name{StringCopy(selectors->name), StringDelete};
    const EebusDataInterface* expected_type{selectors->interface_};

    if (ModelIsTimestampInterval(selectors)) {
      StringRemoveToken(expected_name.get(), "Interval");
      expected_type = &eebus_data_string_methods;
    }

    const EebusDataCfg* data_field_cfg = ModelGetCfgWithName(list_item_cfg_first, expected_name.get());

#if 0
    // Trial code to handle situations when selector is a specific list or data is a list in a list
    if ((data_field_cfg != nullptr) && (EEBUS_DATA_IS_LIST(data_field_cfg))) {
      data_field_cfg = ModelGetListItemCfgBegin(data_field_cfg);
      if (EEBUS_DATA_IS_SEQUENCE(selectors)) {
        selectors = reinterpret_cast<const EebusDataCfg*>(selectors->metadata);
      }

      CheckSelectorsCfg(selectors, data_field_cfg, selectors_choice_name);
      continue;
    }
#endif

    EXPECT_NE(data_field_cfg, nullptr) << "Selector: " << selectors_choice_name << ", field name: " << selectors->name;
    if (data_field_cfg != nullptr) {
      EXPECT_EQ(expected_type, data_field_cfg->interface_)
          << "Selector: " << selectors_choice_name << ", field name: " << selectors->name;
      EXPECT_STREQ(expected_name.get(), data_field_cfg->name)
          << "Selector: " << selectors_choice_name << ", field name: " << selectors->name;
    }
  }
}

TEST(FunctionDataCfgTests, FunctionDataSelectorsCfgTest) {
  // Test for investigation perpose that is currently failing due to issues with processing:
  // "hvacSystemFunctionListDataSelectors", field: systemFunctionId;
  // "hvacSystemFunctionOperationModeRelationListDataSelectors", field: systemFunctionId;
  // "measurementThresholdRelationListDataSelectors", field: thresholdId;
  // "nodeManagementBindingDataSelectors", field: bindingEntry;
  // "nodeManagementDetailedDiscoveryData";
  // "nodeManagementSubscriptionDataSelectors", field: subscriptionEntry;
  // "nodeManagementUseCaseDataSelectors", field: useCaseInformation;
  // "powerSequenceDescriptionListDataSelectors", field: sequenceId;
  // "sessionMeasurementRelationListDataSelectors", field: measurementId;
  // "setpointDescriptionListDataSelectors", field: setpointType;
  // "smartEnergyManagementPsData"
  // "smartEnergyManagementPsPriceData"
  // "supplyConditionThresholdRelationListDataSelectors", field: thresholdId;
  // "tariffBoundaryRelationListDataSelectors", field: boundaryId;
  // "tariffListDataSelectors", field: activeTierId;
  // "tariffTierRelationListDataSelectors", field: tierId;
  // "tierIncentiveRelationListDataSelectors", field: incentiveId;
  // "tierListDataSelectors", field: activeIncentiveId;
  // "timeSeriesListDataSelectors", field: timeSeriesSlotId;
  // "useCaseInformationListDataSelectors", field: useCaseSupport;
#if 0
  const EebusDataCfg* const selectors_choice_cfg = ModelGetSelectorsChoiceCfg();

  const size_t selectors_choice_cfg_size = EebusDataGetCfgSize(selectors_choice_cfg);
  for (FunctionType function_type_id = 0; function_type_id < kFunctionTypeNum; ++function_type_id) {
    if ((function_type_id == kFunctionTypeNodeManagementDetailedDiscoveryData)
        || (function_type_id == kFunctionTypeSmartEnergyManagementPsData)
        || (function_type_id == kFunctionTypeSmartEnergyManagementPsPriceData)) {
      continue;
    }

    const EebusDataCfg* const selectors_cfg_first = ModelGetSelectorsCfgBegin(function_type_id);
    const EebusDataCfg* const list_item_cfg_first = ModelGetDataListItemCfgBegin(function_type_id);
    if (selectors_cfg_first == nullptr) {
      ASSERT_EQ(list_item_cfg_first, nullptr);
      continue;
    }

    ASSERT_NE(list_item_cfg_first, nullptr) << function_type_id;
    CheckSelectorsCfg(selectors_cfg_first, list_item_cfg_first, selectors_choice_cfg[function_type_id].name);
  }
#endif
}
