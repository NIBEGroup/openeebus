cmake_minimum_required(VERSION 3.15)

set(TEST_NAME ship_pairing_test)

project(${TESTS_NAME} LANGUAGES C CXX)

add_executable(${TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

target_sources(
  ${TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  # Main project sources
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/tls_certificate/tls_certificate.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_pairing/ship_pairing_entry.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_pairing/ship_pairing_digest.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_pairing/ship_pairing.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_pairing/ship_pairing_request.c

  # Mocks. The timer is mocked so the fifteen minutes of section 4.3.1 can be
  # reached without waiting them out, and the certificate so the node's own
  # fingerprint is the one of the Annex A example.
  ${MOCKS_SOURCES_PATH}/common/eebus_timer/eebus_timer_mock.cpp
  ${MOCKS_SOURCES_PATH}/ship/tls_certificate/tls_certificate_mock.cpp

  ship_pairing_test_suite.cpp
  ship_pairing_entry_test.cpp
  ship_pairing_digest_test.cpp
  ship_pairing_evaluator_test.cpp
  ship_pairing_request_test.cpp
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
  ${PROJECT_LINK_OPTIONS}
)

target_link_libraries(
  ${TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_LIBRARIES}
  ${OPEN_SSL_LIBRARIES}
)

add_test(
  NAME
  ${TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${TEST_NAME}
)

gtest_discover_tests(${TEST_NAME})
