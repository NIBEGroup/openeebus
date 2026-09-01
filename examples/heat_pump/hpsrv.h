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
 * @brief EEBUS Heat Pump Service
 */

#ifndef SRC_HPSRV_H_
#define SRC_HPSRV_H_

#include <stddef.h>
#include <stdint.h>

#include "src/common/eebus_malloc.h"
#include "src/service/api/service_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/use_case/model/scaled_value.h"

typedef struct HpsrvObject HpsrvObject;

struct HpsrvObject {
    /** Implement service reader interface */
    ServiceReaderObject service_reader;
};

#define HPSRV_OBJECT(obj) ((HpsrvObject*)(obj))

/**
 * @brief Create the EEBUS Heat Pump Service instance
 * @param port Port to be used in mDNS server
 * @param role Role "client", "server" or "auto" to be used in SHIP connection
 * @param tls_certificate TLS Certificate object containing the certificate and private key
 * @return Pointer to the EEBUS Heat Pump Service instance
 */
HpsrvObject* HpsrvOpen(int32_t port, const char* role, TlsCertificateObject* tls_certificate);

static inline void HpsrvClose(HpsrvObject* hpsrv) {
    if (hpsrv != NULL) {
        SERVICE_READER_DESTRUCT(SERVICE_READER_OBJECT(hpsrv));
        EEBUS_FREE(hpsrv);
    }
}

/**
 * @brief Register remote SKI to the EEBUS Heat Pump Service instance
 * @param self Pointer to the HpsrvObject instance to register the remote SKI for
 * @param ski Remote SKI to be registered
 */
void HpsrvRegisterRemoteSki(HpsrvObject* self, const char* ski);

/**
 * @brief Unregister remote SKI from the EEBUS Heat Pump Service instance
 * @param self Pointer to the HpsrvObject instance to unregister the remote SKI for
 * @param ski Remote SKI to be unregistered
 */
void HpsrvUnregisterRemoteSki(HpsrvObject* self, const char* ski);

/**
 * @brief Set the MU MPC Scenario 1 Total Power (produced/consumed).
 * Passing e.g 99000 to power_total will result in setting the 990.00W
 * @param self Pointer to the HpsrvObject instance to set the total power for
 * @param power_total Total power value in W scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetPowerTotal(HpsrvObject* self, int32_t power_total);

/**
 * @brief Set the MU MPC Scenario 1 Power per phase.
 * Passing e.g. 100000 to any power_phase_a - power_phase_c will result in setting the 1000.00W
 * @param self Pointer to the HpsrvObject instance to set the power per phase for
 * @param power_phase_a Power for phase A in W scaled by 10^(-2)
 * @param power_phase_b Power for phase B in W scaled by 10^(-2)
 * @param power_phase_c Power for phase C in W scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError
HpsrvSetPowerPerPhase(HpsrvObject* self, int32_t power_phase_a, int32_t power_phase_b, int32_t power_phase_c);

/**
 * @brief Set the MU MPC Scenario 2 Energy consumed.
 * Passing e.g. 990000 to energy_consumed will result in setting the 9900.00Wh
 * @param self Pointer to the HpsrvObject instance to set the energy consumed for
 * @param energy_consumed Energy consumed value in Wh scaled by 10^(-2)
 */
EebusError HpsrvSetEnergyConsumed(HpsrvObject* self, int32_t energy_consumed);

