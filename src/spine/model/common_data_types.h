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
 * @brief Common data types declarations
 */

#ifndef SRC_SPINE_MODEL_COMMON_TYPES_H_
#define SRC_SPINE_MODEL_COMMON_TYPES_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/common/eebus_data/eebus_data_tag_types.h"
#include "src/common/eebus_date_time/eebus_date_time.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef TagType ElementTagType;

typedef char* LabelType;

typedef char* DescriptionType;

typedef const char* SpecificationVersionType;

typedef EebusTime TimeType;

typedef EebusDate DateType;

typedef EebusDateTime DateTimeType;

typedef EebusDuration DurationType;

typedef int32_t AbsoluteOrRelativeTimeTypeType;

enum {
    kAbsoluteOrRelativeTimeTypeDateTime,
    kAbsoluteOrRelativeTimeTypeDuration,
};

typedef struct AbsoluteOrRelativeTimeType AbsoluteOrRelativeTimeType;

struct AbsoluteOrRelativeTimeType {
    AbsoluteOrRelativeTimeTypeType type;

    union {
        EebusDateTime date_time;
        EebusDuration duration;
    };
};

typedef struct TimePeriodType TimePeriodType;

struct TimePeriodType {
    const AbsoluteOrRelativeTimeType* start_time; /**< "startTime"*/
    const AbsoluteOrRelativeTimeType* end_time;   /**< "endTime" */
};

typedef struct TimePeriodElementsType TimePeriodElementsType;

struct TimePeriodElementsType {
    ElementTagType start_time; /**< "startTime" */
    ElementTagType end_time;   /**< "endTime" */
};

typedef struct TimestampIntervalType TimestampIntervalType;

struct TimestampIntervalType {
    const AbsoluteOrRelativeTimeType* start_time; /**< "startTime"*/
    const AbsoluteOrRelativeTimeType* end_time;   /**< "endTime" */
};

typedef int32_t RecurringIntervalType;

enum {
    kRecurringIntervalTypeYearly,      /**< "yearly" */
    kRecurringIntervalTypeMonthly,     /**< "monthly" */
    kRecurringIntervalTypeWeekly,      /**< "weekly" */
    kRecurringIntervalTypeDaily,       /**< "daily" */
    kRecurringIntervalTypeHourly,      /**< "hourly" */
    kRecurringIntervalTypeEveryminute, /**< "everyMinute" */
    kRecurringIntervalTypeEverysecond, /**< "everySecond" */
};

typedef int32_t MonthType;

enum {
    kMonthTypeJanuary,   /**< "january" */
    kMonthTypeFebruary,  /**< "february" */
    kMonthTypeMarch,     /**< "march" */
    kMonthTypeApril,     /**< "april" */
    kMonthTypeMay,       /**< "may" */
    kMonthTypeJune,      /**< "june" */
    kMonthTypeJuly,      /**< "july" */
    kMonthTypeAugust,    /**< "august" */
    kMonthTypeSeptember, /**< "september" */
    kMonthTypeOctober,   /**< "october" */
    kMonthTypeNovember,  /**< "november" */
    kMonthTypeDecember,  /**< "december" */
};

typedef uint8_t DayOfMonthType;

typedef uint8_t CalendarWeekType;

typedef int32_t DayOfWeekType;

enum {
    kDayOfWeekTypeMonday,    /**< "monday" */
    kDayOfWeekTypeTuesday,   /**< "tuesday" */
    kDayOfWeekTypeWednesday, /**< "wednesday" */
    kDayOfWeekTypeThursday,  /**< "thursday" */
    kDayOfWeekTypeFriday,    /**< "friday" */
    kDayOfWeekTypeSaturday,  /**< "saturday" */
    kDayOfWeekTypeSunday,    /**< "sunday" */
};

typedef struct DaysOfWeekType DaysOfWeekType;

struct DaysOfWeekType {
    ElementTagType monday;    /**< "monday" */
    ElementTagType tuesday;   /**< "tuesday" */
    ElementTagType wednesday; /**< "wednesday" */
    ElementTagType thursday;  /**< "thursday" */
    ElementTagType friday;    /**< "friday" */
    ElementTagType saturday;  /**< "saturday" */
    ElementTagType sunday;    /**< "sunday" */
};

typedef int32_t OccurrenceType;

