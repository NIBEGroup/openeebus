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
 * @brief SPINE Datagram Threshold data types declarations
 */

#ifndef SRC_SPINE_MODEL_THRESHOLD_TYPES_H_
#define SRC_SPINE_MODEL_THRESHOLD_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t ThresholdIdType;

typedef int32_t ThresholdTypeType;

enum {
    kThresholdTypeTypeGoodAbove,                /**< "goodAbove" */
    kThresholdTypeTypeBadAbove,                 /**< "badAbove" */
    kThresholdTypeTypeGoodBelow,                /**< "goodBelow" */
    kThresholdTypeTypeBadBelow,                 /**< "badBelow" */
    kThresholdTypeTypeMinValueThreshold,        /**< "minValueThreshold" */
    kThresholdTypeTypeMaxValueThreshold,        /**< "maxValueThreshold" */
    kThresholdTypeTypeMinValueThresholdExtreme, /**< "minValueThresholdExtreme" */
    kThresholdTypeTypeMaxValueThresholdExtreme, /**< "maxValueThresholdExtreme" */
    kThresholdTypeTypeSagThreshold,             /**< "sagThreshold" */
    kThresholdTypeTypeSwellThreshold,           /**< "swellThreshold" */
};

typedef struct ThresholdDataType ThresholdDataType;

struct ThresholdDataType {
    const ThresholdIdType* threshold_id;     /**< "thresholdId" */
    const ScaledNumberType* threshold_value; /**< "thresholdValue" */
};

typedef struct ThresholdDataElementsType ThresholdDataElementsType;

struct ThresholdDataElementsType {
    ElementTagType threshold_id;                     /**< "thresholdId" */
    const ScaledNumberElementsType* threshold_value; /**< "thresholdValue" */
};

typedef struct ThresholdListDataType ThresholdListDataType;

struct ThresholdListDataType {
    const ThresholdDataType* const* threshold_data; /**< "thresholdData" */
    size_t threshold_data_size;
};

typedef struct ThresholdListDataSelectorsType ThresholdListDataSelectorsType;

struct ThresholdListDataSelectorsType {
    const ThresholdIdType* threshold_id; /**< "thresholdId" */
};

typedef struct ThresholdConstraintsDataType ThresholdConstraintsDataType;

struct ThresholdConstraintsDataType {
    const ThresholdIdType* threshold_id;         /**< "thresholdId" */
    const ScaledNumberType* threshold_range_min; /**< "thresholdRangeMin" */
    const ScaledNumberType* threshold_range_max; /**< "thresholdRangeMax" */
    const ScaledNumberType* threshold_step_size; /**< "thresholdStepSize" */
};

typedef struct ThresholdConstraintsDataElementsType ThresholdConstraintsDataElementsType;

struct ThresholdConstraintsDataElementsType {
    ElementTagType threshold_id;                         /**< "thresholdId" */
    const ScaledNumberElementsType* threshold_range_min; /**< "thresholdRangeMin" */
    const ScaledNumberElementsType* threshold_range_max; /**< "thresholdRangeMax" */
    const ScaledNumberElementsType* threshold_step_size; /**< "thresholdStepSize" */
};

typedef struct ThresholdConstraintsListDataType ThresholdConstraintsListDataType;

struct ThresholdConstraintsListDataType {
    const ThresholdConstraintsDataType* const* threshold_constraints_data; /**< "thresholdConstraintsData" */
    size_t threshold_constraints_data_size;
};

typedef struct ThresholdConstraintsListDataSelectorsType ThresholdConstraintsListDataSelectorsType;

struct ThresholdConstraintsListDataSelectorsType {
    const ThresholdIdType* threshold_id; /**< "thresholdId" */
};

typedef struct ThresholdDescriptionDataType ThresholdDescriptionDataType;

struct ThresholdDescriptionDataType {
    const ThresholdIdType* threshold_id;     /**< "thresholdId" */
    const ThresholdTypeType* threshold_type; /**< "thresholdType" */
    const UnitOfMeasurementType* unit;       /**< "unit" */
    const ScopeTypeType* scope_type;         /**< "scopeType" */
    LabelType label;                         /**< "label" */
    DescriptionType description;             /**< "description" */
};

typedef struct ThresholdDescriptionDataElementsType ThresholdDescriptionDataElementsType;

struct ThresholdDescriptionDataElementsType {
    ElementTagType threshold_id;   /**< "thresholdId" */
    ElementTagType threshold_type; /**< "thresholdType" */
    ElementTagType unit;           /**< "unit" */
    ElementTagType scope_type;     /**< "scopeType" */
    ElementTagType label;          /**< "label" */
    ElementTagType description;    /**< "description" */
};

typedef struct ThresholdDescriptionListDataType ThresholdDescriptionListDataType;

struct ThresholdDescriptionListDataType {
    const ThresholdDescriptionDataType* const* threshold_description_data; /**< "thresholdDescriptionData" */
    size_t threshold_description_data_size;
};

typedef struct ThresholdDescriptionListDataSelectorsType ThresholdDescriptionListDataSelectorsType;

struct ThresholdDescriptionListDataSelectorsType {
    const ThresholdIdType* threshold_id; /**< "thresholdId" */
    const ScopeTypeType* scope_type;     /**< "scopeType" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_THRESHOLD_TYPES_H_
