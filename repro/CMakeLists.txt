cmake_minimum_required(VERSION 3.30)

project(Repro LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(staticlib)
add_subdirectory(exe)