enum {
    kOccurrenceTypeFirst,  /**< "first" */
    kOccurrenceTypeSecond, /**< "second" */
    kOccurrenceTypeThird,  /**< "third" */
    kOccurrenceTypeFourth, /**< "fourth" */
    kOccurrenceTypeLast,   /**< "last" */
};

typedef struct AbsoluteOrRecurringTimeType AbsoluteOrRecurringTimeType;

struct AbsoluteOrRecurringTimeType {
    const DateTimeType* date_time;                /**< "dateTime" */
    const MonthType* month;                       /**< "month" */
    const DayOfMonthType* day_of_month;           /**< "dayOfMonth" */
    const CalendarWeekType* calendar_week;        /**< "calendarWeek" */
    const OccurrenceType* day_of_week_occurrence; /**< "dayOfWeekOccurrence" */
    const DaysOfWeekType* days_of_week;           /**< "daysOfWeek" */
    const TimeType* time;                         /**< "time" */
    const DurationType* relative;                 /**< "relative" */
};

typedef struct AbsoluteOrRecurringTimeElementsType AbsoluteOrRecurringTimeElementsType;

struct AbsoluteOrRecurringTimeElementsType {
    ElementTagType date_time;              /**< "dateTime" */
    ElementTagType month;                  /**< "month" */
    ElementTagType day_of_month;           /**< "dayOfMonth" */
    ElementTagType calendar_week;          /**< "calendarWeek" */
    ElementTagType day_of_week_occurrence; /**< "dayOfWeekOccurrence" */
    ElementTagType days_of_week;           /**< "daysOfWeek" */
    ElementTagType time;                   /**< "time" */
    ElementTagType relative;               /**< "relative" */
};

typedef struct RecurrenceInformationType RecurrenceInformationType;

struct RecurrenceInformationType {
    const RecurringIntervalType* recurring_interval; /**< "recurringInterval" */
    const uint32_t* recurring_interval_step;         /**< "recurringIntervalStep" */
    const DateTimeType* first_execution;             /**< "firstExecution" */
    const uint32_t* execution_count;                 /**< "executionCount" */
    const DateTimeType* last_execution;              /**< "lastExecution" */
};

typedef struct RecurrenceInformationElementsType RecurrenceInformationElementsType;

struct RecurrenceInformationElementsType {
    ElementTagType recurring_interval;      /**< "recurringInterval" */
    ElementTagType recurring_interval_step; /**< "recurringIntervalStep" */
    ElementTagType first_execution;         /**< "firstExecution" */
    ElementTagType execution_count;         /**< "executionCount" */
    ElementTagType last_execution;          /**< "lastExecution" */
};

typedef int64_t NumberType;

typedef int8_t ScaleType;

typedef struct ScaledNumberType ScaledNumberType;

struct ScaledNumberType {
    const NumberType* number; /**< "number" */
    const ScaleType* scale;   /**< "scale" */
};

typedef struct ScaledNumberElementsType ScaledNumberElementsType;
struct ScaledNumberElementsType {
    ElementTagType number; /**< "number" */
    ElementTagType scale;  /**< "scale" */
};

typedef struct ScaledNumberRangeType ScaledNumberRangeType;

struct ScaledNumberRangeType {
    const ScaledNumberType* min; /**< "min" */
    const ScaledNumberType* max; /**< "max" */
};

typedef struct ScaledNumberRangeElementsType ScaledNumberRangeElementsType;

struct ScaledNumberRangeElementsType {
    ElementTagType min; /**< "min" */
    ElementTagType max; /**< "max" */
};

typedef struct ScaledNumberSetType ScaledNumberSetType;

struct ScaledNumberSetType {
    const ScaledNumberType* const* value; /**< "value" */
    size_t value_size;
    const ScaledNumberRangeType* const* range; /**< "range" */
    size_t range_size;
};

typedef struct ScaledNumberSetElementsType ScaledNumberSetElementsType;

struct ScaledNumberSetElementsType {
    ElementTagType value; /**< "value" */
    ElementTagType range; /**< "range" */
};

typedef DurationType MaxResponseDelayType;

typedef int32_t CommodityTypeType;

