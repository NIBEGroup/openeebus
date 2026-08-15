# Integration Tests

The `integration_tests/` directory contains a pytest-based end-to-end test suite that runs
the `heat_pump` and `hems` demo binaries as subprocesses, drives them via their CLI stdin
interface, and verifies protocol behaviour by polling their log output.

The suite covers four EEBUS use cases, simultaneous-open (SIMOPEN) tiebreaker logic, and
service shutdown timing.

**Supported platforms: Linux, macOS, and Windows.**

---

## Prerequisites

### Build the demo binaries

Follow the platform-specific build guide in [Building and Running](../README.md#building-and-running)
and ensure both `heat_pump` and `hems` binaries exist in `build/`.

### Python dependencies

Python 3.8 or newer is required. Install the test dependencies:

```sh
pip install pytest pytest-html
```

---

## TLS certificates

The demos use mutual TLS authentication. Development certificates are included in
`integration_tests/certificates/`:

| File | Node | Purpose |
|---|---|---|
| `integration_tests/certificates/heat_pump.crt` | heat_pump (CS/EV) | TLS certificate |
| `integration_tests/certificates/heat_pump.key` | heat_pump (CS/EV) | Private key |
| `integration_tests/certificates/hems.crt` | hems (EG/grid) | TLS certificate |
| `integration_tests/certificates/hems.key` | hems (EG/grid) | Private key |

The tests use these files by default — no configuration needed on a fresh checkout.

To use different certificates, override the paths via environment variables before running:

```sh
# heat_pump (CS / EV side)
export EEBUS_HP_SKI="<40-char hex SKI>"
export EEBUS_HP_CERT="/path/to/heat_pump.crt"
export EEBUS_HP_KEY="/path/to/heat_pump.key"

# hems (EG / grid side)
export EEBUS_HEMS_SKI="<40-char hex SKI>"
export EEBUS_HEMS_CERT="/path/to/hems.crt"
export EEBUS_HEMS_KEY="/path/to/hems.key"
```

---

## Running the tests

Run the full suite (excluding the debug-only close-timing test) from the repository root:

```sh
pytest integration_tests/ --ignore=integration_tests/test_close_timing.py
```

Run a single use-case module:

```sh
pytest integration_tests/test_lpc.py
```

Generate an HTML report:

```sh
pytest integration_tests/ --ignore=integration_tests/test_close_timing.py --html=report.html --self-contained-html
```

Run with verbose output:

```sh
pytest integration_tests/test_lpc.py -v
```

---

## Test modules

Each module shares a single SHIP connection across all its tests (pytest `module` scope),
so setup cost is paid once per file rather than once per test.

| Module | Use case | Tests |
|---|---|---|
| `test_lpc.py` | Limitation of Power Consumption | 9 + 1 xfail |
| `test_lpp.py` | Limitation of Power Production | 9 + 1 xfail |
| `test_mpc.py` | Monitoring of Power Consumption | 5 |
| `test_mgcp.py` | Monitoring of Grid Connection Point | 7 |
| `test_simopen_stress.py` | SIMOPEN tiebreaker stress test | 1 |
| `test_close_timing.py` | Service shutdown timing | 1 (debug flags required) |

### Close-timing test

`test_close_timing.py` measures wall-clock time from `EebusService::Stop()` to
`EebusService::Destruct()` over multiple iterations. It manages its own per-iteration
process pairs independently of the shared-connection fixture used by other modules.

The number of iterations can be overridden:

```sh
pytest integration_tests/test_close_timing.py --close-timing-iter=20
```

**Required compile-time flags:** `EEBUS_SERVICE_DEBUG=1` and `SHIP_NODE_DEBUG=1`
(see [Enabling debug flags](#enabling-debug-flags))

---

## Enabling debug flags

The debug flags are guarded with `#ifndef` and default to `1` in the current source. If they
have been switched off, re-enable them by passing them as compile definitions.

CMake (add to the target that builds the demo):

```cmake
target_compile_definitions(heat_pump PRIVATE EEBUS_SERVICE_DEBUG=1 SHIP_NODE_DEBUG=1)
target_compile_definitions(hems      PRIVATE EEBUS_SERVICE_DEBUG=1 SHIP_NODE_DEBUG=1)
```

Or pass them on the `cmake` command line:

```sh
cmake -DCMAKE_C_FLAGS="-DEEBUS_SERVICE_DEBUG=1 -DSHIP_NODE_DEBUG=1" ..
```

The flags are defined in:

- `EEBUS_SERVICE_DEBUG` — `src/service/service/eebus_service.c`
- `SHIP_NODE_DEBUG` — `src/ship/ship_node/ship_node.c`
