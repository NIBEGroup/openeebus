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

import time

import pytest

from conftest import parse_field

_PROPAGATION = 3.0
_SETTLE      = 2.0


@pytest.fixture(scope="module")
def nodes_mpc(nodes):
    hp, hems = nodes
    assert hems.wait_for("MA MPC remote entity connected", 30), \
        "MA MPC use case not ready after 30 s"
    return hp, hems


def _set_and_verify(hp, hems, measurements):
    """Set a list of (name, value) measurements on MU, read back from both sides."""
    for name, value in measurements:
        pos = hems.line_count()
        hp.send(f"mu_mpc set {name} {value}")
        hems.wait_for_new(f"MA MPC Measurement received: {name}", pos, _PROPAGATION)

    pos_hp = hp.line_count()
    for name, _ in measurements:
        hp.send(f"mu_mpc get {name}")
        time.sleep(0.2)
    pos_hems = hems.line_count()
    for name, _ in measurements:
        hems.send(f"ma_mpc get {name}")
        time.sleep(0.2)

    last_name = measurements[-1][0]
    hp.wait_for_new(f"MU MPC measurement {last_name}:", pos_hp, _SETTLE)
    hems.wait_for_new(f"MA MPC measurement {last_name}:", pos_hems, _SETTLE)

    for name, expected in measurements:
        mu_line = hp.last_line_with(f"MU MPC measurement {name}:")
        ma_line = hems.last_line_with(f"MA MPC measurement {name}:")
        mu_val  = parse_field(mu_line, "value")
        ma_val  = parse_field(ma_line, "value")
        print(f"  {name}: MU={mu_val}  MA={ma_val}")
        assert mu_val == str(expected), \
            f"MU {name}: expected {expected}, got {mu_val!r}"
        assert ma_val == str(expected), \
            f"MA {name}: expected {expected}, got {ma_val!r}"


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