enum {
    kCommodityTypeTypeElectricity,      /**< "electricity" */
    kCommodityTypeTypeGas,              /**< "gas" */
    kCommodityTypeTypeOil,              /**< "oil" */
    kCommodityTypeTypeWater,            /**< "water" */
    kCommodityTypeTypeWasteWater,       /**< "wasteWater" */
    kCommodityTypeTypeDomesticHotWater, /**< "domesticHotWater" */
    kCommodityTypeTypeHeatingWater,     /**< "heatingWater" */
    kCommodityTypeTypeSteam,            /**< "steam" */
    kCommodityTypeTypeHeat,             /**< "heat" */
    kCommodityTypeTypeCoolingLoad,      /**< "coolingLoad" */
    kCommodityTypeTypeAir,              /**< "air" */
};

typedef int32_t EnergyDirectionType;

enum {
    kEnergyDirectionTypeConsume, /**< "consume" */
    kEnergyDirectionTypeProduce, /**< "produce" */
};

typedef int32_t EnergyModeType;

enum {
    kEnergyModeTypeConsume, /**< "consume" */
    kEnergyModeTypeProduce, /**< "produce" */
    kEnergyModeTypeIdle,    /**< "idle" */
    kEnergyModeTypeAuto,    /**< "auto" */
};

typedef int32_t UnitOfMeasurementType;

