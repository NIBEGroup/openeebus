import os
import re
import shutil
import subprocess
import tempfile
import time
from contextlib import ExitStack
from pathlib import Path

import pytest
from pytest_html import extras as html_extras

from _certificates import TestIdentities, generate_test_identities

ROOT      = Path(__file__).parent.parent.parent
BUILD_DIR = ROOT / "build"

HP_BINARY   = str(BUILD_DIR / "heat_pump")
HEMS_BINARY = str(BUILD_DIR / "hems")
EV_BINARY   = str(BUILD_DIR / "ev_charger")

HP_PORT   = int(os.environ.get("EEBUS_HP_PORT", "4712"))
HEMS_PORT = int(os.environ.get("EEBUS_HEMS_PORT", "4710"))
EV_PORT   = int(os.environ.get("EEBUS_EV_PORT", "4714"))

SHIP_CONNECTED_MARKER = "Remote SKI connected"
NODES_CONNECT_TIMEOUT = 120.0
UC_READY_TIMEOUT      = 30.0

DEFAULT_PROPAGATION = 3.0
DEFAULT_SETTLE      = 2.0

_STDBUF = shutil.which("stdbuf")
_INSTANCE = os.environ.get("EEBUS_INTEGRATION_INSTANCE")


@pytest.fixture(scope="session")
def test_identities():
    """Generate TLS identities on demand and remove them after node teardown."""
    with generate_test_identities() as identities:
        yield identities


def _cmd(binary: str, port: int, remote_ski: str, cert: str, key: str, extra_args: tuple = ()) -> list:
    base = [binary, str(port), remote_ski, cert, key, "auto", *extra_args]
    return ([_STDBUF, "-oL"] + base) if _STDBUF else base


class NodeProcess:
    """Manages a single heat_pump, hems, or ev_charger process."""

    def __init__(self, binary: str, port: int, remote_ski: str, cert: str, key: str,
                 extra_args: tuple = (), mdns_service_name: str = None):
        if _INSTANCE and mdns_service_name is None:
            node_name = Path(binary).stem.replace("_", "-")
            mdns_service_name = f"openeebus-{_INSTANCE}-{node_name}"
        fd, self.log_path = tempfile.mkstemp(suffix=".log")
        self._log_fd = os.fdopen(fd, "w")
        try:
            self.proc = subprocess.Popen(
                _cmd(binary, port, remote_ski, cert, key, extra_args),
                stdin=subprocess.PIPE,
                stdout=self._log_fd,
                stderr=self._log_fd,
                env={**os.environ, "EEBUS_MDNS_SERVICE_NAME": mdns_service_name} if mdns_service_name else None,
            )
        except BaseException:
            self._log_fd.close()
            os.unlink(self.log_path)
            raise

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.stop()

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

    def wait_for_new_line(self, marker: str, after: int, timeout: float) -> str:
        """Return the first matching log line at index >= after, or time out."""
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            for line in self.log_lines()[after:]:
                if marker in line:
                    return line.rstrip()
            time.sleep(0.1)
        raise TimeoutError(f"No new log line containing {marker!r}")

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

    def wait_for_count(self, marker: str, count: int, timeout: float) -> bool:
        """Return True once marker appears at least `count` times in total log."""
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            if self.count_occurrences(marker) >= count:
                return True
            time.sleep(0.1)
        return False

    def is_alive(self) -> bool:
        return self.proc.poll() is None

    def wait_dead(self, timeout: float) -> bool:
        try:
            self.proc.wait(timeout=timeout)
            return True
        except subprocess.TimeoutExpired:
            return False

    def stop(self, timeout: float = 5.0) -> None:
        if self._log_fd.closed and self.proc.poll() is not None:
            return
        try:
            self.send("exit")
        except (OSError, ValueError):
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


def stop_nodes(*nodes: NodeProcess, timeout: float = 5.0) -> None:
    """Stop every node, even if stopping an earlier node raises an error."""
    with ExitStack() as stack:
        for node in reversed(nodes):
            stack.callback(node.stop, timeout=timeout)


def parse_field(line: str, field: str) -> str:
    """Extract 'field=VALUE' from a log line (stops at space, comma, or newline)."""
    m = re.search(rf'{re.escape(field)}=([^ ,\r\n]+)', line)
    return m.group(1) if m else None


def parse_pt(line: str) -> str:
    """Extract the first ISO 8601 duration token (PTxH, PTxS …) from a log line."""
    m = re.search(r'(PT[^ ,\r\n]+)', line)
    return m.group(1) if m else None



def make_node_pair(identities: TestIdentities):
    """Create a fresh heat_pump + hems pair with this test process's identity."""
    with ExitStack() as stack:
        hp = stack.enter_context(NodeProcess(
            HP_BINARY, HP_PORT, identities.hems.ski,
            identities.heat_pump.certificate, identities.heat_pump.key,
        ))
        hems = stack.enter_context(NodeProcess(
            HEMS_BINARY, HEMS_PORT, identities.heat_pump.ski,
            identities.hems.certificate, identities.hems.key,
        ))
        stack.pop_all()
        return hp, hems


def make_three_node_group(identities: TestIdentities):
    """Create fresh heat_pump + hems + ev_charger NodeProcess triple.

    HEMS trusts both HP and EV; HP and EV each trust HEMS.
    """
    with ExitStack() as stack:
        hp = stack.enter_context(NodeProcess(
            HP_BINARY, HP_PORT, identities.hems.ski,
            identities.heat_pump.certificate, identities.heat_pump.key,
        ))
        hems = stack.enter_context(NodeProcess(
            HEMS_BINARY, HEMS_PORT, identities.heat_pump.ski,
            identities.hems.certificate, identities.hems.key,
            extra_args=("--remote", identities.ev_charger.ski),
        ))
        ev = stack.enter_context(NodeProcess(
            EV_BINARY, EV_PORT, identities.hems.ski,
            identities.ev_charger.certificate, identities.ev_charger.key,
        ))
        stack.pop_all()
        return hp, hems, ev


def await_uc_ready(nodes, ready_marker: str):
    """Wait for a use-case handshake marker on hems, then return (hp, hems)."""
    hp, hems = nodes
    assert hems.wait_for(ready_marker, UC_READY_TIMEOUT), \
        f"Use case not ready after {UC_READY_TIMEOUT:.0f}s: {ready_marker!r}"
    return hp, hems


def require_binaries(*extra: str):
    missing = [b for b in (HP_BINARY, HEMS_BINARY, *extra) if not os.path.isfile(b)]
    if missing:
        if _INSTANCE:
            pytest.fail(f"Binaries not found: {missing!r} — build the project first")
        pytest.skip(f"Binaries not found: {missing!r}  — build the project first")


@pytest.fixture(scope="module")
def nodes(request, test_identities):
    """Start heat_pump + hems and wait for the SHIP connection; tear down after the test."""
    require_binaries()
    hp, hems = make_node_pair(test_identities)
    try:
        assert hp.wait_for(SHIP_CONNECTED_MARKER, NODES_CONNECT_TIMEOUT),   "heat_pump: SHIP connection timeout"
        assert hems.wait_for(SHIP_CONNECTED_MARKER, NODES_CONNECT_TIMEOUT), "hems: SHIP connection timeout"
        yield hp, hems
    finally:
        stop_nodes(hp, hems)
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
