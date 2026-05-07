cmake_minimum_required(VERSION 3.24)
project(CozyFarmRPG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(COZY_BUILD_TESTS "Build tests" ON)

add_library(cozy_engine INTERFACE)
target_include_directories(cozy_engine INTERFACE src)

add_executable(cozy_game src/main.cpp)
target_link_libraries(cozy_game PRIVATE cozy_engine)
