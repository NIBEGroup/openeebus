import os
import re
import shutil
import subprocess
import tempfile
import time
from pathlib import Path

import pytest
from pytest_html import extras as html_extras

ROOT      = Path(__file__).parent.parent
BUILD_DIR = ROOT / "build"
CERTS_DIR = ROOT / "integration_tests" / "certificates"

HP_BINARY   = str(BUILD_DIR / "heat_pump")
HEMS_BINARY = str(BUILD_DIR / "hems")

# Each node is started with the OTHER node's certificate SKI as its trusted remote.
# heat_pump registers hems.crt's SKI; hems registers heat_pump.crt's SKI.
HP_REMOTE_SKI   = os.environ.get("EEBUS_HP_SKI",   "1bb991d59a94cc1925486be3addb07200b9d7680")
HEMS_REMOTE_SKI = os.environ.get("EEBUS_HEMS_SKI", "40c61c3526f271e8e1547851c46f6ea20d4c6f83")
HP_CERT         = os.environ.get("EEBUS_HP_CERT",   str(CERTS_DIR / "heat_pump.crt"))
HP_KEY          = os.environ.get("EEBUS_HP_KEY",    str(CERTS_DIR / "heat_pump.key"))
HEMS_CERT       = os.environ.get("EEBUS_HEMS_CERT", str(CERTS_DIR / "hems.crt"))
HEMS_KEY        = os.environ.get("EEBUS_HEMS_KEY",  str(CERTS_DIR / "hems.key"))

HP_PORT   = 4712
HEMS_PORT = 4710

SHIP_CONNECTED_MARKER = "Remote SKI connected"
NODES_CONNECT_TIMEOUT = 120.0
UC_READY_TIMEOUT      = 30.0

DEFAULT_PROPAGATION = 3.0
DEFAULT_SETTLE      = 2.0

_STDBUF = shutil.which("stdbuf")


def _cmd(binary: str, port: int, remote_ski: str, cert: str, key: str) -> list:
    base = [binary, str(port), remote_ski, cert, key, "auto"]
    return ([_STDBUF, "-oL"] + base) if _STDBUF else base


class NodeProcess:
    """Manages a single heat_pump or hems process."""

    def __init__(self, binary: str, port: int, remote_ski: str, cert: str, key: str):
        fd, self.log_path = tempfile.mkstemp(suffix=".log")
        self._log_fd = os.fdopen(fd, "w")
        self.proc = subprocess.Popen(
            _cmd(binary, port, remote_ski, cert, key),
            stdin=subprocess.PIPE,
            stdout=self._log_fd,
            stderr=self._log_fd,
        )

    def send(self, cmd: str) -> None:
        self.proc.stdin.write(f"{cmd}\n".encode())
        self.proc.stdin.flush()

    def wait_for(self, marker: str, timeout: float = 120) -> bool:
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            with open(self.log_path) as f:
                if marker in f.read():
                    return True
            time.sleep(0.1)
        return False

    def line_count(self) -> int:
        return len(self.log_lines())

    def wait_for_new(self, marker: str, after: int, timeout: float) -> bool:
        """Return True once marker appears in any log line at index >= after."""
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            if any(marker in ln for ln in self.log_lines()[after:]):
                return True
            time.sleep(0.1)
        return False

    def log_lines(self) -> list:
        with open(self.log_path) as f:
            return f.readlines()

    def last_line_with(self, text: str, exclude: str = None) -> str:
        for line in reversed(self.log_lines()):
            if text in line and (exclude is None or exclude not in line):
                return line.rstrip()
        return ""

    def count_occurrences(self, text: str) -> int:
        return sum(1 for line in self.log_lines() if text in line)

    def is_alive(self) -> bool:
        return self.proc.poll() is None

    def wait_dead(self, timeout: float) -> bool:
        try:
            self.proc.wait(timeout=timeout)
            return True
        except subprocess.TimeoutExpired:
            return False

    def stop(self, timeout: float = 5.0) -> None:
        try:
            self.send("exit")
        except OSError:
            pass
        try:
            self.proc.stdin.close()
        except OSError:
            pass
        try:
            self.proc.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            self.proc.kill()
            self.proc.wait()
        self._log_fd.close()


def parse_field(line: str, field: str) -> str:
    """Extract 'field=VALUE' from a log line (stops at space, comma, or newline)."""
    m = re.search(rf'{re.escape(field)}=([^ ,\r\n]+)', line)
    return m.group(1) if m else None


