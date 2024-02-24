cmake_minimum_required(VERSION 3.16)
project(rls)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
#set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(rls rls.cpp)