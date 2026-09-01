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
 * @brief Feature interface declarations
 */

#ifndef SRC_SPINE_API_FEATURE_INTERFACE_H_
#define SRC_SPINE_API_FEATURE_INTERFACE_H_

#include "src/spine/api/operations_interface.h"
#include "src/spine/model/feature_types.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Feature Interface
 * (Feature "virtual functions table" declaration)
 */
typedef struct FeatureInterface FeatureInterface;

/**
 * @brief Feature Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct FeatureObject FeatureObject;

/**
 * @brief Feature Interface Structure
 */
struct FeatureInterface {
    void (*destruct)(FeatureObject* self);
    const FeatureAddressType* (*get_address)(const FeatureObject* self);
    FeatureTypeType (*get_type)(const FeatureObject* self);
    RoleType (*get_role)(const FeatureObject* self);
    const OperationsObject* (*get_function_operations)(const FeatureObject* self, FunctionType fcn_type);
    const char* (*get_description)(const FeatureObject* self);
    void (*set_description)(FeatureObject* self, const char* description);
    const char* (*to_string)(const FeatureObject* self);
};

/**
 * @brief Feature Object Structure
 */
struct FeatureObject {
    const FeatureInterface* interface_;
};

/**
 * @brief Feature pointer typecast
 */
#define FEATURE_OBJECT(obj) ((FeatureObject*)(obj))

/**
 * @brief Feature Interface class pointer typecast
 */
#define FEATURE_INTERFACE(obj) (FEATURE_OBJECT(obj)->interface_)

/**
 * @brief Feature Destruct caller definition
 */
#define FEATURE_DESTRUCT(obj) (FEATURE_INTERFACE(obj)->destruct(obj))

/**
 * @brief Feature Get Address caller definition
 */
#define FEATURE_GET_ADDRESS(obj) (FEATURE_INTERFACE(obj)->get_address(obj))

/**
 * @brief Feature Get Type caller definition
 */
#define FEATURE_GET_TYPE(obj) (FEATURE_INTERFACE(obj)->get_type(obj))

/**
 * @brief Feature Get Role caller definition
 */
#define FEATURE_GET_ROLE(obj) (FEATURE_INTERFACE(obj)->get_role(obj))

/**
 * @brief Feature Get Function Operations caller definition
 */
#define FEATURE_GET_FUNCTION_OPERATIONS(obj, fcn_type) (FEATURE_INTERFACE(obj)->get_function_operations(obj, fcn_type))

/**
 * @brief Feature Get Description caller definition
 */
#define FEATURE_GET_DESCRIPTION(obj) (FEATURE_INTERFACE(obj)->get_description(obj))

/**
 * @brief Feature Set Description caller definition
 */
#define FEATURE_SET_DESCRIPTION(obj, description) (FEATURE_INTERFACE(obj)->set_description(obj, description))

/**
 * @brief Feature To String caller definition
 */
#define FEATURE_TO_STRING(obj) (FEATURE_INTERFACE(obj)->to_string(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_FEATURE_INTERFACE_H_
