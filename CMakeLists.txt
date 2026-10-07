# EEBUS library main CMake file

cmake_minimum_required (VERSION 3.15)

set(PROJECT_NAME "openeebus")

# Set consistent runtime library for all targets on Windows BEFORE project()
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

include(ProcessorCount)
ProcessorCount(PROCESSORS_NUM)

set(CMAKE_ECLIPSE_MAKE_ARGUMENTS ${CMAKE_ECLIPSE_MAKE_ARGUMENTS} "--no-print-directory -j${PROCESSORS_NUM} -r -s" CACHE STRING "Make parameters")

project(${PROJECT_NAME} LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(OPTION_ASAN "Enable AddressSanitizer" OFF)
option(OPTION_MDNS_USE_AVAHI_CLIENT "Enable mDNS avahi-client implementation" OFF)

set(PROJECT_COMPILE_OPTIONS "")

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    list(APPEND PROJECT_COMPILE_OPTIONS -Wall -Wextra -Werror)

    if(OPTION_ASAN)
        message(STATUS "AddressSanitizer enabled")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g3")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    endif()
elseif(OPTION_ASAN)
    message(WARNING "AddressSanitizer is only supported with GCC and Clang compilers.")
endif()

# TODO: Change STATIC to SHARED when libeebus is a part of SDK
add_library(${PROJECT_NAME} STATIC)

if(WIN32)
    # Set vcpkg installation paths
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
    set(VCPKG_INSTALLED "${VCPKG_ROOT}/installed/x64-windows")
    message(STATUS "Using vcpkg installed libraries from: ${VCPKG_INSTALLED}")

    # Add vcpkg to CMAKE_PREFIX_PATH so find_package can locate packages
    list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALLED}")

    # Set proper runtime library for Windows to avoid LIBCMT conflicts
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

    # Add Bonjour SDK paths for Windows
    set(BONJOUR_SDK_HOME $ENV{BONJOUR_SDK_HOME})
    set(BONJOUR_LIB_PATH "${BONJOUR_SDK_HOME}/Lib/x64/dnssd.lib")
    if(EXISTS "${BONJOUR_SDK_HOME}" AND EXISTS "${BONJOUR_LIB_PATH}")
        target_include_directories(${PROJECT_NAME} PRIVATE "${BONJOUR_SDK_HOME}/Include")
        target_link_directories(${PROJECT_NAME} PUBLIC "${BONJOUR_SDK_HOME}/Lib/x64")
        message(STATUS "Found Bonjour SDK at: ${BONJOUR_SDK_HOME}")
        set(BONJOUR_FOUND TRUE)
    else()
        # Try vcpkg-installed mdnsresponder as fallback
        find_library(DNSSD_LIBRARY
            NAMES dnssd
            PATHS "${VCPKG_INSTALLED}/lib"
            NO_DEFAULT_PATH)
        find_path(DNSSD_INCLUDE_DIR
            NAMES dns_sd.h
            PATHS "${VCPKG_INSTALLED}/include"
            NO_DEFAULT_PATH)
        if(DNSSD_LIBRARY AND DNSSD_INCLUDE_DIR)
            target_include_directories(${PROJECT_NAME} PRIVATE "${DNSSD_INCLUDE_DIR}")
            target_link_directories(${PROJECT_NAME} PUBLIC "${VCPKG_INSTALLED}/lib")
            message(STATUS "Found dnssd via vcpkg: ${DNSSD_LIBRARY}")
            set(BONJOUR_FOUND TRUE)
        else()
            message(WARNING "Bonjour SDK not found at: ${BONJOUR_SDK_HOME}")
            message(FATAL_ERROR "Please install Bonjour SDK or mdnsresponder via vcpkg")
            set(BONJOUR_FOUND FALSE)
        endif()
    endif()

    # Try to find pthreads via vcpkg with more specific library names
    find_library(PTHREAD_LIBRARY
        NAMES pthreadVCE32 pthreadVCE3 pthreadVCE3 pthread
        PATHS "${VCPKG_INSTALLED}/lib" "${VCPKG_INSTALLED}/debug/lib"
        NO_DEFAULT_PATH)

    find_path(PTHREAD_INCLUDE_DIR
        NAMES pthread.h
        PATHS "${VCPKG_INSTALLED}/include"
        NO_DEFAULT_PATH)

    if(PTHREAD_LIBRARY AND PTHREAD_INCLUDE_DIR)
        message(STATUS "Found pthread library: ${PTHREAD_LIBRARY}")
        message(STATUS "Found pthread include: ${PTHREAD_INCLUDE_DIR}")
        add_library(pthread_imported STATIC IMPORTED)
        set_target_properties(pthread_imported PROPERTIES IMPORTED_LOCATION "${PTHREAD_LIBRARY}")
        target_include_directories(pthread_imported INTERFACE "${PTHREAD_INCLUDE_DIR}")
        set(THREAD_LIBRARY pthread_imported)
    else()
        message(ERROR "Could not find pthread library!")
    endif()
endif()

# Find dependencies - use pkg-config on Linux/macOS, find_package on Windows (vcpkg)
if(NOT WIN32)
  find_package(PkgConfig REQUIRED)
endif()

# libwebsockets Library
if(WIN32)
  find_package(libwebsockets CONFIG REQUIRED)
  # Pick the correct libwebsockets target depending on how it was built
  if(TARGET websockets_shared)
    set(LIBWEBSOCKETS_TARGET websockets_shared)
  elseif(TARGET websockets)
    set(LIBWEBSOCKETS_TARGET websockets)
  elseif(TARGET libwebsockets::websockets_shared)
    set(LIBWEBSOCKETS_TARGET libwebsockets::websockets_shared)
  elseif(TARGET libwebsockets::websockets)
    set(LIBWEBSOCKETS_TARGET libwebsockets::websockets)
  else()
    message(FATAL_ERROR "libwebsockets found, but no known CMake target exists (websockets/websockets_shared).")
  endif()
