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
#include "src/spine/model/binding_management_types.h"

#include "src/spine/model/model.h"

BindingManagementEntryDataType* BindingManagementEntryDataCreate(
    BindingIdType id,
    const FeatureAddressType* server_addr,
    const FeatureAddressType* client_addr
) {
    BindingManagementEntryDataType binding_entry_data_tmp = {
        .binding_id     = &id,
        .client_address = client_addr,
        .server_address = server_addr,
    };

    const EebusDataCfg* const cfg = ModelGetBindingManagementEntryDataCfg();
    return ModelDataCopy(cfg, &binding_entry_data_tmp);
}
