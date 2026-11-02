# Testing

Install the platform dependencies using the [Linux](build_linux.md),
[macOS](build_macos.md), or [Windows](build_windows.md) build guide first.
Run the commands below from the repository root.

## Test layout

The test suites are grouped by type:

```text
tests/
├── CMakeLists.txt
├── unit_tests/
│   ├── CMakeLists.txt
│   ├── common/
│   ├── service/
│   ├── ship/
│   ├── spine/
│   ├── use_case/
│   └── mocks/
└── integration_tests/
    ├── CMakeLists.txt
    ├── conftest.py
    ├── requirements.txt
    └── test_*.py
```

`unit_tests/` contains the GoogleTest/GoogleMock tests, grouped by production module.
Shared helpers are directly under `unit_tests/`; module-specific fixtures and test data
are beside their tests. Test doubles are under `unit_tests/mocks/`.

`integration_tests/` contains pytest scenarios that run the demo applications as separate
processes.

## Unit tests

From the repository root, configure and run the standalone unit suite:

```sh
cmake -S tests -B tests/build -G Ninja
cmake --build tests/build
ctest --test-dir tests/build --output-on-failure
```

To run a focused test target:

```sh
ctest --test-dir tests/build -R '^string_util_test$' --output-on-failure
```

Unit tests are also included in the root build when `OPTION_OPENEEBUS_TESTS=ON`
(the default).

For a multi-configuration generator such as Visual Studio, select the same
configuration when building and running tests:

```sh
cmake --build tests/build --config Debug
ctest --test-dir tests/build -C Debug --output-on-failure
```

The configure examples above use Ninja, a single-configuration generator.

## Integration tests

Integration tests require the root build and its demo applications. A standalone
unit-test build does not register integration tests.

Configure integration-test support, build the library, demos, and unit tests,
then run all registered tests:

```sh
cmake -S . -B build -G Ninja -DOPTION_OPENEEBUS_TESTS=ON -DOPTION_INTEGRATION_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

For a multi-configuration build, use `cmake --build build --config Debug` and add
`-C Debug` to each `ctest --test-dir build` command.

To run only integration tests:

```sh
ctest --test-dir build -L integration --output-on-failure
```

See [Integration Tests](integration_tests.md) for dependencies, configuration,
and direct pytest commands. Generated binaries and reports belong in the build
directories.
