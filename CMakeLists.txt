cmake_minimum_required(VERSION 3.0)
project(rls)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
add_library(termcolor INTERFACE IMPORTED)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE termcolor)
