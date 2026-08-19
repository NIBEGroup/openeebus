"""
LPC integration tests — Limitation of Power Consumption.

Thin wrapper around _lp_tests: only the use-case string and variant-
specific watt values differ from test_lpp.py, mirroring the pattern
used in the C implementation (cs_lpc.c / cs_lpp.c over cs_lp.c).
"""

import pytest

from conftest import await_uc_ready
from _lp_tests import (
    lp_s1_failsafe_power_limit,
    lp_s1_failsafe_duration,
    lp_s2_active_power_limit,
    lp_s3_eg_writes_failsafe_limit,
    lp_s3_eg_writes_failsafe_duration,
    lp_s4_nominal_max,
    lp_s5_cs_announces_active_limit,
)

_UC  = "lpc"
_FS  = 5000   # S1 failsafe limit (W)
_EG  = 7000   # S2 EG writes power limit (W)
_NOM = 11000  # S4 nominal max (W)
_CS  = 9000   # S5 CS announces active limit (W)


@pytest.fixture(scope="module")
def nodes_lpc(nodes):
    return await_uc_ready(nodes, "EG LPC Failsafe Active Power Limit received")


# ---------------------------------------------------------------------------
# Scenario 1 — CS (heat_pump) announces failsafe parameters
# ---------------------------------------------------------------------------

def test_lpc_s1_failsafe_power_limit(nodes_lpc):
    lp_s1_failsafe_power_limit(*nodes_lpc, _UC, _FS)

def test_lpc_s1_failsafe_duration(nodes_lpc):
    lp_s1_failsafe_duration(*nodes_lpc, _UC)


# ---------------------------------------------------------------------------
# Scenario 2 — EG (HEMS) writes active power limit
# ---------------------------------------------------------------------------

def test_lpc_s2_active_power_limit(nodes_lpc):
    lp_s2_active_power_limit(*nodes_lpc, _UC, _EG)


# ---------------------------------------------------------------------------
# Scenario 3 — EG (HEMS) writes failsafe parameters to CS
# ---------------------------------------------------------------------------

def test_lpc_s3_eg_writes_failsafe_limit(nodes_lpc):
    lp_s3_eg_writes_failsafe_limit(*nodes_lpc, _UC)

def test_lpc_s3_eg_writes_failsafe_duration(nodes_lpc):
    lp_s3_eg_writes_failsafe_duration(*nodes_lpc, _UC)


# ---------------------------------------------------------------------------
# Scenario 4 — CS (heat_pump) announces consumption nominal max
# ---------------------------------------------------------------------------

def test_lpc_s4_nominal_max(nodes_lpc):
    lp_s4_nominal_max(*nodes_lpc, _UC, _NOM)


# ---------------------------------------------------------------------------
# Scenario 5 — CS (heat_pump) announces its own active power limit
# ---------------------------------------------------------------------------

def test_lpc_s5_cs_announces_active_limit(nodes_lpc):
    lp_s5_cs_announces_active_limit(*nodes_lpc, _UC, _CS)
