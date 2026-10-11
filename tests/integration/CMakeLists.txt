# pytest-based end-to-end tests for the heat_pump and hems demo binaries

if(NOT OPTION_INTEGRATION_TESTS)
  return()
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

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
foreach(ITEST_NAME lpc lpp mpc mgcp simopen_stress)
  math(EXPR ITEST_HEMS_PORT "${ITEST_PORT_BASE} + (${ITEST_INDEX} * 10)")
  math(EXPR ITEST_HP_PORT "${ITEST_HEMS_PORT} + 2")

  add_test(
    NAME integration_${ITEST_NAME}
    COMMAND "${ITEST_PYTHON}" -m pytest
            "${CMAKE_CURRENT_SOURCE_DIR}/test_${ITEST_NAME}.py"
            "--html=${CMAKE_CURRENT_BINARY_DIR}/integration_${ITEST_NAME}.html"
            -v
  )

  set_tests_properties(integration_${ITEST_NAME} PROPERTIES
    LABELS integration
    ENVIRONMENT "EEBUS_HEMS_PORT=${ITEST_HEMS_PORT};EEBUS_HP_PORT=${ITEST_HP_PORT};EEBUS_INTEGRATION_INSTANCE=${ITEST_NAME}"
    TIMEOUT 600
  )
  math(EXPR ITEST_INDEX "${ITEST_INDEX} + 1")
endforeach()
