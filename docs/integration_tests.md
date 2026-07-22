# Integration Tests

The `scripts/` directory contains a set of shell scripts for running end-to-end integration
tests against the `heat_pump` and `hems` demo binaries. The tests cover the four supported
use cases, simultaneous-open (SIMOPEN) tiebreaker logic, and service shutdown timing.

**Supported platforms: Linux and macOS.**
Windows (Git Bash / MSYS2 / Cygwin) is not supported — see [Platform limitations](#platform-limitations).

---

## Prerequisites

### Build the demo binaries

Follow the platform-specific build guide in [Building and Running](../README.md#building-and-running)
and ensure both `heat_pump` and `hems` binaries exist in `build/`.

### TLS certificates

The demos use mutual TLS authentication. Each node needs a certificate file, a private key
file, and its corresponding SKI (the hex-encoded SHA-1 of the certificate's public key).

Development certificates are included in `scripts/certificates/`:

| File | Node | Purpose |
|---|---|---|
| `scripts/certificates/heat_pump.crt` | heat_pump (CS/EV) | TLS certificate |
| `scripts/certificates/heat_pump.key` | heat_pump (CS/EV) | Private key |
| `scripts/certificates/hems.crt` | hems (EG/grid) | TLS certificate |
| `scripts/certificates/hems.key` | hems (EG/grid) | Private key |

The scripts use these files by default — no configuration needed on a fresh checkout.

To use different certificates, override the paths via environment variables before running
any script:

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

## Launching nodes manually

`hp_pipe.sh` and `hems_pipe.sh` start the respective demo binary in the background, redirect
its stdin from a named pipe, and log all output to a file. Use these launchers when you want
to interact with a live session.

```sh
scripts/hp_pipe.sh    # starts heat_pump on port 4712, log → /tmp/hp.log,   pipe → /tmp/hp_pipe
scripts/hems_pipe.sh  # starts hems      on port 4710, log → /tmp/hems.log, pipe → /tmp/hems_pipe
```

After the SHIP handshake completes, send CLI commands by writing to the pipe:

```sh
echo "cs_lpc set failsafe_limit 5000 true" > /tmp/hp_pipe
echo "eg_lpc set power_limit 7000 PT0S true" > /tmp/hems_pipe
```

Watch the log in real time:

```sh
tail -f /tmp/hp.log
tail -f /tmp/hems.log
```

Shut down gracefully:

```sh
echo "exit" > /tmp/hp_pipe
echo "exit" > /tmp/hems_pipe
```

Force-kill if needed:

```sh
kill $(cat /tmp/hp.pid) $(cat /tmp/hems.pid) $(cat /tmp/hp_keeper.pid) $(cat /tmp/hems_keeper.pid) 2>/dev/null || true
```

---

## Test scripts

### Use-case tests

These scripts start both nodes, wait for the SHIP connection, exercise the full scenario set
for the given use case, and print a text table plus an HTML report.

No special compile-time flags are required.

| Script | Use case | HTML report |
|---|---|---|
| `scripts/test_lpc.sh` | Limitation of Power Consumption | `/tmp/eebus_lpc_test_report.html` |
| `scripts/test_lpp.sh` | Limitation of Power Production | `/tmp/eebus_lpp_test_report.html` |
| `scripts/test_mpc.sh` | Monitoring of Power Consumption | `/tmp/eebus_mpc_test_report.html` |
| `scripts/test_mgcp.sh` | Monitoring of Grid Connection Point | `/tmp/eebus_mgcp_test_report.html` |

Run example:

```sh
scripts/test_lpc.sh
```

### Simultaneous-open stress test

`scripts/test_simopen_stress.sh` runs 20 back-to-back connection attempts and verifies that
both nodes always connect. After each run it reports which SIMOPEN tiebreaker branch each
node took (`took-server`, `yielded`, or `no-simopen`).

**Required compile-time flag:** `SHIP_NODE_DEBUG=1` (see [Enabling debug flags](#enabling-debug-flags))

```sh
scripts/test_simopen_stress.sh
```

### Simultaneous-open integration test

`scripts/test_simopen_integration.sh [TARGET]` runs all four use-case tests in a loop until
a total of `TARGET` SIMOPEN events (default 25) have been observed, verifying that every test
still passes after a SIMOPEN resolution.

**Required compile-time flag:** `SHIP_NODE_DEBUG=1`

```sh
scripts/test_simopen_integration.sh        # default target: 25 events
scripts/test_simopen_integration.sh 50     # run until 50 events observed
```

### Close-timing test

`scripts/test_close_timing.sh [N]` measures the wall-clock time from `EebusService::Stop()`
to `EebusService::Destruct()` end over up to `N` iterations (default 50). It exercises a
minimal LPC exchange each iteration to ensure a full SHIP data-exchange state before close.

The timing is derived from `DebugPrintf` timestamps in the log (`lwsl_timestamp` format
`[YYYY/MM/DD HH:MM:SS:TTTT]`, where `TTTT = tv_usec / 100`).

**Required compile-time flags:** `EEBUS_SERVICE_DEBUG=1` and `SHIP_NODE_DEBUG=1`

```sh
scripts/test_close_timing.sh        # 50 iterations
scripts/test_close_timing.sh 20     # 20 iterations
```

Iteration logs are saved to `/tmp/hp_na_iterN.log` and `/tmp/hems_na_iterN.log`.

---

## Platform limitations

The test scripts require a POSIX shell environment with working Unix named pipes. They have
been tested on Linux and macOS. **Windows is not supported** for the following reasons:

| Dependency | Why it breaks on Windows |
|---|---|
| `mkfifo` + FIFO IPC | Git Bash / MSYS2 expose `mkfifo` via their POSIX layer, but cross-process FIFO reads/writes do not work reliably — the underlying Windows kernel uses a different named-pipe model (`\\.\pipe\…`) that the MSYS2 shim does not fully bridge. The entire stdin-multiplexing architecture of the launcher scripts depends on this. |
| `pkill` | Used by `test_simopen_integration.sh` and `test_simopen_stress.sh`. Not available in Git Bash; the Windows equivalent is `taskkill`. |
| `stdbuf` | Used by the launchers for line-buffered log output. Part of GNU coreutils, which must be separately installed and is often absent from Git Bash. |

**Recommended Windows workaround:** run the scripts inside a WSL2 (Windows Subsystem for
Linux) shell. WSL2 provides a real Linux kernel, so all three dependencies work correctly
and no script changes are needed.

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
