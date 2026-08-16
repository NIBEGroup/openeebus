"""
LPP integration tests — Limitation of Power Production.

Thin wrapper around _lp_tests: only the use-case string and variant-
specific watt values differ from test_lpc.py, mirroring the pattern
used in the C implementation (cs_lpc.c / cs_lpp.c over cs_lp.c).
"""

import pytest

from _lp_tests import (
    lp_s1_failsafe_power_limit,
    lp_s1_failsafe_duration,
    lp_s2_active_power_limit,
    lp_s3_eg_writes_failsafe_limit,
    lp_s3_eg_writes_failsafe_duration,
    lp_s4_nominal_max,
    lp_s5_cs_announces_active_limit,
)

_UC  = "lpp"
_FS  = 3000   # S1 failsafe limit (W)
_EG  = 4000   # S2 EG writes power limit (W)
_NOM = 12000  # S4 nominal max (W)
_CS  = 10000  # S5 CS announces active limit (W)


@pytest.fixture(scope="module")
def nodes_lpp(nodes):
    hp, hems = nodes
    assert hems.wait_for("EG LPP Failsafe Active Power Limit received", 30), \
        "EG LPP use case not ready after 30 s"
    return hp, hems


# ---------------------------------------------------------------------------
# Scenario 1 — CS (heat_pump) announces failsafe parameters
# ---------------------------------------------------------------------------

def test_lpp_s1_failsafe_power_limit(nodes_lpp):
    lp_s1_failsafe_power_limit(*nodes_lpp, _UC, _FS)

def test_lpp_s1_failsafe_duration(nodes_lpp):
    lp_s1_failsafe_duration(*nodes_lpp, _UC)


# ---------------------------------------------------------------------------
# Scenario 2 — EG (HEMS) writes active power limit
# ---------------------------------------------------------------------------

def test_lpp_s2_active_power_limit(nodes_lpp):
    lp_s2_active_power_limit(*nodes_lpp, _UC, _EG)


# ---------------------------------------------------------------------------
# Scenario 3 — EG (HEMS) writes failsafe parameters to CS
# ---------------------------------------------------------------------------

def test_lpp_s3_eg_writes_failsafe_limit(nodes_lpp):
    lp_s3_eg_writes_failsafe_limit(*nodes_lpp, _UC)

def test_lpp_s3_eg_writes_failsafe_duration(nodes_lpp):
    lp_s3_eg_writes_failsafe_duration(*nodes_lpp, _UC)


# ---------------------------------------------------------------------------
# Scenario 4 — CS (heat_pump) announces production nominal max
# ---------------------------------------------------------------------------

@pytest.mark.xfail(reason="cs_lpp nominal_max CLI command not yet implemented (N/A in bash script)")
def test_lpp_s4_nominal_max(nodes_lpp):
    lp_s4_nominal_max(*nodes_lpp, _UC, _NOM)


# ---------------------------------------------------------------------------
# Scenario 5 — CS (heat_pump) announces its own active power limit
# ---------------------------------------------------------------------------

def test_lpp_s5_cs_announces_active_limit(nodes_lpp):
    lp_s5_cs_announces_active_limit(*nodes_lpp, _UC, _CS)
