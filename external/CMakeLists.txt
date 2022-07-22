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
# dear imgui
add_library(imgui_lib STATIC
    imgui-1.78/imgui_demo.cpp
    imgui-1.78/imgui_widgets.cpp
    imgui-1.78/imgui.cpp
    imgui-1.78/imgui_draw.cpp
    imgui-1.78/examples/imgui_impl_sdl.cpp
    imgui-1.78/examples/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib
    PUBLIC
        imgui-1.78/
        imgui-1.78/examples/
    PRIVATE
        external/glad/include
)
target_link_libraries(imgui_lib
    PUBLIC
        external::opengl
)
target_compile_definitions(imgui_lib
    PUBLIC
        -D IMGUI_IMPL_OPENGL_LOADER_GLAD
)
add_library(external::dear_imgui ALIAS imgui_lib)




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
