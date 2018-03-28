cmake_minimum_required(VERSION 3.9)

project(aap)

set(CMAKE_CXX_STANDARD 11)

find_package(glm REQUIRED)

add_executable(aap aap.cpp)

include_directories(
    ${GLM_INCLUDE_DIRS})

target_link_libraries(
    aap
    ${GLM_LIBRARIES})

