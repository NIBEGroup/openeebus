"""
Shared helpers for three-node (HEMS + HP + EV charger) tests.
"""

import re
import time


def wait_for_entity_addresses(hems, count: int, timeout: float) -> list:
    """Poll 'ma_mpc list' until at least `count` entity address strings appear.

    Returns the list of formatted entity address strings in connection order.
    The list header line is 'ma_mpc connected remotes (N):' and each address
    is printed on its own line indented with two spaces.
    """
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        pos = hems.line_count()
        hems.send("ma_mpc list")
        inner = time.monotonic() + 2.0
        while time.monotonic() < inner:
            if any("ma_mpc" in l for l in hems.log_lines()[pos:]):
                break
            time.sleep(0.05)
        addrs = [
            l.strip()
            for l in hems.log_lines()[pos:]
            if re.match(r"^\s{2}\S", l)
        ]
        if len(addrs) >= count:
            return addrs
        time.sleep(0.3)
    return []
