cmake_minimum_required(VERSION 3.15)

set(TEST_NAME cs_lpc_test)

project(${TESTS_NAME} LANGUAGES C CXX)

add_executable(${TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

include(${MAIN_PROJ_SOURCES_PATH}/common/eebus_date_time/eebus_date_time_sources.cmake)
include(${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_sources.cmake)

target_sources(
  ${TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  # Main project sources
  ${EEBUS_DATE_TIME_SOURCES}
  ${EEBUS_DATA_SOURCES}
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_device_info.c
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/message_buffer.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_math/eebus_math.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_mutex/eebus_mutex.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_queue/eebus_queue.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_thread/eebus_thread.c
  ${MAIN_PROJ_SOURCES_PATH}/common/service_details.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_lut.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/uint64_lut.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/binding/binding_manager.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/device/data_reader.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/device/device_local.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/device/device_remote.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/device/device.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/device/sender.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/entity/entity_local.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/entity/entity_remote.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/entity/entity.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/events/events.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_address_container.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_local.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_remote.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_functions.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/operations.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_result.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_result_container.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_reply.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_reply_container.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_write_request.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_write_request_container.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature_link/feature_link.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature_link/feature_link_container.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/function/function.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/heartbeat/heartbeat_manager.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/absolute_or_relative_time.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/binding_management_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/cmd.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/datagram.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/device_configuration_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/electrical_connection_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/entity_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/filter.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/function_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/loadcontrol_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/node_management_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/possible_operations_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/specification_version.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/subscription_management_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/usecase_information_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_binding.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_destination_list.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_detailed_discovery.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_remote.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_subscription.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_usecase.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/subscription/subscription_manager.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/actor/cs/lpc/cs_lpc.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/actor/cs/cs_lp_events.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/actor/cs/cs_lp_public.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/actor/cs/cs_lp.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/model/load_limit_types.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/model/scaled_value.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/device_configuration/device_configuration_common.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/device_configuration/device_configuration_server.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/device_diagnosis/device_diagnosis_client.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/device_diagnosis/device_diagnosis_common.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/electrical_connection/electrical_connection_common.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/electrical_connection/electrical_connection_server.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/feature_info_client.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/feature_info_server.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/helper.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/load_control/load_control_client.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/load_control/load_control_common.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/specialization/load_control/load_control_server.c
  ${MAIN_PROJ_SOURCES_PATH}/use_case/use_case.c

  # Mocks sources
  ${MOCKS_SOURCES_PATH}/ship/ship_connection/data_writer_mock.cpp
  ${MOCKS_SOURCES_PATH}/common/eebus_timer/eebus_timer_mock.cpp
  ${MOCKS_SOURCES_PATH}/use_case/api/cs_lp_listener_mock.cpp

  # Test helpers
  ${CMAKE_SOURCE_DIR}/src/spine/function_data.c
  ${CMAKE_SOURCE_DIR}/src/use_case/use_case_test_fixture.cpp

  cs_lpc_test.cpp
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
