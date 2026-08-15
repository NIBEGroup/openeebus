"""
MGCP integration tests — Monitoring of Grid Connection Point.
Equivalent of scripts/test_mgcp.sh.

All measurements are set on heat_pump (GCP) and verified on both
heat_pump (GCP get) and HEMS (MA get).

Scenarios:
  1. PV curtailment limit factor
  2. Power total
  3 & 4. Energy (feed-in and consumed)
  5. Current per phase
  6. Voltage per phase
  7. Frequency
"""

import time

import pytest

from conftest import parse_field

_PROPAGATION = 3.0
_SETTLE      = 2.0


@pytest.fixture(scope="module")
def nodes_mgcp(nodes):
    hp, hems = nodes
    time.sleep(5.0)  # let use-case subscriptions settle
    return hp, hems


def _gcp_line(hp, name):
    if name == "pv_curtailment_limit_factor":
        return hp.last_line_with("GCP MGCP pv_curtailment_limit_factor:")
    return hp.last_line_with(f"GCP MGCP measurement {name}:")


def _ma_line(hems, name):
    if name == "pv_curtailment_limit_factor":
        return hems.last_line_with("MA MGCP pv_curtailment_limit_factor:")
    return hems.last_line_with(f"MA MGCP measurement {name}:")


def _ma_receive_marker(name):
    if name == "pv_curtailment_limit_factor":
        return "MA MGCP PV curtailment limit factor received:"
    return f"MA MGCP Measurement received: {name}"


def _gcp_settle_marker(name):
    if name == "pv_curtailment_limit_factor":
        return "GCP MGCP pv_curtailment_limit_factor:"
    return f"GCP MGCP measurement {name}:"


def _ma_settle_marker(name):
    if name == "pv_curtailment_limit_factor":
        return "MA MGCP pv_curtailment_limit_factor:"
    return f"MA MGCP measurement {name}:"


def _set_and_verify(hp, hems, measurements):
    for name, value in measurements:
        pos = hems.line_count()
        hp.send(f"gcp_mgcp set {name} {value}")
        hems.wait_for_new(_ma_receive_marker(name), pos, _PROPAGATION)

    pos_hp = hp.line_count()
    for name, _ in measurements:
        hp.send(f"gcp_mgcp get {name}")
        time.sleep(0.2)
    pos_hems = hems.line_count()
    for name, _ in measurements:
        hems.send(f"ma_mgcp get {name}")
        time.sleep(0.2)

    last_name = measurements[-1][0]
    hp.wait_for_new(_gcp_settle_marker(last_name), pos_hp, _SETTLE)
    hems.wait_for_new(_ma_settle_marker(last_name), pos_hems, _SETTLE)

    for name, expected in measurements:
        gcp_val = parse_field(_gcp_line(hp, name),   "value")
        ma_val  = parse_field(_ma_line(hems, name),  "value")
        print(f"  {name}: GCP={gcp_val}  MA={ma_val}")
        assert gcp_val == str(expected), \
            f"GCP {name}: expected {expected}, got {gcp_val!r}"
        assert ma_val == str(expected), \
            f"MA {name}: expected {expected}, got {ma_val!r}"


# ---------------------------------------------------------------------------
# Scenario 1 — PV curtailment limit factor
# ---------------------------------------------------------------------------

def test_mgcp_s1_pv_curtailment_limit_factor(nodes_mgcp):
    hp, hems = nodes_mgcp
    _set_and_verify(hp, hems, [("pv_curtailment_limit_factor", 75)])


# ---------------------------------------------------------------------------
# Scenario 2 — Power total
# ---------------------------------------------------------------------------

def test_mgcp_s2_power_total(nodes_mgcp):
    hp, hems = nodes_mgcp
    _set_and_verify(hp, hems, [("power_total", 500)])


# ---------------------------------------------------------------------------
# Scenarios 3 & 4 — Energy feed-in and consumed
# ---------------------------------------------------------------------------

def test_mgcp_s3_s4_energy(nodes_mgcp):
    hp, hems = nodes_mgcp
    _set_and_verify(hp, hems, [
        ("energy_feed_in",  600),
        ("energy_consumed", 700),
    ])


# ---------------------------------------------------------------------------
# Scenario 5 — Current per phase
# ---------------------------------------------------------------------------

def test_mgcp_s5_current_per_phase(nodes_mgcp):
    hp, hems = nodes_mgcp
    _set_and_verify(hp, hems, [
        ("current_phase_a", 25),
        ("current_phase_b", 26),
        ("current_phase_c", 27),
    ])


# ---------------------------------------------------------------------------
# Scenario 6 — Voltage per phase
# ---------------------------------------------------------------------------

def test_mgcp_s6_voltage_per_phase(nodes_mgcp):
    hp, hems = nodes_mgcp
    _set_and_verify(hp, hems, [
        ("voltage_phase_a",  240),
        ("voltage_phase_b",  241),
        ("voltage_phase_c",  242),
        ("voltage_phase_ab", 410),
        ("voltage_phase_bc", 411),
        ("voltage_phase_ac", 412),
    ])


# ---------------------------------------------------------------------------
# Scenario 7 — Frequency
# ---------------------------------------------------------------------------

def test_mgcp_s7_frequency(nodes_mgcp):
    hp, hems = nodes_mgcp
    _set_and_verify(hp, hems, [("frequency", 60)])
