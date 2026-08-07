"""
LPC integration tests — Limitation of Power Consumption.
Equivalent of scripts/test_lpc.sh.

Scenarios:
  1. CS (heat_pump) announces failsafe parameters → EG (HEMS) reads them
  2. EG (HEMS) writes active power limit → CS (heat_pump) receives it
  3. EG (HEMS) writes failsafe parameters → CS (heat_pump) stores them
  4. CS (heat_pump) announces consumption nominal max → EG (HEMS) reads it
  5. CS (heat_pump) announces its own active power limit → EG (HEMS) reads it
"""

import time

import pytest

from conftest import parse_field, parse_pt

_PROPAGATION = 3.0
_SETTLE      = 1.0


@pytest.fixture
def nodes_lpc(nodes):
    """Extends the base nodes fixture with LPC-specific readiness wait."""
    hp, hems = nodes
    assert hems.wait_for("EG LPC Failsafe Active Power Limit received", 30), \
        "EG LPC use case not ready after 30 s"
    return hp, hems


# ---------------------------------------------------------------------------
# Scenario 1 — CS (heat_pump) announces failsafe parameters
# ---------------------------------------------------------------------------

def test_lpc_s1_failsafe_power_limit(nodes_lpc):
    hp, hems = nodes_lpc
    hp.send("cs_lpc set failsafe_limit 5000 true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get failsafe_limit")
    hems.send("eg_lpc get failsafe_limit")
    time.sleep(_SETTLE)

    cs = parse_field(hp.last_line_with("CS LPC Failsafe Active Power Limit"), "value")
    eg = parse_field(hems.last_line_with("EG LPC Failsafe Active Power Limit"), "value")
    print(f"  failsafe_limit (W): CS={cs}  EG={eg}")
    assert cs == "5000", f"CS failsafe limit: expected 5000, got {cs!r}"
    assert eg == "5000", f"EG failsafe limit: expected 5000, got {eg!r}"


def test_lpc_s1_failsafe_duration(nodes_lpc):
    hp, hems = nodes_lpc
    hp.send("cs_lpc set failsafe_duration PT2H true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get failsafe_duration")
    hems.send("eg_lpc get failsafe_duration")
    time.sleep(_SETTLE)

    cs = parse_pt(hp.last_line_with("CS LPC Failsafe Duration Minimum"))
    eg = parse_pt(hems.last_line_with("EG LPC Failsafe Duration Minimum"))
    print(f"  failsafe_duration: CS={cs}  EG={eg}")
    assert cs == "PT2H", f"CS failsafe duration: expected PT2H, got {cs!r}"
    assert eg == "PT2H", f"EG failsafe duration: expected PT2H, got {eg!r}"


# ---------------------------------------------------------------------------
# Scenario 2 — EG (HEMS) writes active power limit
# ---------------------------------------------------------------------------

def test_lpc_s2_active_power_limit(nodes_lpc):
    hp, hems = nodes_lpc
    hems.send("eg_lpc set power_limit 7000 PT0S true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get power_limit")
    hems.send("eg_lpc get power_limit")
    time.sleep(_SETTLE)

    cs_val = parse_field(hp.last_line_with("CS LPC Active Power Limit", exclude="Failsafe"), "value")
    eg_val = parse_field(hems.last_line_with("EG LPC Active Power Limit", exclude="Failsafe"), "value")
    print(f"  active_limit (W): CS={cs_val}  EG={eg_val}")
    assert cs_val == "7000", f"CS active limit value: expected 7000, got {cs_val!r}"
    assert eg_val == "7000", f"EG active limit value: expected 7000, got {eg_val!r}"


def test_lpc_s2_active_power_limit_duration(nodes_lpc):
    hp, hems = nodes_lpc
    hems.send("eg_lpc set power_limit 7000 PT0S true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get power_limit")
    hems.send("eg_lpc get power_limit")
    time.sleep(_SETTLE)

    cs_dur = parse_field(hp.last_line_with("CS LPC Active Power Limit", exclude="Failsafe"), "duration")
    eg_dur = parse_field(hems.last_line_with("EG LPC Active Power Limit", exclude="Failsafe"), "duration")
    print(f"  active_limit duration: CS={cs_dur}  EG={eg_dur}")
    assert cs_dur == "PT0S", f"CS active limit duration: expected PT0S, got {cs_dur!r}"
    assert eg_dur == "PT0S", f"EG active limit duration: expected PT0S, got {eg_dur!r}"