def parse_pt(line: str) -> str:
    """Extract the first ISO 8601 duration token (PTxH, PTxS …) from a log line."""
    m = re.search(r'(PT[^ ,\r\n]+)', line)
    return m.group(1) if m else None


def monitor_set_and_verify(hp, hems, measurements,
                           hp_prefix, hems_prefix,
                           receive_marker_fn, hp_settle_fn, hems_settle_fn,
                           hp_log_fn, hems_log_fn,
                           hp_label, hems_label="MA",
                           propagation=DEFAULT_PROPAGATION, settle=DEFAULT_SETTLE):
    """Shared set/get/verify loop for MPC and MGCP monitoring use cases.

    Callables receive a measurement name and return the appropriate log marker
    or log-line string.  Call sites pass simple lambdas (MPC) or existing
    special-case helpers (MGCP) — the loop body is identical either way.
    """
    for name, value in measurements:
        pos = hems.line_count()
        hp.send(f"{hp_prefix} set {name} {value}")
        hems.wait_for_new(receive_marker_fn(name), pos, propagation)

    pos_hp = hp.line_count()
    for name, _ in measurements:
        hp.send(f"{hp_prefix} get {name}")
    pos_hems = hems.line_count()
    for name, _ in measurements:
        hems.send(f"{hems_prefix} get {name}")

    last_name = measurements[-1][0]
    hp.wait_for_new(hp_settle_fn(last_name), pos_hp, settle)
    hems.wait_for_new(hems_settle_fn(last_name), pos_hems, settle)

    for name, expected in measurements:
        hp_val   = parse_field(hp_log_fn(name),   "value")
        hems_val = parse_field(hems_log_fn(name), "value")
        print(f"  {name}: {hp_label}={hp_val}  {hems_label}={hems_val}")
        assert hp_val   == str(expected), f"{hp_label} {name}: expected {expected}, got {hp_val!r}"
        assert hems_val == str(expected), f"{hems_label} {name}: expected {expected}, got {hems_val!r}"


def make_node_pair():
    """Create a fresh heat_pump + hems NodeProcess pair on the standard ports."""
    return (
        NodeProcess(HP_BINARY,   HP_PORT,   HP_REMOTE_SKI,   HP_CERT,   HP_KEY),
        NodeProcess(HEMS_BINARY, HEMS_PORT, HEMS_REMOTE_SKI, HEMS_CERT, HEMS_KEY),
    )


def await_uc_ready(nodes, ready_marker: str):
    """Wait for a use-case handshake marker on hems, then return (hp, hems)."""
    hp, hems = nodes
    assert hems.wait_for(ready_marker, UC_READY_TIMEOUT), \
        f"Use case not ready after {UC_READY_TIMEOUT:.0f}s: {ready_marker!r}"
    return hp, hems


def require_binaries():
    missing = [b for b in (HP_BINARY, HEMS_BINARY) if not os.path.isfile(b)]
    if missing:
        pytest.skip(f"Binaries not found: {missing!r}  — build the project first")


@pytest.fixture(scope="module")
def nodes(request):
    """Start heat_pump + hems and wait for the SHIP connection; tear down after the test."""
    require_binaries()
    hp, hems = make_node_pair()
    try:
        assert hp.wait_for(SHIP_CONNECTED_MARKER, NODES_CONNECT_TIMEOUT),   "heat_pump: SHIP connection timeout"
        assert hems.wait_for(SHIP_CONNECTED_MARKER, NODES_CONNECT_TIMEOUT), "hems: SHIP connection timeout"
        yield hp, hems
    finally:
        hp.stop()
        hems.stop()
        # Attach the last 40 lines of each log to the HTML report on failure.
        rep_call = getattr(request.node, "rep_call", None)
        if rep_call is not None and rep_call.failed:
            for label, node in (("heat_pump log", hp), ("hems log", hems)):
                try:
                    tail = "".join(node.log_lines()[-40:])
                    extra = getattr(request.node, "extras", [])
                    extra.append(html_extras.text(tail, name=label))
                    request.node.extras = extra
                except Exception:
                    pass


def pytest_addoption(parser):
    parser.addoption(
        "--close-timing-iter",
        type=int,
        default=50,
        help="Number of close-timing iterations (default: 50)",
    )


@pytest.hookimpl(tryfirst=True, hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    rep = outcome.get_result()
    setattr(item, f"rep_{rep.when}", rep)
