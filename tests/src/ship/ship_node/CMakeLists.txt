# Tls Certificate integration test
cmake_minimum_required(VERSION 3.15)

set(TEST_NAME ship_node_test)

project(${TESTS_NAME} LANGUAGES C CXX)

add_executable(${TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
  # Add linker option to exclude LIBCMT
  target_link_options(${TEST_NAME} PRIVATE "/NODEFAULTLIB:LIBCMT")

  # Add Bonjour SDK paths for Windows
  set(BONJOUR_SDK_HOME $ENV{BONJOUR_SDK_HOME})
  set(BONJOUR_LIB_PATH "${BONJOUR_SDK_HOME}/Lib/x64/dnssd.lib")
  if(EXISTS "${BONJOUR_SDK_HOME}" AND EXISTS "${BONJOUR_LIB_PATH}")
    target_include_directories(${TEST_NAME} PRIVATE "${BONJOUR_SDK_HOME}/Include")
    target_link_directories(${TEST_NAME} PUBLIC "${BONJOUR_SDK_HOME}/Lib/x64")
    message(STATUS "Found Bonjour SDK at: ${BONJOUR_SDK_HOME}")
    set(BONJOUR_FOUND TRUE)
  else()
    message(WARNING "Bonjour SDK not found at: ${BONJOUR_SDK_HOME}")
    message(WARNING "Please install Bonjour SDK for Windows from Apple Developer")
    set(BONJOUR_FOUND FALSE)
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

# Find libwebsockets - use pkg-config on Linux/macOS, find_package on Windows (vcpkg)
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
  # PkgConfig is already found in parent tests/CMakeLists.txt
  pkg_check_modules(LIBWEBSOCKETS REQUIRED IMPORTED_TARGET libwebsockets)
  set(LIBWEBSOCKETS_TARGET PkgConfig::LIBWEBSOCKETS)
endif()

# OpenSSL Library (required for tls_certificate.c)
find_package(OpenSSL REQUIRED)
if(APPLE)
  set(OPEN_SSL_LIBRARIES OpenSSL::Crypto)
elseif(WIN32)
  set(OPEN_SSL_LIBRARIES OpenSSL::SSL)
  set(OPEN_SSL_LIBRARIES OpenSSL::Crypto)
else()
  set(OPEN_SSL_LIBRARIES crypto)
endif()

target_sources(
  ${TEST_NAME}
  PRIVATE

  # Main project sources
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_node/ship_node.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/mdns/ship_mdns_bonjour.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/mdns/mdns_entry.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/mdns/mdns_debug.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/tls_certificate/tls_certificate.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_device_info.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_mutex/eebus_mutex.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_queue/eebus_queue.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_thread/eebus_thread.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_lut.c
  ${MAIN_PROJ_SOURCES_PATH}/common/service_details.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_linux.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_apple.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_windows.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/http_server.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket_client_creator.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket_server_creator.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket_client.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket_server.c

  ship_node_test.cpp
)

target_include_directories(
  ${TEST_NAME}
  PRIVATE
  ${PROJECT_INCLUDES_PATH}
)

target_compile_options(
  ${TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_OPTIONS}
)

target_compile_definitions(
  ${TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS}
  MEMORY_LEAKS_TEST
)

target_link_options(
  ${TEST_NAME}
  PRIVATE
)

if(WIN32)
  set(DNS_SD_LIB "dnssd")
elseif (NOT APPLE)
  set(DNS_SD_LIB "dns_sd")
endif()

target_link_libraries(
  ${TEST_NAME}
  PRIVATE
  ${THREAD_LIBRARY}
  ${DNS_SD_LIB}
  ${LIBWEBSOCKETS_TARGET}
  ${OPEN_SSL_LIBRARIES}
)

# ── Multi-connection test ────────────────────────────────────────────────────
set(MULTI_TEST_NAME ship_node_multi_test)

add_executable(${MULTI_TEST_NAME})

if(WIN32)
  set_property(TARGET ${MULTI_TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
  target_link_options(${MULTI_TEST_NAME} PRIVATE "/NODEFAULTLIB:LIBCMT")
  if(BONJOUR_FOUND)
    target_include_directories(${MULTI_TEST_NAME} PRIVATE "${BONJOUR_SDK_HOME}/Include")
    target_link_directories(${MULTI_TEST_NAME} PUBLIC "${BONJOUR_SDK_HOME}/Lib/x64")
  endif()
endif()

target_sources(
  ${MULTI_TEST_NAME}
  PRIVATE
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_node/ship_node.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/mdns/ship_mdns_bonjour.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/mdns/mdns_entry.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/mdns/mdns_debug.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/tls_certificate/tls_certificate.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_device_info.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_mutex/eebus_mutex.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_queue/eebus_queue.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_thread/eebus_thread.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_lut.c
  ${MAIN_PROJ_SOURCES_PATH}/common/service_details.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_linux.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_apple.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_windows.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/http_server.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket_client_creator.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket_server_creator.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket_client.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/websocket/websocket_server.c
  ship_node_multi_test.cpp
)

target_include_directories(${MULTI_TEST_NAME} PRIVATE ${PROJECT_INCLUDES_PATH})

target_compile_options(${MULTI_TEST_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})

target_compile_definitions(
  ${MULTI_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS}
  MEMORY_LEAKS_TEST
)

target_link_libraries(
  ${MULTI_TEST_NAME}
  PRIVATE
  ${THREAD_LIBRARY}
  ${DNS_SD_LIB}
  ${LIBWEBSOCKETS_TARGET}
  ${OPEN_SSL_LIBRARIES}
)
