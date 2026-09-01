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
 * @brief SPINE Datagram Node Management data types declarations
 */

#ifndef SRC_SPINE_MODEL_NODE_MANAGEMENT_TYPES_H_
#define SRC_SPINE_MODEL_NODE_MANAGEMENT_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/spine/model/binding_management_types.h"
#include "src/spine/model/common_data_types.h"
#include "src/spine/model/network_management_types.h"
#include "src/spine/model/subscription_management_types.h"
#include "src/spine/model/usecase_information_types.h"
#include "src/spine/model/version_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define NODE_MANAGEMENT_ENTITY_ID ((uint32_t)0)
#define NODE_MANAGEMENT_FEATURE_ID ((uint32_t)0)

typedef struct NodeManagementSpecificationVersionListType NodeManagementSpecificationVersionListType;

struct NodeManagementSpecificationVersionListType {
    const SpecificationVersionDataType* specification_version; /**< "specificationVersion" */
    size_t specification_version_size;
};

typedef struct NodeManagementSpecificationVersionListElementsType NodeManagementSpecificationVersionListElementsType;

struct NodeManagementSpecificationVersionListElementsType {
    SpecificationVersionDataElementsType specification_version; /**< "specificationVersion" */
};

typedef struct NodeManagementDetailedDiscoveryDeviceInformationType
    NodeManagementDetailedDiscoveryDeviceInformationType;

struct NodeManagementDetailedDiscoveryDeviceInformationType {
    const NetworkManagementDeviceDescriptionDataType* description; /**< "description" */
};

typedef struct NodeManagementDetailedDiscoveryDeviceInformationElementsType
    NodeManagementDetailedDiscoveryDeviceInformationElementsType;

struct NodeManagementDetailedDiscoveryDeviceInformationElementsType {
    const NetworkManagementDeviceDescriptionDataElementsType* description; /**< "description" */
};

typedef struct NodeManagementDetailedDiscoveryEntityInformationType
    NodeManagementDetailedDiscoveryEntityInformationType;

struct NodeManagementDetailedDiscoveryEntityInformationType {
    const NetworkManagementEntityDescriptionDataType* description; /**< "description" */
};

typedef struct NodeManagementDetailedDiscoveryEntityInformationElementsType
    NodeManagementDetailedDiscoveryEntityInformationElementsType;

struct NodeManagementDetailedDiscoveryEntityInformationElementsType {
    const NetworkManagementEntityDescriptionDataElementsType* description; /**< "description" */
};

typedef struct NodeManagementDetailedDiscoveryFeatureInformationType
    NodeManagementDetailedDiscoveryFeatureInformationType;

struct NodeManagementDetailedDiscoveryFeatureInformationType {
    const NetworkManagementFeatureDescriptionDataType* description; /**< "description" */
};

typedef struct NodeManagementDetailedDiscoveryFeatureInformationElementsType
    NodeManagementDetailedDiscoveryFeatureInformationElementsType;

struct NodeManagementDetailedDiscoveryFeatureInformationElementsType {
    const NetworkManagementFeatureDescriptionDataElementsType* description; /**< "description" */
};

typedef struct NodeManagementDetailedDiscoveryDataType NodeManagementDetailedDiscoveryDataType;

struct NodeManagementDetailedDiscoveryDataType {
    const NodeManagementSpecificationVersionListType* specification_version_list;   /**< "specificationVersionList" */
    const NodeManagementDetailedDiscoveryDeviceInformationType* device_information; /**< "deviceInformation" */
    const NodeManagementDetailedDiscoveryEntityInformationType* const* entity_information; /**< "entityInformation" */
    size_t entity_information_size;
    const NodeManagementDetailedDiscoveryFeatureInformationType* const*
        feature_information; /**< "featureInformation" */
    size_t feature_information_size;
};

typedef struct NodeManagementDetailedDiscoveryDataElementsType NodeManagementDetailedDiscoveryDataElementsType;

struct NodeManagementDetailedDiscoveryDataElementsType {
    const NodeManagementSpecificationVersionListElementsType*
        specification_version_list; /**< "specificationVersionList" */
    const NodeManagementDetailedDiscoveryDeviceInformationElementsType* device_information; /**< "deviceInformation" */
    const NodeManagementDetailedDiscoveryEntityInformationElementsType* entity_information; /**< "entityInformation" */
    const NodeManagementDetailedDiscoveryFeatureInformationElementsType*
        feature_information; /**< "featureInformation" */
};

