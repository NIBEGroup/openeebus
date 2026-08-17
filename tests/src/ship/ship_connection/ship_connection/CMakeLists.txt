cmake_minimum_required(VERSION 3.15)

set(TEST_NAME ship_connection_test)

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
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/message_buffer.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_queue/eebus_queue.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_thread/eebus_thread.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_connection/ship_connection.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_connection/client.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_connection/server.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_connection/ship_message_deserialize.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_connection/ship_message_serialize.c

  # Mocks sources
  ${MOCKS_SOURCES_PATH}/common/eebus_timer/eebus_timer_mock.cpp
  ${MOCKS_SOURCES_PATH}/ship/tls_certificate/tls_certificate_mock.cpp
  ${MOCKS_SOURCES_PATH}/ship/websocket/websocket_mock.cpp
  ${MOCKS_SOURCES_PATH}/ship/api/data_reader_mock.cpp
  ${MOCKS_SOURCES_PATH}/ship/api/info_provider_mock.cpp
  ${MOCKS_SOURCES_PATH}/ship/websocket/websocket_creator_mock.cpp

  ship_connection_test_suite.cpp
  ship_connection_test.cpp
  client/cmi/client_send_init_message_test.cpp
  client/cmi/client_wait_for_init_message_test.cpp
  client/cmi/client_evaluate_init_message_test.cpp
  data_exchange/data_exchange_test.cpp
  server/cmi/server_wait_test.cpp
  server/cmi/server_evaluate_init_message_test.cpp
  client/sme_prot_handshake/announce_protocol_version_test.cpp
  client/sme_prot_handshake/select_protocol_version_test.cpp
  server/sme_prot_handshake/sme_prot_handshake_server_init_test.cpp
  server/sme_prot_handshake/sme_prot_handshake_server_listen_proposal_test.cpp
  server/sme_prot_handshake/sme_prot_handshake_server_listen_confirm_test.cpp
  sme_access_methods/sme_access_methods_init_test.cpp
  sme_hello/sme_hello_ready_init_test.cpp
  sme_hello/sme_hello_ready_listen_test.cpp
  sme_hello/sme_hello_abort_test.cpp
  sme_hello/sme_hello_pending_init_test.cpp
  sme_hello/sme_hello_pending_listen_test.cpp
  sme_pin/sme_pin_check_init_test.cpp
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
  cjson
)

add_test(
  NAME
  ${TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${TEST_NAME}
)

gtest_discover_tests(${TEST_NAME})
