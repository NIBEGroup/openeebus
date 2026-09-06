"""
HEMS + HP + EV charger integration tests — three-node MPC and LPC scenarios.

HP and EV both connect to HEMS as MPC senders (MU) and LPC controllable
systems (CS). HEMS acts as MPC monitor (MA) and LPC energy guard (EG) for
both. HP is connected first so it is guaranteed to be entity[0] in the MA
MPC remote list.

Scenarios:
  1. HP sets MPC measurements — HEMS reads from HP entity
  2. EV sets MPC measurements — HEMS reads from EV entity
  3. HEMS sends LPC power limit to HP (EV unaffected)
  4. EV disconnects — HP and HEMS LPC continue unaffected
"""

import pytest

from conftest import (
    NodeProcess,
    HP_BINARY, HP_PORT, HP_REMOTE_SKI, HP_CERT, HP_KEY,
    HEMS_BINARY, HEMS_PORT, HEMS_REMOTE_SKI, HEMS_CERT, HEMS_KEY,
    EV_BINARY, EV_PORT, EV_SKI, EV_CERT, EV_KEY,
    SHIP_CONNECTED_MARKER, NODES_CONNECT_TIMEOUT, UC_READY_TIMEOUT,
    DEFAULT_PROPAGATION, DEFAULT_SETTLE,
    parse_field, require_binaries,
)
from _ev_tests import wait_for_entity_addresses

_MPC_READY_MARKER = "MA MPC remote entity connected"
_LPC_READY_MARKER = "EG LPC Failsafe Active Power Limit received"
_ENTITY_TIMEOUT   = 30.0


@pytest.fixture(scope="module")
def nodes_three():
    """Start HP+HEMS, wait for first SHIP connection, then start EV.

    Sequential startup guarantees HP registers first in HEMS's MPC remote
    list, so entity[0] is always HP and entity[1] is always EV.
    """
    require_binaries(EV_BINARY)
    hp   = NodeProcess(HP_BINARY,   HP_PORT,   HP_REMOTE_SKI,   HP_CERT,   HP_KEY)
    hems = NodeProcess(HEMS_BINARY, HEMS_PORT, HEMS_REMOTE_SKI, HEMS_CERT, HEMS_KEY,
                       extra_args=("--remote", EV_SKI))
    assert hp.wait_for(SHIP_CONNECTED_MARKER,   NODES_CONNECT_TIMEOUT), "HP: SHIP connection timeout"
    assert hems.wait_for(SHIP_CONNECTED_MARKER, NODES_CONNECT_TIMEOUT), "HEMS: first SHIP connection timeout"
    ev = NodeProcess(EV_BINARY, EV_PORT, HP_REMOTE_SKI, EV_CERT, EV_KEY)
    try:
        assert ev.wait_for(SHIP_CONNECTED_MARKER, NODES_CONNECT_TIMEOUT), "EV: SHIP connection timeout"
        assert hems.wait_for_count(SHIP_CONNECTED_MARKER, 2, NODES_CONNECT_TIMEOUT), "HEMS: second SHIP connection timeout"
        yield hp, hems, ev
    finally:
        hp.stop()
        hems.stop()
        ev.stop()


@pytest.fixture(scope="module")
def nodes_hems_hp_ev(nodes_three):
    """Extend nodes_three: wait for both MPC use cases and LPC to be ready."""
    hp, hems, ev = nodes_three
    assert hems.wait_for_count(_MPC_READY_MARKER, 2, UC_READY_TIMEOUT), \
        f"HEMS MPC not ready for both remotes after {UC_READY_TIMEOUT:.0f}s"
    assert hems.wait_for(_LPC_READY_MARKER, UC_READY_TIMEOUT), \
        f"HEMS LPC not ready after {UC_READY_TIMEOUT:.0f}s"
    return hp, hems, ev


@pytest.fixture(scope="module")
def entity_addrs(nodes_hems_hp_ev):
    """Return (hp_entity, ev_entity) address strings; HP started first → addrs[0]."""
    hp, hems, ev = nodes_hems_hp_ev
    addrs = wait_for_entity_addresses(hems, 2, _ENTITY_TIMEOUT)
    assert len(addrs) == 2, f"Expected 2 remote entities, got {len(addrs)}: {addrs}"
    return addrs[0], addrs[1]


# ---------------------------------------------------------------------------
# Scenario 1 — HP sets MPC measurements, HEMS reads from HP entity
# ---------------------------------------------------------------------------

