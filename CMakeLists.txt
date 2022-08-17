cmake_minimum_required(VERSION 3.16.3)
project(ProductionSet CXX)

# Use C++20 standard since we are using std::ranges library
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard to conform to" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

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