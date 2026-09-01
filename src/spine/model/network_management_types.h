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
 * @brief SPINE Datagram Network Management data types declarations
 */

#ifndef SRC_SPINE_MODEL_NETWORK_MANAGEMENT_TYPES_H_
#define SRC_SPINE_MODEL_NETWORK_MANAGEMENT_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/device_types.h"
#include "src/spine/model/entity_types.h"
#include "src/spine/model/feature_types.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef const char* NetworkManagementNativeSetupType;

typedef const char* NetworkManagementScanSetupType;

typedef const char* NetworkManagementSetupType;

typedef const char* NetworkManagementCandidateSetupType;

typedef const char* NetworkManagementTechnologyAddressType;

typedef const char* NetworkManagementCommunicationsTechnologyInformationType;

typedef const char* NetworkManagementMinimumTrustLevelType;

typedef DurationType NetworkManagementProcessTimeoutType;

typedef int32_t NetworkManagementFeatureSetType;

enum {
    kNetworkManagementFeatureSetTypeGateway, /**< "gateway" */
    kNetworkManagementFeatureSetTypeRouter,  /**< "router" */
    kNetworkManagementFeatureSetTypeSmart,   /**< "smart" */
    kNetworkManagementFeatureSetTypeSimple,  /**< "simple" */
};

typedef int32_t NetworkManagementProcessStateStateType;

enum {
    kNetworkManagementProcessStateStateTypeSucceeded, /**< "succeeded" */
    kNetworkManagementProcessStateStateTypeFailed,    /**< "failed" */
    kNetworkManagementProcessStateStateTypeAborted,   /**< "aborted" */
};

typedef int32_t NetworkManagementStateChangeType;

enum {
    kNetworkManagementStateChangeTypeAdded,    /**< "added" */
    kNetworkManagementStateChangeTypeRemoved,  /**< "removed" */
    kNetworkManagementStateChangeTypeModified, /**< "modified" */
};

typedef struct NetworkManagementAddNodeCallType NetworkManagementAddNodeCallType;

struct NetworkManagementAddNodeCallType {
    const FeatureAddressType* node_address;             /**< "nodeAddress" */
    NetworkManagementNativeSetupType native_setup;      /**< "nativeSetup" */
    const NetworkManagementProcessTimeoutType* timeout; /**< "timeout" */
    LabelType label;                                    /**< "label" */
    DescriptionType description;                        /**< "description" */
};

typedef struct NetworkManagementAddNodeCallElementsType NetworkManagementAddNodeCallElementsType;

struct NetworkManagementAddNodeCallElementsType {
    const FeatureAddressElementsType* node_address; /**< "nodeAddress" */
    ElementTagType native_setup;                    /**< "nativeSetup" */
    ElementTagType timeout;                         /**< "timeout" */
    ElementTagType label;                           /**< "label" */
    ElementTagType description;                     /**< "description" */
};

typedef struct NetworkManagementRemoveNodeCallType NetworkManagementRemoveNodeCallType;

struct NetworkManagementRemoveNodeCallType {
    const FeatureAddressType* node_address;             /**< "nodeAddress" */
    const NetworkManagementProcessTimeoutType* timeout; /**< "timeout" */
};

typedef struct NetworkManagementRemoveNodeCallElementsType NetworkManagementRemoveNodeCallElementsType;

struct NetworkManagementRemoveNodeCallElementsType {
    const FeatureAddressElementsType* node_address; /**< "nodeAddress" */
    ElementTagType timeout;                         /**< "timeout" */
};

typedef struct NetworkManagementModifyNodeCallType NetworkManagementModifyNodeCallType;

struct NetworkManagementModifyNodeCallType {
    const FeatureAddressType* node_address;             /**< "nodeAddress" */
    NetworkManagementNativeSetupType native_setup;      /**< "nativeSetup" */
    const NetworkManagementProcessTimeoutType* timeout; /**< "timeout" */
    LabelType label;                                    /**< "label" */
    DescriptionType description;                        /**< "description" */
};

typedef struct NetworkManagementModifyNodeCallElementsType NetworkManagementModifyNodeCallElementsType;

struct NetworkManagementModifyNodeCallElementsType {
    const FeatureAddressElementsType* node_address; /**< "nodeAddress" */
    ElementTagType native_setup;                    /**< "nativeSetup" */
    ElementTagType timeout;                         /**< "timeout" */
    ElementTagType label;                           /**< "label" */
    ElementTagType description;                     /**< "description" */
};

typedef struct NetworkManagementScanNetworkCallType NetworkManagementScanNetworkCallType;

struct NetworkManagementScanNetworkCallType {
    NetworkManagementScanSetupType scan_setup;          /**< "scanSetup" */
    const NetworkManagementProcessTimeoutType* timeout; /**< "timeout" */
};

typedef struct NetworkManagementScanNetworkCallElementsType NetworkManagementScanNetworkCallElementsType;