def test_hems_hp_ev_s1_hp_mpc(nodes_hems_hp_ev, entity_addrs):
    hp, hems, ev = nodes_hems_hp_ev
    hp_entity, _ = entity_addrs
    _verify_mpc_from_sender(hp, hems, hp_entity, [
        ("power_total",   1000),
        ("power_phase_a", 1100),
        ("power_phase_b", 1200),
        ("power_phase_c", 1300),
    ])


# ---------------------------------------------------------------------------
# Scenario 2 — EV sets MPC measurements, HEMS reads from EV entity
# ---------------------------------------------------------------------------

def test_hems_hp_ev_s2_ev_mpc(nodes_hems_hp_ev, entity_addrs):
    hp, hems, ev = nodes_hems_hp_ev
    _, ev_entity = entity_addrs
    _verify_mpc_from_sender(ev, hems, ev_entity, [
        ("power_total",   2000),
        ("power_phase_a", 2100),
        ("power_phase_b", 2200),
        ("power_phase_c", 2300),
    ])


# ---------------------------------------------------------------------------
# Scenario 3 — HEMS sends LPC power limit to HP (must use --remote because
#              EV also has CS-LPC, giving HEMS two EG-LPC remotes)
# ---------------------------------------------------------------------------

def test_hems_hp_ev_s3_lpc_to_hp(nodes_hems_hp_ev, entity_addrs):
    hp, hems, ev = nodes_hems_hp_ev
    hp_entity, _ = entity_addrs
    _verify_lpc_from_hems(hp, hems, hp_entity, 5000)


# ---------------------------------------------------------------------------
# Scenario 4 — EV disconnects; HP+HEMS LPC continue unaffected
# ---------------------------------------------------------------------------

def test_hems_hp_ev_s4_ev_disconnect_lpc(nodes_hems_hp_ev, entity_addrs):
    hp, hems, ev = nodes_hems_hp_ev
    hp_entity, _ = entity_addrs
    ev.stop()
    _verify_lpc_from_hems(hp, hems, hp_entity, 7000)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _verify_mpc_from_sender(sender, hems, entity_addr, measurements):
    """Set MPC measurements on sender, verify both MU and MA sides."""
    for name, value in measurements:
        pos = hems.line_count()
        sender.send(f"mu_mpc set {name} {value}")
        hems.wait_for_new(f"MA MPC Measurement received: {name}", pos, DEFAULT_PROPAGATION)

    pos_sender = sender.line_count()
    for name, _ in measurements:
        sender.send(f"mu_mpc get {name}")
    pos_hems = hems.line_count()
    for name, _ in measurements:
        hems.send(f"ma_mpc get {name} --remote {entity_addr}")

    last_name = measurements[-1][0]
    sender.wait_for_new(f"MU MPC measurement {last_name}:", pos_sender, DEFAULT_SETTLE)
    hems.wait_for_new(f"MA MPC measurement {last_name}:", pos_hems, DEFAULT_SETTLE)

    for name, expected in measurements:
        mu_lines = [l for l in sender.log_lines()[pos_sender:] if f"MU MPC measurement {name}:" in l]
        ma_lines = [l for l in hems.log_lines()[pos_hems:] if f"MA MPC measurement {name}:" in l]
        mu_val = parse_field(mu_lines[-1], "value") if mu_lines else None
        ma_val = parse_field(ma_lines[-1], "value") if ma_lines else None
        print(f"  {name}: MU={mu_val}  MA={ma_val}")
        assert mu_val == str(expected), f"MU {name}: expected {expected}, got {mu_val!r}"
        assert ma_val == str(expected), f"MA {name}: expected {expected}, got {ma_val!r}"


def _verify_lpc_from_hems(hp, hems, hp_entity, limit):
    """Have HEMS send an LPC power limit to HP via --remote and verify HP CS side."""
    pos_hp = hp.line_count()
    hems.send(f"eg_lpc set power_limit {limit} PT0S true --remote {hp_entity}")
    assert hp.wait_for_new("CS LPC Power Limit received", pos_hp, DEFAULT_PROPAGATION), \
        f"HP did not receive LPC power limit {limit} W"
    pos_hp2 = hp.line_count()
    hp.send("cs_lpc get power_limit")
    hp.wait_for_new("CS LPC Active Power Limit: value", pos_hp2, DEFAULT_SETTLE)
    cs_lines = [l for l in hp.log_lines()[pos_hp2:]
                if "CS LPC Active Power Limit" in l and "Failsafe" not in l]
    cs_val = parse_field(cs_lines[-1], "value") if cs_lines else None
    print(f"  LPC power_limit: CS={cs_val}")
    assert cs_val == str(limit), f"CS LPC: expected {limit}, got {cs_val!r}"
