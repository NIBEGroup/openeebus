"""
SIMOPEN stress test — equivalent of scripts/test_simopen_stress.sh.

Runs TOTAL iterations back-to-back and verifies both nodes always connect.
Reports which SHIP simultaneous-open tiebreaker branch each node took
(took-server, yielded, no-simopen, client-only) per run.

Build requirement: SHIP_NODE_DEBUG=1 (src/ship/ship_node/ship_node.c).
Without it the branch classification will always be "unknown" and the test
warns but does not skip — connectivity is still verified.
"""

import time

import pytest

from conftest import (
    HP_BINARY, HEMS_BINARY,
    HP_REMOTE_SKI, HEMS_REMOTE_SKI,
    HP_CERT, HP_KEY,
    HEMS_CERT, HEMS_KEY,
    NodeProcess,
    require_binaries,
)

TOTAL            = 20
CONNECT_TIMEOUT  = 90.0


def _branch_of(node: NodeProcess) -> str:
    for line in node.log_lines():
        if "SHIP-SIMOPEN" in line and "take server" in line:
            return "took-server"
        if "SHIP-SIMOPEN" in line and "yield" in line:
            return "yielded"
        if "server accept" in line and "no simul" in line:
            return "no-simopen"
        if "[SHIP] client connecting" in line:
            return "client-only"
    return "unknown"


def _start_pair():
    hp   = NodeProcess(HP_BINARY,   4712, HP_REMOTE_SKI,   HP_CERT,   HP_KEY)
    hems = NodeProcess(HEMS_BINARY, 4710, HEMS_REMOTE_SKI, HEMS_CERT, HEMS_KEY)
    return hp, hems


def _stop_pair(hp, hems):
    hp.stop()
    hems.stop()


@pytest.mark.simopen
def test_simopen_stress():
    require_binaries()

    passed   = 0
    failed   = 0
    times    = []
    hp_branches   = []
    hems_branches = []

    print(f"\n{'='*70}")
    print(f"  Simultaneous-open stress test  ({TOTAL} runs, timeout {CONNECT_TIMEOUT:.0f}s)")
    print(f"{'='*70}")

    for i in range(1, TOTAL + 1):
        hp, hems = _start_pair()
        start = time.monotonic()

        connected = (
            hp.wait_for("Remote SKI connected", CONNECT_TIMEOUT) and
            hems.wait_for("Remote SKI connected", CONNECT_TIMEOUT)
        )
        elapsed = time.monotonic() - start

        hp_b   = _branch_of(hp)
        hems_b = _branch_of(hems)
        _stop_pair(hp, hems)

        if connected:
            passed += 1
            status = "PASS"
        else:
            failed += 1
            if hp_b == "unknown" and hems_b == "unknown":
                status = f"FAIL (no attempt, timeout {CONNECT_TIMEOUT:.0f}s)"
            else:
                status = f"FAIL (partial, timeout {CONNECT_TIMEOUT:.0f}s)"

        print(f"Run {i:2d}/{TOTAL}: {status:6s}  {elapsed:5.1f}s | "
              f"hp={hp_b:<12s}  hems={hems_b:<12s}")

        times.append(elapsed)
        hp_branches.append(hp_b)
        hems_branches.append(hems_b)

    min_t = min(times)
    max_t = max(times)
    avg_t = sum(times) / len(times)

    hp_server = hp_branches.count("took-server")
    hp_yield  = hp_branches.count("yielded")
    hp_none   = hp_branches.count("no-simopen")

    hems_server = hems_branches.count("took-server")
    hems_yield  = hems_branches.count("yielded")
    hems_none   = hems_branches.count("no-simopen")

    print(f"\n{'='*70}")
    print("  SUMMARY")
    print(f"{'='*70}")
    print(f"  Passed : {passed} / {TOTAL}")
    print(f"  Failed : {failed} / {TOTAL}")
    print(f"  Connect time: min={min_t:.1f}s  avg={avg_t:.1f}s  max={max_t:.1f}s")
    print(f"  hp   branches: took-server={hp_server}  yielded={hp_yield}  "
          f"no-simopen={hp_none}")
    print(f"  hems branches: took-server={hems_server}  yielded={hems_yield}  "
          f"no-simopen={hems_none}")

    if hp_branches.count("unknown") == TOTAL:
        pytest.warns(
            UserWarning,
            match="SHIP_NODE_DEBUG",
        )

    assert failed == 0, f"{failed} run(s) failed to connect within {CONNECT_TIMEOUT:.0f}s"
