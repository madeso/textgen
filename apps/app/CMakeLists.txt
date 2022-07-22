# -----------------------------------------------------------------------------
# main application
set(app_src
    src/app/main.cc
    src/app/app.cc src/app/app.h
    src/app/debug_opengl.cc src/app/debug_opengl.h
)
source_group("" FILES ${app_src})
add_executable(textgen WIN32 MACOSX_BUNDLE ${app_src})
target_include_directories(textgen
    PUBLIC
        src/
)
target_link_libraries(
    textgen
    PUBLIC
        external::sdl
        external::opengl
        external::dear_imgui
        external::imgui_node_editor
        external::stb_image
        textgen::lib
    PRIVATE
        textgen::project_options
        textgen::project_warnings
)