def test_lpc_s2_active_power_limit_is_active(nodes_lpc):
    hp, hems = nodes_lpc
    hems.send("eg_lpc set power_limit 7000 PT0S true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get power_limit")
    hems.send("eg_lpc get power_limit")
    time.sleep(_SETTLE)

    cs_act = parse_field(hp.last_line_with("CS LPC Active Power Limit", exclude="Failsafe"), "is active")
    eg_act = parse_field(hems.last_line_with("EG LPC Active Power Limit", exclude="Failsafe"), "active")
    print(f"  is_active: CS={cs_act}  EG={eg_act}")
    assert cs_act == "true", f"CS is active: expected true, got {cs_act!r}"
    assert eg_act == "true", f"EG active: expected true, got {eg_act!r}"


# ---------------------------------------------------------------------------
# Scenario 3 — EG (HEMS) writes failsafe parameters to CS
# ---------------------------------------------------------------------------

def test_lpc_s3_eg_writes_failsafe_limit(nodes_lpc):
    hp, hems = nodes_lpc
    hems.send("eg_lpc set failsafe_limit 600")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get failsafe_limit")
    hems.send("eg_lpc get failsafe_limit")
    time.sleep(_SETTLE)

    cs = parse_field(hp.last_line_with("CS LPC Failsafe Active Power Limit"), "value")
    eg = parse_field(hems.last_line_with("EG LPC Failsafe Active Power Limit"), "value")
    print(f"  failsafe_limit (W): CS={cs}  EG={eg}")
    assert cs == "600", f"CS failsafe limit: expected 600, got {cs!r}"
    assert eg == "600", f"EG failsafe limit: expected 600, got {eg!r}"


def test_lpc_s3_eg_writes_failsafe_duration(nodes_lpc):
    hp, hems = nodes_lpc
    hems.send("eg_lpc set failsafe_duration PT3H")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get failsafe_duration")
    hems.send("eg_lpc get failsafe_duration")
    time.sleep(_SETTLE)

    cs = parse_pt(hp.last_line_with("CS LPC Failsafe Duration Minimum"))
    eg = parse_pt(hems.last_line_with("EG LPC Failsafe Duration Minimum"))
    print(f"  failsafe_duration: CS={cs}  EG={eg}")
    assert cs == "PT3H", f"CS failsafe duration: expected PT3H, got {cs!r}"
    assert eg == "PT3H", f"EG failsafe duration: expected PT3H, got {eg!r}"


# ---------------------------------------------------------------------------
# Scenario 4 — CS (heat_pump) announces consumption nominal max
# ---------------------------------------------------------------------------

@pytest.mark.xfail(reason="cs_lpc nominal_max CLI command not yet implemented (N/A in bash script)")
def test_lpc_s4_nominal_max(nodes_lpc):
    hp, hems = nodes_lpc
    hp.send("cs_lpc set nominal_max 11000")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get nominal_max")
    hems.send("eg_lpc get power_nominal_max")
    time.sleep(_SETTLE)

    cs = parse_field(hp.last_line_with("CS LPC Nominal Max"), "value")
    eg = parse_field(hems.last_line_with("EG LPC Power Nominal Max"), "value")
    print(f"  nominal_max (W): CS={cs}  EG={eg}")
    assert cs == "11000", f"CS nominal max: expected 11000, got {cs!r}"
    assert eg == "11000", f"EG nominal max: expected 11000, got {eg!r}"


# ---------------------------------------------------------------------------
# Scenario 5 — CS (heat_pump) announces its own active power limit
# ---------------------------------------------------------------------------

def test_lpc_s5_cs_announces_active_limit(nodes_lpc):
    hp, hems = nodes_lpc
    hp.send("cs_lpc set power_limit 9000 true true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get power_limit")
    hems.send("eg_lpc get power_limit")
    time.sleep(_SETTLE)

    cs_val = parse_field(hp.last_line_with("CS LPC Active Power Limit", exclude="Failsafe"), "value")
    eg_val = parse_field(hems.last_line_with("EG LPC Active Power Limit", exclude="Failsafe"), "value")
    print(f"  active_limit (W): CS={cs_val}  EG={eg_val}")
    assert cs_val == "9000", f"CS active limit: expected 9000, got {cs_val!r}"
    assert eg_val == "9000", f"EG active limit: expected 9000, got {eg_val!r}"


def test_lpc_s5_cs_announces_active_limit_is_active(nodes_lpc):
    hp, hems = nodes_lpc
    hp.send("cs_lpc set power_limit 9000 true true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpc get power_limit")
    hems.send("eg_lpc get power_limit")
    time.sleep(_SETTLE)

    cs_act = parse_field(hp.last_line_with("CS LPC Active Power Limit", exclude="Failsafe"), "is active")
    eg_act = parse_field(hems.last_line_with("EG LPC Active Power Limit", exclude="Failsafe"), "active")
    print(f"  is_active: CS={cs_act}  EG={eg_act}")
    assert cs_act == "true", f"CS is active: expected true, got {cs_act!r}"
    assert eg_act == "true", f"EG active: expected true, got {eg_act!r}"
