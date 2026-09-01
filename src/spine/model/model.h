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
 * @brief SPINE Model declaration
 */

#ifndef SRC_SPINE_MODEL_MODEL_H_
#define SRC_SPINE_MODEL_MODEL_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/spine/model/device_types.h"
#include "src/spine/model/feature_types.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @defgroup EebusDataConfigurationGetters EEBUS Data Configuration getters
 * @{
 */
const EebusDataCfg* ModelGetDatagramCfg(void);
const EebusDataCfg* ModelGetDatagramHeaderCfg(void);
const EebusDataCfg* ModelGetHeaderCfg(void);
const EebusDataCfg* ModelGetFeatureAddressCfg(void);
const EebusDataCfg* ModelGetEntityAddressCfg(void);
const EebusDataCfg* ModelGetFeatureAddressElementsCfg(void);
const EebusDataCfg* ModelGetFunctionPropertyCfg(void);
const EebusDataCfg* ModelGetFunctionPropertyElementsCfg(void);
const EebusDataCfg* ModelGetPossibleOperationsCfg(void);
const EebusDataCfg* ModelGetCmdCfg(void);
const EebusDataCfg* GetFilterCfg(void);
const EebusDataCfg* ModelGetDataCfg(FunctionType function_type);
const EebusDataCfg* ModelGetDataSelectorsCfg(FunctionType function_type);
const EebusDataCfg* ModelGetDataElementsCfg(FunctionType function_type);
const EebusDataCfg* ModelGetDetailedDiscoveryDeviceInfoCfg(void);
const EebusDataCfg* ModelGetDetailedDiscoveryEntityInfoCfg(void);
const EebusDataCfg* ModelGetDetailedDiscoveryFeatureInfoCfg(void);
const EebusDataCfg* ModelGetSubscriptionManagementEntryDataCfg(void);
const EebusDataCfg* ModelGetBindingManagementEntryDataCfg(void);
const EebusDataCfg* ModelGetDestinationDataCfg(void);
const EebusDataCfg* ModelGetUseCaseInformationDataCfg(void);
const EebusDataCfg* ModelGetUseCaseSupportElementDataCfg(void);

void* ModelDataCreateEmpty(const EebusDataCfg* cfg);
void* ModelDataCopy(const EebusDataCfg* cfg, const void* data);
void ModelDataDelete(const EebusDataCfg* cfg, void* data);

void* ModelFunctionDataCreateEmpty(FunctionType function_type);
void ModelFunctionDataDelete(FunctionType function_type, void* data);
void* ModelFunctionDataCopy(FunctionType function_type, const void* data);

const EebusDataCfg* ModelGetDataListElementCfg(FunctionType function_type);
void* ModelDataListElementCopy(FunctionType function_type, const void* element_data);
void ModelDataListElementDelete(FunctionType function_type, void* element_data);

const DeviceTypeType* ModelStringToDeviceType(const char* s);
const char* ModelFeatureTypeToString(FeatureTypeType feature_type);
const char* ModelRoleToString(RoleType role);
/** @{ */

/**
 * @defgroup SpineRfeConfigurationGetters EEBUS SPINE RFE related Data Configuration Getters
 */
const EebusDataCfg* ModelGetDataListItemCfgBegin(FunctionType function_type);
const EebusDataCfg* ModelGetSelectorsCfgBegin(FunctionType function_type);
const EebusDataCfg* ModelGetCfgWithName(const EebusDataCfg* cfg_first, const char* name);
/** @{ */

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_MODEL_H_
