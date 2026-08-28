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
 * @brief SPINE Datagram Incentive Table data types declarations
 */

#ifndef SRC_SPINE_MODEL_INCENTIVE_TABLE_TYPES_H_
#define SRC_SPINE_MODEL_INCENTIVE_TABLE_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/tariff_information_types.h"
#include "src/spine/model/timetable_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct IncentiveTableTierType IncentiveTableTierType;

struct IncentiveTableTierType {
    const TierDataType* tier;                    /**< "tier" */
    const TierBoundaryDataType* const* boundary; /**< "boundary" */
    size_t boundary_size;
    const IncentiveDataType* const* incentive; /**< "incentive" */
    size_t incentive_size;
};

typedef struct IncentiveTableIncentiveSlotType IncentiveTableIncentiveSlotType;

struct IncentiveTableIncentiveSlotType {
    const TimeTableDataType* time_interval;    /**< "timeInterval" */
    const IncentiveTableTierType* const* tier; /**< "tier" */
    size_t tier_size;
};

typedef struct IncentiveTableType IncentiveTableType;

struct IncentiveTableType {
    const TariffDataType* tariff;                                 /**< "tariff" */
    const IncentiveTableIncentiveSlotType* const* incentive_slot; /**< "incentiveSlot" */
    size_t incentive_slot_size;
};

typedef struct IncentiveTableTierElementsType IncentiveTableTierElementsType;

struct IncentiveTableTierElementsType {
    const TierDataElementsType* tier;             /**< "tier" */
    const TierBoundaryDataElementsType* boundary; /**< "boundary" */
    const IncentiveDataElementsType* incentive;   /**< "incentive" */
};

typedef struct IncentiveTableIncentiveSlotElementsType IncentiveTableIncentiveSlotElementsType;

struct IncentiveTableIncentiveSlotElementsType {
    const TimeTableDataElementsType* time_interval; /**< "timeInterval" */
    const IncentiveTableTierElementsType* tier;     /**< "tier" */
};

typedef struct IncentiveTableElementsType IncentiveTableElementsType;

struct IncentiveTableElementsType {
    const TariffDataElementsType* tariff;                          /**< "tariff" */
    const IncentiveTableIncentiveSlotElementsType* incentive_slot; /**< "incentiveSlot" */
};

typedef struct IncentiveTableDataType IncentiveTableDataType;

struct IncentiveTableDataType {
    const IncentiveTableType* const* incentive_table; /**< "incentiveTable" */
    size_t incentive_table_size;
};

typedef struct IncentiveTableDataElementsType IncentiveTableDataElementsType;

struct IncentiveTableDataElementsType {
    const IncentiveTableElementsType* incentive_table; /**< "incentiveTable" */
};

typedef struct IncentiveTableDataSelectorsType IncentiveTableDataSelectorsType;

struct IncentiveTableDataSelectorsType {
    const TariffListDataSelectorsType* tariff; /**< "tariff" */
};

typedef struct IncentiveTableDescriptionTierType IncentiveTableDescriptionTierType;

struct IncentiveTableDescriptionTierType {
    const TierDescriptionDataType* tier_description;              /**< "tierDescription" */
    TierBoundaryDescriptionDataType* const* boundary_description; /**< "boundaryDescription" */
    size_t boundary_description_size;
    const IncentiveDescriptionDataType* const* incentive_description; /**< "incentiveDescription" */
    size_t incentive_description_size;
};

typedef struct IncentiveTableDescriptionType IncentiveTableDescriptionType;

struct IncentiveTableDescriptionType {
    const TariffDescriptionDataType* tariff_description;  /**< "tariffDescription" */
    const IncentiveTableDescriptionTierType* const* tier; /**< "tier" */
    size_t tier_size;
};

typedef struct IncentiveTableDescriptionElementsType IncentiveTableDescriptionElementsType;

struct IncentiveTableDescriptionElementsType {
    const TariffDescriptionDataElementsType* tariff_description; /**< "tariffDescription" */
    const IncentiveTableDescriptionTierType* tier;               /**< "tier" */
};

typedef struct IncentiveTableDescriptionTierElementsType IncentiveTableDescriptionTierElementsType;

struct IncentiveTableDescriptionTierElementsType {
    const TierDescriptionDataElementsType* tier_description;             /**< "tierDescription" */
    const TierBoundaryDescriptionDataElementsType* boundary_description; /**< "boundaryDescription" */
    const IncentiveDescriptionDataElementsType* incentive_description;   /**< "incentiveDescription" */
};

typedef struct IncentiveTableDescriptionDataType IncentiveTableDescriptionDataType;

struct IncentiveTableDescriptionDataType {
    const IncentiveTableDescriptionType* const* incentive_table_description; /**< "incentiveTableDescription" */
    size_t incentive_table_description_size;
};

typedef struct IncentiveTableDescriptionDataElementsType IncentiveTableDescriptionDataElementsType;

struct IncentiveTableDescriptionDataElementsType {
    const IncentiveTableDescriptionElementsType* incentive_table_description; /**< "incentiveTableDescription" */
};

typedef struct IncentiveTableDescriptionDataSelectorsType IncentiveTableDescriptionDataSelectorsType;

struct IncentiveTableDescriptionDataSelectorsType {
    const TariffDescriptionListDataSelectorsType* tariff_description; /**< "tariffDescription" */
};

typedef struct IncentiveTableConstraintsType IncentiveTableConstraintsType;

struct IncentiveTableConstraintsType {
    const TariffDataType* tariff;                                   /**< "tariff" */
    const TariffOverallConstraintsDataType* tariff_constraints;     /**< "tariffConstraints" */
    const TimeTableConstraintsDataType* incentive_slot_constraints; /**< "incentiveSlotConstraints" */
};

typedef struct IncentiveTableConstraintsElementsType IncentiveTableConstraintsElementsType;

struct IncentiveTableConstraintsElementsType {
    const TariffDataElementsType* tariff;                                   /**< "tariff" */
    const TariffOverallConstraintsDataElementsType* tariff_constraints;     /**< "tariffConstraints" */
    const TimeTableConstraintsDataElementsType* incentive_slot_constraints; /**< "incentiveSlotConstraints" */
};

typedef struct IncentiveTableConstraintsDataType IncentiveTableConstraintsDataType;

struct IncentiveTableConstraintsDataType {
    const IncentiveTableConstraintsType* const* incentive_table_constraints; /**< "incentiveTableConstraints" */
    size_t incentive_table_constraints_size;
};

typedef struct IncentiveTableConstraintsDataElementsType IncentiveTableConstraintsDataElementsType;

struct IncentiveTableConstraintsDataElementsType {
    const IncentiveTableConstraintsElementsType* incentive_table_constraints; /**< "incentiveTableConstraints" */
};

typedef struct IncentiveTableConstraintsDataSelectorsType IncentiveTableConstraintsDataSelectorsType;

struct IncentiveTableConstraintsDataSelectorsType {
    const TariffListDataSelectorsType* tariff; /**< "tariff" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_INCENTIVE_TABLE_TYPES_H_
