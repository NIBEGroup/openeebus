# Unit tests project for EEBUS library

cmake_minimum_required(VERSION 3.15)

set(TESTS_NAME eebus_tests)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Specify suffix depending on OS type
if(SYSTEM_WINDOWS)
  set(CMAKE_EXECUTABLE_SUFFIX ".exe")
elseif(SYSTEM_UNIX OR SYSTEM_APPLE)
  set(CMAKE_EXECUTABLE_SUFFIX "")
endif()

project(${TESTS_NAME} LANGUAGES C CXX)

if(CMAKE_CROSSCOMPILING)
  set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
endif()

include(GoogleTest)
include(GNUInstallDirs)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include(FetchContent)

if(WIN32)
    # Set vcpkg installation paths
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
    set(VCPKG_INSTALLED "${VCPKG_ROOT}/installed/x64-windows")
    message(STATUS "Using vcpkg installed libraries from: ${VCPKG_INSTALLED}")

    # Add vcpkg to CMAKE_PREFIX_PATH so find_package can locate packages
    list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALLED}")
else()
    # Use pkg-config on Linux/macOS for finding libraries
    find_package(PkgConfig REQUIRED)
endif()

if (NOT WIN32)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
  )
  
  # Use ccache if it's installed
  find_program(CCACHE_FOUND ccache)
  if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
  endif(CCACHE_FOUND)
  
  FetchContent_MakeAvailable(googletest)
else()
  # For Windows, use the pre-installed GTest from vcpkg
  find_package(GTest CONFIG REQUIRED)
endif()

# Find cJSON - use pkg-config on Linux/macOS, find_package on Windows (vcpkg)
if(WIN32)
  find_package(cJSON REQUIRED)
else()
  pkg_check_modules(CJSON REQUIRED IMPORTED_TARGET libcjson)
  # Create alias so existing 'cjson' references in subdirectories work
  add_library(cjson ALIAS PkgConfig::CJSON)
endif()

# OpenSSL, for the suites covering the OpenSSL certificate backend.
find_package(OpenSSL REQUIRED)
set(OPEN_SSL_LIBRARIES OpenSSL::SSL OpenSSL::Crypto)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

set(MAIN_PROJ_SOURCES_PATH ${CMAKE_SOURCE_DIR}/../src)
set(MOCKS_SOURCES_PATH ${CMAKE_SOURCE_DIR}/src/mocks)

