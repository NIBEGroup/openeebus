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
 * @brief SPINE Datagram Binding Management data types declarations
 */

#ifndef SRC_SPINE_MODEL_BINDING_MANAGEMENT_TYPES_H_
#define SRC_SPINE_MODEL_BINDING_MANAGEMENT_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/feature_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t BindingIdType;

typedef struct BindingManagementEntryDataType BindingManagementEntryDataType;

struct BindingManagementEntryDataType {
    const BindingIdType* binding_id;          /**< "bindingId" */
    const FeatureAddressType* client_address; /**< "clientAddress" */
    const FeatureAddressType* server_address; /**< "serverAddress" */
    LabelType label;                          /**< "label" */
    DescriptionType description;              /**< "description" */
};

typedef struct BindingManagementEntryDataElementsType BindingManagementEntryDataElementsType;

struct BindingManagementEntryDataElementsType {
    ElementTagType binding_id;     /**< "bindingId" */
    ElementTagType client_address; /**< "clientAddress" */
    ElementTagType server_address; /**< "serverAddress" */
    ElementTagType label;          /**< "label" */
    ElementTagType description;    /**< "description" */
};

typedef struct BindingManagementEntryListDataType BindingManagementEntryListDataType;

struct BindingManagementEntryListDataType {
    const BindingManagementEntryDataType* const* binding_management_entry_data; /**< "bindingManagementEntryData" */
    size_t binding_management_entry_data_size;
};

typedef struct BindingManagementEntryListDataSelectorsType BindingManagementEntryListDataSelectorsType;

struct BindingManagementEntryListDataSelectorsType {
    const BindingIdType* binding_id;          /**< "bindingId" */
    const FeatureAddressType* client_address; /**< "clientAddress" */
    const FeatureAddressType* server_address; /**< "serverAddress" */
};

typedef struct BindingManagementRequestCallType BindingManagementRequestCallType;

struct BindingManagementRequestCallType {
    const FeatureAddressType* client_address;   /**< "clientAddress" */
    const FeatureAddressType* server_address;   /**< "serverAddress" */
    const FeatureTypeType* server_feature_type; /**< "serverFeatureType" */
};

typedef struct BindingManagementRequestCallElementsType BindingManagementRequestCallElementsType;

struct BindingManagementRequestCallElementsType {
    ElementTagType client_address;      /**< "clientAddress" */
    ElementTagType server_address;      /**< "serverAddress" */
    ElementTagType server_feature_type; /**< "serverFeatureType" */
};

typedef struct BindingManagementDeleteCallType BindingManagementDeleteCallType;

struct BindingManagementDeleteCallType {
    const BindingIdType* binding_id;          /**< "bindingId" */
    const FeatureAddressType* client_address; /**< "clientAddress" */
    const FeatureAddressType* server_address; /**< "serverAddress" */
};

typedef struct BindingManagementDeleteCallElementsType BindingManagementDeleteCallElementsType;

struct BindingManagementDeleteCallElementsType {
    ElementTagType binding_id;     /**< "bindingId" */
    ElementTagType client_address; /**< "clientAddress" */
    ElementTagType server_address; /**< "serverAddress" */
};

BindingManagementEntryDataType* BindingManagementEntryDataCreate(
    BindingIdType id,
    const FeatureAddressType* server_addr,
    const FeatureAddressType* client_addr
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_BINDING_MANAGEMENT_TYPES_H_
