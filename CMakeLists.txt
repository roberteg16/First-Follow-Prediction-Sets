cmake_minimum_required(VERSION 3.16.3)
project(ProductionSet CXX)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pretty print for Clang and warning flags 
IF(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  add_compile_options(-fcolor-diagnostics)
    add_compile_options(-Wall -Wextra -pedantic)
ENDIF()

# Include all the includes
include_directories(include)

# Add subdirectories
add_subdirectory(lib)

# Add executables to be built
add_subdirectory(tools)