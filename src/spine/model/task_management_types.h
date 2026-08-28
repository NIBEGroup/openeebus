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
 * @brief SPINE Datagram Task Management data types declarations
 */

#ifndef SRC_SPINE_MODEL_TASK_MANAGEMENT_TYPES_H_
#define SRC_SPINE_MODEL_TASK_MANAGEMENT_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/hvac_types.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/power_sequences_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t TaskManagementJobIdType;

typedef int32_t TaskManagementJobStateType;

enum {
    kTaskManagementJobStateTypeRunning,         /**< "Running" */
    kTaskManagementJobStateTypePaused,          /**< "paused" */
    kTaskManagementJobStateTypeInactive,        /**< "inactive" */
    kTaskManagementJobStateTypeActive,          /**< "active" */
    kTaskManagementJobStateTypeFinished,        /**< "finished" */
    kTaskManagementJobStateTypeEventAccepted,   /**< "eventAccepted" */
    kTaskManagementJobStateTypeEventStarted,    /**< "eventStarted" */
    kTaskManagementJobStateTypeEventStopped,    /**< "eventStopped" */
    kTaskManagementJobStateTypeEventRejected,   /**< "eventRejected" */
    kTaskManagementJobStateTypeEventCancelled,  /**< "eventCancelled" */
    kTaskManagementJobStateTypeEventError,      /**< "eventError" */
    kTaskManagementJobStateTypeScheduled,       /**< "scheduled" */
    kTaskManagementJobStateTypeScheduledPaused, /**< "scheduledPaused" */
    kTaskManagementJobStateTypePending,         /**< "pending" */
    kTaskManagementJobStateTypeCompleted,       /**< "completed" */
    kTaskManagementJobStateTypeInvalid,         /**< "invalid" */
};

typedef int32_t TaskManagementJobSourceType;

enum {
    kTaskManagementJobSourceTypeInternalMechanism,     /**< "InternalMechanism" */
    kTaskManagementJobSourceTypeUserInteraction,       /**< "UserInteraction" */
    kTaskManagementJobSourceTypeExternalConfiguration, /**< "ExternalConfiguration" */
};

typedef TagType TaskManagementDirectControlRelatedType;

typedef TagType TaskManagementDirectControlRelatedElementsType;

typedef struct TaskManagementHvacRelatedType TaskManagementHvacRelatedType;

struct TaskManagementHvacRelatedType {
    const HvacOverrunIdType* overrun_id; /**< "overrunId" */
};

typedef struct TaskManagementHvacRelatedElementsType TaskManagementHvacRelatedElementsType;

struct TaskManagementHvacRelatedElementsType {
    ElementTagType overrun_id; /**< "overrunId" */
};

typedef struct TaskManagementLoadControlReleatedType TaskManagementLoadControlReleatedType;

struct TaskManagementLoadControlReleatedType {
    const LoadControlEventIdType* event_id; /**< "eventId" */
};

typedef struct TaskManagementLoadControlReleatedElementsType TaskManagementLoadControlReleatedElementsType;

struct TaskManagementLoadControlReleatedElementsType {
    ElementTagType event_id; /**< "eventId" */
};

typedef struct TaskManagementPowerSequencesRelatedType TaskManagementPowerSequencesRelatedType;

struct TaskManagementPowerSequencesRelatedType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct TaskManagementPowerSequencesRelatedElementsType TaskManagementPowerSequencesRelatedElementsType;

struct TaskManagementPowerSequencesRelatedElementsType {
    ElementTagType sequence_id; /**< "sequenceId" */
};

typedef struct TaskManagementSmartEnergyManagementPsRelatedType TaskManagementSmartEnergyManagementPsRelatedType;

struct TaskManagementSmartEnergyManagementPsRelatedType {
    const PowerSequenceIdType* sequence_id; /**< "sequenceId" */
};

typedef struct TaskManagementSmartEnergyManagementPsRelatedElementsType
    TaskManagementSmartEnergyManagementPsRelatedElementsType;

struct TaskManagementSmartEnergyManagementPsRelatedElementsType {
    ElementTagType sequence_id; /**< "sequenceId" */
};

typedef struct TaskManagementJobDataType TaskManagementJobDataType;

struct TaskManagementJobDataType {
    const TaskManagementJobIdType* job_id;       /**< "jobId" */
    const AbsoluteOrRelativeTimeType* timestamp; /**< "timestamp" */
    const TaskManagementJobStateType* job_state; /**< "jobState" */
    const DurationType* elapsed_time;            /**< "elapsedTime" */
    const DurationType* remaining_time;          /**< "remainingTime" */
};

typedef struct TaskManagementJobDataElementsType TaskManagementJobDataElementsType;

struct TaskManagementJobDataElementsType {
    ElementTagType job_id;         /**< "jobId" */
    ElementTagType timestamp;      /**< "timestamp" */
    ElementTagType job_state;      /**< "jobState" */
    ElementTagType elapsed_time;   /**< "elapsedTime" */
    ElementTagType remaining_time; /**< "remainingTime" */
};

