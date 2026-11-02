"""Generate temporary TLS identities for integration test subprocesses."""

import re
import shutil
import subprocess
import tempfile
from contextlib import contextmanager
from dataclasses import dataclass
from pathlib import Path
from typing import Iterator, List


@dataclass(frozen=True)
class NodeIdentity:
    certificate: str
    key: str
    ski: str


@dataclass(frozen=True)
class TestIdentities:
    heat_pump: NodeIdentity
    hems: NodeIdentity
    ev_charger: NodeIdentity


def _run_openssl(command: List[str], node: str) -> str:
    try:
        result = subprocess.run(
            command, check=True, capture_output=True, text=True, timeout=30,
        )
    except subprocess.TimeoutExpired as exc:
        stderr = exc.stderr or ""
        if isinstance(stderr, bytes):
            stderr = stderr.decode(errors="replace")
        raise RuntimeError(
            f"{node}: OpenSSL timed out after 30 seconds: {stderr.strip()}"
        ) from exc
    except subprocess.CalledProcessError as exc:
        raise RuntimeError(
            f"{node}: OpenSSL failed (exit {exc.returncode}): {(exc.stderr or '').strip()}"
        ) from exc
    except OSError as exc:
        raise RuntimeError(f"{node}: could not run OpenSSL: {exc}") from exc
    return result.stdout


def _create_identity(openssl: str, directory: Path, node: str) -> NodeIdentity:
    certificate = directory / f"{node}.crt"
    key = directory / f"{node}.key"
    _run_openssl(
        [
            openssl, "req", "-x509", "-newkey", "ec",
            "-pkeyopt", "ec_paramgen_curve:prime256v1", "-nodes", "-sha256",
            "-keyout", str(key), "-out", str(certificate), "-days", "1",
            "-subj", f"/CN=openeebus-{node}",
            "-addext", "subjectKeyIdentifier=hash",
        ],
        node,
    )
    output = _run_openssl(
        [openssl, "x509", "-in", str(certificate), "-noout", "-ext", "subjectKeyIdentifier"],
        node,
    )
    match = re.search(r"(?m)^\s*((?:[0-9A-Fa-f]{2}:){19}[0-9A-Fa-f]{2})\s*$", output)
    if match is None:
        raise RuntimeError(f"{node}: OpenSSL did not print a valid 20-byte subject key identifier")
    return NodeIdentity(str(certificate), str(key), match.group(1).replace(":", "").lower())


@contextmanager
def generate_test_identities() -> Iterator[TestIdentities]:
    """Keep fresh node identities available until the caller finishes using them."""
    openssl = shutil.which("openssl")
    if openssl is None:
        raise RuntimeError("OpenSSL command-line tool not found; install it to run integration tests")

    with tempfile.TemporaryDirectory(prefix="openeebus-certificates-") as directory:
        root = Path(directory)
        yield TestIdentities(
            heat_pump=_create_identity(openssl, root, "heat-pump"),
            hems=_create_identity(openssl, root, "hems"),
            ev_charger=_create_identity(openssl, root, "ev-charger"),
        )
