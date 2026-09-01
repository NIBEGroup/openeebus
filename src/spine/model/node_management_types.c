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
#include "src/spine/model/node_management_types.h"

#include <string.h>

#include "src/common/array_util.h"
#include "src/common/bool_ptr.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/model/model.h"

NodeManagementDetailedDiscoveryDeviceInformationType* NodeManagementDetailedDiscoveryDeviceInformationCreate(
    const char* device_addr,
    const DeviceTypeType* device_type,
    const NetworkManagementFeatureSetType* feature_set
) {
    const DeviceAddressType device_addr_tmp = {
        .device = device_addr,
    };

    const NetworkManagementDeviceDescriptionDataType device_description_tmp = {
        .device_address      = &device_addr_tmp,
        .device_type         = device_type,
        .network_feature_set = feature_set,
    };

    NodeManagementDetailedDiscoveryDeviceInformationType device_info_tmp = {
        .description = &device_description_tmp,
    };

    const EebusDataCfg* const cfg = ModelGetDetailedDiscoveryDeviceInfoCfg();
    return ModelDataCopy(cfg, &device_info_tmp);
}

NodeManagementDetailedDiscoveryEntityInformationType* NodeManagementDetailedDiscoveryEntityInformationCreate(
    const EntityAddressType* entity_addr,
    EntityTypeType entity_type
) {
    if (entity_addr == NULL) {
        return NULL;
    }

    const EntityAddressType entity_addr_tmp = {
        .device      = NULL,
        .entity      = entity_addr->entity,
        .entity_size = entity_addr->entity_size,
    };

    const NetworkManagementEntityDescriptionDataType entity_description_tmp = {
        .entity_address = &entity_addr_tmp,
        .entity_type    = &entity_type,
    };

    NodeManagementDetailedDiscoveryEntityInformationType entity_info_tmp = {
        .description = &entity_description_tmp,
    };

    const EebusDataCfg* const cfg = ModelGetDetailedDiscoveryEntityInfoCfg();
    return ModelDataCopy(cfg, &entity_info_tmp);
}

NodeManagementDetailedDiscoveryFeatureInformationType* NodeManagementDetailedDiscoveryFeatureInformationCreate(
    const FeatureAddressType* feature_address,
    FeatureTypeType feature_type,
    RoleType role,
    DescriptionType description
) {
    if (feature_address == NULL) {
        return NULL;
    }

    const FeatureAddressType feature_address_tmp = {
        .device      = NULL,
        .entity      = feature_address->entity,
        .entity_size = feature_address->entity_size,
        .feature     = feature_address->feature,
    };

    const NetworkManagementFeatureDescriptionDataType feature_description = {
        .feature_address    = &feature_address_tmp,
        .feature_type       = &feature_type,
        .role               = &role,
        .description        = description,
        .supported_function = NULL,
    };

    NodeManagementDetailedDiscoveryFeatureInformationType nm_feature_info_tmp = {
        .description = &feature_description,
    };

    const EebusDataCfg* const cfg = ModelGetDetailedDiscoveryFeatureInfoCfg();
    return ModelDataCopy(cfg, &nm_feature_info_tmp);
}

void NodeManagementDetailedDiscoveryFeatureInformationDelete(
    NodeManagementDetailedDiscoveryFeatureInformationType* feature_info
) {
    const EebusDataCfg* const cfg = ModelGetDetailedDiscoveryFeatureInfoCfg();
    EEBUS_DATA_DELETE(cfg, &feature_info);
}

NodeManagementDetailedDiscoveryDataType* NodeManagementDetailedDiscoveryDataCreate(
    const SpecificationVersionDataType* spec_versions,
    size_t spec_versions_size
) {
    NodeManagementSpecificationVersionListType version_list = {
        .specification_version      = spec_versions,
        .specification_version_size = spec_versions_size,
    };

    NodeManagementDetailedDiscoveryDataType discovery_data_tmp = {
        .specification_version_list = &version_list,
    };

    const EebusDataCfg* const cfg = ModelGetDataCfg(kFunctionTypeNodeManagementDetailedDiscoveryData);
    return ModelDataCopy(cfg, &discovery_data_tmp);
}

void NodeManagementDetailedDiscoveryDataDelete(NodeManagementDetailedDiscoveryDataType* discovery_data) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(kFunctionTypeNodeManagementDetailedDiscoveryData);
    EEBUS_DATA_DELETE(cfg, &discovery_data);
}

