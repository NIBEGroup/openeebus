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
 * @file
 * @brief SPINE Data Model implementation
 */

#include <string.h>

#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data.h"
#include "src/common/eebus_data/eebus_data_util.h"
#include "src/spine/model/datagram.inc"
#include "src/spine/model/function_types.h"

//---------------------------------------------------------------------------//
//
// SPINE Model functions for unit tests purpose only
//
//---------------------------------------------------------------------------//
const EnumMapping* ModelGetFunctionEnumCfg(void) {
    return function_lut;
}

const EebusDataCfg* ModelGetSelectorsChoiceCfg(void) {
    return data_selectors_choice_data_cfg;
}

const EebusDataCfg* ModelGetElementsChoiceCfg(void) {
    return data_elements_choice_data_cfg;
}

const EebusDataCfg* ModelGetDataChoiceCfg(void) {
    return data_choice_data_cfg;
}

bool ModelIsTimestampInterval(const EebusDataCfg* cfg) {
    if ((cfg == NULL) || (!EEBUS_DATA_IS_SEQUENCE(cfg))) {
        return false;
    }

    return cfg->metadata == timestamp_interval_cfg;
}

const EebusDataCfg* GetFilterCfg(void) {
    static const EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_ROOT("filter", FilterType, filter_cfg);
    return &cfg;
}

//---------------------------------------------------------------------------//
//
// EEBUS Data Configuration getters
//
//---------------------------------------------------------------------------//
const EebusDataCfg* ModelGetDatagramCfg(void) {
    return &datagram_data_cfg;
}

const EebusDataCfg* ModelGetDatagramHeaderCfg(void) {
    return datagram_header_data_cfg;
}

const EebusDataCfg* ModelGetFeatureAddressCfg(void) {
    return feature_address_cfg;
}

const EebusDataCfg* ModelGetEntityAddressCfg(void) {
    static const EebusDataCfg entity_address_tmp_cfg = EEBUS_DATA_SEQUENCE_TMP(FeatureAddressType, entity_address_cfg);
    return &entity_address_tmp_cfg;
}

const EebusDataCfg* ModelGetFeatureAddressElementsCfg(void) {
    return feature_address_elements_cfg;
}

const EebusDataCfg* ModelGetFunctionPropertyCfg(void) {
    return function_property_cfg;
}

const EebusDataCfg* ModelGetFunctionPropertyElementsCfg(void) {
    return function_property_elements_cfg;
}

const EebusDataCfg* ModelGetPossibleOperationsCfg(void) {
    return possible_operations_cfg;
}

const EebusDataCfg* ModelGetCmdCfg(void) {
    return &cmd_element_data_cfg;
}

const EebusDataCfg* ModelGetDataCfg(FunctionType function_type) {
    if (function_type >= kFunctionTypeNum) {
        EEBUS_ASSERT_ALWAYS();
        return NULL;
    }

    return &data_choice_data_cfg[function_type];
}

const EebusDataCfg* ModelGetHeaderCfg(void) {
    return &datagram_sequence_data_cfg[0];
}

const EebusDataCfg* ModelGetDataSelectorsCfg(FunctionType function_type) {
    if (function_type >= kFunctionTypeNum) {
        EEBUS_ASSERT_ALWAYS();
        return NULL;
    }

    return &data_selectors_choice_data_cfg[function_type];
}

const EebusDataCfg* ModelGetDataElementsCfg(FunctionType function_type) {
    if (function_type >= kFunctionTypeNum) {
        EEBUS_ASSERT_ALWAYS();
        return NULL;
    }

    return &data_elements_choice_data_cfg[function_type];
}

const EebusDataCfg* ModelGetDetailedDiscoveryDeviceInfoCfg(void) {
    static const EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(
        NodeManagementDetailedDiscoveryDeviceInformationType,
        node_management_detailed_discovery_device_information_cfg
    );
    return &cfg;
}

const EebusDataCfg* ModelGetDetailedDiscoveryEntityInfoCfg(void) {
    return &entity_information_element_data_cfg;
}

const EebusDataCfg* ModelGetDetailedDiscoveryFeatureInfoCfg(void) {
    return &feature_information_element_data_cfg;
}

const EebusDataCfg* ModelGetSubscriptionManagementEntryDataCfg(void) {
    return &subscription_entry_element_data_cfg;
}

const EebusDataCfg* ModelGetBindingManagementEntryDataCfg(void) {
    return &binding_entry_element_data_cfg;
}

const EebusDataCfg* ModelGetDestinationDataCfg(void) {
    return &node_management_destination_data_element_data_cfg;
}

const EebusDataCfg* ModelGetUseCaseInformationDataCfg(void) {
    return &use_case_information_data_element_data_cfg;
}

const EebusDataCfg* ModelGetUseCaseSupportElementDataCfg(void) {
    return &use_case_support_element_data_cfg;
}

void* ModelDataCreateEmpty(const EebusDataCfg* cfg) {
    void* data = NULL;
    EEBUS_DATA_CREATE_EMPTY(cfg, &data);
    return data;
}