enum {
    kUnitOfMeasurementTypeUnknown,   /**< "unknown" */
    kUnitOfMeasurementType1,         /**< "1" */
    kUnitOfMeasurementTypem,         /**< "m" */
    kUnitOfMeasurementTypekg,        /**< "kg" */
    kUnitOfMeasurementTypes,         /**< "s" */
    kUnitOfMeasurementTypeA,         /**< "A" */
    kUnitOfMeasurementTypeK,         /**< "K" */
    kUnitOfMeasurementTypemol,       /**< "mol" */
    kUnitOfMeasurementTypecd,        /**< "cd" */
    kUnitOfMeasurementTypeV,         /**< "V" */
    kUnitOfMeasurementTypeW,         /**< "W" */
    kUnitOfMeasurementTypeWh,        /**< "Wh" */
    kUnitOfMeasurementTypeVA,        /**< "VA" */
    kUnitOfMeasurementTypeVAh,       /**< "VAh" */
    kUnitOfMeasurementTypevar,       /**< "var" */
    kUnitOfMeasurementTypevarh,      /**< "varh" */
    kUnitOfMeasurementTypedegC,      /**< "degC" */
    kUnitOfMeasurementTypedegF,      /**< "degF" */
    kUnitOfMeasurementTypeLm,        /**< "Lm" */
    kUnitOfMeasurementTypelx,        /**< "lx" */
    kUnitOfMeasurementTypeOhm,       /**< "Ohm" */
    kUnitOfMeasurementTypeHz,        /**< "Hz" */
    kUnitOfMeasurementTypedB,        /**< "dB" */
    kUnitOfMeasurementTypedBm,       /**< "dBm" */
    kUnitOfMeasurementTypepct,       /**< "pct" */
    kUnitOfMeasurementTypeppm,       /**< "ppm" */
    kUnitOfMeasurementTypel,         /**< "l" */
    kUnitOfMeasurementTypels,        /**< "l/s" */
    kUnitOfMeasurementTypelh,        /**< "l/h" */
    kUnitOfMeasurementTypedeg,       /**< "deg" */
    kUnitOfMeasurementTyperad,       /**< "rad" */
    kUnitOfMeasurementTyperads,      /**< "rad/s" */
    kUnitOfMeasurementTypesr,        /**< "sr" */
    kUnitOfMeasurementTypeGy,        /**< "Gy" */
    kUnitOfMeasurementTypeBq,        /**< "Bq" */
    kUnitOfMeasurementTypeBqm3,      /**< "Bq/m^3" */
    kUnitOfMeasurementTypeSv,        /**< "Sv" */
    kUnitOfMeasurementTypeRd,        /**< "Rd" */
    kUnitOfMeasurementTypeC,         /**< "C" */
    kUnitOfMeasurementTypeF,         /**< "F" */
    kUnitOfMeasurementTypeH,         /**< "H" */
    kUnitOfMeasurementTypeJ,         /**< "J" */
    kUnitOfMeasurementTypeN,         /**< "N" */
    kUnitOfMeasurementTypeNm,        /**< "N_m" */
    kUnitOfMeasurementTypeNs,        /**< "N_s" */
    kUnitOfMeasurementTypeWb,        /**< "Wb" */
    kUnitOfMeasurementTypeT,         /**< "T" */
    kUnitOfMeasurementTypePa,        /**< "Pa" */
    kUnitOfMeasurementTypebar,       /**< "bar" */
    kUnitOfMeasurementTypeatm,       /**< "atm" */
    kUnitOfMeasurementTypepsi,       /**< "psi" */
    kUnitOfMeasurementTypemmHg,      /**< "mmHg" */
    kUnitOfMeasurementTypem2,        /**< "m^2" */
    kUnitOfMeasurementTypem3,        /**< "m^3" */
    kUnitOfMeasurementTypem3h,       /**< "m^3/h" */
    kUnitOfMeasurementTypems,        /**< "m/s" */
    kUnitOfMeasurementTypems2,       /**< "m/s^2" */
    kUnitOfMeasurementTypem3s,       /**< "m^3/s" */
    kUnitOfMeasurementTypemm3,       /**< "m/m^3" */
    kUnitOfMeasurementTypekgm3,      /**< "kg/m^3" */
    kUnitOfMeasurementTypekgm,       /**< "kg_m" */
    kUnitOfMeasurementTypem2s,       /**< "m^2/s" */
    kUnitOfMeasurementTypewmk,       /**< "W/m_K" */
    kUnitOfMeasurementTypeJK,        /**< "J/K" */
    kUnitOfMeasurementType1s,        /**< "1/s" */
    kUnitOfMeasurementTypeWm2,       /**< "W/m^2" */
    kUnitOfMeasurementTypeJm2,       /**< "J/m^2" */
    kUnitOfMeasurementTypeS,         /**< "S" */
    kUnitOfMeasurementTypeSm,        /**< "S/m" */
    kUnitOfMeasurementTypeKs,        /**< "K/s" */
    kUnitOfMeasurementTypePas,       /**< "Pa/s" */
    kUnitOfMeasurementTypeJkgK,      /**< "J/kg_K" */
    kUnitOfMeasurementTypeVs,        /**< "Vs" */
    kUnitOfMeasurementTypeVm,        /**< "V/m" */
    kUnitOfMeasurementTypeVHz,       /**< "V/Hz" */
    kUnitOfMeasurementTypeAs,        /**< "As" */
    kUnitOfMeasurementTypeAm,        /**< "A/m" */
    kUnitOfMeasurementTypeHzs,       /**< "Hz/s" */
    kUnitOfMeasurementTypekgs,       /**< "kg/s" */
    kUnitOfMeasurementTypekgm2,      /**< "kg_m^2" */
    kUnitOfMeasurementTypeJWh,       /**< "J/Wh" */
    kUnitOfMeasurementTypeWs,        /**< "W/s" */
    kUnitOfMeasurementTypeft3,       /**< "ft^3" */
    kUnitOfMeasurementTypeft3h,      /**< "ft^3/h" */
    kUnitOfMeasurementTypeccf,       /**< "ccf" */
    kUnitOfMeasurementTypeccfh,      /**< "ccf/h" */
    kUnitOfMeasurementTypeUSliqgal,  /**< "US.liq.gal" */
    kUnitOfMeasurementTypeUSliqgalh, /**< "US.liq.gal/h" */
    kUnitOfMeasurementTypeImpgal,    /**< "Imp.gal" */
    kUnitOfMeasurementTypeImpgalh,   /**< "Imp.gal/h" */
    kUnitOfMeasurementTypeBtu,       /**< "Btu" */
    kUnitOfMeasurementTypeBtuh,      /**< "Btu/h" */
    kUnitOfMeasurementTypeAh,        /**< "Ah" */
    kUnitOfMeasurementTypekgWh,      /**< "kg/Wh" */
};

typedef int32_t CurrencyType;