/**
 * @brief Set the MU MPC Scenario 2 Energy produced
 * Passing e.g. 1000000 to energy_produced will result in setting the 10000.00Wh
 * @param self Pointer to the HpsrvObject instance to set the energy produced for
 * @param energy_produced Energy produced value in Wh scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetEnergyProduced(HpsrvObject* self, int32_t energy_produced);

/**
 * @brief Set the MU MPC Scenario 3 AC current per phase.
 * Passing e.g. 200 to any current_phase_a - current_phase_c will result in setting the 2.00A
 * @param self Pointer to the HpsrvObject instance to set the AC current per phase
 * @param current_phase_a AC current for phase A in mA scaled by 10^(-2)
 * @param current_phase_b AC current for phase B in mA scaled by 10^(-2)
 * @param current_phase_c AC current for phase C in mA scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError
HpsrvSetAcCurrentPerPhase(HpsrvObject* self, int32_t current_phase_a, int32_t current_phase_b, int32_t current_phase_c);

/**
 * @brief Set the MU MPC Scenario 4 AC voltage per phase
 * Passing e.g. 22000 to any voltage_phase_a - voltage_phase_ac will result in setting the 220.00V
 * @param self Pointer to the HpsrvObject instance to set the AC voltage per phase
 * @param voltage_phase_a A to Neutral AC voltage scaled by 10^(-2)
 * @param voltage_phase_b B to Neutral AC voltage scaled by 10^(-2)
 * @param voltage_phase_c C to Neutral AC voltage scaled by 10^(-2)
 * @param voltage_phase_ab B to A AC voltage scaled by 10^(-2)
 * @param voltage_phase_bc C to B AC voltage scaled by 10^(-2)
 * @param voltage_phase_ac C to A AC voltage scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetVoltagePerPhase(
    HpsrvObject* self,
    int32_t voltage_phase_a,
    int32_t voltage_phase_b,
    int32_t voltage_phase_c,
    int32_t voltage_phase_ab,
    int32_t voltage_phase_bc,
    int32_t voltage_phase_ac
);

/**
 * @brief Set the MU MPC Scenario 5 AC frequency.
 * Passing e.g. 5000 to ac_frequency will result in setting the 50.00Hz
 * @param self Pointer to the HpsrvObject instance to set the AC frequency
 * @param ac_frequency AC frequency in mHz scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetAcFrequency(HpsrvObject* self, int32_t ac_frequency);

/**
 * @brief Set the GCP MGCP power total.
 * Passing e.g. 50000 will result in setting 500.00W
 * @param self Pointer to the HpsrvObject instance
 * @param power_total Total active power in W scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetGcpMgcpPowerTotal(HpsrvObject* self, int32_t power_total);

/**
 * @brief Set the GCP MGCP grid feed-in energy (Scenario 3).
 * Passing e.g. 100000 will result in setting 1000.00Wh
 * @param self Pointer to the HpsrvObject instance
 * @param energy_feed_in Feed-in energy in Wh scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetGcpMgcpEnergyFeedIn(HpsrvObject* self, int32_t energy_feed_in);

/**
 * @brief Set the GCP MGCP grid consumed energy (Scenario 4).
 * Passing e.g. 100000 will result in setting 1000.00Wh
 * @param self Pointer to the HpsrvObject instance
 * @param energy_consumed Consumed energy in Wh scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetGcpMgcpEnergyConsumed(HpsrvObject* self, int32_t energy_consumed);

/**
 * @brief Set the GCP MGCP AC current per phase (Scenario 5).
 * Passing e.g. 200 will result in setting 2.00A
 * @param self Pointer to the HpsrvObject instance
 * @param current_phase_a Phase A RMS current in A scaled by 10^(-2)
 * @param current_phase_b Phase B RMS current in A scaled by 10^(-2)
 * @param current_phase_c Phase C RMS current in A scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetGcpMgcpCurrentPerPhase(
    HpsrvObject* self,
    int32_t current_phase_a,
    int32_t current_phase_b,
    int32_t current_phase_c
);

/**
 * @brief Set the GCP MGCP AC voltage per phase (Scenario 6).
 * Passing e.g. 22000 will result in setting 220.00V
 * @param self Pointer to the HpsrvObject instance
 * @param voltage_phase_a  A to Neutral voltage scaled by 10^(-2)
 * @param voltage_phase_b  B to Neutral voltage scaled by 10^(-2)
 * @param voltage_phase_c  C to Neutral voltage scaled by 10^(-2)
 * @param voltage_phase_ab A to B voltage scaled by 10^(-2)
 * @param voltage_phase_bc B to C voltage scaled by 10^(-2)
 * @param voltage_phase_ac C to A voltage scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetGcpMgcpVoltagePerPhase(
    HpsrvObject* self,
    int32_t voltage_phase_a,
    int32_t voltage_phase_b,
    int32_t voltage_phase_c,
    int32_t voltage_phase_ab,
    int32_t voltage_phase_bc,
    int32_t voltage_phase_ac
);

/**
 * @brief Set the GCP MGCP AC frequency (Scenario 7).
 * Passing e.g. 5000 will result in setting 50.00Hz
 * @param self Pointer to the HpsrvObject instance
 * @param frequency AC grid frequency in Hz scaled by 10^(-2)
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetGcpMgcpFrequency(HpsrvObject* self, int32_t frequency);

/**
 * @brief Set the GCP MGCP PV curtailment limit factor (Scenario 1).
 * Passing e.g. a ScaledValue with value=75 scale=0 will result in setting 75%
 * @param self Pointer to the HpsrvObject instance
 * @param value PV curtailment limit factor in % as a ScaledValue
 * @return kEebusErrorOk on success or error code on failure
 */
EebusError HpsrvSetGcpMgcpPvCurtailmentLimitFactor(HpsrvObject* self, const ScaledValue* value);

/**
 * @brief Handle command line input
 * @param self HEMS instance to handle the command for
 * @param cmd Command string to be handled
 */
void HpsrvHandleCmd(HpsrvObject* self, char* cmd);

#endif  // SRC_HPSRV_H_
