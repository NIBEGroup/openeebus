# Integration Tests

The `tests/integration/` directory contains a pytest-based end-to-end test suite that runs
the `heat_pump`, `hems`, and `ev_charger` demo binaries as subprocesses, drives them via their CLI stdin
interface, and verifies protocol behaviour by polling their log output.

CTest registers all eight modules: four EEBUS use cases, simultaneous-open
(SIMOPEN) stress, three-node HEMS + heat-pump + EV scenarios and stress, and
service shutdown timing. Direct pytest commands remain available.

**Supported platforms: Linux, macOS, and Windows.**

---

## Prerequisites

### Build the demo binaries

Follow the platform-specific build guide in [Building and Running](../README.md#building-and-running)
and ensure `heat_pump`, `hems`, and `ev_charger` binaries exist in `build/`.

### Python dependencies

Python 3.8 or newer with venv support is required. Parallel CTest runs also
require the OpenSSL command-line tool to generate temporary certificates.
Enable integration-test support:

```sh
cmake -S . -B build -DOPTION_OPENEEBUS_TESTS=ON -DOPTION_INTEGRATION_TESTS=ON
```

---

## TLS certificates

The demos use mutual TLS authentication. Development certificates are included in
`tests/integration/certificates/`:

| File | Node | Purpose |
|---|---|---|
| `tests/integration/certificates/heat_pump.crt` | heat_pump (CS/EV) | TLS certificate |
| `tests/integration/certificates/heat_pump.key` | heat_pump (CS/EV) | Private key |
| `tests/integration/certificates/hems.crt` | hems (EG/grid) | TLS certificate |
| `tests/integration/certificates/hems.key` | hems (EG/grid) | Private key |
| `tests/integration/certificates/ev_charger.crt` | ev_charger (CS/MU) | TLS certificate |
| `tests/integration/certificates/ev_charger.key` | ev_charger (CS/MU) | Private key |

Direct pytest runs use these files by default. CTest runs generate temporary
certificates with distinct identities for each module.

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

Configure once to create the isolated Python environment and install its
requirements, then build the demo binaries before running CTest:

```sh
cmake --build build
ctest --test-dir build -L integration --output-on-failure
```

CTest gives each module distinct ports, mDNS names, and TLS identities, so the
suite can be run in parallel, for example with
`ctest --test-dir build -L integration -j 8 --output-on-failure`.

The EV stress module runs 20 iterations by default (`STRESS_TOTAL` overrides
this for pytest or CTest). Close timing runs 50 iterations. EV stress and close
timing have thirty-minute timeouts; other entries use ten minutes.

A standalone unit-test configuration (`cmake -S tests -B tests/build`) does
not register integration tests because it does not build the demo targets.

Run a single use-case module:

```sh
ctest --test-dir build -R '^integration_lpc$' --output-on-failure
```

Run the EV modules or the close-timing module:

```sh
ctest --test-dir build -L three_node --output-on-failure
ctest --test-dir build -R '^integration_close_timing$' --output-on-failure
```

To run integration tests without stress and timing measurements:

```sh
ctest --test-dir build -L integration -LE 'stress|debug_flags' --output-on-failure
```

Generate an HTML report:

```sh
build/tests/integration/itest_venv/bin/python -m pytest tests/integration/ --ignore=tests/integration/test_close_timing.py --html=report.html --self-contained-html
```

Run with verbose output:

```sh
ctest --test-dir build -L integration -V
```

Run the three-node scenarios directly (build `ev_charger` along with the other demos):

```sh
build/tests/integration/itest_venv/bin/python -m pytest tests/integration/test_hems_hp_ev.py -v
```

The three-node stress module is `tests/integration/test_hems_hp_ev_stress.py`.
Both three-node modules have the CTest label `three_node`; stress modules also
have `stress`, and close timing has `debug_flags`. All have `integration`.

On Windows, replace `build/tests/integration/itest_venv/bin/python` in the direct
pytest commands with `build/tests/integration/itest_venv/Scripts/python.exe`.

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
| `test_hems_hp_ev.py` | Three-node MPC/LPC and EV disconnect | 4 |
| `test_hems_hp_ev_stress.py` | Three-node MPC/LPC and shutdown stress | 1 |

### Close-timing test

`test_close_timing.py` measures wall-clock time from `EebusService::Stop()` to
`EebusService::Destruct()` over multiple iterations. It manages its own per-iteration
process pairs independently of the shared-connection fixture used by other modules.

The number of iterations can be overridden:

```sh
build/tests/integration/itest_venv/bin/python -m pytest tests/integration/test_close_timing.py --close-timing-iter=20
```

**Required compile-time flags:** `EEBUS_SERVICE_DEBUG=1` and `SHIP_NODE_DEBUG=1`
(see [Enabling debug flags](#enabling-debug-flags))

---

## Enabling debug flags

The debug flags default to `0` in the source. Configuring integration tests
through the repository root automatically sets `EEBUS_SERVICE_DEBUG=1` and
`SHIP_NODE_DEBUG=1` on the `openeebus` library, where these logs are produced.
This enables shutdown timestamps and SIMOPEN diagnostics for every demo.

For a build without CTest integration support, enable them explicitly before
running the debug modules through pytest:

```sh
cmake -S . -B build -DOPTION_INTEGRATION_TESTS=OFF -DCMAKE_C_FLAGS="-DEEBUS_SERVICE_DEBUG=1 -DSHIP_NODE_DEBUG=1"
```

The flags are defined in:

- `EEBUS_SERVICE_DEBUG` — `src/service/service/eebus_service.c`
- `SHIP_NODE_DEBUG` — `src/ship/ship_node/ship_node.c`