enum {
    kCurrencyTypeAed, /**< "AED" */
    kCurrencyTypeAfn, /**< "AFN" */
    kCurrencyTypeAll, /**< "ALL" */
    kCurrencyTypeAmd, /**< "AMD" */
    kCurrencyTypeAng, /**< "ANG" */
    kCurrencyTypeAoa, /**< "AOA" */
    kCurrencyTypeArs, /**< "ARS" */
    kCurrencyTypeAud, /**< "AUD" */
    kCurrencyTypeAwg, /**< "AWG" */
    kCurrencyTypeAzn, /**< "AZN" */
    kCurrencyTypeBam, /**< "BAM" */
    kCurrencyTypeBbd, /**< "BBD" */
    kCurrencyTypeBdt, /**< "BDT" */
    kCurrencyTypeBgn, /**< "BGN" */
    kCurrencyTypeBhd, /**< "BHD" */
    kCurrencyTypeBif, /**< "BIF" */
    kCurrencyTypeBmd, /**< "BMD" */
    kCurrencyTypeBnd, /**< "BND" */
    kCurrencyTypeBob, /**< "BOB" */
    kCurrencyTypeBov, /**< "BOV" */
    kCurrencyTypeBrl, /**< "BRL" */
    kCurrencyTypeBsd, /**< "BSD" */
    kCurrencyTypeBtn, /**< "BTN" */
    kCurrencyTypeBwp, /**< "BWP" */
    kCurrencyTypeByr, /**< "BYR" */
    kCurrencyTypeBzd, /**< "BZD" */
    kCurrencyTypeCad, /**< "CAD" */
    kCurrencyTypeCdf, /**< "CDF" */
    kCurrencyTypeChe, /**< "CHE" */
    kCurrencyTypeChf, /**< "CHF" */
    kCurrencyTypeChw, /**< "CHW" */
    kCurrencyTypeClf, /**< "CLF" */
    kCurrencyTypeClp, /**< "CLP" */
    kCurrencyTypeCny, /**< "CNY" */
    kCurrencyTypeCop, /**< "COP" */
    kCurrencyTypeCou, /**< "COU" */
    kCurrencyTypeCrc, /**< "CRC" */
    kCurrencyTypeCuc, /**< "CUC" */
    kCurrencyTypeCup, /**< "CUP" */
    kCurrencyTypeCve, /**< "CVE" */
    kCurrencyTypeCzk, /**< "CZK" */
    kCurrencyTypeDjf, /**< "DJF" */
    kCurrencyTypeDkk, /**< "DKK" */
    kCurrencyTypeDop, /**< "DOP" */
    kCurrencyTypeDzd, /**< "DZD" */
    kCurrencyTypeEgp, /**< "EGP" */
    kCurrencyTypeErn, /**< "ERN" */
    kCurrencyTypeEtb, /**< "ETB" */
    kCurrencyTypeEur, /**< "EUR" */
    kCurrencyTypeFjd, /**< "FJD" */
    kCurrencyTypeFkp, /**< "FKP" */
    kCurrencyTypeGbp, /**< "GBP" */
    kCurrencyTypeGel, /**< "GEL" */
    kCurrencyTypeGhs, /**< "GHS" */
    kCurrencyTypeGip, /**< "GIP" */
    kCurrencyTypeGmd, /**< "GMD" */
    kCurrencyTypeGnf, /**< "GNF" */
    kCurrencyTypeGtq, /**< "GTQ" */
    kCurrencyTypeGyd, /**< "GYD" */
    kCurrencyTypeHkd, /**< "HKD" */
    kCurrencyTypeHnl, /**< "HNL" */
    kCurrencyTypeHrk, /**< "HRK" */
    kCurrencyTypeHtg, /**< "HTG" */
    kCurrencyTypeHuf, /**< "HUF" */
    kCurrencyTypeIdr, /**< "IDR" */
    kCurrencyTypeIls, /**< "ILS" */
    kCurrencyTypeInr, /**< "INR" */
    kCurrencyTypeIqd, /**< "IQD" */
    kCurrencyTypeIrr, /**< "IRR" */
    kCurrencyTypeIsk, /**< "ISK" */
    kCurrencyTypeJmd, /**< "JMD" */
    kCurrencyTypeJod, /**< "JOD" */
    kCurrencyTypeJpy, /**< "JPY" */
    kCurrencyTypeKes, /**< "KES" */
    kCurrencyTypeKgs, /**< "KGS" */
    kCurrencyTypeKhr, /**< "KHR" */
    kCurrencyTypeKmf, /**< "KMF" */
    kCurrencyTypeKpw, /**< "KPW" */
    kCurrencyTypeKrw, /**< "KRW" */
    kCurrencyTypeKwd, /**< "KWD" */
    kCurrencyTypeKyd, /**< "KYD" */
    kCurrencyTypeKzt, /**< "KZT" */
    kCurrencyTypeLak, /**< "LAK" */
    kCurrencyTypeLbp, /**< "LBP" */
    kCurrencyTypeLkr, /**< "LKR" */
    kCurrencyTypeLrd, /**< "LRD" */
    kCurrencyTypeLsl, /**< "LSL" */
    kCurrencyTypeLyd, /**< "LYD" */
    kCurrencyTypeMad, /**< "MAD" */
    kCurrencyTypeMdl, /**< "MDL" */
    kCurrencyTypeMga, /**< "MGA" */
    kCurrencyTypeMkd, /**< "MKD" */
    kCurrencyTypeMmk, /**< "MMK" */
    kCurrencyTypeMnt, /**< "MNT" */
    kCurrencyTypeMop, /**< "MOP" */
    kCurrencyTypeMro, /**< "MRO" */
    kCurrencyTypeMur, /**< "MUR" */
    kCurrencyTypeMvr, /**< "MVR" */
    kCurrencyTypeMwk, /**< "MWK" */
    kCurrencyTypeMxn, /**< "MXN" */
    kCurrencyTypeMxv, /**< "MXV" */
    kCurrencyTypeMyr, /**< "MYR" */
    kCurrencyTypeMzn, /**< "MZN" */
    kCurrencyTypeNad, /**< "NAD" */
    kCurrencyTypeNgn, /**< "NGN" */
    kCurrencyTypeNio, /**< "NIO" */
    kCurrencyTypeNok, /**< "NOK" */
    kCurrencyTypeNpr, /**< "NPR" */
    kCurrencyTypeNzd, /**< "NZD" */
    kCurrencyTypeOmr, /**< "OMR" */
    kCurrencyTypePab, /**< "PAB" */
    kCurrencyTypePen, /**< "PEN" */
    kCurrencyTypePgk, /**< "PGK" */
    kCurrencyTypePhp, /**< "PHP" */
    kCurrencyTypePkr, /**< "PKR" */
    kCurrencyTypePln, /**< "PLN" */
    kCurrencyTypePyg, /**< "PYG" */
    kCurrencyTypeQar, /**< "QAR" */
    kCurrencyTypeRon, /**< "RON" */
    kCurrencyTypeRsd, /**< "RSD" */
    kCurrencyTypeRub, /**< "RUB" */
    kCurrencyTypeRwf, /**< "RWF" */
    kCurrencyTypeSar, /**< "SAR" */
    kCurrencyTypeSbd, /**< "SBD" */
    kCurrencyTypeScr, /**< "SCR" */
    kCurrencyTypeSdg, /**< "SDG" */
    kCurrencyTypeSek, /**< "SEK" */
    kCurrencyTypeSgd, /**< "SGD" */
    kCurrencyTypeShp, /**< "SHP" */
    kCurrencyTypeSll, /**< "SLL" */
    kCurrencyTypeSos, /**< "SOS" */
    kCurrencyTypeSrd, /**< "SRD" */
    kCurrencyTypeSsp, /**< "SSP" */
    kCurrencyTypeStd, /**< "STD" */
    kCurrencyTypeSvc, /**< "SVC" */
    kCurrencyTypeSyp, /**< "SYP" */
    kCurrencyTypeSzl, /**< "SZL" */
    kCurrencyTypeThb, /**< "THB" */
    kCurrencyTypeTjs, /**< "TJS" */
    kCurrencyTypeTmt, /**< "TMT" */
    kCurrencyTypeTnd, /**< "TND" */
    kCurrencyTypeTop, /**< "TOP" */
    kCurrencyTypeTry, /**< "TRY" */
    kCurrencyTypeTtd, /**< "TTD" */
    kCurrencyTypeTwd, /**< "TWD" */
    kCurrencyTypeTzs, /**< "TZS" */
    kCurrencyTypeUah, /**< "UAH" */
    kCurrencyTypeUgx, /**< "UGX" */
    kCurrencyTypeUsd, /**< "USD" */
    kCurrencyTypeUsn, /**< "USN" */
    kCurrencyTypeUyi, /**< "UYI" */
    kCurrencyTypeUyu, /**< "UYU" */
    kCurrencyTypeUzs, /**< "UZS" */
    kCurrencyTypeVef, /**< "VEF" */
    kCurrencyTypeVnd, /**< "VND" */
    kCurrencyTypeVuv, /**< "VUV" */
    kCurrencyTypeWst, /**< "WST" */
    kCurrencyTypeXaf, /**< "XAF" */
    kCurrencyTypeXag, /**< "XAG" */
    kCurrencyTypeXau, /**< "XAU" */
    kCurrencyTypeXba, /**< "XBA" */
    kCurrencyTypeXbb, /**< "XBB" */
    kCurrencyTypeXbc, /**< "XBC" */
    kCurrencyTypeXbd, /**< "XBD" */
    kCurrencyTypeXcd, /**< "XCD" */
    kCurrencyTypeXdr, /**< "XDR" */
    kCurrencyTypeXof, /**< "XOF" */
    kCurrencyTypeXpd, /**< "XPD" */
    kCurrencyTypeXpf, /**< "XPF" */
    kCurrencyTypeXpt, /**< "XPT" */
    kCurrencyTypeXsu, /**< "XSU" */
    kCurrencyTypeXts, /**< "XTS" */
    kCurrencyTypeXua, /**< "XUA" */
    kCurrencyTypeXxx, /**< "XXX" */
    kCurrencyTypeYer, /**< "YER" */
    kCurrencyTypeZar, /**< "ZAR" */
    kCurrencyTypeZmw, /**< "ZMW" */
    kCurrencyTypeZwl, /**< "ZWL" */
};

