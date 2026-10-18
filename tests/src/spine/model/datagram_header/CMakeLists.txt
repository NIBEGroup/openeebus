cmake_minimum_required(VERSION 3.15)

set(TEST_NAME datagram_header_test)

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

  ${MAIN_PROJ_SOURCES_PATH}/spine/model/datagram.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/entity_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/function_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/possible_operations_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/smart_energy_management_ps_cfg.c

  # Test helpers
  ${CMAKE_SOURCE_DIR}/src/string_ptr.cpp
  ${CMAKE_SOURCE_DIR}/src/spine/model/feature_address_test_data.cpp

  datagram_header_test.cpp
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