typedef struct NodeManagementDetailedDiscoveryDataSelectorsType NodeManagementDetailedDiscoveryDataSelectorsType;

struct NodeManagementDetailedDiscoveryDataSelectorsType {
    const NetworkManagementDeviceDescriptionListDataSelectorsType* device_information;   /**< "deviceInformation" */
    const NetworkManagementEntityDescriptionListDataSelectorsType* entity_information;   /**< "entityInformation" */
    const NetworkManagementFeatureDescriptionListDataSelectorsType* feature_information; /**< "featureInformation" */
};

typedef struct NodeManagementBindingDataType NodeManagementBindingDataType;

struct NodeManagementBindingDataType {
    const BindingManagementEntryDataType* const* binding_entry; /**< "bindingEntry" */
    size_t binding_entry_size;
};

typedef struct NodeManagementBindingDataElementsType NodeManagementBindingDataElementsType;

struct NodeManagementBindingDataElementsType {
    const BindingManagementEntryDataElementsType* binding_entry; /**< "bindingEntry" */
};

typedef struct NodeManagementBindingDataSelectorsType NodeManagementBindingDataSelectorsType;

struct NodeManagementBindingDataSelectorsType {
    const BindingManagementEntryListDataSelectorsType* binding_entry; /**< "bindingEntry" */
};

typedef struct NodeManagementBindingRequestCallType NodeManagementBindingRequestCallType;

struct NodeManagementBindingRequestCallType {
    const BindingManagementRequestCallType* binding_request; /**< "bindingRequest" */
};

typedef struct NodeManagementBindingRequestCallElementsType NodeManagementBindingRequestCallElementsType;

struct NodeManagementBindingRequestCallElementsType {
    const BindingManagementRequestCallElementsType* binding_request; /**< "bindingRequest" */
};

typedef struct NodeManagementBindingDeleteCallType NodeManagementBindingDeleteCallType;

struct NodeManagementBindingDeleteCallType {
    const BindingManagementDeleteCallType* binding_delete; /**< "bindingDelete" */
};

typedef struct NodeManagementBindingDeleteCallElementsType NodeManagementBindingDeleteCallElementsType;

struct NodeManagementBindingDeleteCallElementsType {
    const BindingManagementDeleteCallElementsType* binding_delete; /**< "bindingDelete" */
};

typedef struct NodeManagementSubscriptionDataType NodeManagementSubscriptionDataType;

struct NodeManagementSubscriptionDataType {
    const SubscriptionManagementEntryDataType* const* subscription_entry; /**< "subscriptionEntry" */
    size_t subscription_entry_size;
};

typedef struct NodeManagementSubscriptionDataElementsType NodeManagementSubscriptionDataElementsType;

struct NodeManagementSubscriptionDataElementsType {
    const SubscriptionManagementEntryDataElementsType* subscription_entry; /**< "subscriptionEntry" */
};

typedef struct NodeManagementSubscriptionDataSelectorsType NodeManagementSubscriptionDataSelectorsType;

struct NodeManagementSubscriptionDataSelectorsType {
    const SubscriptionManagementEntryListDataSelectorsType* subscription_entry; /**< "subscriptionEntry" */
};

typedef struct NodeManagementSubscriptionRequestCallType NodeManagementSubscriptionRequestCallType;

struct NodeManagementSubscriptionRequestCallType {
    const SubscriptionManagementRequestCallType* subscription_request; /**< "subscriptionRequest" */
};

typedef struct NodeManagementSubscriptionRequestCallElementsType NodeManagementSubscriptionRequestCallElementsType;

struct NodeManagementSubscriptionRequestCallElementsType {
    const SubscriptionManagementRequestCallElementsType* subscription_request; /**< "subscriptionRequest" */
};

typedef struct NodeManagementSubscriptionDeleteCallType NodeManagementSubscriptionDeleteCallType;

struct NodeManagementSubscriptionDeleteCallType {
    const SubscriptionManagementDeleteCallType* subscription_delete; /**< "subscriptionDelete" */
};

typedef struct NodeManagementSubscriptionDeleteCallElementsType NodeManagementSubscriptionDeleteCallElementsType;

struct NodeManagementSubscriptionDeleteCallElementsType {
    const SubscriptionManagementDeleteCallElementsType* subscription_delete; /**< "subscriptionDelete" */
};

typedef struct NodeManagementDestinationDataType NodeManagementDestinationDataType;

struct NodeManagementDestinationDataType {
    const NetworkManagementDeviceDescriptionDataType* device_description; /**< "deviceDescription" */
};

