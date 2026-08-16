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

import pytest

from conftest import monitor_set_and_verify


@pytest.fixture(scope="module")
def nodes_mgcp(nodes):
    hp, hems = nodes
    assert hems.wait_for("MA MGCP remote entity connected", 30), \
        "MA MGCP use case not ready after 30 s"
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
    monitor_set_and_verify(
        hp, hems, measurements,
        hp_prefix         = "gcp_mgcp",
        hems_prefix       = "ma_mgcp",
        receive_marker_fn = _ma_receive_marker,
        hp_settle_fn      = _gcp_settle_marker,
        hems_settle_fn    = _ma_settle_marker,
        hp_log_fn         = lambda n: _gcp_line(hp, n),
        hems_log_fn       = lambda n: _ma_line(hems, n),
        hp_label          = "GCP",
    )


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
