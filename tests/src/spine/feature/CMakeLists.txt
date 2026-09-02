cmake_minimum_required(VERSION 3.15)

set(TEST_NAME write_approve_test)

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

  # Main project sources needed for feature local interface
  ${EEBUS_DATE_TIME_SOURCES}
  ${EEBUS_DATA_SOURCES}
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_local.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_address_container.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/operations.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/function/function.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/feature_functions.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/filter.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/node_management_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/cmd.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/entity_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/function_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/possible_operations_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/usecase_information_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/specification_version.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/events/events.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_detailed_discovery.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_subscription.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_binding.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_usecase.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/node_management/node_management_destination_list.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c
  ${MAIN_PROJ_SOURCES_PATH}/common/uint64_lut.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_device_info.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_list.c
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_reply.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_reply_container.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_result.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_result_container.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_write_request.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_write_request_container.c

  # Mocks sources
  ${MOCKS_SOURCES_PATH}/spine/device/device_local_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/device/device_remote_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/entity/entity_local_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/device/sender_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/feature/feature_remote_mock.cpp
  ${MOCKS_SOURCES_PATH}/spine/entity/entity_remote_mock.cpp

  # Test file
  write_approve_test.cpp
  write_approve_test_suite.cpp
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

set(PENDING_RESULT_TEST_NAME pending_result_test)

add_executable(${PENDING_RESULT_TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${PENDING_RESULT_TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

target_sources(
  ${PENDING_RESULT_TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  ${EEBUS_DATE_TIME_SOURCES}
  ${EEBUS_DATA_SOURCES}
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/entity_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_result.c

  pending_result_test.cpp
)

target_include_directories(
  ${PENDING_RESULT_TEST_NAME}
  PRIVATE
  ${PROJECT_INCLUDES_PATH}
)

target_compile_options(
  ${PENDING_RESULT_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_OPTIONS}
)

target_compile_definitions(
  ${PENDING_RESULT_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS}
  MEMORY_LEAKS_TEST
)

target_link_options(
  ${PENDING_RESULT_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_OPTIONS}
)

target_link_libraries(
  ${PENDING_RESULT_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_LIBRARIES}
  cjson
)

add_test(
  NAME
  ${PENDING_RESULT_TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${PENDING_RESULT_TEST_NAME}
)

gtest_discover_tests(${PENDING_RESULT_TEST_NAME})

set(PENDING_RESULT_CONTAINER_TEST_NAME pending_result_container_test)

add_executable(${PENDING_RESULT_CONTAINER_TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${PENDING_RESULT_CONTAINER_TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

target_sources(
  ${PENDING_RESULT_CONTAINER_TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  ${EEBUS_DATE_TIME_SOURCES}
  ${EEBUS_DATA_SOURCES}
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/entity_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_result.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_result_container.c

  pending_result_container_test.cpp
)

target_include_directories(
  ${PENDING_RESULT_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_INCLUDES_PATH}
)

target_compile_options(
  ${PENDING_RESULT_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_OPTIONS}
)

target_compile_definitions(
  ${PENDING_RESULT_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS}
  MEMORY_LEAKS_TEST
)

target_link_options(
  ${PENDING_RESULT_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_OPTIONS}
)

target_link_libraries(
  ${PENDING_RESULT_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_LIBRARIES}
  cjson
)

add_test(
  NAME
  ${PENDING_RESULT_CONTAINER_TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${PENDING_RESULT_CONTAINER_TEST_NAME}
)

gtest_discover_tests(${PENDING_RESULT_CONTAINER_TEST_NAME})

set(PENDING_REPLY_TEST_NAME pending_reply_test)

add_executable(${PENDING_REPLY_TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${PENDING_REPLY_TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

target_sources(
  ${PENDING_REPLY_TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  ${EEBUS_DATE_TIME_SOURCES}
  ${EEBUS_DATA_SOURCES}
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/entity_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_reply.c

  pending_reply_test.cpp
)

target_include_directories(
  ${PENDING_REPLY_TEST_NAME}
  PRIVATE
  ${PROJECT_INCLUDES_PATH}
)

target_compile_options(
  ${PENDING_REPLY_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_OPTIONS}
)

target_compile_definitions(
  ${PENDING_REPLY_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS}
  MEMORY_LEAKS_TEST
)

target_link_options(
  ${PENDING_REPLY_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_OPTIONS}
)

target_link_libraries(
  ${PENDING_REPLY_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_LIBRARIES}
  cjson
)

add_test(
  NAME
  ${PENDING_REPLY_TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${PENDING_REPLY_TEST_NAME}
)

gtest_discover_tests(${PENDING_REPLY_TEST_NAME})

set(PENDING_REPLY_CONTAINER_TEST_NAME pending_reply_container_test)

add_executable(${PENDING_REPLY_CONTAINER_TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${PENDING_REPLY_CONTAINER_TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

target_sources(
  ${PENDING_REPLY_CONTAINER_TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  ${EEBUS_DATE_TIME_SOURCES}
  ${EEBUS_DATA_SOURCES}
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/vector.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/entity_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/feature_types.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/model/model.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_reply.c
  ${MAIN_PROJ_SOURCES_PATH}/spine/feature/pending_reply_container.c

  pending_reply_container_test.cpp
)

target_include_directories(
  ${PENDING_REPLY_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_INCLUDES_PATH}
)

target_compile_options(
  ${PENDING_REPLY_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_OPTIONS}
)

target_compile_definitions(
  ${PENDING_REPLY_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS}
  MEMORY_LEAKS_TEST
)

target_link_options(
  ${PENDING_REPLY_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_OPTIONS}
)

target_link_libraries(
  ${PENDING_REPLY_CONTAINER_TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_LIBRARIES}
  cjson
)

add_test(
  NAME
  ${PENDING_REPLY_CONTAINER_TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${PENDING_REPLY_CONTAINER_TEST_NAME}
)

gtest_discover_tests(${PENDING_REPLY_CONTAINER_TEST_NAME})