set(PROJECT_INCLUDES_PATH
  ${GTestIncludes}
  ${CMAKE_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mocks
)

set(GTEST_SOURCES
  ${GTestFiles}
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(PROJECT_COMPILE_OPTIONS
    -Wno-psabi
    -Wall
    -Wextra
    -Wno-missing-field-initializers
    -Werror
    --coverage
    -fprofile-arcs
  )
else()
  # MSVC-specific compile options
  set(PROJECT_COMPILE_OPTIONS
    /W3
  )
endif()

set(PROJECT_COMPILE_DEFINITIONS
  # Gtest definitions
  GTEST_HAS_PTHREAD=1
  GTEST
)

if (WIN32) # MSVC-specific definitions
  list(APPEND PROJECT_COMPILE_DEFINITIONS
    _CRT_SECURE_NO_WARNINGS=1
    GTEST_OS_WINDOWS_MINGW=0
  )
endif()

if (NOT APPLE)
  set(GCOV "-lgcov")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(PROJECT_LINK_OPTIONS
    --coverage
    ${GCOV}
  )
else()
  # MSVC doesn't need coverage link options
  set(PROJECT_LINK_OPTIONS
    /NODEFAULTLIB:LIBCMT
  )
endif()

set(PROJECT_LINK_LIBRARIES
  GTest::gmock_main
)

include(GoogleTest)

enable_testing()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CTEST_ARGS -V CACHE STRING "ctest arguments")
set(GENHTML_ARGS CACHE STRING "genhtml arguments")
set(LCOVRC_ARGS --rc lcov_branch_coverage=1 CACHE STRING "lcovrc arguments")

set(LCOV lcov --gcov-tool=$ENV{CROSS_COMPILE}gcov ${LCOVRC_ARGS})
set(CMAKE_CTEST_ARGUMENTS ${CTEST_ARGS})

# Note: enable_testing() added standard "test" target to run unit tests.
# You can use it it if you want to (re-)execute unit tests. Unfortunately there
# is no easy way to add a dependency on test executable for "test" target,
# hence you might have to (re-)build unit tests explicitly before executing
# "test" target. The following "run_tests" target comparing to "test" target
# adds proper dependency on the test executable as well as some Qt
# Creator-specific workarounds.
add_custom_target(run_tests
  DEPENDS all
  COMMAND ctest ${CMAKE_CTEST_ARGUMENTS}
  USES_TERMINAL # print to console without buffering
  SOURCES CMakeLists.txt # dummy value just to show this target in Qt Creator project window
)

# The following commands are dedicated to unit testing coverage measurement.
# Note: we want to re-execute unit tests for coverage measurement only if unit
# tests executable has been updated. If unit tests executable didn't change
# since previous coverage measurement, assume coverage information didn't
# change as well (it is still valid and doesn't need to be updated).
add_custom_command(
  DEPENDS all
  COMMENT "ctest ${CMAKE_CTEST_ARGUMENTS}"
  COMMAND ctest ${CMAKE_CTEST_ARGUMENTS}
  COMMAND touch .unit_tests_executed
  OUTPUT .unit_tests_executed
  USES_TERMINAL # print to console without buffering
)

add_custom_command(
  DEPENDS .unit_tests_executed
  COMMENT "lcov: COVERAGE(unit_tests) -> coverage_all.info"
  COMMAND ${LCOV} --capture --directory . -o coverage_all.info
  OUTPUT coverage_all.info
  USES_TERMINAL # print to console without buffering
)
add_custom_target(coverage_all_info
  DEPENDS coverage_all.info
  COMMENT "lcov --summary coverage_all.info -> stdout"
  COMMAND ${LCOV} --summary coverage_all.info
  USES_TERMINAL # print to console without buffering
  SOURCES CMakeLists.txt # dummy value just to show this target in Qt Creator project window
)

add_custom_command(
  DEPENDS coverage_all.info
  COMMENT "lcov: FILTER(coverage_all.info) -> coverage.info"
  COMMAND ${LCOV} --extract coverage_all.info -o coverage_tmp.info
                  '*src/*'
  COMMAND ${LCOV} --remove coverage_tmp.info -o coverage_tmp.info
                  '*/tests/*'
                  '*/googletest/*'
                  '*/googlemock/*'
  COMMAND mv coverage_tmp.info coverage.info
  OUTPUT coverage.info
  USES_TERMINAL # print to console without buffering
)
add_custom_target(coverage_info
  DEPENDS coverage.info
  COMMENT "lcov --summary coverage.info -> stdout"
  COMMAND ${LCOV} --summary coverage.info
  USES_TERMINAL # print to console without buffering
  SOURCES CMakeLists.txt # dummy value just to show this target in Qt Creator project window
)

add_custom_command(
  DEPENDS coverage.info
  COMMENT "lcov --summary coverage.info -> coverage.md"
  COMMAND echo 'Code coverage summary for the project.' > tmp-coverage.md
  COMMAND echo '|Type|Percentage|Details|' >> tmp-coverage.md
  COMMAND echo '|---|---|---|' >> tmp-coverage.md
  COMMAND ${LCOV} --summary coverage.info | tail -n +3 | sed
                  -e 's-^ -|-g'
                  -e 's-[\.]*: - | -g'
                  -e 's-% -% | -g'
                  -e 's-\)-\) |-g'
                  >> tmp-coverage.md
  COMMAND mv tmp-coverage.md coverage.md
  OUTPUT coverage.md
  USES_TERMINAL # print to console without buffering
)
add_custom_target(coverage_md
  DEPENDS coverage.md
  COMMENT "cat coverage.md -> stdout"
  COMMAND cat coverage.md
  USES_TERMINAL # print to console without buffering
  SOURCES CMakeLists.txt # dummy value just to show this target in Qt Creator project window
)

add_custom_command(
  DEPENDS coverage.info
  COMMENT "genhtml coverage.info -> html/*"
  COMMAND rm -rf html
  COMMAND genhtml ${LCOVRC_ARGS} ${GENHTML_ARGS} coverage.info -o html
  COMMAND touch .coverage_html_generated
  OUTPUT .coverage_html_generated
  USES_TERMINAL # print to console without buffering
)
add_custom_target(coverage_html
  DEPENDS .coverage_html_generated
  SOURCES CMakeLists.txt # dummy value just to show this target in Qt Creator project window
)
