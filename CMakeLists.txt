cmake_minimum_required(VERSION 3.10)

# Project name
project(lattia-vm C)

# Use the latest supported C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set source files directory
file(GLOB_RECURSE SOURCES src/*.c)

# Define the executable
add_executable(lattia-vm ${SOURCES})

# Set default build types if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
target_compile_options(lattia-vm PRIVATE
    $<$<CONFIG:Debug>:-g>   # Debug: Enable debugging symbols
    $<$<CONFIG:Release>:-O2> # Release: Optimize for speed
)

# Include directories (not needed for standard headers)
target_include_directories(lattia-vm PRIVATE src/)

# Enable testing
enable_testing()

# Placeholder for unit tests (you can add tests later)
# add_custom_target(test COMMAND ctest --output-on-failure)

