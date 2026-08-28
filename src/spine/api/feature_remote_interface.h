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
 * @brief Feature Remote interface declarations
 */

#ifndef SRC_SPINE_API_FEATURE_REMOTE_INTERFACE_H_
#define SRC_SPINE_API_FEATURE_REMOTE_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/spine/api/operations_interface.h"
#include "src/spine/feature/feature.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct DeviceRemoteObject DeviceRemoteObject;
typedef struct EntityRemoteObject EntityRemoteObject;

/**
 * @brief Feature Remote Interface
 * (Feature Remote "virtual functions table" declaration)
 */
typedef struct FeatureRemoteInterface FeatureRemoteInterface;

/**
 * @brief Feature Remote Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct FeatureRemoteObject FeatureRemoteObject;

/**
 * @brief FeatureRemote Interface Structure
 */
struct FeatureRemoteInterface {
    /** Extends FeatureInterface */
    FeatureInterface feature_interface;

    DeviceRemoteObject* (*get_device)(const FeatureRemoteObject* self);
    EntityRemoteObject* (*get_entity)(const FeatureRemoteObject* self);
    const void* (*get_data)(const FeatureRemoteObject* self, FunctionType function_type);
    void* (*data_copy)(const FeatureRemoteObject* self, FunctionType fcn_type);
    EebusError (*update_data)(
        FeatureRemoteObject* self,
        FunctionType function_type,
        const void* new_data,
        const FilterType* filter_partial,
        const FilterType* filter_delete,
        bool persist
    );
    void (*set_function_operations)(
        FeatureRemoteObject* self,
        const FunctionPropertyType* const* supported_functions,
        size_t supported_functions_size
    );
    void (*set_max_response_delay)(FeatureRemoteObject* self, uint32_t max_delay);
    uint32_t (*get_max_response_delay)(const FeatureRemoteObject* self);
};

/**
 * @brief Feature Remote Object Structure
 */
struct FeatureRemoteObject {
    const FeatureRemoteInterface* interface_;
};

/**
 * @brief Feature Remote pointer typecast
 */
#define FEATURE_REMOTE_OBJECT(obj) ((FeatureRemoteObject*)(obj))

/**
 * @brief Feature Remote Interface class pointer typecast
 */
#define FEATURE_REMOTE_INTERFACE(obj) (FEATURE_REMOTE_OBJECT(obj)->interface_)

/**
 * @brief Feature Remote Get Device caller definition
 */
#define FEATURE_REMOTE_GET_DEVICE(obj) (FEATURE_REMOTE_INTERFACE(obj)->get_device(obj))

/**
 * @brief Feature Remote Get Entity caller definition
 */
#define FEATURE_REMOTE_GET_ENTITY(obj) (FEATURE_REMOTE_INTERFACE(obj)->get_entity(obj))

/**
 * @brief Feature Remote Get Data caller definition
 */
#define FEATURE_REMOTE_GET_DATA(obj, function_type) (FEATURE_REMOTE_INTERFACE(obj)->get_data(obj, function_type))

/**
 * @brief Feature Remote Data Copy caller definition
 */
#define FEATURE_REMOTE_DATA_COPY(obj, fcn_type) (FEATURE_REMOTE_INTERFACE(obj)->data_copy(obj, fcn_type))

/**
 * @brief Feature Remote Update Data caller definition
 */
#define FEATURE_REMOTE_UPDATE_DATA(obj, function_type, new_data, filter_partial, filter_delete, persist) \
    (FEATURE_REMOTE_INTERFACE(obj)->update_data(obj, function_type, new_data, filter_partial, filter_delete, persist))

/**
 * @brief Feature Remote Set Function Operations caller definition
 */
#define FEATURE_REMOTE_SET_FUNCTION_OPERATIONS(obj, supported_functions, supported_functions_size) \
    (FEATURE_REMOTE_INTERFACE(obj)->set_function_operations(obj, supported_functions, supported_functions_size))

/**
 * @brief Feature Remote Set Max Response Delay caller definition
 */
#define FEATURE_REMOTE_SET_MAX_RESPONSE_DELAY(obj, max_delay) \
    (FEATURE_REMOTE_INTERFACE(obj)->set_max_response_delay(obj, max_delay))

/**
 * @brief Feature Remote Get Max Response Delay caller definition
 */
#define FEATURE_REMOTE_GET_MAX_RESPONSE_DELAY(obj) (FEATURE_REMOTE_INTERFACE(obj)->get_max_response_delay(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_FEATURE_REMOTE_INTERFACE_H_
