cmake_minimum_required(VERSION 3.15)
project(textgen CXX C)


###################################################################################################
# global setup
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
include(cmake/StandardProjectSettings.cmake)

Include(FetchContent)
include(ExternalProject)


###################################################################################################
# options
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

###################################################################################################
# helper libraries


# edit and continue
add_library(edit_and_continue INTERFACE)
add_library(klotter::edit_and_continue ALIAS edit_and_continue)
if(MSVC)
    # ZI = add debug information with edit and continue support to the target
    # Zi = same with edit and continue disabled
    target_compile_options(edit_and_continue INTERFACE "/ZI")
    target_link_options(edit_and_continue INTERFACE "/INCREMENTAL")
endif()

# project options
add_library(textgen_project_options INTERFACE)
add_library(textgen::project_options ALIAS textgen_project_options)
target_link_libraries(textgen_project_options INTERFACE klotter::edit_and_continue)
target_compile_features(textgen_project_options INTERFACE cxx_std_20)
include(cmake/Sanitizers.cmake)
enable_sanitizers(textgen_project_options)


# project warnings
add_library(textgen_project_warnings INTERFACE)
add_library(textgen::project_warnings ALIAS textgen_project_warnings)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(textgen_project_warnings)





###################################################################################################
# start adding sub folders
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(examples)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "textgen")
