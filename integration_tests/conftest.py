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
            time.sleep(0.5)
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


def require_binaries():
    missing = [b for b in (HP_BINARY, HEMS_BINARY) if not os.path.isfile(b)]
    if missing:
        pytest.skip(f"Binaries not found: {missing!r}  — build the project first")


@pytest.fixture(scope="module")
def nodes(request):
    """Start heat_pump + hems and wait for the SHIP connection; tear down after the test."""
    require_binaries()
    hp   = NodeProcess(HP_BINARY,   4712, HP_REMOTE_SKI,   HP_CERT,   HP_KEY)
    hems = NodeProcess(HEMS_BINARY, 4710, HEMS_REMOTE_SKI, HEMS_CERT, HEMS_KEY)
    try:
        assert hp.wait_for("Remote SKI connected", 120),   "heat_pump: SHIP connection timeout"
        assert hems.wait_for("Remote SKI connected", 120), "hems: SHIP connection timeout"
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
