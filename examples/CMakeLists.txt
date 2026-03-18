# -----------------------------------------------------------------------------
# main application
set(app_src
    app/main.cc
    app/app.cc app/app.h
    app/debug_opengl.cc app/debug_opengl.h
)
source_group("" FILES ${app_src})
add_executable(textgen WIN32 MACOSX_BUNDLE ${app_src})
target_include_directories(textgen
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(
    textgen
    PUBLIC
        external::sdl2
        external::opengl
        external::dear_imgui
        external::imgui_node_editor
        external::stb_image
        textgen::lib
    PRIVATE
        textgen::project_options
        textgen::project_warnings
)

