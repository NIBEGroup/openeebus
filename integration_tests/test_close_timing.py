"""
Close-timing test.

Measures wall-clock time from EebusService::Stop(): begin to
EebusService::Destruct(): end over MAX_ITER iterations.
Each iteration starts both nodes, performs a minimal LPC exchange,
sends exit, waits for both processes to die, then extracts the
close duration from DebugPrintf timestamps.

Build requirements (must be set at compile time):
  EEBUS_SERVICE_DEBUG=1  — enables EebusService::Stop/Destruct begin/end prints
  SHIP_NODE_DEBUG=1      — enables ShipNode::Stop/Destruct prints

The iteration count can be overridden with the --close-timing-iter pytest option.
"""

import re

import pytest

from conftest import (
    NodeProcess,
    make_node_pair,
    require_binaries,
    SHIP_CONNECTED_MARKER,
)

DEFAULT_ITER     = 50
CONNECT_TIMEOUT  = 60.0
CLOSE_TIMEOUT    = 60.0

# DebugPrintf timestamp: [YYYY/MM/DD HH:MM:SS:TTTT] where TTTT = tv_usec/100
_TS_RE = re.compile(r'\[(\d{4}/\d{2}/\d{2}) (\d{2}):(\d{2}):(\d{2}):(\d{4})\]')



def _ts_to_tenth_ms(h: int, m: int, s: int, t: int) -> int:
    return h * 36_000_000 + m * 600_000 + s * 10_000 + t


def _close_duration_ms(log_path: str):
    begin = end = None
    with open(log_path) as f:
        for line in f:
            match = _TS_RE.search(line)
            if not match:
                continue
            ts = _ts_to_tenth_ms(
                int(match.group(2)), int(match.group(3)),
                int(match.group(4)), int(match.group(5)),
            )
            if "EebusService::Stop(): begin" in line:
                begin = ts
            elif "EebusService::Destruct(): end" in line:
                end = ts
    if begin is None or end is None:
        return None
    delta = end - begin
    if delta < 0:
        delta += 864_000_000  # midnight rollover in 0.1 ms units
    return delta // 10  # → milliseconds


def _stats(values):
    nums = [v for v in values if v is not None]
    if not nums:
        return None
    return {"n": len(nums), "min": min(nums), "max": max(nums), "avg": sum(nums) // len(nums)}


def _print_summary(hp_times, hems_times, statuses):
    print(f"\n{'='*72}")
    print(f"  Close-Timing Summary  ({len(hp_times)} iterations)")
    print(f"{'='*72}")
    print(f"{'Iter':<6}  {'heat_pump (ms)':>14}  {'hems (ms)':>14}  {'Status':<12}")
    print(f"{'------':<6}  {'--------------':>14}  {'--------------':>14}  {'------------':<12}")
    for i, (hp_ms, hems_ms, st) in enumerate(zip(hp_times, hems_times, statuses), 1):
        hp_s   = str(hp_ms)   if hp_ms   is not None else "N/A"
        hems_s = str(hems_ms) if hems_ms is not None else "N/A"
        print(f"{i:<6}  {hp_s:>14}  {hems_s:>14}  {st:<12}")
    for label, vals in (("heat_pump", hp_times), ("hems", hems_times)):
        s = _stats(vals)
        if s:
            print(f"  {label:<14}  n={s['n']:<4}  "
                  f"min={s['min']:<6}  avg={s['avg']:<6}  max={s['max']} ms")
        else:
            print(f"  {label:<14}  no numeric data "
                  "(build with EEBUS_SERVICE_DEBUG=1 to enable timestamps)")
    print(f"{'='*72}")


def _connect_and_close(hp: NodeProcess, hems: NodeProcess) -> str:
    """Connect, perform a minimal LPC exchange, send exit, return status string."""
    connected = (
        hp.wait_for(SHIP_CONNECTED_MARKER, CONNECT_TIMEOUT) and
        hems.wait_for(SHIP_CONNECTED_MARKER, CONNECT_TIMEOUT)
    )
    if not connected:
        print("  ERROR: connection timeout — skipping iteration")
        return "CONN_TIMEOUT"

    hems.wait_for("EG LPC Failsafe Active Power Limit received", 10.0)

    print("  LPC exchange...")
    pos_hems = hems.line_count()
    hp.send("cs_lpc set failsafe_limit 5000 true")
    hems.wait_for_new("EG LPC Failsafe Active Power Limit received", pos_hems, 3.0)
    pos_hems = hems.line_count()
    hp.send("cs_lpc set failsafe_duration PT2H true")
    hems.wait_for_new("EG LPC Failsafe Duration Minimum received", pos_hems, 3.0)
    pos_hp = hp.line_count()
    hems.send("eg_lpc set power_limit 7000 PT0S true")
    hp.wait_for_new("CS LPC Power Limit received", pos_hp, 3.0)

    print("  Sending exit...")
    hp.send("exit")
    hems.send("exit")

    hp_died   = hp.wait_dead(CLOSE_TIMEOUT)
    hems_died = hems.wait_dead(CLOSE_TIMEOUT)

    if not hp_died or not hems_died:
        print(f"  WARNING: process(es) did not exit within {CLOSE_TIMEOUT:.0f}s, "
              "force-killing")
        return "FORCE_KILL"

    return "OK"


@pytest.mark.debug_flags
def test_close_timing(request):
    require_binaries()
    max_iter = request.config.getoption("--close-timing-iter", default=DEFAULT_ITER)

    hp_times   = []
    hems_times = []
    statuses   = []

    print(f"\n{'='*72}")
    print(f"  Close-timing test  ({max_iter} iterations)")
    print(f"{'='*72}")

    for it in range(1, max_iter + 1):
        print(f"\n=== Iteration {it} / {max_iter} ===")

        hp, hems = make_node_pair()

        try:
            status = _connect_and_close(hp, hems)
        finally:
            hp.stop(timeout=2.0)
            hems.stop(timeout=2.0)

        statuses.append(status)

        if status == "CONN_TIMEOUT":
            hp_times.append(None)
            hems_times.append(None)
            continue

        hp_ms   = _close_duration_ms(hp.log_path)
        hems_ms = _close_duration_ms(hems.log_path)
        hp_times.append(hp_ms)
        hems_times.append(hems_ms)

        hp_str   = f"{hp_ms}ms"   if hp_ms   is not None else "N/A"
        hems_str = f"{hems_ms}ms" if hems_ms is not None else "N/A"
        print(f"  RESULT  heat_pump={hp_str}  hems={hems_str}  status={status}")

    _print_summary(hp_times, hems_times, statuses)

    force_kills = statuses.count("FORCE_KILL")
    conn_timeouts = statuses.count("CONN_TIMEOUT")
    assert force_kills == 0, f"{force_kills} iteration(s) required force-kill"
    assert conn_timeouts == 0, f"{conn_timeouts} iteration(s) timed out on connection"