typedef int32_t ScopeTypeType;

enum {
    kScopeTypeTypeAC,                              /**< "ac" */
    kScopeTypeTypeACCosPhiGrid,                    /**< "acCosPhiGrid" */
    kScopeTypeTypeACCurrentA,                      /**< "acCurrentA" */
    kScopeTypeTypeACCurrentB,                      /**< "acCurrentB" */
    kScopeTypeTypeACCurrentC,                      /**< "acCurrentC" */
    kScopeTypeTypeACFrequency,                     /**< "acFrequency" */
    kScopeTypeTypeACFrequencyGrid,                 /**< "acFrequencyGrid" */
    kScopeTypeTypeACPowerA,                        /**< "acPowerA" */
    kScopeTypeTypeACPowerB,                        /**< "acPowerB" */
    kScopeTypeTypeACPowerC,                        /**< "acPowerC" */
    kScopeTypeTypeACPowerLimitPct,                 /**< "acPowerLimitPct" */
    kScopeTypeTypeACPowerTotal,                    /**< "acPowerTotal" */
    kScopeTypeTypeACVoltageA,                      /**< "acVoltageA" */
    kScopeTypeTypeACVoltageB,                      /**< "acVoltageB" */
    kScopeTypeTypeACVoltageC,                      /**< "acVoltageC" */
    kScopeTypeTypeACYieldDay,                      /**< "acYieldDay" */
    kScopeTypeTypeACYieldTotal,                    /**< "acYieldTotal" */
    kScopeTypeTypeDCCurrent,                       /**< "dcCurrent" */
    kScopeTypeTypeDCPower,                         /**< "dcPower" */
    kScopeTypeTypeDCString1,                       /**< "dcString1" */
    kScopeTypeTypeDCString2,                       /**< "dcString2" */
    kScopeTypeTypeDCString3,                       /**< "dcString3" */
    kScopeTypeTypeDCString4,                       /**< "dcString4" */
    kScopeTypeTypeDCString5,                       /**< "dcString5" */
    kScopeTypeTypeDCString6,                       /**< "dcString6" */
    kScopeTypeTypeDCTotal,                         /**< "dcTotal" */
    kScopeTypeTypeDCVoltage,                       /**< "dcVoltage" */
    kScopeTypeTypeDhwTemperature,                  /**< "dhwTemperature" */
    kScopeTypeTypeFlowTemperature,                 /**< "flowTemperature" */
    kScopeTypeTypeOutsideAirTemperature,           /**< "outsideAirTemperature" */
    kScopeTypeTypeReturnTemperature,               /**< "returnTemperature" */
    kScopeTypeTypeRoomAirTemperature,              /**< "roomAirTemperature" */
    kScopeTypeTypeCharge,                          /**< "charge" */
    kScopeTypeTypeStateOfCharge,                   /**< "stateOfCharge" */
    kScopeTypeTypeDischarge,                       /**< "discharge" */
    kScopeTypeTypeGridConsumption,                 /**< "gridConsumption" */
    kScopeTypeTypeGridFeedIn,                      /**< "gridFeedIn" */
    kScopeTypeTypeSelfConsumption,                 /**< "selfConsumption" */
    kScopeTypeTypeOverloadProtection,              /**< "overloadProtection" */
    kScopeTypeTypeACPower,                         /**< "acPower" */
    kScopeTypeTypeACEnergy,                        /**< "acEnergy" */
    kScopeTypeTypeACCurrent,                       /**< "acCurrent" */
    kScopeTypeTypeACVoltage,                       /**< "acVoltage" */
    kScopeTypeTypeBatteryControl,                  /**< "batteryControl" */
    kScopeTypeTypeSimpleIncentiveTable,            /**< "simpleIncentiveTable" */
    kScopeTypeTypeStateOfHealth,                   /**< "stateOfHealth" */
    kScopeTypeTypeTravelRange,                     /**< "travelRange" */
    kScopeTypeTypeNominalEnergyCapacity,           /**< "nominalEnergyCapacity" */
    kScopeTypeTypeACPowerReal,                     /**< "acPowerReal" */
    kScopeTypeTypeACPowerApparent,                 /**< "acPowerApparent" */
    kScopeTypeTypeACPowerReactive,                 /**< "acPowerReactive" */
    kScopeTypeTypeACYieldMonth,                    /**< "acYieldMonth" */
    kScopeTypeTypeACYieldYear,                     /**< "acYieldYear" */
    kScopeTypeTypeACCosPhi,                        /**< "acCosPhi" */
    kScopeTypeTypeDCEnergy,                        /**< "dcEnergy" */
    kScopeTypeTypeInsulationResistance,            /**< "insulationResistance" */
    kScopeTypeTypeStateOfEnergy,                   /**< "stateOfEnergy" */
    kScopeTypeTypeUseableCapacity,                 /**< "useableCapacity" */
    kScopeTypeTypeDCChargeEnergy,                  /**< "dcChargeEnergy" */
    kScopeTypeTypeDCDischargeEnergy,               /**< "dcDischargeEnergy" */
    kScopeTypeTypeLoadCycleCount,                  /**< "loadCycleCount" */
    kScopeTypeTypeComponentTemperature,            /**< "componentTemperature" */
    kScopeTypeTypeGridLimit,                       /**< "gridLimit" */
    kScopeTypeTypeGridLimitFallback,               /**< "gridLimitFallback" */
    kScopeTypeTypeACPowerApparentTotal,            /**< "acPowerApparentTotal" */
    kScopeTypeTypeACPowerReactiveTotal,            /**< "acPowerReactiveTotal" */
    kScopeTypeTypeACCurrentTotal,                  /**< "acCurrentTotal" */
    kScopeTypeTypeACEnergyConsumed,                /**< "acEnergyConsumed" */
    kScopeTypeTypeACEnergyProduced,                /**< "acEnergyProduced" */
    kScopeTypeTypeBatteryAcPower,                  /**< "batteryAcPower" */
    kScopeTypeTypeBatteryAcPowerPhaseSpecific,     /**< "batteryAcPowerPhaseSpecific" */
    kScopeTypeTypeBatteryDcPower,                  /**< "batteryDcPower" */
    kScopeTypeTypePccPower,                        /**< "pccPower" */
    kScopeTypeTypeActivePowerLimit,                /**< "activePowerLimit" */
    kScopeTypeTypeActivePowerLimitPercentage,      /**< "activePowerLimitPercentage" */
    kScopeTypeTypeSimpleCommittedIncentiveTable,   /**< "simpleCommittedIncentiveTable" */
    kScopeTypeTypeSimplePreliminaryIncentiveTable, /**< "simplePreliminaryIncentiveTable" */
    kScopeTypeTypeCommittedPowerPlan,              /**< "committedPowerPlan" */
    kScopeTypeTypePreliminaryPowerPlan,            /**< "preliminaryPowerPlan" */
    kScopeTypeTypeIncentiveTableEnConsWithPoETF,   /**< "incentiveTableEnConsWithPoETF" */
    kScopeTypeTypeIncentiveTableEnProdWithPoETF,   /**< "incentiveTableEnProdWithPoETF" */
    kScopeTypeTypeIncentiveTableEnConsWithPoE,     /**< "incentiveTableEnConsWithPoE" */
    kScopeTypeTypeIncentiveTableEnProdWithPoE,     /**< "incentiveTableEnProdWithPoE" */
    kScopeTypeTypeIncentiveTableEnConsWithTF,      /**< "incentiveTableEnConsWithTF" */
    kScopeTypeTypeIncentiveTableEnProdWithTF,      /**< "incentiveTableEnProdWithTF" */
    kScopeTypeTypeActivePowerForecast,             /**< "activePowerForecast" */
};

typedef uint32_t SessionIdType;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_COMMON_TYPES_H_
