cmake_minimum_required(VERSION 3.15)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common/eebus_data
    ${EXECUTABLE_OUTPUT_PATH}/common/eebus_data)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common/eebus_date_time
    ${EXECUTABLE_OUTPUT_PATH}/common/eebus_date_time)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common/eebus_queue
    ${EXECUTABLE_OUTPUT_PATH}/common/eebus_queue)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common/eebus_timer
    ${EXECUTABLE_OUTPUT_PATH}/common/eebus_timer)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common/string_lut
    ${EXECUTABLE_OUTPUT_PATH}/common/string_lut)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common/string_util
    ${EXECUTABLE_OUTPUT_PATH}/common/string_util)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ship/ship_connection/ship_connection
    ${EXECUTABLE_OUTPUT_PATH}/ship/ship_connection/ship_connection)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/service/service/eebus_service_config
    ${EXECUTABLE_OUTPUT_PATH}/service/service/eebus_service_config)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/service/service/eebus_service
   ${EXECUTABLE_OUTPUT_PATH}/service/eebus_service)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ship/mdns/mdns_entry
    ${EXECUTABLE_OUTPUT_PATH}/ship/mdns/mdns_entry)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ship/ship_connection/ship_message_serialize
    ${EXECUTABLE_OUTPUT_PATH}/ship/ship_connection/ship_message_serialize)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ship/ship_connection/ship_message_deserialize
    ${EXECUTABLE_OUTPUT_PATH}/ship/ship_connection/ship_message_deserialize)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spine/model/datagram_header
    ${EXECUTABLE_OUTPUT_PATH}/spine/model/datagram_header)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spine/model/datagram_payload
    ${EXECUTABLE_OUTPUT_PATH}/spine/model/datagram_payload)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spine/model/function_data
    ${EXECUTABLE_OUTPUT_PATH}/spine/model/function_data)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spine/feature
    ${EXECUTABLE_OUTPUT_PATH}/spine/feature)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spine/events
    ${EXECUTABLE_OUTPUT_PATH}/spine/events)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spine/function
    ${EXECUTABLE_OUTPUT_PATH}/spine/function)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spine/node_management/node_management_remote
    ${EXECUTABLE_OUTPUT_PATH}/spine/node_management/node_management_remote)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spine/device/sender
    ${EXECUTABLE_OUTPUT_PATH}/spine/device/sender)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ship/ship_node
    ${EXECUTABLE_OUTPUT_PATH}/ship/ship_node)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/cs/lpc
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/cs/lpc)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/cs/lpp
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/cs/lpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/eg/lpc
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/eg/lpc)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/eg/lpc/two_remote
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/eg/lpc/two_remote)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/eg/lpp
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/eg/lpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/gcp/mgcp
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/gcp/mgcp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/ma/mgcp
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/ma/mgcp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/ma/mpc
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/ma/mpc)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/ma/two_remote
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/ma/two_remote)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/actor/mu/mpc
    ${EXECUTABLE_OUTPUT_PATH}/use_case/actor/mu/mpc)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/model/scaled_value
    ${EXECUTABLE_OUTPUT_PATH}/use_case/model/scaled_value)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/use_case/specialization/helper
    ${EXECUTABLE_OUTPUT_PATH}/use_case/specialization/helper)