struct NetworkManagementScanNetworkCallElementsType {
    ElementTagType scan_setup; /**< "scanSetup" */
    ElementTagType timeout;    /**< "timeout" */
};

typedef struct NetworkManagementDiscoverCallType NetworkManagementDiscoverCallType;

struct NetworkManagementDiscoverCallType {
    const FeatureAddressType* discover_address; /**< "discoverAddress" */
};

typedef struct NetworkManagementDiscoverCallElementsType NetworkManagementDiscoverCallElementsType;

struct NetworkManagementDiscoverCallElementsType {
    const FeatureAddressElementsType* discover_address; /**< "discoverAddress" */
};

typedef TagType NetworkManagementAbortCallType;

typedef TagType NetworkManagementAbortCallElementsType;

typedef struct NetworkManagementProcessStateDataType NetworkManagementProcessStateDataType;

struct NetworkManagementProcessStateDataType {
    const NetworkManagementProcessStateStateType* state; /**< "state" */
    DescriptionType description;                         /**< "description" */
};

typedef struct NetworkManagementProcessStateDataElementsType NetworkManagementProcessStateDataElementsType;

struct NetworkManagementProcessStateDataElementsType {
    ElementTagType state;       /**< "state" */
    ElementTagType description; /**< "description" */
};

typedef struct NetworkManagementJoiningModeDataType NetworkManagementJoiningModeDataType;

struct NetworkManagementJoiningModeDataType {
    NetworkManagementSetupType setup; /**< "setup" */
};

typedef struct NetworkManagementJoiningModeDataElementsType NetworkManagementJoiningModeDataElementsType;

struct NetworkManagementJoiningModeDataElementsType {
    ElementTagType setup; /**< "setup" */
};

typedef struct NetworkManagementReportCandidateDataType NetworkManagementReportCandidateDataType;

struct NetworkManagementReportCandidateDataType {
    NetworkManagementCandidateSetupType candidate_setup; /**< "candidateSetup" */
    const bool* setup_usable_for_add;                    /**< "setupUsableForAdd" */
    LabelType label;                                     /**< "label" */
    DescriptionType description;                         /**< "description" */
};

typedef struct NetworkManagementReportCandidateDataElementsType NetworkManagementReportCandidateDataElementsType;

struct NetworkManagementReportCandidateDataElementsType {
    ElementTagType candidate_setup;      /**< "candidateSetup" */
    ElementTagType setup_usable_for_add; /**< "setupUsableForAdd" */
    ElementTagType label;                /**< "label" */
    ElementTagType description;          /**< "description" */
};

typedef struct NetworkManagementDeviceDescriptionDataType NetworkManagementDeviceDescriptionDataType;

struct NetworkManagementDeviceDescriptionDataType {
    const DeviceAddressType* device_address;                          /**< "deviceAddress" */
    const DeviceTypeType* device_type;                                /**< "deviceType" */
    const FeatureAddressType* network_management_responsible_address; /**< "networkManagementResponsibleAddress" */
    NetworkManagementNativeSetupType native_setup;                    /**< "nativeSetup" */
    NetworkManagementTechnologyAddressType technology_address;        /**< "technologyAddress" */
    NetworkManagementCommunicationsTechnologyInformationType
        communications_technology_information;                  /**< "communicationsTechnologyInformation" */
    const NetworkManagementFeatureSetType* network_feature_set; /**< "networkFeatureSet" */
    const NetworkManagementStateChangeType* last_state_change;  /**< "lastStateChange" */
    NetworkManagementMinimumTrustLevelType minimum_trust_level; /**< "minimumTrustLevel" */
    LabelType label;                                            /**< "label" */
    DescriptionType description;                                /**< "description" */
};

typedef struct NetworkManagementDeviceDescriptionDataElementsType NetworkManagementDeviceDescriptionDataElementsType;

struct NetworkManagementDeviceDescriptionDataElementsType {
    ElementTagType device_address;                         /**< "deviceAddress" */
    ElementTagType device_type;                            /**< "deviceType" */
    ElementTagType network_management_responsible_address; /**< "networkManagementResponsibleAddress" */
    ElementTagType native_setup;                           /**< "nativeSetup" */
    ElementTagType technology_address;                     /**< "technologyAddress" */
    ElementTagType communications_technology_information;  /**< "communicationsTechnologyInformation" */
    ElementTagType network_feature_set;                    /**< "networkFeatureSet" */
    ElementTagType last_state_change;                      /**< "lastStateChange" */
    ElementTagType minimum_trust_level;                    /**< "minimumTrustLevel" */
    ElementTagType label;                                  /**< "label" */
    ElementTagType description;                            /**< "description" */
};

typedef struct NetworkManagementDeviceDescriptionListDataType NetworkManagementDeviceDescriptionListDataType;

struct NetworkManagementDeviceDescriptionListDataType {
    const NetworkManagementDeviceDescriptionDataType* const*
        network_management_device_description_data; /**< "networkManagementDeviceDescriptionData" */
    size_t network_management_device_description_data_size;
};