NodeManagementSubscriptionDataType* NodeManagementSubscriptionDataCreateEmpty(void) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(kFunctionTypeNodeManagementSubscriptionData);
    return (NodeManagementSubscriptionDataType*)ModelDataCreateEmpty(cfg);
}

void NodeManagementSubscriptionDataDelete(NodeManagementSubscriptionDataType* subscription_data) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(kFunctionTypeNodeManagementSubscriptionData);
    EEBUS_DATA_DELETE(cfg, &subscription_data);
}

NodeManagementBindingDataType* NodeManagementBindingDataCreateEmpty(void) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(kFunctionTypeNodeManagementBindingData);
    return (NodeManagementBindingDataType*)ModelDataCreateEmpty(cfg);
}

void NodeManagementBindingDataDelete(NodeManagementBindingDataType* binding_data) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(kFunctionTypeNodeManagementBindingData);
    EEBUS_DATA_DELETE(cfg, &binding_data);
}

NodeManagementDestinationDataType* NodeManagementDestinationDataCreate(
    const char* device_addr,
    const DeviceTypeType* device_type,
    const NetworkManagementFeatureSetType* feature_set
) {
    const DeviceAddressType addr = {
        .device = device_addr,
    };

    const NetworkManagementDeviceDescriptionDataType description = {
        .device_address      = &addr,
        .device_type         = device_type,
        .network_feature_set = feature_set,
    };

    const NodeManagementDestinationDataType detiantion_data_tmp = {
        .device_description = &description,
    };

    const EebusDataCfg* const cfg = ModelGetDestinationDataCfg();
    return ModelDataCopy(cfg, &detiantion_data_tmp);
}

void NodeManagementDestinationDataDelete(NodeManagementDestinationDataType* destination_data) {
    const EebusDataCfg* const cfg = ModelGetDestinationDataCfg();
    EEBUS_DATA_DELETE(cfg, &destination_data);
}

NodeManagementUseCaseDataType* NodeManagementUseCaseDataCreateEmpty(void) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(kFunctionTypeNodeManagementUseCaseData);
    return ModelDataCreateEmpty(cfg);
}

const UseCaseInformationDataType* NodeManagementUseCaseDataFindInfo(
    const NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr,
    const UseCaseActorType* actor,
    UseCaseNameType* use_case_name_id
) {
    for (size_t i = 0; i < use_case_data->use_case_information_size; ++i) {
        if (UseCaseInformationMatch(use_case_data->use_case_information[i], addr, actor, use_case_name_id)) {
            return use_case_data->use_case_information[i];
        }
    }

    return NULL;
}

