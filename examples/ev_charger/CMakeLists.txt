cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "ev_charger")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME})

set(MATH_LIBRARY "")
if(NOT WIN32)
  set(MATH_LIBRARY "m")
endif()

if(WIN32)
  set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
  target_link_options(${PROJECT_NAME} PRIVATE "/NODEFAULTLIB:LIBCMT")
endif()

target_sources(${PROJECT_NAME}
  PRIVATE
  main.c
  evsrv.c
  cs_lpc_listener.c
  cs_lpp_listener.c
  mu_mpc_listener.c
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
  ${PROJECT_SOURCE_DIR}
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
  ${PROJECT_COMPILE_OPTIONS}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  ${MATH_LIBRARY}
  openeebus
)
