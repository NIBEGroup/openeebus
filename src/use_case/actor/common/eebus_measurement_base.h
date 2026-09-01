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
 * @brief Shared measurement base for MU MPC and GCP MGCP server-side use cases
 */

#ifndef SRC_USE_CASE_ACTOR_COMMON_EEBUS_MEASUREMENT_BASE_H_
#define SRC_USE_CASE_ACTOR_COMMON_EEBUS_MEASUREMENT_BASE_H_

#include "src/use_case/api/eebus_measurement_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EebusMeasurementBase EebusMeasurementBase;

/**
 * @brief Strategy function pointer type used to register a measurement's SPINE
 *        description and electrical connection parameter description during
 *        use-case initialisation.
 */
typedef EebusError (*EebusMeasurementConfigureStrategy)(
    EebusMeasurementBase* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
);

typedef struct EebusMeasurementBaseConfig {
    MeasurementValueSourceType value_source;
    MeasurementConstraintsDataType* constraints; /**< May be NULL */
} EebusMeasurementBaseConfig;

/**
 * @brief Allocate and initialise a measurement object.
 * @param name     Measurement name identifier (used to look up this object later).
 * @param scope    SPINE scope type (e.g. acPowerTotal, acCurrent).
 * @param phases   Electrical connection phase(s) this measurement applies to.
 * @param cfg      Value source and optional constraints — must not be NULL.
 * @param strategy Configuration strategy called during use-case initialisation
 *                 to register the SPINE measurement and parameter descriptions.
 * @return Pointer to the created object, or NULL on allocation failure.
 */
EebusMeasurementObject* EebusMeasurementBaseCreate(
    EebusMeasurementNameId name,
    ScopeTypeType scope,
    ElectricalConnectionPhaseNameType phases,
    const EebusMeasurementBaseConfig* cfg,
    EebusMeasurementConfigureStrategy strategy
);

/**
 * @brief Configure strategy: registers a power measurement description and
 *        the corresponding electrical connection parameter description.
 *        Intended to be passed as the @p strategy argument of EebusMeasurementBaseCreate().
 * @param self  Measurement object being configured.
 * @param msrv  Measurement server used to add the measurement description.
 * @param ecsrv Electrical connection server used to add the parameter description.
 * @param ec_id Electrical connection ID to associate with the parameter description.
 * @return kEebusErrorOk on success, or an error code on failure.
 */
EebusError EebusMeasurementBaseConfigurePower(
    EebusMeasurementBase* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
);

/**
 * @brief Configure strategy: registers an energy measurement description and
 *        the corresponding electrical connection parameter description.
 *        Intended to be passed as the @p strategy argument of EebusMeasurementBaseCreate().
 * @param self  Measurement object being configured.
 * @param msrv  Measurement server used to add the measurement description.
 * @param ecsrv Electrical connection server used to add the parameter description.
 * @param ec_id Electrical connection ID to associate with the parameter description.
 * @return kEebusErrorOk on success, or an error code on failure.
 */
EebusError EebusMeasurementBaseConfigureEnergy(
    EebusMeasurementBase* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
);

/**
 * @brief Configure strategy: registers a current measurement description and
 *        the corresponding electrical connection parameter description.
 *        Intended to be passed as the @p strategy argument of EebusMeasurementBaseCreate().
 * @param self  Measurement object being configured.
 * @param msrv  Measurement server used to add the measurement description.
 * @param ecsrv Electrical connection server used to add the parameter description.
 * @param ec_id Electrical connection ID to associate with the parameter description.
 * @return kEebusErrorOk on success, or an error code on failure.
 */
EebusError EebusMeasurementBaseConfigureCurrent(
    EebusMeasurementBase* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
);

/**
 * @brief Configure strategy: registers a voltage measurement description and
 *        the corresponding electrical connection parameter description.
 *        Phase mapping (from/to) is derived from the phases field set at creation time.
 *        Intended to be passed as the @p strategy argument of EebusMeasurementBaseCreate().
 * @param self  Measurement object being configured.
 * @param msrv  Measurement server used to add the measurement description.
 * @param ecsrv Electrical connection server used to add the parameter description.
 * @param ec_id Electrical connection ID to associate with the parameter description.
 * @return kEebusErrorOk on success, or an error code on failure.
 */
EebusError EebusMeasurementBaseConfigureVoltage(
    EebusMeasurementBase* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
);

/**
 * @brief Configure strategy: registers a frequency measurement description and
 *        the corresponding electrical connection parameter description.
 *        Intended to be passed as the @p strategy argument of EebusMeasurementBaseCreate().
 * @param self  Measurement object being configured.
 * @param msrv  Measurement server used to add the measurement description.
 * @param ecsrv Electrical connection server used to add the parameter description.
 * @param ec_id Electrical connection ID to associate with the parameter description.
 * @return kEebusErrorOk on success, or an error code on failure.
 */
EebusError EebusMeasurementBaseConfigureFrequency(
    EebusMeasurementBase* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_COMMON_EEBUS_MEASUREMENT_BASE_H_