typedef struct NodeManagementDestinationDataElementsType NodeManagementDestinationDataElementsType;

struct NodeManagementDestinationDataElementsType {
    const NetworkManagementDeviceDescriptionDataElementsType* device_description; /**< "deviceDescription" */
};

typedef struct NodeManagementDestinationListDataType NodeManagementDestinationListDataType;

struct NodeManagementDestinationListDataType {
    const NodeManagementDestinationDataType* const*
        node_management_destination_data; /**< "nodeManagementDestinationData" */
    size_t node_management_destination_data_size;
};

typedef struct NodeManagementDestinationListDataSelectorsType NodeManagementDestinationListDataSelectorsType;

struct NodeManagementDestinationListDataSelectorsType {
    const NetworkManagementDeviceDescriptionListDataSelectorsType* device_description; /**< "deviceDescription" */
};

typedef struct NodeManagementUseCaseDataType NodeManagementUseCaseDataType;

struct NodeManagementUseCaseDataType {
    const UseCaseInformationDataType* const* use_case_information; /**< "useCaseInformation" */
    size_t use_case_information_size;
};

typedef struct NodeManagementUseCaseDataElementsType NodeManagementUseCaseDataElementsType;

struct NodeManagementUseCaseDataElementsType {
    const UseCaseInformationDataElementsType* use_case_information; /**< "useCaseInformation" */
};

typedef struct NodeManagementUseCaseDataSelectorsType NodeManagementUseCaseDataSelectorsType;

struct NodeManagementUseCaseDataSelectorsType {
    const UseCaseInformationListDataSelectorsType* use_case_information; /**< "useCaseInformation" */
};

/** Custom helper type for easier filtering a specific UseCase element */
typedef struct UseCaseFilterType UseCaseFilterType;

struct UseCaseFilterType {
    UseCaseActorType actor;
    UseCaseNameType use_case_name_id;
};

NodeManagementDetailedDiscoveryDeviceInformationType* NodeManagementDetailedDiscoveryDeviceInformationCreate(
    const char* device_addr,
    const DeviceTypeType* device_type,
    const NetworkManagementFeatureSetType* feature_set
);

NodeManagementDetailedDiscoveryEntityInformationType* NodeManagementDetailedDiscoveryEntityInformationCreate(
    const EntityAddressType* entity_addr,
    EntityTypeType entity_type
);

NodeManagementDetailedDiscoveryFeatureInformationType* NodeManagementDetailedDiscoveryFeatureInformationCreate(
    const FeatureAddressType* feature_address,
    FeatureTypeType feature_type,
    RoleType role,
    DescriptionType description
);

void NodeManagementDetailedDiscoveryFeatureInformationDelete(
    NodeManagementDetailedDiscoveryFeatureInformationType* feature_info
);

NodeManagementDetailedDiscoveryDataType*
NodeManagementDetailedDiscoveryDataCreate(const SpecificationVersionDataType* spec_versions, size_t spec_versions_size);

void NodeManagementDetailedDiscoveryDataDelete(NodeManagementDetailedDiscoveryDataType* discovery_data);

NodeManagementSubscriptionDataType* NodeManagementSubscriptionDataCreateEmpty(void);
void NodeManagementSubscriptionDataDelete(NodeManagementSubscriptionDataType* subscription_data);

NodeManagementBindingDataType* NodeManagementBindingDataCreateEmpty(void);
void NodeManagementBindingDataDelete(NodeManagementBindingDataType* binding_data);

NodeManagementDestinationDataType* NodeManagementDestinationDataCreate(
    const char* device_addr,
    const DeviceTypeType* device_type,
    const NetworkManagementFeatureSetType* feature_set
);
void NodeManagementDestinationDataDelete(NodeManagementDestinationDataType* data);

NodeManagementUseCaseDataType* NodeManagementUseCaseDataCreateEmpty(void);

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
);

bool NodeManagementUseCaseDataHasUseCaseSupport(
    const NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id
);

void NodeManagementUseCaseDataSetAvailability(
    NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id,
    bool available
);

void NodeManagementUseCaseDataRemoveWithAddress(
    NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr
);

void NodeManagementUseCaseDataRemoveWithFilters(
    NodeManagementUseCaseDataType* use_case_data,
    const FeatureAddressType* addr,
    const UseCaseFilterType* use_case_filters,
    size_t use_case_filters_size
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_NODE_MANAGEMENT_TYPES_H_
