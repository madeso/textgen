cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(textgen C CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(DEFAULT_EU_SDL_SOURCE "FindLibrary")
if(WIN32)
    set(DEFAULT_EU_SDL_SOURCE "FetchContent")
endif()
set(EU_SDL_SOURCE "${DEFAULT_EU_SDL_SOURCE}" CACHE STRING "How to get SDL library (FindLibrary or FetchContent)")
set_property(CACHE EU_SDL_SOURCE PROPERTY STRINGS FindLibrary FetchContent)

if(EU_SDL_SOURCE STREQUAL "FindLibrary")
    set(EU_SDL_SOURCE_FETCH OFF)
    set(EU_SDL_SOURCE_FIND ON)
elseif(EU_SDL_SOURCE STREQUAL "FetchContent")
    set(EU_SDL_SOURCE_FETCH ON)
    set(EU_SDL_SOURCE_FIND OFF)
else()
    message(ERROR "invalid EU_SDL_SOURCE (${EU_SDL_SOURCE})")
endif()

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
