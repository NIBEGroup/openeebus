cmake_minimum_required(VERSION 3.15)

set(TEST_NAME node_connection_test)

project(${TESTS_NAME} LANGUAGES C CXX)

add_executable(${TEST_NAME})

if(WIN32)
  set_property(TARGET ${TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

target_sources(
  ${TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  # Under-test sources
  ${MAIN_PROJ_SOURCES_PATH}/ship/ship_node/node_connection.c
  ${MAIN_PROJ_SOURCES_PATH}/common/service_details.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c

  # Mock sources
  ${MOCKS_SOURCES_PATH}/common/eebus_timer/eebus_timer_mock.cpp
  ${MOCKS_SOURCES_PATH}/ship/ship_connection/ship_connection_mock.cpp

  node_connection_test.cpp
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
)

add_test(
  NAME
  ${TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${TEST_NAME}
)

gtest_discover_tests(${TEST_NAME})
