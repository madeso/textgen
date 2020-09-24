cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(tred C CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/compress.cmake)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_17)
set_project_warnings(project_warnings)
enable_sanitizers(project_options)

set(OpenGL_GL_PREFERENCE "GLVND")


find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)


# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})


# -----------------------------------------------------------------------------
# imgui project dependency
add_library(imgui STATIC
    external/imgui-1.78/imgui_demo.cpp
    external/imgui-1.78/imgui_widgets.cpp
    external/imgui-1.78/imgui.cpp
    external/imgui-1.78/imgui_draw.cpp
    external/imgui-1.78/examples/imgui_impl_sdl.cpp
    external/imgui-1.78/examples/imgui_impl_opengl3.cpp
)
target_include_directories(imgui
    PUBLIC
    external/imgui-1.78/
    external/imgui-1.78/examples/
    PRIVATE
    external/glad/include
    ${SDL2_INCLUDE_DIR}
)
target_link_libraries(
    imgui
    PUBLIC ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS}
)
target_compile_definitions(imgui PUBLIC -D IMGUI_IMPL_OPENGL_LOADER_GLAD)


# -----------------------------------------------------------------------------
# library
set(lib_src
    textgen/textgen.h
    textgen/textgen.cc
)
source_group("" FILES ${lib_src})
add_library(textgen STATIC ${lib_src})
target_include_directories(textgen
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# -----------------------------------------------------------------------------
# main application
set(app_src
    app/main.cc
    app/debug_opengl.cc
    app/debug_opengl.h
    app/stb.cc
    
    external/glad/src/glad.c
    external/glad/include/glad/glad.h
)
source_group("" FILES ${app_src})
add_executable(textgen_app WIN32 MACOSX_BUNDLE ${app_src})
target_include_directories(textgen_app SYSTEM PRIVATE
    ${SDL2_INCLUDE_DIR}
    external/glad/include
    external/glm-0.9.9.8
    external/stb
)
target_link_libraries(
    textgen_app
    PUBLIC ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS} imgui textgen
    PRIVATE project_options project_warnings
)

