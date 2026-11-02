cmake_minimum_required(VERSION 3.15)

set(TEST_NAME ship_message_deserialize_test)

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
  ${MAIN_PROJ_SOURCES_PATH}/common/message_buffer.c
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_connection/ship_message_deserialize.c

  data_deserialize_test.cpp
  sme_close_deserialize_test.cpp
  sme_connection_access_methods_deserialize_test.cpp
  sme_connection_access_methods_request_deserialize_test.cpp
  sme_connection_pin_error_deserialize_test.cpp
  sme_connection_pin_error_deserialize_test.cpp
  sme_connection_pin_state_deserialize_test.cpp
  sme_connection_pin_input_deserialize_test.cpp
  sme_hello_deserialize_test.cpp
  sme_protocol_handshake_deserialize_test.cpp
  sme_protocol_handshake_error_deserialize_test.cpp
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
  cjson
)

add_test(
  NAME
  ${TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${TEST_NAME}
)

gtest_discover_tests(${TEST_NAME})