typedef struct TaskManagementJobListDataType TaskManagementJobListDataType;

struct TaskManagementJobListDataType {
    const TaskManagementJobDataType* const* task_management_job_data; /**< "taskManagementJobData" */
    size_t task_management_job_data_size;
};

typedef struct TaskManagementJobListDataSelectorsType TaskManagementJobListDataSelectorsType;

struct TaskManagementJobListDataSelectorsType {
    const TaskManagementJobIdType* job_id;       /**< "jobId" */
    const TaskManagementJobStateType* job_state; /**< "jobState" */
};

typedef struct TaskManagementJobRelationDataType TaskManagementJobRelationDataType;

struct TaskManagementJobRelationDataType {
    const TaskManagementJobIdType* job_id;                                  /**< "jobId" */
    TaskManagementDirectControlRelatedType direct_control_related;          /**< "directControlRelated" */
    const TaskManagementHvacRelatedType* hvac_related;                      /**< "hvacRelated" */
    const TaskManagementLoadControlReleatedType* load_control_releated;     /**< "loadControlReleated" */
    const TaskManagementPowerSequencesRelatedType* power_sequences_related; /**< "powerSequencesRelated" */
    const TaskManagementSmartEnergyManagementPsRelatedType*
        smart_energy_management_ps_related; /**< "smartEnergyManagementPsRelated" */
};

typedef struct TaskManagementJobRelationDataElementsType TaskManagementJobRelationDataElementsType;

struct TaskManagementJobRelationDataElementsType {
    ElementTagType job_id;                                                          /**< "jobId" */
    TaskManagementDirectControlRelatedElementsType direct_control_related;          /**< "directControlRelated" */
    const TaskManagementHvacRelatedElementsType* hvac_related;                      /**< "hvacRelated" */
    const TaskManagementLoadControlReleatedElementsType* load_control_releated;     /**< "loadControlReleated" */
    const TaskManagementPowerSequencesRelatedElementsType* power_sequences_related; /**< "powerSequencesRelated" */
    const TaskManagementSmartEnergyManagementPsRelatedElementsType*
        smart_energy_management_ps_related; /**< "smartEnergyManagementPsRelated" */
};

typedef struct TaskManagementJobRelationListDataType TaskManagementJobRelationListDataType;

struct TaskManagementJobRelationListDataType {
    const TaskManagementJobRelationDataType* const*
        task_management_job_relation_data; /**< "taskManagementJobRelationData" */
    size_t task_management_job_relation_data_size;
};

typedef struct TaskManagementJobRelationListDataSelectorsType TaskManagementJobRelationListDataSelectorsType;

struct TaskManagementJobRelationListDataSelectorsType {
    const TaskManagementJobIdType* job_id; /**< "jobId" */
};

typedef struct TaskManagementJobDescriptionDataType TaskManagementJobDescriptionDataType;

struct TaskManagementJobDescriptionDataType {
    const TaskManagementJobIdType* job_id;         /**< "jobId" */
    const TaskManagementJobSourceType* job_source; /**< "jobSource" */
    LabelType label;                               /**< "label" */
    DescriptionType description;                   /**< "description" */
};

typedef struct TaskManagementJobDescriptionDataElementsType TaskManagementJobDescriptionDataElementsType;

struct TaskManagementJobDescriptionDataElementsType {
    ElementTagType job_id;      /**< "jobId" */
    ElementTagType job_source;  /**< "jobSource" */
    ElementTagType label;       /**< "label" */
    ElementTagType description; /**< "description" */
};

typedef struct TaskManagementJobDescriptionListDataType TaskManagementJobDescriptionListDataType;

struct TaskManagementJobDescriptionListDataType {
    const TaskManagementJobDescriptionDataType* const*
        task_management_job_description_data; /**< "taskManagementJobDescriptionData" */
    size_t task_management_job_description_data_size;
};

typedef struct TaskManagementJobDescriptionListDataSelectorsType TaskManagementJobDescriptionListDataSelectorsType;

struct TaskManagementJobDescriptionListDataSelectorsType {
    const TaskManagementJobIdType* job_id;         /**< "jobId" */
    const TaskManagementJobSourceType* job_source; /**< "jobSource" */
};

typedef struct TaskManagementOverviewDataType TaskManagementOverviewDataType;

struct TaskManagementOverviewDataType {
    const bool* remote_controllable; /**< "remoteControllable" */
    const bool* jobs_active;         /**< "jobsActive" */
};

typedef struct TaskManagementOverviewDataElementsType TaskManagementOverviewDataElementsType;

struct TaskManagementOverviewDataElementsType {
    ElementTagType remote_controllable; /**< "remoteControllable" */
    ElementTagType jobs_active;         /**< "jobsActive" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_TASK_MANAGEMENT_TYPES_H_
