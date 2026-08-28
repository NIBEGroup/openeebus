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
 * @brief SPINE Datagram Bill data types declarations
 */

#ifndef SRC_SPINE_MODEL_BILL_TYPES_H_
#define SRC_SPINE_MODEL_BILL_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t BillIdType;

typedef int32_t BillTypeType;

enum {
    kBillTypeTypeChargingSummary, /**< "chargingSummary" */
};

typedef uint32_t BillPositionIdType;

typedef BillPositionIdType BillPositionCountType;

typedef int32_t BillPositionTypeType;

enum {
    kBillPositionTypeTypeGridElectricEnergy,         /**< "gridElectricEnergy" */
    kBillPositionTypeTypeSelfProducedElectricEnergy, /**< "selfProducedElectricEnergy" */
};

typedef uint32_t BillValueIdType;

typedef uint32_t BillCostIdType;

typedef int32_t BillCostTypeType;

enum {
    kBillCostTypeTypeAbsolutePrice,    /**< "absolutePrice" */
    kBillCostTypeTypeRelativePrice,    /**< "relativePrice" */
    kBillCostTypeTypeCo2Emission,      /**< "co2Emission" */
    kBillCostTypeTypeRenewableEnergy,  /**< "renewableEnergy" */
    kBillCostTypeTypeRadioactiveWaste, /**< "radioactiveWaste" */
};

typedef struct BillValueType BillValueType;

struct BillValueType {
    const BillValueIdType* value_id;          /**< "valueId" */
    const UnitOfMeasurementType* unit;        /**< "unit" */
    const ScaledNumberType* value;            /**< "value" */
    const ScaledNumberType* value_percentage; /**< "valuePercentage" */
};

typedef struct BillValueElementsType BillValueElementsType;

struct BillValueElementsType {
    ElementTagType value_id;         /**< "valueId" */
    ElementTagType unit;             /**< "unit" */
    ElementTagType value;            /**< "value" */
    ElementTagType value_percentage; /**< "valuePercentage" */
};

typedef struct BillCostType BillCostType;

struct BillCostType {
    const BillCostIdType* cost_id;           /**< "costId" */
    const BillCostTypeType* cost_type;       /**< "costType" */
    const BillValueIdType* value_id;         /**< "valueId" */
    const UnitOfMeasurementType* unit;       /**< "unit" */
    const CurrencyType* currency;            /**< "currency" */
    const ScaledNumberType* cost;            /**< "cost" */
    const ScaledNumberType* cost_percentage; /**< "costPercentage" */
};

typedef struct BillCostElementsType BillCostElementsType;

struct BillCostElementsType {
    ElementTagType cost_id;                          /**< "costId" */
    ElementTagType cost_type;                        /**< "costType" */
    ElementTagType value_id;                         /**< "valueId" */
    ElementTagType unit;                             /**< "unit" */
    ElementTagType currency;                         /**< "currency" */
    const ScaledNumberElementsType* cost;            /**< "cost" */
    const ScaledNumberElementsType* cost_percentage; /**< "costPercentage" */
};

typedef struct BillPositionType BillPositionType;

struct BillPositionType {
    const BillPositionIdType* position_id;     /**< "positionId" */
    const BillPositionTypeType* position_type; /**< "positionType" */
    const TimePeriodType* time_period;         /**< "timePeriod" */
    const BillValueType* value;                /**< "value" */
    const BillCostType* cost;                  /**< "cost" */
    LabelType label;                           /**< "label" */
    DescriptionType description;               /**< "description" */
};

typedef struct BillPositionElementsType BillPositionElementsType;

struct BillPositionElementsType {
    ElementTagType position_id;                /**< "positionId" */
    ElementTagType position_type;              /**< "positionType" */
    const TimePeriodElementsType* time_period; /**< "timePeriod" */
    const BillValueElementsType* value;        /**< "value" */
    const BillCostElementsType* cost;          /**< "cost" */
    ElementTagType label;                      /**< "label" */
    ElementTagType description;                /**< "description" */
};

typedef struct BillDataType BillDataType;

struct BillDataType {
    const BillIdType* bill_id;               /**< "billId" */
    const BillTypeType* bill_type;           /**< "billType" */
    const ScopeTypeType* scope_type;         /**< "scopeType" */
    const BillPositionType* total;           /**< "total" */
    const BillPositionType* const* position; /**< "position" */
    size_t position_size;
};

typedef struct BillDataElementsType BillDataElementsType;

struct BillDataElementsType {
    ElementTagType bill_id;                   /**< "billId" */
    ElementTagType bill_type;                 /**< "billType" */
    ElementTagType scope_type;                /**< "scopeType" */
    const BillPositionElementsType* total;    /**< "total" */
    const BillPositionElementsType* position; /**< "position" */
};

typedef struct BillListDataType BillListDataType;

struct BillListDataType {
    const BillDataType* const* bill_data; /**< "billData" */
    size_t bill_data_size;
};

typedef struct BillListDataSelectorsType BillListDataSelectorsType;

struct BillListDataSelectorsType {
    const BillIdType* bill_id;       /**< "billId" */
    const ScopeTypeType* scope_type; /**< "scopeType" */
};

typedef struct BillConstraintsDataType BillConstraintsDataType;

struct BillConstraintsDataType {
    const BillIdType* bill_id;                       /**< "billId" */
    const BillPositionCountType* position_count_min; /**< "positionCountMin" */
    const BillPositionCountType* position_count_max; /**< "positionCountMax" */
};

typedef struct BillConstraintsDataElementsType BillConstraintsDataElementsType;

struct BillConstraintsDataElementsType {
    ElementTagType bill_id;            /**< "billId" */
    ElementTagType position_count_min; /**< "positionCountMin" */
    ElementTagType position_count_max; /**< "positionCountMax" */
};

typedef struct BillConstraintsListDataType BillConstraintsListDataType;

struct BillConstraintsListDataType {
    const BillConstraintsDataType* const* bill_constraints_data; /**< "billConstraintsData" */
    size_t bill_constraints_data_size;
};

typedef struct BillConstraintsListDataSelectorsType BillConstraintsListDataSelectorsType;

struct BillConstraintsListDataSelectorsType {
    const BillIdType* bill_id; /**< "billId" */
};

typedef struct BillDescriptionDataType BillDescriptionDataType;

struct BillDescriptionDataType {
    const BillIdType* bill_id;                      /**< "billId" */
    const bool* bill_writeable;                     /**< "billWriteable" */
    const bool* update_required;                    /**< "updateRequired" */
    const BillTypeType* const* supported_bill_type; /**< "supportedBillType" */
    size_t supported_bill_type_size;
    const SessionIdType* session_id; /**< "sessionId" */
};

typedef struct BillDescriptionDataElementsType BillDescriptionDataElementsType;

struct BillDescriptionDataElementsType {
    ElementTagType bill_id;             /**< "billId" */
    ElementTagType bill_writeable;      /**< "billWriteable" */
    ElementTagType update_required;     /**< "updateRequired" */
    ElementTagType supported_bill_type; /**< "supportedBillType" */
    ElementTagType session_id;          /**< "sessionId" */
};

typedef struct BillDescriptionListDataType BillDescriptionListDataType;

struct BillDescriptionListDataType {
    const BillDescriptionDataType* const* bill_description_data; /**< "billDescriptionData" */
    size_t bill_description_data_size;
};

typedef struct BillDescriptionListDataSelectorsType BillDescriptionListDataSelectorsType;

struct BillDescriptionListDataSelectorsType {
    const BillIdType* bill_id; /**< "billId" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_BILL_TYPES_H_
