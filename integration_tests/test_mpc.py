"""
MPC integration tests — Monitoring of Power Consumption.
Equivalent of scripts/test_mpc.sh.

All measurements are set on heat_pump (MU) and verified on both
heat_pump (MU get) and HEMS (MA get).

Scenarios:
  1. Momentary active power  — power_total, power_phase_a/b/c
  2. Energy                  — energy_consumed, energy_produced
  3. Current per phase       — current_phase_a/b/c
  4. Voltage per phase       — voltage_phase_a/b/c, _ab/_bc/_ac
  5. Frequency               — frequency
"""

import pytest

from conftest import monitor_set_and_verify


@pytest.fixture(scope="module")
def nodes_mpc(nodes):
    hp, hems = nodes
    assert hems.wait_for("MA MPC remote entity connected", 30), \
        "MA MPC use case not ready after 30 s"
    return hp, hems


def _set_and_verify(hp, hems, measurements):
    monitor_set_and_verify(
        hp, hems, measurements,
        hp_prefix         = "mu_mpc",
        hems_prefix       = "ma_mpc",
        receive_marker_fn = lambda n: f"MA MPC Measurement received: {n}",
        hp_settle_fn      = lambda n: f"MU MPC measurement {n}:",
        hems_settle_fn    = lambda n: f"MA MPC measurement {n}:",
        hp_log_fn         = lambda n: hp.last_line_with(f"MU MPC measurement {n}:"),
        hems_log_fn       = lambda n: hems.last_line_with(f"MA MPC measurement {n}:"),
        hp_label          = "MU",
    )


# ---------------------------------------------------------------------------
# Scenario 1 — Momentary active power
# ---------------------------------------------------------------------------

def test_mpc_s1_momentary_active_power(nodes_mpc):
    hp, hems = nodes_mpc
    _set_and_verify(hp, hems, [
        ("power_total",   100),
        ("power_phase_a", 110),
        ("power_phase_b", 120),
        ("power_phase_c", 130),
    ])


# ---------------------------------------------------------------------------
# Scenario 2 — Energy consumed / produced
# ---------------------------------------------------------------------------

def test_mpc_s2_energy(nodes_mpc):
    hp, hems = nodes_mpc
    _set_and_verify(hp, hems, [
        ("energy_consumed", 200),
        ("energy_produced", 210),
    ])


# ---------------------------------------------------------------------------
# Scenario 3 — Current per phase
# ---------------------------------------------------------------------------

def test_mpc_s3_current_per_phase(nodes_mpc):
    hp, hems = nodes_mpc
    _set_and_verify(hp, hems, [
        ("current_phase_a", 15),
        ("current_phase_b", 16),
        ("current_phase_c", 17),
    ])


# ---------------------------------------------------------------------------
# Scenario 4 — Voltage per phase
# ---------------------------------------------------------------------------

def test_mpc_s4_voltage_per_phase(nodes_mpc):
    hp, hems = nodes_mpc
    _set_and_verify(hp, hems, [
        ("voltage_phase_a",  230),
        ("voltage_phase_b",  231),
        ("voltage_phase_c",  232),
        ("voltage_phase_ab", 400),
        ("voltage_phase_bc", 401),
        ("voltage_phase_ac", 402),
    ])


# ---------------------------------------------------------------------------
# Scenario 5 — Frequency
# ---------------------------------------------------------------------------

def test_mpc_s5_frequency(nodes_mpc):
    hp, hems = nodes_mpc
    _set_and_verify(hp, hems, [
        ("frequency", 50),
    ])
