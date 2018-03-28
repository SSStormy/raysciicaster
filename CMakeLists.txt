cmake_minimum_required(VERSION 3.9)

project(aap)

set(CMAKE_CXX_STANDARD 11)

set(COMPILE_OPTIONS
    "-pedantic"
    "-Wall"
    "-Wextra"
    "-Wfatal-errors"
    "-fPIC"
    "-ggdb")

find_package(glm REQUIRED)

add_executable(aap aap.cpp)

target_compile_options(aap PUBLIC ${COMPILE_OPTIONS})

include_directories(
    ${GLM_INCLUDE_DIRS})

target_link_libraries(
    aap
    ${GLM_LIBRARIES})

