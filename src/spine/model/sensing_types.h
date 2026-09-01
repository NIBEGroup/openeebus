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
 * @brief SPINE Datagram Sensing data types declarations
 */

#ifndef SRC_SPINE_MODEL_SENSING_TYPES_H_
#define SRC_SPINE_MODEL_SENSING_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef int32_t SensingStateType;

enum {
    kSensingStateTypeOn,                      /**< "on" */
    kSensingStateTypeOff,                     /**< "off" */
    kSensingStateTypeToggle,                  /**< "toggle" */
    kSensingStateTypeLevel,                   /**< "level" */
    kSensingStateTypeLevelUp,                 /**< "levelUp" */
    kSensingStateTypeLevelDown,               /**< "levelDown" */
    kSensingStateTypeLevelStart,              /**< "levelStart" */
    kSensingStateTypeLevelStop,               /**< "levelStop" */
    kSensingStateTypeLevelAbsolute,           /**< "levelAbsolute" */
    kSensingStateTypeLevelRelative,           /**< "levelRelative" */
    kSensingStateTypeLevelPercentageAbsolute, /**< "levelPercentageAbsolute" */
    kSensingStateTypeLevelPercentageRelative, /**< "levelPercentageRelative" */
    kSensingStateTypePressed,                 /**< "pressed" */
    kSensingStateTypeLongPressed,             /**< "longPressed" */
    kSensingStateTypeReleased,                /**< "released" */
    kSensingStateTypeChanged,                 /**< "changed" */
    kSensingStateTypeStarted,                 /**< "started" */
    kSensingStateTypeStopped,                 /**< "stopped" */
    kSensingStateTypePaused,                  /**< "paused" */
    kSensingStateTypeMiddle,                  /**< "middle" */
    kSensingStateTypeUp,                      /**< "up" */
    kSensingStateTypeDown,                    /**< "down" */
    kSensingStateTypeForward,                 /**< "forward" */
    kSensingStateTypeBackwards,               /**< "backwards" */
    kSensingStateTypeOpen,                    /**< "open" */
    kSensingStateTypeClosed,                  /**< "closed" */
    kSensingStateTypeOpening,                 /**< "opening" */
    kSensingStateTypeClosing,                 /**< "closing" */
    kSensingStateTypeHigh,                    /**< "high" */
    kSensingStateTypeLow,                     /**< "low" */
    kSensingStateTypeDay,                     /**< "day" */
    kSensingStateTypeNight,                   /**< "night" */
    kSensingStateTypeDetected,                /**< "detected" */
    kSensingStateTypeNotDetected,             /**< "notDetected" */
    kSensingStateTypeAlarmed,                 /**< "alarmed" */
    kSensingStateTypeNotAlarmed,              /**< "notAlarmed" */
};

typedef int32_t SensingTypeType;

enum {
    kSensingTypeTypeSwitch,            /**< "switch" */
    kSensingTypeTypeButton,            /**< "button" */
    kSensingTypeTypeLevel,             /**< "level" */
    kSensingTypeTypeLevelSwitch,       /**< "levelSwitch" */
    kSensingTypeTypeWindowHandle,      /**< "windowHandle" */
    kSensingTypeTypeContactSensor,     /**< "contactSensor" */
    kSensingTypeTypeOccupancySensor,   /**< "occupancySensor" */
    kSensingTypeTypeMotionDetector,    /**< "motionDetector" */
    kSensingTypeTypeFireDetector,      /**< "fireDetector" */
    kSensingTypeTypeSmokeDetector,     /**< "smokeDetector" */
    kSensingTypeTypeHeatDetector,      /**< "heatDetector" */
    kSensingTypeTypeWaterDetector,     /**< "waterDetector" */
    kSensingTypeTypeGasDetector,       /**< "gasDetector" */
    kSensingTypeTypeAlarmSensor,       /**< "alarmSensor" */
    kSensingTypeTypePowerAlarmSensor,  /**< "powerAlarmSensor" */
    kSensingTypeTypeDayNightIndicator, /**< "dayNightIndicator" */
};

typedef struct SensingDataType SensingDataType;

struct SensingDataType {
    const AbsoluteOrRelativeTimeType* timestamp; /**< "timestamp" */
    const SensingStateType* state;               /**< "state" */
    const ScaledNumberType* value;               /**< "value" */
};

typedef struct SensingDataElementsType SensingDataElementsType;

struct SensingDataElementsType {
    ElementTagType timestamp;              /**< "timestamp" */
    ElementTagType state;                  /**< "state" */
    const ScaledNumberElementsType* value; /**< "value" */
};

typedef struct SensingListDataType SensingListDataType;

struct SensingListDataType {
    const SensingDataType* const* sensing_data; /**< "sensingData" */
    size_t sensing_data_size;
};

typedef struct SensingListDataSelectorsType SensingListDataSelectorsType;

struct SensingListDataSelectorsType {
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
};

typedef struct SensingDescriptionDataType SensingDescriptionDataType;

struct SensingDescriptionDataType {
    const SensingTypeType* sensing_type; /**< "sensingType" */
    const UnitOfMeasurementType* unit;   /**< "unit" */
    const ScopeTypeType* scope_type;     /**< "scopeType" */
    LabelType label;                     /**< "label" */
    DescriptionType description;         /**< "description" */
};

typedef struct SensingDescriptionDataElementsType SensingDescriptionDataElementsType;

struct SensingDescriptionDataElementsType {
    ElementTagType sensing_type; /**< "sensingType" */
    ElementTagType unit;         /**< "unit" */
    ElementTagType scope_type;   /**< "scopeType" */
    ElementTagType label;        /**< "label" */
    ElementTagType description;  /**< "description" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_SENSING_TYPES_H_
