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
 * @brief Feature implementation
 */

#include "src/spine/feature/feature.h"

#include <string.h>

#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/num_ptr.h"
#include "src/common/string_util.h"
#include "src/spine/api/feature_interface.h"
#include "src/spine/feature/feature_functions.h"
#include "src/spine/function/function.h"
#include "src/spine/model/entity_types.h"
#include "src/spine/model/model.h"

void FeatureConstruct(
    Feature* self,
    FeatureTypeType type,
    const EntityAddressType* entity_addr,
    uint32_t feature_id,
    RoleType role
) {
    self->type        = type;
    self->address     = FeatureAddressCreate(entity_addr, feature_id);
    self->description = NULL;
    self->role        = role;

    VectorConstruct(&self->functions);
    const FeatureFunctions* feature_functions = GetFeatureFunctions(type);
    if (feature_functions != NULL) {
        for (size_t i = 0; i < feature_functions->functions_list_size; ++i) {
            VectorPushBack(&self->functions, FunctionCreate(feature_functions->functions_list[i]));
        }
    }
}

void FeatureDestruct(FeatureObject* self) {
    Feature* const feature = FEATURE(self);

    FeatureAddressDelete(feature->address);
    feature->address = NULL;

    StringDelete(feature->description);
    feature->description = NULL;

    for (size_t i = 0; i < VectorGetSize(&feature->functions); ++i) {
        FunctionDelete(VectorGetElement(&feature->functions, i));
    }

    VectorDestruct(&feature->functions);
}

const FeatureAddressType* FeatureGetAddress(const FeatureObject* self) {
    return FEATURE(self)->address;
}

FeatureTypeType FeatureGetType(const FeatureObject* self) {
    return FEATURE(self)->type;
}

RoleType FeatureGetRole(const FeatureObject* self) {
    return FEATURE(self)->role;
}

const OperationsObject* FeatureGetFunctionOperations(const FeatureObject* self, FunctionType fcn_type) {
    Feature* const feature = FEATURE(self);

    const FunctionObject* const function = FeatureGetFunction(feature, fcn_type);
    if (function == NULL) {
        return NULL;
    }

    return FUNCTION_GET_OPERATIONS(function);
}

const char* FeatureGetDescription(const FeatureObject* self) {
    return FEATURE(self)->description;
}

void FeatureSetDescription(FeatureObject* self, const char* description) {
    Feature* const feature = FEATURE(self);

    StringDelete(feature->description);
    feature->description = StringCopy(description);
}

const char* FeatureToString(const FeatureObject* self) {
    const Feature* const feature = FEATURE(self);

    return StringFmtSprintf("Id: %d (%s)", *feature->address->feature, ModelFeatureTypeToString(feature->type));
}

FunctionObject* FeatureGetFunction(const Feature* self, FeatureTypeType type) {
    for (size_t i = 0; i < VectorGetSize(&self->functions); ++i) {
        FunctionObject* const function = (FunctionObject*)VectorGetElement(&self->functions, i);
        if (FUNCTION_GET_FUNCTION_TYPE(function) == type) {
            return function;
        }
    }

    return NULL;
}

bool FeatureParametersMatch(const Feature* feature, RoleType role, FeatureTypeType type) {
    if (feature == NULL) {
        return false;
    }

    const RoleType feature_role = FEATURE_GET_ROLE(FEATURE_OBJECT(feature));
    if ((feature_role != kRoleTypeSpecial) && (feature_role != role)) {
        return false;
    }

    const FeatureTypeType feature_type = FEATURE_GET_TYPE(FEATURE_OBJECT(feature));
    if ((feature_type != kFeatureTypeTypeGeneric) && (feature_type != type)) {
        return false;
    }

    return true;
}
