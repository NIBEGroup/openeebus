cmake_minimum_required(VERSION 3.15)

set(TEST_NAME node_management_remote_test)

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
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/events/events.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/cmd.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/filter.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/possible_operations_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/node_management_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/usecase_information_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/operations.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/function/function.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_functions.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_remote.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_remote.c

  # Mocks sources
  ${MOCKS_SOURCES_PATH}/spine/device/device_local_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/device/device_remote_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/entity/entity_remote_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/events/event_handler_mock.cpp

  # Test helpers
  ${CMAKE_SOURCE_DIR}/src/spine/function_data.c
  ${CMAKE_SOURCE_DIR}/src/spine/function/filter_test_data.cpp
  ${CMAKE_SOURCE_DIR}/src/spine/function/function_data_test_data.cpp

  node_management_remote_test.cpp
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
