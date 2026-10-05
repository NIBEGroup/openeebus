# OpenEEBUS by NIBE — agent instructions

OpenEEBUS is a C library implementing EEBUS SHIP and SPINE. Follow the existing C/C++ conventions and keep generated build files in `build/` or `tests/build/`.

## Build and test

Run commands from the repository root unless noted otherwise.

### Build the library and examples

Install CMake, Ninja, a C/C++ compiler, pkg-config, OpenSSL development files, libwebsockets development files, cJSON development files, and an mDNS/DNS-SD development library. On Debian or Ubuntu, CI installs:

```sh
sudo apt-get install cmake ninja-build pkg-config libssl-dev libwebsockets-dev libcjson-dev libavahi-compat-libdnssd-dev
```

Configure and build:

```sh
cmake -B build -G Ninja
cmake --build build
```

For AddressSanitizer with GCC or Clang, configure with `-DOPTION_ASAN=ON`. Linux can select the Avahi client mDNS implementation with `-DOPTION_MDNS_USE_AVAHI_CLIENT=ON`; the default uses the DNS-SD compatibility library.

See `docs/build_linux.md`, `docs/build_macos.md`, and `docs/build_windows.md` for platform-specific dependency and configuration details. Windows uses vcpkg and Bonjour/DNS-SD dependencies.

### Unit tests

The unit test CMake project is rooted at `tests/`. On Linux/macOS, configuration fetches GoogleTest; Windows uses the vcpkg GoogleTest package. Configure, build, and run the suite:

```sh
cmake -B tests/build -S tests -G Ninja
cmake --build tests/build
ctest --test-dir tests/build --output-on-failure
```

Use CTest options such as `-R <pattern>` to select matching tests, or `-V` for verbose output. The test project enables coverage instrumentation with GCC/Clang.

### Integration tests

Integration tests use pytest to run the `heat_pump` and `hems` demo binaries. Build the examples first, then install test dependencies and run from the repository root:

```sh
python3 -m pip install pytest pytest-html
pytest integration_tests/ --ignore=integration_tests/test_close_timing.py
```

Run one module with, for example, `pytest integration_tests/test_lpc.py`. The close-timing test requires debug flags described in `docs/integration_tests.md`; that document also covers certificates and report generation.

### Code style

CI checks formatting with clang-format 18:

```sh
./scripts/codestyle/format_diff.sh -v .
```

See `scripts/codestyle/README.md` for formatter configuration details.
