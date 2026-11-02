cmake_minimum_required(VERSION 3.15)

set(TEST_NAME eebus_timer_test)

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
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_thread/eebus_thread.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_linux.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_apple.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_timer/eebus_timer_windows.c

  # Mocks sources
  eebus_timer_test.cpp
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
