cmake_minimum_required(VERSION 3.18.0)

# Check if project is build in seperate folder
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

include(vendor/cmake_modules/set_startup_project.cmake)
include(vendor/cmake_modules/ignore_precompiled_header_msvc.cmake)
include(vendor/cmake_modules/set_target_output_directories.cmake)
include(vendor/cmake_modules/set_target_precompiled_header_msvc.cmake)
include(vendor/cmake_modules/update_submodules.cmake)

project(hydraulic-erosion VERSION 1.0)

# Use folder structure 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Specify the c++ standard
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED true)
# Setup output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)

update_submodules()

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/spdlog)
add_subdirectory(hydraulic_erosion)

set_startup_project(hydraulic-erosion)

