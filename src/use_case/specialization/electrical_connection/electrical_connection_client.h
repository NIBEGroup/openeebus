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
 * @brief Electrical Connection Server functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_CLIENT_H_
#define SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_CLIENT_H_

#include "src/spine/entity/entity_local.h"
#include "src/spine/model/electrical_connection_types.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_common.h"
#include "src/use_case/specialization/feature_info_client.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct ElectricalConnectionClient ElectricalConnectionClient;

struct ElectricalConnectionClient {
    FeatureInfoClient feature_info_client;
    ElectricalConnectionCommon el_connection_common;
};

/**
 * @brief Constructs an ElectricalConnectionClient instance.
 * @param self A pointer to the ElectricalConnectionServer instance to be constructed.
 * @param local_entity A pointer to the local entity object to associate with the server.
 * @param remote_entity A pointer to the remote entity object to associate with the server.
 * @return An EebusError indicating the success or failure of the construction.
 */
EebusError ElectricalConnectionClientConstruct(
    ElectricalConnectionClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
);

/**
 * @brief Requests electrical connection descriptions from a remote entity
 *
 * This function sends a request to get electrical connection descriptions from a remote
 * EEBUS entity. The descriptions can be filtered using selectors and the response
 * can be customized to include specific data elements.
 *
 * @param self ElectricalConnectionClient instance to send the request with
 * @param selectors List element selectors for electrical connection descriptions
 * @param elements Data elements to be included in the response
 * @return EebusError Returns error code indicating success or failure of the request
 */
EebusError ElectricalConnectionClientRequestDescriptions(
    ElectricalConnectionClient* self,
    const ElectricalConnectionDescriptionListDataSelectorsType* selectors,
    const ElectricalConnectionDescriptionDataElementsType* elements
);

/**
 * @brief Requests FunctionTypeElectricalConnectionParameterDescriptionListData from a remote entity
 *
 * This function sends a request to retrieve parameter descriptions for electrical connections
 * from a remote EEBUS entity using the specified selectors and elements.
 *
 * @param self ElectricalConnectionClient instance to send the request with
 * @param selectors List element selectors for electrical connection parameter descriptions
 * @param elements Data elements to be included in the response
 * @return EebusError Error code indicating success or failure of the request
 */
EebusError ElectricalConnectionClientRequestParameterDescriptions(
    ElectricalConnectionClient* self,
    const ElectricalConnectionDescriptionListDataSelectorsType* selectors,
    const ElectricalConnectionDescriptionDataElementsType* elements
);

/**
 * @brief Requests function type electrical connection permitted value set list data from a remote entity
 *
 * This function sends a request to get the permitted value sets for electrical connection
 * from a remote EEBUS entity.
 *
 * @param self ElectricalConnectionClient instance to send the request with
 * @param selectors List element selectors for permitted value set list data
 * @param elements Data elements to be included in the response
 * @return EebusError Returns error code indicating success or failure
 */
EebusError ElectricalConnectionClientRequestPermittedValueSets(
    ElectricalConnectionClient* self,
    const ElectricalConnectionPermittedValueSetListDataSelectorsType* selectors,
    const ElectricalConnectionPermittedValueSetDataElementsType* elements
);

/**
 * @brief Requests the electrical connection characteristic list data from a remote entity
 *
 * This function sends a request to retrieve electrical connection characteristics
 * from a remote EEBUS entity using the specified selectors and elements.
 *
 * @param self ElectricalConnectionClient instance to send the request with
 * @param selectors List element selectors for characteristic list data
 * @param elements Data elements to be included in the response
 * @return EebusError Returns error code indicating success or failure
 */
EebusError ElectricalConnectionClientRequestCharacteristics(
    ElectricalConnectionClient* self,
    const ElectricalConnectionCharacteristicListDataSelectorsType* selectors,
    const ElectricalConnectionCharacteristicDataElementsType* elements,
    ReplyMessageCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_CLIENT_H_