typedef struct NetworkManagementDeviceDescriptionListDataSelectorsType
    NetworkManagementDeviceDescriptionListDataSelectorsType;

struct NetworkManagementDeviceDescriptionListDataSelectorsType {
    const DeviceAddressType* device_address; /**< "deviceAddress" */
    const DeviceTypeType* device_type;       /**< "deviceType" */
};

typedef struct NetworkManagementEntityDescriptionDataType NetworkManagementEntityDescriptionDataType;

struct NetworkManagementEntityDescriptionDataType {
    const EntityAddressType* entity_address;                    /**< "entityAddress" */
    const EntityTypeType* entity_type;                          /**< "entityType" */
    const NetworkManagementStateChangeType* last_state_change;  /**< "lastStateChange" */
    NetworkManagementMinimumTrustLevelType minimum_trust_level; /**< "minimumTrustLevel" */
    LabelType label;                                            /**< "label" */
    DescriptionType description;                                /**< "description" */
};

typedef struct NetworkManagementEntityDescriptionDataElementsType NetworkManagementEntityDescriptionDataElementsType;

struct NetworkManagementEntityDescriptionDataElementsType {
    ElementTagType entity_address;      /**< "entityAddress" */
    ElementTagType entity_type;         /**< "entityType" */
    ElementTagType last_state_change;   /**< "lastStateChange" */
    ElementTagType minimum_trust_level; /**< "minimumTrustLevel" */
    ElementTagType label;               /**< "label" */
    ElementTagType description;         /**< "description" */
};

typedef struct NetworkManagementEntityDescriptionListDataType NetworkManagementEntityDescriptionListDataType;

struct NetworkManagementEntityDescriptionListDataType {
    const NetworkManagementEntityDescriptionDataType* const*
        network_management_entity_description_data; /**< "networkManagementEntityDescriptionData" */
    size_t network_management_entity_description_data_size;
};

typedef struct NetworkManagementEntityDescriptionListDataSelectorsType
    NetworkManagementEntityDescriptionListDataSelectorsType;

struct NetworkManagementEntityDescriptionListDataSelectorsType {
    const EntityAddressType* entity_address; /**< "entityAddress" */
    const EntityTypeType* entity_type;       /**< "entityType" */
};

typedef struct NetworkManagementFeatureDescriptionDataType NetworkManagementFeatureDescriptionDataType;

struct NetworkManagementFeatureDescriptionDataType {
    const FeatureAddressType* feature_address;             /**< "featureAddress" */
    const FeatureTypeType* feature_type;                   /**< "featureType" */
    const FeatureSpecificUsageType* const* specific_usage; /**< "specificUsage" */
    size_t specific_usage_size;
    FeatureGroupType feature_group;                        /**< "featureGroup" */
    const RoleType* role;                                  /**< "role" */
    const FunctionPropertyType* const* supported_function; /**< "supportedFunction" */
    size_t supported_function_size;
    const NetworkManagementStateChangeType* last_state_change;  /**< "lastStateChange" */
    NetworkManagementMinimumTrustLevelType minimum_trust_level; /**< "minimumTrustLevel" */
    LabelType label;                                            /**< "label" */
    DescriptionType description;                                /**< "description" */
    const MaxResponseDelayType* max_response_delay;             /**< "maxResponseDelay" */
};

typedef struct NetworkManagementFeatureDescriptionDataElementsType NetworkManagementFeatureDescriptionDataElementsType;

struct NetworkManagementFeatureDescriptionDataElementsType {
    const FeatureAddressElementsType* feature_address;      /**< "featureAddress" */
    ElementTagType feature_type;                            /**< "featureType" */
    ElementTagType specific_usage;                          /**< "specificUsage" */
    ElementTagType feature_group;                           /**< "featureGroup" */
    ElementTagType role;                                    /**< "role" */
    const FunctionPropertyElementsType* supported_function; /**< "supportedFunction" */
    ElementTagType last_state_change;                       /**< "lastStateChange" */
    ElementTagType minimum_trust_level;                     /**< "minimumTrustLevel" */
    ElementTagType label;                                   /**< "label" */
    ElementTagType description;                             /**< "description" */
    ElementTagType max_response_delay;                      /**< "maxResponseDelay" */
};

typedef struct NetworkManagementFeatureDescriptionListDataType NetworkManagementFeatureDescriptionListDataType;

struct NetworkManagementFeatureDescriptionListDataType {
    const NetworkManagementFeatureDescriptionDataType* const*
        network_management_feature_description_data; /**< "networkManagementFeatureDescriptionData" */
    size_t network_management_feature_description_data_size;
};

typedef struct NetworkManagementFeatureDescriptionListDataSelectorsType
    NetworkManagementFeatureDescriptionListDataSelectorsType;

struct NetworkManagementFeatureDescriptionListDataSelectorsType {
    const FeatureAddressType* feature_address; /**< "featureAddress" */
    const FeatureTypeType* feature_type;       /**< "featureType" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_NETWORK_MANAGEMENT_TYPES_H_