else()
  pkg_check_modules(LIBWEBSOCKETS REQUIRED IMPORTED_TARGET libwebsockets)
  set(LIBWEBSOCKETS_TARGET PkgConfig::LIBWEBSOCKETS)
endif()

find_package(OpenSSL REQUIRED)
if (APPLE)
  set(OPEN_SSL_LIBRARIES ${OPEN_SSL_LIBRARIES} OpenSSL::Crypto)
elseif(WIN32)
  # For Windows, we need to link both SSL and Crypto libraries
  # to ensure all OpenSSL functionality is available
  set(OPEN_SSL_LIBRARIES ${OPEN_SSL_LIBRARIES} OpenSSL::SSL)
  set(OPEN_SSL_LIBRARIES ${OPEN_SSL_LIBRARIES} OpenSSL::Crypto)
else()
  set(OPEN_SSL_LIBRARIES ${OPEN_SSL_LIBRARIES} crypto)
endif()

# cJSON Library
if(WIN32)
  find_package(cJSON REQUIRED)
  set(CJSON_LINK_TARGET cjson)
else()
  pkg_check_modules(CJSON REQUIRED IMPORTED_TARGET libcjson)
  set(CJSON_LINK_TARGET PkgConfig::CJSON)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

message(STATUS "TARGET_COMPILE_OPTIONS    " ${TARGET_COMPILE_OPTIONS})
message(STATUS "TARGET_DEFINITION_OPTIONS " ${TARGET_DEFINITION_OPTIONS})
message(STATUS "TARGET_LINK_OPTIONS       " ${TARGET_LINK_OPTIONS})

include(sources.cmake)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    ${SOURCES}
    ${HEADERS}
)

# Add Windows-specific applink.c to have the OPENSSL_Applink() available (required for OpenSSL on Windows)
if(WIN32)
  target_sources(
    ${PROJECT_NAME}
    PUBLIC
      src/ship/tls_certificate/applink.c
  )
endif()

set(OPENEEBUS_EXTRA_INCLUDES "" CACHE STRING "Additional include directories injected by the product")

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
    ${OPENEEBUS_EXTRA_INCLUDES}
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
    ${PROJECT_COMPILE_OPTIONS}
)

# Warn if there are unresolved symbols in libeebus.
# There should not be any unresolved symbols, and we don't want to
# wait until the executable is linked to find out.
if(WIN32)
  # Windows-specific linker options to avoid runtime library conflicts
  target_link_options(
    ${PROJECT_NAME}
    PRIVATE
      "/NODEFAULTLIB:LIBCMT"
  )
else()
  target_link_options(
    ${PROJECT_NAME}
    PRIVATE
      "-Wl,--no-undefined"
  )
endif()

# mDNS/DNS-SD Library
if(WIN32)
  # Windows uses Bonjour SDK (checked earlier in the file)
  set(DNS_SD_LIB "dnssd")
elseif(APPLE)
  # macOS has native mDNS support, no extra library needed
  set(DNS_SD_LIB "")
else()
  if (OPTION_MDNS_USE_AVAHI_CLIENT)
    # Linux uses Avahi client library
    pkg_check_modules(AVAHI REQUIRED IMPORTED_TARGET avahi-client)
    set(DNS_SD_LIB PkgConfig::AVAHI)
  else()
    # Linux uses libavahi-compat-libdnssd
    find_library(DNS_SD_LIBRARY NAMES dns_sd)
    if(NOT DNS_SD_LIBRARY)
      message(FATAL_ERROR "dns_sd library not found. Install libavahi-compat-libdnssd-dev (Debian/Ubuntu) or avahi-compat-libdns_sd-devel (Fedora/RHEL)")
    endif()
    set(DNS_SD_LIB ${DNS_SD_LIBRARY})
  endif()
endif()

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
    ${THREAD_LIBRARY}
    ${CJSON_LINK_TARGET}
    ${LIBWEBSOCKETS_TARGET}
    ${LIBWEBSOCKETS_DEP_LIBS}
    ${DNS_SD_LIB}
    ${OPEN_SSL_LIBRARIES}
)

add_subdirectory(examples/heat_pump)
add_subdirectory(examples/hems)
add_subdirectory(examples/ev_charger)

# Integration tests — only when building in CI (ENV{CI} is set by GitHub Actions)
if(DEFINED ENV{CI} AND NOT WIN32)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)

  set(ITEST_VENV_DIR "${CMAKE_BINARY_DIR}/itest_venv")
  set(ITEST_PYTHON   "${ITEST_VENV_DIR}/bin/python")
  set(ITEST_PIP      "${ITEST_VENV_DIR}/bin/pip")

  # Stamp file prevents re-creating the venv on every build.
  add_custom_command(
    OUTPUT  "${ITEST_VENV_DIR}/stamp"
    COMMAND ${Python3_EXECUTABLE} -m venv "${ITEST_VENV_DIR}"
    COMMAND "${ITEST_PIP}" install --quiet pytest pytest-html
    COMMAND ${CMAKE_COMMAND} -E touch "${ITEST_VENV_DIR}/stamp"
    COMMENT "Setting up Python venv for integration tests"
    VERBATIM
  )

  add_custom_target(itest_venv DEPENDS "${ITEST_VENV_DIR}/stamp")

  add_custom_target(integration-tests
    COMMAND "${ITEST_PYTHON}" -m pytest "${CMAKE_SOURCE_DIR}/integration_tests" -v
    DEPENDS heat_pump hems itest_venv
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/integration_tests"
    USES_TERMINAL
    COMMENT "Running integration tests"
  )
endif()
