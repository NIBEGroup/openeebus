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
#include "src/spine/model/usecase_information_types.h"

#include "src/common/array_util.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/spine/model/model.h"

UseCaseInformationDataType* UseCaseInformationDataCreate(
    const FeatureAddressType* addr,
    UseCaseActorType actor,
    const UseCaseSupportType* use_case_support
) {
    // Create a new element for this entity
    FeatureAddressType addr_tmp = {
        .device      = addr->device,
        .entity      = addr->entity,
        .entity_size = addr->entity_size,
    };

    const UseCaseSupportType* const use_case_support_ar[1] = {(UseCaseSupportType*)use_case_support};

    const UseCaseInformationDataType use_case_info_tmp = {
        .address               = &addr_tmp,
        .actor                 = &actor,
        .use_case_support      = use_case_support_ar,
        .use_case_support_size = ARRAY_SIZE(use_case_support_ar),
    };

    const EebusDataCfg* cfg = ModelGetUseCaseInformationDataCfg();
    return ModelDataCopy(cfg, &use_case_info_tmp);
}

void UseCaseInformationDataDelete(UseCaseInformationDataType* use_case_info) {
    const EebusDataCfg* cfg = ModelGetUseCaseInformationDataCfg();
    EEBUS_DATA_DELETE(cfg, &use_case_info);
}

bool ActorMatch(const UseCaseInformationDataType* use_case_info, const UseCaseActorType* actor) {
    if (actor == NULL) {
        return true;
    }

    return (use_case_info->actor != NULL) && (*use_case_info->actor == *actor);
}

const UseCaseSupportType*
UseCaseInformationFind(const UseCaseInformationDataType* use_case_info, const UseCaseNameType* use_case_name_id) {
    if (use_case_name_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < use_case_info->use_case_support_size; ++i) {
        const UseCaseSupportType* const use_case_support = use_case_info->use_case_support[i];
        if ((use_case_support->use_case_name != NULL) && (*use_case_support->use_case_name == *use_case_name_id)) {
            return use_case_support;
        }
    }

    return NULL;
}

bool UseCaseNameIdMatch(const UseCaseInformationDataType* use_case_info, const UseCaseNameType* use_case_name_id) {
    if (use_case_name_id == NULL) {
        return true;
    }

    return UseCaseInformationFind(use_case_info, use_case_name_id) != NULL;
}

bool UseCaseInformationMatch(
    const UseCaseInformationDataType* use_case_info,
    const FeatureAddressType* addr,
    const UseCaseActorType* actor,
    const UseCaseNameType* use_case_name_id
) {
    // TODO: Update address match procedure accroding to specification
    const FeatureAddressType addr_a = {
        .device      = use_case_info->address->device,
        .entity      = use_case_info->address->entity,
        .entity_size = use_case_info->address->entity_size,
    };

    const FeatureAddressType addr_b = {
        .device      = addr->device,
        .entity      = addr->entity,
        .entity_size = addr->entity_size,
    };

    bool match = true;
    if ((addr_a.entity == NULL) || (addr_b.entity == NULL)) {
        const size_t device_addr_len = strlen(addr_a.device);

        match = match && (strncmp(addr_a.device, addr_b.device, device_addr_len) == 0);
    } else {
        match = match && FeatureAddressCompare(&addr_a, &addr_b);
    }

    match = match && ActorMatch(use_case_info, actor);
    match = match && UseCaseNameIdMatch(use_case_info, use_case_name_id);
    return match;
}

EebusError
UseCaseInformationAdd(UseCaseInformationDataType* use_case_info, const UseCaseSupportType* use_case_support) {
    if ((use_case_info == NULL) || (use_case_support == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    const EebusDataCfg* cfg                         = ModelGetUseCaseSupportElementDataCfg();
    UseCaseSupportType* const use_case_support_copy = ModelDataCopy(cfg, use_case_support);

    return EebusDataListDataAppend(
        (void***)&use_case_info->use_case_support,
        &use_case_info->use_case_support_size,
        use_case_support_copy
    );
}
