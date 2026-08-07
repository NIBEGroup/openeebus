"""
LPP integration tests — Limitation of Power Production.
Equivalent of scripts/test_lpp.sh.

Scenarios:
  1. CS (heat_pump) announces failsafe parameters → EG (HEMS) reads them
  2. EG (HEMS) writes active power limit → CS (heat_pump) receives it
  3. EG (HEMS) writes failsafe parameters → CS (heat_pump) stores them
  4. CS (heat_pump) announces production nominal max → EG (HEMS) reads it
  5. CS (heat_pump) announces its own active power limit → EG (HEMS) reads it
"""

import time

import pytest

from conftest import parse_field, parse_pt

_PROPAGATION = 3.0
_SETTLE      = 1.0


@pytest.fixture
def nodes_lpp(nodes):
    hp, hems = nodes
    assert hems.wait_for("EG LPP Failsafe Active Power Limit received", 30), \
        "EG LPP use case not ready after 30 s"
    return hp, hems


# ---------------------------------------------------------------------------
# Scenario 1 — CS (heat_pump) announces failsafe parameters
# ---------------------------------------------------------------------------

def test_lpp_s1_failsafe_power_limit(nodes_lpp):
    hp, hems = nodes_lpp
    hp.send("cs_lpp set failsafe_limit 3000 true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get failsafe_limit")
    hems.send("eg_lpp get failsafe_limit")
    time.sleep(_SETTLE)

    cs = parse_field(hp.last_line_with("CS LPP Failsafe Active Power Limit"), "value")
    eg = parse_field(hems.last_line_with("EG LPP Failsafe Active Power Limit"), "value")
    print(f"  failsafe_limit (W): CS={cs}  EG={eg}")
    assert cs == "3000", f"CS failsafe limit: expected 3000, got {cs!r}"
    assert eg == "3000", f"EG failsafe limit: expected 3000, got {eg!r}"


def test_lpp_s1_failsafe_duration(nodes_lpp):
    hp, hems = nodes_lpp
    hp.send("cs_lpp set failsafe_duration PT2H true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get failsafe_duration")
    hems.send("eg_lpp get failsafe_duration")
    time.sleep(_SETTLE)

    cs = parse_pt(hp.last_line_with("CS LPP Failsafe Duration Minimum"))
    eg = parse_pt(hems.last_line_with("EG LPP Failsafe Duration Minimum"))
    print(f"  failsafe_duration: CS={cs}  EG={eg}")
    assert cs == "PT2H", f"CS failsafe duration: expected PT2H, got {cs!r}"
    assert eg == "PT2H", f"EG failsafe duration: expected PT2H, got {eg!r}"


# ---------------------------------------------------------------------------
# Scenario 2 — EG (HEMS) writes active power limit
# ---------------------------------------------------------------------------

def test_lpp_s2_active_power_limit(nodes_lpp):
    hp, hems = nodes_lpp
    hems.send("eg_lpp set power_limit 4000 PT0S true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get power_limit")
    hems.send("eg_lpp get power_limit")
    time.sleep(_SETTLE)

    cs_val = parse_field(hp.last_line_with("CS LPP Active Power Limit", exclude="Failsafe"), "value")
    eg_val = parse_field(hems.last_line_with("EG LPP Active Power Limit", exclude="Failsafe"), "value")
    print(f"  active_limit (W): CS={cs_val}  EG={eg_val}")
    assert cs_val == "4000", f"CS active limit value: expected 4000, got {cs_val!r}"
    assert eg_val == "4000", f"EG active limit value: expected 4000, got {eg_val!r}"


def test_lpp_s2_active_power_limit_duration(nodes_lpp):
    hp, hems = nodes_lpp
    hems.send("eg_lpp set power_limit 4000 PT0S true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get power_limit")
    hems.send("eg_lpp get power_limit")
    time.sleep(_SETTLE)

    cs_dur = parse_field(hp.last_line_with("CS LPP Active Power Limit", exclude="Failsafe"), "duration")
    eg_dur = parse_field(hems.last_line_with("EG LPP Active Power Limit", exclude="Failsafe"), "duration")
    print(f"  active_limit duration: CS={cs_dur}  EG={eg_dur}")
    assert cs_dur == "PT0S", f"CS duration: expected PT0S, got {cs_dur!r}"
    assert eg_dur == "PT0S", f"EG duration: expected PT0S, got {eg_dur!r}"


def test_lpp_s2_active_power_limit_is_active(nodes_lpp):
    hp, hems = nodes_lpp
    hems.send("eg_lpp set power_limit 4000 PT0S true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get power_limit")
    hems.send("eg_lpp get power_limit")
    time.sleep(_SETTLE)

    cs_act = parse_field(hp.last_line_with("CS LPP Active Power Limit", exclude="Failsafe"), "is active")
    eg_act = parse_field(hems.last_line_with("EG LPP Active Power Limit", exclude="Failsafe"), "active")
    print(f"  is_active: CS={cs_act}  EG={eg_act}")
    assert cs_act == "true", f"CS is active: expected true, got {cs_act!r}"
    assert eg_act == "true", f"EG active: expected true, got {eg_act!r}"


