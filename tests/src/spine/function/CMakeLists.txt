cmake_minimum_required(VERSION 3.15)

set(TEST_NAME function_test)

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
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/cmd.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/datagram.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/filter.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/possible_operations_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/smart_energy_management_ps_cfg.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/operations.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/function/function.c

  # Test helpers
  ${CMAKE_SOURCE_DIR}/src/spine/function_data.c

  cmd_test_data.cpp
  filter_test_data.cpp
  function_data_test_data.cpp

  function_create_read_cmd_test.cpp
  function_create_reply_cmd_test.cpp
  function_create_write_cmd_test.cpp
  function_actuator_level_update_test.cpp
  function_limit_control_create_reply_cmd_test.cpp
  function_limit_control_create_write_cmd_test.cpp
  function_limit_control_update_test.cpp
  smart_energy_management_ps/write_partial/alternatives_id_test.cpp
  smart_energy_management_ps/write_partial/compressor_announce_notify_test.cpp
  smart_energy_management_ps/write_partial/power_sequence_id_test.cpp
  smart_energy_management_ps/write_partial/power_time_slot_number.cpp
  smart_energy_management_ps/write_partial/start_time_test.cpp
  smart_energy_management_ps/write_partial/state_scheduled_test.cpp
  function_update_test.cpp
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
