cmake_minimum_required(VERSION 3.13)
project(demorgan CXX)

# Options
option(USE_CCACHE "Use ccache to speed-up build" OFF)

# General options
set(DEP_DIR "${CMAKE_SOURCE_DIR}/dep")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${USE_CCACHE})
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
        message(STATUS "ccache found at ${CCACHE_PROGRAM}")
    else()
        message(FATAL "ccache support required, but ccache cannot be found")
    endif()
endif()

# Dependencies
## fmtlib
add_subdirectory("${DEP_DIR}/fmt")

# Main target
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_SOURCE_DIR}/include")

add_executable(
    "${PROJECT_NAME}"
    "${SRC_DIR}/expression.cpp"
    "${SRC_DIR}/lexer.cpp"
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/parser.cpp"
    "${SRC_DIR}/position.cpp"
    "${SRC_DIR}/simplifier.cpp"
    "${SRC_DIR}/token.cpp")
target_include_directories(
    "${PROJECT_NAME}"
    PRIVATE
    "${INC_DIR}")
target_link_libraries(
    "${PROJECT_NAME}"
    PRIVATE
    fmt::fmt)
target_compile_options(
    "${PROJECT_NAME}"
    PRIVATE
    "-Wall"
    "-Wextra")
target_compile_options(
    "${PROJECT_NAME}"
    PUBLIC
    "-fPIC"
    "-flto")
target_link_options(
    "${PROJECT_NAME}"
    PUBLIC
    "-flto")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_options(
        "${PROJECT_NAME}"
        PUBLIC
        "-O0"
        "-g")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    target_compile_options(
        "${PROJECT_NAME}"
        PUBLIC
        "-O2")
endif()

