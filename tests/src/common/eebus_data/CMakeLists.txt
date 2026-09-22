cmake_minimum_required(VERSION 3.15)

project(${TESTS_NAME} LANGUAGES C CXX)

include(${MAIN_PROJ_SOURCES_PATH}/common/eebus_date_time/eebus_date_time_sources.cmake)
include(${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_sources.cmake)

# ---------------------------------------------------------------------------
# eebus_data_test — core list and data structure tests
# ---------------------------------------------------------------------------

set(TEST_NAME eebus_data_test)

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
  ${EEBUS_DATE_TIME_SOURCES}
  ${EEBUS_DATA_SOURCES}
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c

  # Test helpers
  ${CMAKE_SOURCE_DIR}/src/string_ptr.cpp

  employee.c
  person.c
  somebody.c
  info.c
  address_test_data.cpp
  eebus_data_choice_test.cpp
  eebus_data_employee_test.cpp
  eebus_data_list_test.cpp
  eebus_data_person_test.cpp
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

# ---------------------------------------------------------------------------
# eebus_data_list_max_elements_test — boundary tests for EEBUS_DATA_LIST_MAX_ELEMENTS_NUM
# ---------------------------------------------------------------------------

set(MAX_ELEMENTS_TEST_NAME eebus_data_list_max_elements_test)

add_executable(${MAX_ELEMENTS_TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${MAX_ELEMENTS_TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

target_sources(
  ${MAX_ELEMENTS_TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  # Main project sources
  ${EEBUS_DATE_TIME_SOURCES}
  ${EEBUS_DATA_SOURCES}
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c

  # Test helpers
  ${CMAKE_SOURCE_DIR}/src/string_ptr.cpp

  employee.c
  eebus_data_list_max_elements_test.cpp
)

target_include_directories(
  ${MAX_ELEMENTS_TEST_NAME}
  PRIVATE
  ${PROJECT_INCLUDES_PATH}
)

target_compile_options(
  ${MAX_ELEMENTS_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_OPTIONS}
)

target_compile_definitions(
  ${MAX_ELEMENTS_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS}
  EEBUS_DATA_LIST_MAX_ELEMENTS_NUM=25
)

target_link_options(
  ${MAX_ELEMENTS_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_OPTIONS}
)

target_link_libraries(
  ${MAX_ELEMENTS_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_LIBRARIES}
  cjson
)

add_test(
  NAME
  ${MAX_ELEMENTS_TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${MAX_ELEMENTS_TEST_NAME}
)

gtest_discover_tests(${MAX_ELEMENTS_TEST_NAME})
