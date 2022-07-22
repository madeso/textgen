cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(textgen C CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/compress.cmake)

add_library(textgen_project_options INTERFACE)
add_library(textgen_project_warnings INTERFACE)

target_compile_features(textgen_project_options INTERFACE cxx_std_17)
set_project_warnings(textgen_project_warnings)
enable_sanitizers(textgen_project_options)

add_library(textgen::project_options ALIAS textgen_project_options)
add_library(textgen::project_warnings ALIAS textgen_project_warnings)


add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(apps)
