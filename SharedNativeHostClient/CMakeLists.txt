# cmake_minimum_required is required to be the first instruction in a cmake file
cmake_minimum_required(VERSION 3.29)

project(ExampleNativeHostClient LANGUAGES C CXX)

# Add Objective-C language to project if building for mac
include(${CMAKE_CURRENT_LIST_DIR}/dev/make/mac/ObjectiveCCheck.cmake)

# Set global build system variables and compiler settings
include(${CMAKE_CURRENT_LIST_DIR}/dev/make/BuildSettings.cmake)

# Find external dependencies
include(${CMAKE_CURRENT_LIST_DIR}/dev/make/External.cmake)

# Find external dependencies
include(${CMAKE_CURRENT_LIST_DIR}/dev/make/NpmFunctions.cmake)

# Build commands for npm packages
install_node_modules()
add_npm_build_target()

add_subdirectory(dev)