void ModelDataDelete(const EebusDataCfg* cfg, void* data) {
    if ((data == NULL) || (cfg == NULL)) {
        return;
    }

    EEBUS_DATA_DELETE(cfg, &data);
}

void* ModelDataCopy(const EebusDataCfg* cfg, const void* data) {
    void* data_copy = NULL;

    const EebusError err = EEBUS_DATA_COPY(cfg, &data, &data_copy);
    if (err != kEebusErrorOk) {
        EEBUS_DATA_DELETE(cfg, &data_copy);
        return NULL;
    }

    return data_copy;
}

void* ModelFunctionDataCreateEmpty(FunctionType function_type) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(function_type);
    return ModelDataCreateEmpty(cfg);
}

void ModelFunctionDataDelete(FunctionType function_type, void* data) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(function_type);
    if (cfg == NULL) {
        return;
    }

    ModelDataDelete(cfg, data);
}

void* ModelFunctionDataCopy(FunctionType function_type, const void* data) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(function_type);
    if (cfg == NULL) {
        return NULL;
    }

    return ModelDataCopy(cfg, data);
}

const EebusDataCfg* ModelGetDataListElementCfg(FunctionType function_type) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(function_type);
    if (cfg == NULL) {
        return NULL;
    }

    if ((!EEBUS_DATA_IS_SEQUENCE(cfg)) && (!EEBUS_DATA_IS_CONTAINER(cfg))) {
        return NULL;
    }

    const EebusDataCfg* const list_cfg = (const EebusDataCfg*)cfg->metadata;
    if ((list_cfg == NULL) || (!EEBUS_DATA_IS_LIST(list_cfg))) {
        return NULL;
    }

    return (const EebusDataCfg*)list_cfg->metadata;
}

void* ModelDataListElementCopy(FunctionType function_type, const void* element_data) {
    const EebusDataCfg* const list_element_cfg = ModelGetDataListElementCfg(function_type);
    return ModelDataCopy(list_element_cfg, element_data);
}

void ModelDataListElementDelete(FunctionType function_type, void* element_data) {
    const EebusDataCfg* const list_element_cfg = ModelGetDataListElementCfg(function_type);
    ModelDataDelete(list_element_cfg, element_data);
}

const DeviceTypeType* ModelStringToDeviceType(const char* s) {
    const EnumMapping* mapping = EebusDataGetEnumMappingWithName(device_type_lut, s);
    return (mapping != NULL) ? &mapping->value : NULL;
}

const char* ModelFeatureTypeToString(FeatureTypeType feature_type) {
    const EnumMapping* mapping = EebusDataGetEnumMappingWithValue(feature_type_lut, feature_type);
    return (mapping != NULL) ? mapping->name : NULL;
}

const char* ModelRoleToString(RoleType role) {
    const EnumMapping* mapping = EebusDataGetEnumMappingWithValue(role_lut, role);
    return (mapping != NULL) ? mapping->name : NULL;
}

//---------------------------------------------------------------------------//
//
// EEBUS SPINE RFE related Data Configuration Getters
//
//---------------------------------------------------------------------------//
const EebusDataCfg* ModelGetListItemCfgBegin(const EebusDataCfg* data_list_cfg) {
    if ((data_list_cfg == NULL) || (!EEBUS_DATA_IS_LIST(data_list_cfg))) {
        return NULL;
    }

    const EebusDataCfg* const data_list_item_cfg = (const EebusDataCfg*)data_list_cfg->metadata;
    if ((data_list_item_cfg == NULL) || (!EEBUS_DATA_IS_SEQUENCE(data_list_item_cfg))) {
        return NULL;
    }

    // Return pointer to first element in sequence configuration
    return (const EebusDataCfg*)data_list_item_cfg->metadata;
}

const EebusDataCfg* ModelGetDataListItemCfgBegin(FunctionType function_type) {
    if (function_type >= kFunctionTypeNum) {
        return NULL;
    }

    const EebusDataCfg* const data_choice_cfg = &data_choice_data_cfg[function_type];

    if (!EEBUS_DATA_IS_SEQUENCE(data_choice_cfg)) {
        return NULL;
    }

    return ModelGetListItemCfgBegin((const EebusDataCfg*)data_choice_cfg->metadata);
}

const EebusDataCfg* ModelGetSelectorsCfgBegin(FunctionType function_type) {
    if (function_type >= kFunctionTypeNum) {
        return NULL;
    }

    const EebusDataCfg* selectors_choice_cfg = &data_selectors_choice_data_cfg[function_type];

    if ((!EEBUS_DATA_IS_SEQUENCE(selectors_choice_cfg)) || (selectors_choice_cfg->metadata == NULL)) {
        return NULL;
    }

    return (const EebusDataCfg*)selectors_choice_cfg->metadata;
}

const EebusDataCfg* ModelGetCfgWithName(const EebusDataCfg* cfg_first, const char* name) {
    if ((cfg_first == NULL) || (name == NULL)) {
        return NULL;
    }

    for (const EebusDataCfg* cfg = cfg_first; cfg->name != NULL; ++cfg) {
        if (!strcmp(cfg->name, name)) {
            return cfg;
        }
    }

    return NULL;
}
