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
 * @brief EEBUS CLI implementation
 *
 * Usage example:
 *
 * @code{.c}
 * EebusCli* const cli = EebusCliCreate();
 * EebusCliSetEgLpc(cli, eg_lpc_use_case);
 * // Consider EG LPC connection is established and remote entity address is already known
 * EebusCliSetRemoteEntityAddress(cli, remote_entity_address);
 * // Set power limit to 3500.5W for 12 hours, 7 seconds and activate it
 * EebusCliHandleCmd(cli, "eg_lpc set power_limit 3500.5 PT12H07S true");
 * @endcode
 *
 * Commands examples
 * CS LPC:
 * cs_lpc set power_limit 3500.5 false true
 * cs_lpc get power_limit
 * cs_lpc set failsafe_limit 3500.5 true
 * cs_lpc get failsafe_limit
 * cs_lpc set failsafe_duration PT3H02M3S true
 * cs_lpc get failsafe_duration
 * cs_lpc start heartbeat
 * cs_lpc stop heartbeat
 *
 * CS LPP:
 * cs_lpp set power_limit 3500.5 false true
 * cs_lpp get power_limit
 * cs_lpp set failsafe_limit 3500.5 true
 * cs_lpp get failsafe_limit
 * cs_lpp set failsafe_duration PT3H02M3S true
 * cs_lpp get failsafe_duration
 * cs_lpp start heartbeat
 * cs_lpp stop heartbeat
 *
 * EG LPC:
 * eg_lpc set power_limit 3500.5 PT12H true
 * eg_lpc get power_limit
 * eg_lpc set failsafe_limit 3500.5
 * eg_lpc get failsafe_limit
 * eg_lpc set failsafe_duration PT3H02M3S
 * eg_lpc get failsafe_duration
 * eg_lpc start heartbeat
 * eg_lpc stop heartbeat
 *
 * EG LPP:
 * eg_lpp set power_limit 1500.1 PT3H true
 * eg_lpp get power_limit
 * eg_lpp set failsafe_limit 3500.5
 * eg_lpp get failsafe_limit
 * eg_lpp set failsafe_duration PT1H01M5S
 * eg_lpp start heartbeat
 * eg_lpp stop heartbeat
 *
 * MU MPC:
 * // MU MPC commands format:
 * // mu_mpc get <measurement_name>
 * // mu_mpc set <measurement_name> <value>
 * // The list of available measurement names:
 * // power_total
 * // power_phase_a
 * // power_phase_b
 * // power_phase_c
 * // energy_consumed
 * // energy_produced
 * // current_phase_a
 * // current_phase_b
 * // current_phase_c
 * // voltage_phase_a
 * // voltage_phase_b
 * // voltage_phase_c
 * // voltage_phase_ab
 * // voltage_phase_bc
 * // voltage_phase_ac
 * // frequency
 * // For power_total use e.g.:
 * mu_mpc get power_total
 * mu_mpc set power_total 1200.0
 *
 * MA MPC:
 * // MA MPC commands format:
 * // ma_mpc get <measurement_name>
 * // For power_total use e.g.:
 * ma_mpc get power_total
 *
 * Compressor OHPCF
 * # Announce a process with earliest start time in 1 hour, latest end time in 2 hours,
 * # minimum active duration of 3 minutes, max power consumption of 2000W,
 * # is stoppable but not pausable
 * compressor_ohpcf announce 2000 PT1H PT2H PT3M true false
 * # Report scheduled in 3 minutes from now
 * compressor_ohpcf report_state scheduled PT3M
 * compressor_ohpcf report_state running
 * compressor_ohpcf report_state paused
 * compressor_ohpcf report_state completed
 * compressor_ohpcf report_state stopped
 * compressor_ohpcf clear_process
 * compressor_ohpcf get state
 * compressor_ohpcf set power_total 1500.5
 *
 * CEM OHPCF
 * cem_ohpcf get announced
 * cem_ohpcf get state
 * cem_ohpcf schedule PT10M
 * cem_ohpcf write_command stop
 * cem_ohpcf write_command pause
 * cem_ohpcf write_command resume
 *
 * GCP MGCP:
 * // GCP MGCP commands format:
 * // gcp_mgcp get <measurement_name>
 * // gcp_mgcp set <measurement_name> <value>
 * // The list of available measurement names:
 * // power_total
 * // energy_feed_in
 * // energy_consumed
 * // current_phase_a
 * // current_phase_b
 * // current_phase_c
 * // voltage_phase_a
 * // voltage_phase_b
 * // voltage_phase_c
 * // voltage_phase_ab
 * // voltage_phase_bc
 * // voltage_phase_ac
 * // frequency
 * // For power_total use e.g.:
 * gcp_mgcp get power_total
 * gcp_mgcp set power_total 1200.0
 *
 * MA MGCP:
 * // MA MGCP commands format:
 * // ma_mgcp get <measurement_name>
 * // The list of available measurement names:
 * // power_total
 * // energy_feed_in
 * // energy_consumed
 * // current_phase_a
 * // current_phase_b
 * // current_phase_c
 * // voltage_phase_a
 * // voltage_phase_b
 * // voltage_phase_c
 * // voltage_phase_ab
 * // voltage_phase_bc
 * // voltage_phase_ac
 * // frequency
 * // For power_total use e.g.:
 * ma_mgcp get power_total
 *
 */

#ifndef SRC_CLI_EEBUS_CLI_H_
#define SRC_CLI_EEBUS_CLI_H_

#include <stddef.h>

#include "src/cli/eebus_cli_interface.h"
#include "src/common/eebus_malloc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Dynamically allocate and construct the EEBUS CLI handler instance
 * @return Pointer to the EEBUS CLI handler instance or NULL on failure
 */
EebusCliObject* EebusCliCreate(void);

/**
 * @brief Deallocate the EEBUS CLI handler instance
 * @param eebus_cli Pointer to the EEBUS CLI handler instance to be deallocated
 */
static inline void EebusCliDelete(EebusCliObject* eebus_cli) {
  if (eebus_cli != NULL) {
    EEBUS_CLI_DESTRUCT(eebus_cli);
    EEBUS_FREE(eebus_cli);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_CLI_EEBUS_CLI_H_
