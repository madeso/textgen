# -----------------------------------------------------------------------------
# glad
set(glad_src
    glad/src/glad.c
    glad/include/glad/glad.h
)
source_group("" FILES ${glad_src})
add_library(glad_lib STATIC ${glad_src})
target_include_directories(glad_lib SYSTEM
    PUBLIC
        glad/include
)
add_library(external::glad ALIAS glad_lib)



#${CMAKE_DL_LIBS}


# -----------------------------------------------------------------------------
# sdl
find_package(SDL2 REQUIRED)
add_library(sdl_lib INTERFACE)
target_include_directories(sdl_lib SYSTEM
    INTERFACE
        ${SDL2_INCLUDE_DIR}
)
target_link_libraries(sdl_lib
    INTERFACE
        ${SDL2_LIBRARY}
)
add_library(external::sdl ALIAS sdl_lib)




# -----------------------------------------------------------------------------
# opengl
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
add_library(opengl_lib INTERFACE)
target_include_directories(opengl_lib SYSTEM
    INTERFACE
        ${SDL2_INCLUDE_DIR}
)
target_link_libraries(opengl_lib
    INTERFACE
        ${OPENGL_LIBRARIES}
        external::glad
)
add_library(external::opengl ALIAS opengl_lib)




# -----------------------------------------------------------------------------
# fast noise lite
add_library(fastnoiselite_lib INTERFACE
)
target_include_directories(fastnoiselite_lib SYSTEM
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseLite/Cpp/
)
add_library(external::fast_noise_lite ALIAS fastnoiselite_lib)




# -----------------------------------------------------------------------------
# dear imgui
add_library(imgui_lib STATIC
    imgui/imconfig.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui.h
    # todo: look into how different stb libs work together
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h

    imgui/misc/cpp/imgui_stdlib.cpp
    imgui/misc/cpp/imgui_stdlib.h
    imgui/backends/imgui_impl_sdl.cpp
    imgui/backends/imgui_impl_sdl.h
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_opengl3_loader.h
)
target_include_directories(imgui_lib SYSTEM
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp/
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)
target_compile_definitions(imgui_lib
    PRIVATE
        -DIMGUI_IMPL_OPENGL_LOADER_GLAD
    PUBLIC
        -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS
        # this does not switch to utf-32, this only extends dear imgui to support more fonts 32
        # -DIMGUI_USE_WCHAR32
)
target_link_libraries(imgui_lib
    PUBLIC
        external::sdl
        external::glad
)
add_library(external::dear_imgui ALIAS imgui_lib)




# -----------------------------------------------------------------------------
# imgui node editor
add_library(imgui_node_editor_lib STATIC
    imgui-node-editor/crude_json.cpp
    imgui-node-editor/crude_json.h
    imgui-node-editor/imgui_bezier_math.h
    imgui-node-editor/imgui_bezier_math.inl
    imgui-node-editor/imgui_canvas.cpp
    imgui-node-editor/imgui_canvas.cpp
    imgui-node-editor/imgui_canvas.h
    imgui-node-editor/imgui_canvas.h
    imgui-node-editor/imgui_extra_math.h
    imgui-node-editor/imgui_extra_math.inl
    imgui-node-editor/imgui_node_editor_api.cpp
    imgui-node-editor/imgui_node_editor_internal.h
    imgui-node-editor/imgui_node_editor_internal.inl
    imgui-node-editor/imgui_node_editor.cpp
    imgui-node-editor/imgui_node_editor.h
    imgui-node-editor/misc/imgui_node_editor.natvis
)
target_include_directories(imgui_node_editor_lib
    PUBLIC
        imgui-node-editor
)
target_link_libraries(imgui_node_editor_lib
    PUBLIC
        external::dear_imgui
)
add_library(external::imgui_node_editor ALIAS imgui_node_editor_lib)




# -----------------------------------------------------------------------------
# stb image
add_library(stb_image_lib STATIC
    stb/stb-image.cc
)
target_include_directories(stb_image_lib
    PUBLIC
        stb/include
)
add_library(external::stb_image ALIAS stb_image_lib)
