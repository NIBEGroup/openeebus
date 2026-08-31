cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "battery")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    main.c
    battery.c
)

if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})

target_link_libraries(${PROJECT_NAME} PRIVATE openeebus)
