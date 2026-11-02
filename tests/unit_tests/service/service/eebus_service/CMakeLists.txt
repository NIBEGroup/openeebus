cmake_minimum_required(VERSION 3.15)

set(TEST_NAME eebus_service_test)

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
  ${MAIN_PROJ_SOURCES_PATH}/service/service/eebus_service.c
  ${MAIN_PROJ_SOURCES_PATH}/service/service/eebus_service_config.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_device_info.c
  ${MAIN_PROJ_SOURCES_PATH}/common/service_details.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c

  # Mocks sources
  ${MOCKS_SOURCES_PATH}/ship/ship_node/ship_node_mock.cpp
  ${MOCKS_SOURCES_PATH}/ship/tls_certificate/tls_certificate_mock.cpp
  ${MOCKS_SOURCES_PATH}/service/api/service_reader_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/device/device_local_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/entity/entity_local_mock.cpp

  eebus_service_test.cpp
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
)

add_test(
  NAME
  ${TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${TEST_NAME}
)

gtest_discover_tests(${TEST_NAME})
