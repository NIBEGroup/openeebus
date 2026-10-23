# pytest-based end-to-end tests for the heat_pump, hems, and ev_charger demos

if(NOT OPTION_INTEGRATION_TESTS)
  return()
endif()

# The close-timing and SIMOPEN modules consume debug output from the library.
target_compile_definitions(openeebus PRIVATE EEBUS_SERVICE_DEBUG=1 SHIP_NODE_DEBUG=1)

find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)

set(ITEST_VENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/itest_venv")
set(ITEST_REQUIREMENTS "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")
if(WIN32)
  set(ITEST_PYTHON "${ITEST_VENV_DIR}/Scripts/python.exe")
else()
  set(ITEST_PYTHON "${ITEST_VENV_DIR}/bin/python")
endif()

if(NOT EXISTS "${ITEST_PYTHON}")
  message(STATUS "Creating integration test venv at ${ITEST_VENV_DIR}")
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m venv "${ITEST_VENV_DIR}"
    RESULT_VARIABLE ITEST_VENV_RESULT
  )
  if(NOT ITEST_VENV_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to create integration test venv")
  endif()

  message(STATUS "Installing integration test requirements")
  execute_process(
    COMMAND "${ITEST_PYTHON}" -m pip install --quiet -r "${ITEST_REQUIREMENTS}"
    RESULT_VARIABLE ITEST_PIP_RESULT
  )
  if(NOT ITEST_PIP_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to install integration test requirements")
  endif()
endif()

set(ITEST_PORT_BASE 4710 CACHE STRING "First HEMS port for CTest integration tests")
set(ITEST_INDEX 0)
# The list of integration tests to be executed. Add or remove test names as needed.
set(ITEST_LIST
  lpc
  lpp
  mpc
  mgcp
  simopen_stress
  hems_hp_ev
  hems_hp_ev_stress
  close_timing
)

foreach(ITEST_NAME ${ITEST_LIST})
  math(EXPR ITEST_HEMS_PORT "${ITEST_PORT_BASE} + (${ITEST_INDEX} * 10)")
  math(EXPR ITEST_HP_PORT "${ITEST_HEMS_PORT} + 2")
  math(EXPR ITEST_EV_PORT "${ITEST_HEMS_PORT} + 4")

  set(ITEST_LABELS integration)
  set(ITEST_TIMEOUT 600)
  if(ITEST_NAME MATCHES "stress$")
    list(APPEND ITEST_LABELS stress)
  endif()
  if(ITEST_NAME MATCHES "^hems_hp_ev")
    list(APPEND ITEST_LABELS three_node)
  endif()
  if(ITEST_NAME STREQUAL "hems_hp_ev_stress")
    set(ITEST_TIMEOUT 1800)
  endif()
  if(ITEST_NAME STREQUAL "close_timing")
    list(APPEND ITEST_LABELS debug_flags)
    set(ITEST_TIMEOUT 1800)
  endif()

  add_test(
    NAME integration_${ITEST_NAME}
    COMMAND "${ITEST_PYTHON}" -m pytest
            "${CMAKE_CURRENT_SOURCE_DIR}/test_${ITEST_NAME}.py"
            "--html=${CMAKE_CURRENT_BINARY_DIR}/integration_${ITEST_NAME}.html"
            -v
  )

  set(ITEST_ENVIRONMENT
    "EEBUS_HEMS_PORT=${ITEST_HEMS_PORT}"
    "EEBUS_HP_PORT=${ITEST_HP_PORT}"
    "EEBUS_EV_PORT=${ITEST_EV_PORT}"
    "EEBUS_INTEGRATION_INSTANCE=${ITEST_NAME}"
  )

  set_tests_properties(integration_${ITEST_NAME} PROPERTIES
    LABELS "${ITEST_LABELS}"
    ENVIRONMENT "${ITEST_ENVIRONMENT}"
    TIMEOUT ${ITEST_TIMEOUT}
  )

  math(EXPR ITEST_INDEX "${ITEST_INDEX} + 1")
endforeach()
