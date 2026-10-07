"""
HEMS + HP + EV charger three-node stress test.

Each iteration starts all three nodes sequentially (HP+HEMS first, then EV),
exchanges MPC measurements and LPC limits, then stops them in a random order.

HP is always started before EV so entity[0] = HP and entity[1] = EV in the
MA MPC remote list — allowing unambiguous entity targeting.
"""

import os
import random
import time

import pytest

from conftest import (
    NodeProcess,
    HP_BINARY, HP_PORT, HP_REMOTE_SKI, HP_CERT, HP_KEY,
    HEMS_BINARY, HEMS_PORT, HEMS_REMOTE_SKI, HEMS_CERT, HEMS_KEY,
    EV_BINARY, EV_PORT, EV_SKI, EV_CERT, EV_KEY,
    SHIP_CONNECTED_MARKER,
    DEFAULT_PROPAGATION,
    parse_field, require_binaries,
)
from _ev_tests import wait_for_entity_addresses

TOTAL             = int(os.environ.get("STRESS_TOTAL", 20))
CONNECT_TIMEOUT   = 90.0
ENTITY_TIMEOUT    = 15.0
_MPC_READY_MARKER = "MA MPC remote entity connected"


@pytest.mark.three_node
def test_hems_hp_ev_stress():
    require_binaries(EV_BINARY)

    passed = 0
    failed = 0
    times  = []

    print(f"\n{'='*70}")
    print(f"  HEMS + HP + EV stress test  ({TOTAL} runs)")
    print(f"{'='*70}")

    for i in range(1, TOTAL + 1):
        t_start = time.monotonic()
        hp   = NodeProcess(HP_BINARY,   HP_PORT,   HP_REMOTE_SKI,   HP_CERT,   HP_KEY)
        hems = NodeProcess(HEMS_BINARY, HEMS_PORT, HEMS_REMOTE_SKI, HEMS_CERT, HEMS_KEY,
                           extra_args=("--remote", EV_SKI))

        connected_pair = (
            hp.wait_for(SHIP_CONNECTED_MARKER,   CONNECT_TIMEOUT) and
            hems.wait_for(SHIP_CONNECTED_MARKER, CONNECT_TIMEOUT)
        )

        ev = NodeProcess(EV_BINARY, EV_PORT, HP_REMOTE_SKI, EV_CERT, EV_KEY)
        connected_ev = connected_pair and (
            ev.wait_for(SHIP_CONNECTED_MARKER,             CONNECT_TIMEOUT) and
            hems.wait_for_count(SHIP_CONNECTED_MARKER, 2, CONNECT_TIMEOUT)
        )
        elapsed = time.monotonic() - t_start

        if not connected_ev:
            fail_reason = "connection-timeout"
        elif not hems.wait_for_count(_MPC_READY_MARKER, 2, ENTITY_TIMEOUT):
            fail_reason = "entity-list-timeout"
        else:
            addrs = wait_for_entity_addresses(hems, 2, ENTITY_TIMEOUT)
            if len(addrs) < 2:
                fail_reason = "entity-list-timeout"
            else:
                fail_reason = _run_data_check(hp, hems, ev, addrs[0], addrs[1])

        nodes = [hp, hems, ev]
        random.shuffle(nodes)
        for node in nodes:
            node.stop()
            time.sleep(random.randint(1, 3))

        if not fail_reason:
            passed += 1
            status = "PASS"
        else:
            failed += 1
            status = f"FAIL ({fail_reason})"

        print(f"  Run {i:2d}/{TOTAL}: {status:<20s}  total={elapsed:5.1f}s")
        times.append(elapsed)

    _print_summary(passed, failed, times)
    assert failed == 0, f"{failed} run(s) failed out of {TOTAL}"


def _run_data_check(hp, hems, ev, hp_entity, ev_entity) -> str:
    """Return empty string on success, or a short failure description."""
    # MPC: HP sets 1000, EV sets 2000
    pos = hems.line_count()
    hp.send("mu_mpc set power_total 1000")
    hems.wait_for_new("MA MPC Measurement received: power_total", pos, DEFAULT_PROPAGATION)

    pos = hems.line_count()
    ev.send("mu_mpc set power_total 2000")
    hems.wait_for_new("MA MPC Measurement received: power_total", pos, DEFAULT_PROPAGATION)

    time.sleep(0.2)
    pos_hems = hems.line_count()
    hems.send(f"ma_mpc get power_total --remote {hp_entity}")
    hems.send(f"ma_mpc get power_total --remote {ev_entity}")

    deadline = time.monotonic() + DEFAULT_PROPAGATION
    while time.monotonic() < deadline:
        lines = [l for l in hems.log_lines()[pos_hems:] if "MA MPC measurement power_total:" in l]
        if len(lines) >= 2:
            break
        time.sleep(0.1)

    lines = [l for l in hems.log_lines()[pos_hems:] if "MA MPC measurement power_total:" in l]
    hp_val = parse_field(lines[0], "value") if len(lines) >= 1 else None
    ev_val = parse_field(lines[1], "value") if len(lines) >= 2 else None

    if hp_val != "1000" or ev_val != "2000":
        return f"MA-mpc-values={hp_val!r}/{ev_val!r}"

    # LPC: HEMS sends 5000 W limit to HP only
    pos_hp = hp.line_count()
    hems.send(f"eg_lpc set power_limit 5000 PT0S true --remote {hp_entity}")
    if not hp.wait_for_new("CS LPC Power Limit received", pos_hp, DEFAULT_PROPAGATION):
        return "LPC-timeout"

    pos_hp2 = hp.line_count()
    hp.send("cs_lpc get power_limit")
    hp.wait_for_new("CS LPC Active Power Limit: value", pos_hp2, DEFAULT_PROPAGATION)
    cs_lines = [l for l in hp.log_lines()[pos_hp2:]
                if "CS LPC Active Power Limit" in l and "Failsafe" not in l]
    cs_val = parse_field(cs_lines[-1], "value") if cs_lines else None
    if cs_val != "5000":
        return f"LPC={cs_val!r}"

    return ""


def _print_summary(passed: int, failed: int, times: list) -> None:
    print(f"\n{'='*70}")
    print("  SUMMARY")
    print(f"{'='*70}")
    print(f"  Passed : {passed} / {TOTAL}")
    print(f"  Failed : {failed} / {TOTAL}")
    min_t = min(times)
    max_t = max(times)
    avg_t = sum(times) / len(times)
    print(f"  Total time: min={min_t:.1f}s  avg={avg_t:.1f}s  max={max_t:.1f}s")