# ---------------------------------------------------------------------------
# Scenario 3 — EG (HEMS) writes failsafe parameters to CS
# ---------------------------------------------------------------------------

def test_lpp_s3_eg_writes_failsafe_limit(nodes_lpp):
    hp, hems = nodes_lpp
    hems.send("eg_lpp set failsafe_limit 600")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get failsafe_limit")
    hems.send("eg_lpp get failsafe_limit")
    time.sleep(_SETTLE)

    cs = parse_field(hp.last_line_with("CS LPP Failsafe Active Power Limit"), "value")
    eg = parse_field(hems.last_line_with("EG LPP Failsafe Active Power Limit"), "value")
    print(f"  failsafe_limit (W): CS={cs}  EG={eg}")
    assert cs == "600", f"CS failsafe limit: expected 600, got {cs!r}"
    assert eg == "600", f"EG failsafe limit: expected 600, got {eg!r}"


def test_lpp_s3_eg_writes_failsafe_duration(nodes_lpp):
    hp, hems = nodes_lpp
    hems.send("eg_lpp set failsafe_duration PT3H")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get failsafe_duration")
    hems.send("eg_lpp get failsafe_duration")
    time.sleep(_SETTLE)

    cs = parse_pt(hp.last_line_with("CS LPP Failsafe Duration Minimum"))
    eg = parse_pt(hems.last_line_with("EG LPP Failsafe Duration Minimum"))
    print(f"  failsafe_duration: CS={cs}  EG={eg}")
    assert cs == "PT3H", f"CS failsafe duration: expected PT3H, got {cs!r}"
    assert eg == "PT3H", f"EG failsafe duration: expected PT3H, got {eg!r}"


# ---------------------------------------------------------------------------
# Scenario 4 — CS (heat_pump) announces production nominal max
# ---------------------------------------------------------------------------

@pytest.mark.xfail(reason="cs_lpp nominal_max CLI command not yet implemented (N/A in bash script)")
def test_lpp_s4_nominal_max(nodes_lpp):
    hp, hems = nodes_lpp
    hp.send("cs_lpp set nominal_max 12000")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get nominal_max")
    hems.send("eg_lpp get power_nominal_max")
    time.sleep(_SETTLE)

    cs = parse_field(hp.last_line_with("CS LPP Nominal Max"), "value")
    eg = parse_field(hems.last_line_with("EG LPP Power Nominal Max"), "value")
    print(f"  nominal_max (W): CS={cs}  EG={eg}")
    assert cs == "12000", f"CS nominal max: expected 12000, got {cs!r}"
    assert eg == "12000", f"EG nominal max: expected 12000, got {eg!r}"


# ---------------------------------------------------------------------------
# Scenario 5 — CS (heat_pump) announces its own active power limit
# ---------------------------------------------------------------------------

def test_lpp_s5_cs_announces_active_limit(nodes_lpp):
    hp, hems = nodes_lpp
    hp.send("cs_lpp set power_limit 10000 true true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get power_limit")
    hems.send("eg_lpp get power_limit")
    time.sleep(_SETTLE)

    cs_val = parse_field(hp.last_line_with("CS LPP Active Power Limit", exclude="Failsafe"), "value")
    eg_val = parse_field(hems.last_line_with("EG LPP Active Power Limit", exclude="Failsafe"), "value")
    print(f"  active_limit (W): CS={cs_val}  EG={eg_val}")
    assert cs_val == "10000", f"CS active limit: expected 10000, got {cs_val!r}"
    assert eg_val == "10000", f"EG active limit: expected 10000, got {eg_val!r}"


def test_lpp_s5_cs_announces_active_limit_is_active(nodes_lpp):
    hp, hems = nodes_lpp
    hp.send("cs_lpp set power_limit 10000 true true")
    time.sleep(_PROPAGATION)
    hp.send("cs_lpp get power_limit")
    hems.send("eg_lpp get power_limit")
    time.sleep(_SETTLE)

    cs_act = parse_field(hp.last_line_with("CS LPP Active Power Limit", exclude="Failsafe"), "is active")
    eg_act = parse_field(hems.last_line_with("EG LPP Active Power Limit", exclude="Failsafe"), "active")
    print(f"  is_active: CS={cs_act}  EG={eg_act}")
    assert cs_act == "true", f"CS is active: expected true, got {cs_act!r}"
    assert eg_act == "true", f"EG active: expected true, got {eg_act!r}"
