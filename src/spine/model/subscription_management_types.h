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
 * @brief SPINE Datagram Subscription Management data types declarations
 */

#ifndef SRC_SPINE_MODEL_SUBSCRIPTION_MANAGEMENT_TYPES_H_
#define SRC_SPINE_MODEL_SUBSCRIPTION_MANAGEMENT_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/feature_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t SubscriptionIdType;

typedef struct SubscriptionManagementEntryDataType SubscriptionManagementEntryDataType;

struct SubscriptionManagementEntryDataType {
    const SubscriptionIdType* subscription_id; /**< "subscriptionId" */
    const FeatureAddressType* client_address;  /**< "clientAddress" */
    const FeatureAddressType* server_address;  /**< "serverAddress" */
    LabelType label;                           /**< "label" */
    DescriptionType description;               /**< "description" */
};

typedef struct SubscriptionManagementEntryDataElementsType SubscriptionManagementEntryDataElementsType;

struct SubscriptionManagementEntryDataElementsType {
    const ElementTagType subscription_id;             /**< "subscriptionId" */
    const FeatureAddressElementsType* client_address; /**< "clientAddress" */
    const FeatureAddressElementsType* server_address; /**< "serverAddress" */
    ElementTagType label;                             /**< "label" */
    ElementTagType description;                       /**< "description" */
};

typedef struct SubscriptionManagementEntryListDataType SubscriptionManagementEntryListDataType;

struct SubscriptionManagementEntryListDataType {
    const SubscriptionManagementEntryDataType* const*
        subscription_management_entry_data; /**< "subscriptionManagementEntryData" */
    size_t subscription_management_entry_data_size;
};

typedef struct SubscriptionManagementEntryListDataSelectorsType SubscriptionManagementEntryListDataSelectorsType;

struct SubscriptionManagementEntryListDataSelectorsType {
    const SubscriptionIdType* subscription_id; /**< "subscriptionId" */
    const FeatureAddressType* client_address;  /**< "clientAddress" */
    const FeatureAddressType* server_address;  /**< "serverAddress" */
};

typedef struct SubscriptionManagementRequestCallType SubscriptionManagementRequestCallType;

struct SubscriptionManagementRequestCallType {
    const FeatureAddressType* client_address;   /**< "clientAddress" */
    const FeatureAddressType* server_address;   /**< "serverAddress" */
    const FeatureTypeType* server_feature_type; /**< "serverFeatureType" */
};

typedef struct SubscriptionManagementRequestCallElementsType SubscriptionManagementRequestCallElementsType;

struct SubscriptionManagementRequestCallElementsType {
    const FeatureAddressElementsType* client_address; /**< "clientAddress" */
    const FeatureAddressElementsType* server_address; /**< "serverAddress" */
    ElementTagType server_feature_type;               /**< "serverFeatureType" */
};

typedef struct SubscriptionManagementDeleteCallType SubscriptionManagementDeleteCallType;

struct SubscriptionManagementDeleteCallType {
    const SubscriptionIdType* subscription_id; /**< "subscriptionId" */
    const FeatureAddressType* client_address;  /**< "clientAddress" */
    const FeatureAddressType* server_address;  /**< "serverAddress" */
};

typedef struct SubscriptionManagementDeleteCallElementsType SubscriptionManagementDeleteCallElementsType;

struct SubscriptionManagementDeleteCallElementsType {
    ElementTagType subscription_id;                   /**< "subscriptionId" */
    const FeatureAddressElementsType* client_address; /**< "clientAddress" */
    const FeatureAddressElementsType* server_address; /**< "serverAddress" */
};

SubscriptionManagementEntryDataType* SubscriptionManagementEntryDataCreate(
    SubscriptionIdType id,
    const FeatureAddressType* server_addr,
    const FeatureAddressType* client_addr
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_SUBSCRIPTION_MANAGEMENT_TYPES_H_