EebusError NodeManagementUseCaseDataAddUseCaseSupport(
    NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id,
    SpecificationVersionType version,
    const char* sub_revision,
    bool available,
    const UseCaseScenarioSupportType* scenarios,
    size_t scenarios_size
) {
    const UseCaseScenarioSupportType** const scenarios_ar
        = (const UseCaseScenarioSupportType**)EEBUS_MALLOC(scenarios_size * sizeof(UseCaseScenarioSupportType*));
    if (scenarios_ar == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    for (size_t i = 0; i < scenarios_size; ++i) {
        scenarios_ar[i] = &scenarios[i];
    }

    const UseCaseSupportType use_case_support = {
        .use_case_name                  = &use_case_name_id,
        .use_case_version               = version,
        .use_case_available             = &available,
        .scenario_support               = scenarios_ar,
        .scenario_support_size          = scenarios_size,
        .use_case_document_sub_revision = sub_revision,
    };

    // Is there an entry for the entity address and actor
    const UseCaseInformationDataType* use_case_info
        = NodeManagementUseCaseDataFindInfo(use_case_data, addr, &actor, NULL);

    EebusError result;
    if (use_case_info != NULL) {
        result = UseCaseInformationAdd((UseCaseInformationDataType*)use_case_info, &use_case_support);
    } else {
        use_case_info = UseCaseInformationDataCreate(addr, actor, &use_case_support);
        result        = EebusDataListDataAppend(
            (void***)&use_case_data->use_case_information,
            &use_case_data->use_case_information_size,
            use_case_info
        );
    }

    EEBUS_FREE(scenarios_ar);
    return result;
}

bool NodeManagementUseCaseDataHasUseCaseSupport(
    const NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id
) {
    return NodeManagementUseCaseDataFindInfo(use_case_data, addr, &actor, &use_case_name_id) != NULL;
}

void NodeManagementUseCaseDataSetAvailability(
    NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id,
    bool available
) {
    const UseCaseInformationDataType* const use_case_info
        = NodeManagementUseCaseDataFindInfo(use_case_data, addr, &actor, &use_case_name_id);

    if (use_case_info == NULL) {
        return;
    }

    for (size_t i = 0; i < use_case_info->use_case_support_size; ++i) {
        UseCaseSupportType* const use_case_support = (UseCaseSupportType*)use_case_info->use_case_support[i];
        if ((use_case_support->use_case_name != NULL) && (*use_case_support->use_case_name == use_case_name_id)) {
            BoolDelete((bool*)use_case_support->use_case_available);
            use_case_support->use_case_available = BoolCreate(available);
            return;
        }
    }
}

void NodeManagementUseCaseDataRemoveWithAddress(
    NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr
) {
    size_t n = 0;
    for (size_t i = 0; i < use_case_data->use_case_information_size; ++i) {
        if (!FeatureAddressCompare(use_case_data->use_case_information[i]->address, addr)) {
            ++n;
        }
    }

    if (n == use_case_data->use_case_information_size) {
        return;
    }

    const UseCaseInformationDataType** const use_case_info_new
        = (const UseCaseInformationDataType**)EEBUS_MALLOC(sizeof(use_case_info_new[0]) * n);

    if (use_case_info_new == NULL) {
        return;
    }

    for (size_t i = 0, j = 0; i < use_case_data->use_case_information_size; ++i) {
        if (!FeatureAddressCompare(use_case_data->use_case_information[i]->address, addr)) {
            use_case_info_new[j++] = use_case_data->use_case_information[i];
        } else {
            UseCaseInformationDataDelete((UseCaseInformationDataType*)use_case_data->use_case_information[i]);
        }
    }

    EEBUS_FREE((void*)use_case_data->use_case_information);
    use_case_data->use_case_information      = use_case_info_new;
    use_case_data->use_case_information_size = n;
}

bool UseCaseInformationMatchAnyFilter(
    const UseCaseInformationDataType* use_case_info,
    const FeatureAddressType* addr,
    const UseCaseFilterType* use_case_filters,
    size_t use_case_filters_size
) {
    for (size_t i = 0; i < use_case_filters_size; ++i) {
        const UseCaseActorType* const actor = &use_case_filters[i].actor;
        const UseCaseNameType* const name   = &use_case_filters[i].use_case_name_id;
        if (UseCaseInformationMatch(use_case_info, addr, actor, name)) {
            return true;
        }
    }

    return false;
}

void NodeManagementUseCaseDataRemoveWithFilters(
    NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr,
    const UseCaseFilterType* use_case_filters,
    size_t use_case_filters_size
) {
    size_t n = 0;
    for (size_t i = 0; i < use_case_data->use_case_information_size; ++i) {
        for (size_t j = 0; j < use_case_filters_size; ++j) {
            const UseCaseInformationDataType* const info = use_case_data->use_case_information[i];
            if (!UseCaseInformationMatchAnyFilter(info, addr, use_case_filters, use_case_filters_size)) {
                ++n;
            }
        }
    }

    if (n == use_case_data->use_case_information_size) {
        return;
    }

    const UseCaseInformationDataType** const use_case_info_new
        = (const UseCaseInformationDataType**)EEBUS_MALLOC(sizeof(use_case_info_new[0]) * n);

    if (use_case_info_new == NULL) {
        return;
    }

    for (size_t i = 0, j = 0; i < use_case_data->use_case_information_size; ++i) {
        const UseCaseInformationDataType* const info = use_case_data->use_case_information[i];
        if (!UseCaseInformationMatchAnyFilter(info, addr, use_case_filters, use_case_filters_size)) {
            use_case_info_new[j++] = use_case_data->use_case_information[i];
        } else {
            UseCaseInformationDataDelete((UseCaseInformationDataType*)use_case_data->use_case_information[i]);
        }
    }

    EEBUS_FREE((void*)use_case_data->use_case_information);
    use_case_data->use_case_information      = use_case_info_new;
    use_case_data->use_case_information_size = n;
}
