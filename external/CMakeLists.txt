Include(FetchContent)
include(ExternalProject)

set(SDL_VERSION_STRING "2.32.10") # tar.gz name on sdl github release page

# -----------------------------------------------------------------------------
# glad
set(glad_src
    glad/src/glad.c
    glad/include/glad/glad.h
)
source_group("" FILES ${glad_src})
add_library(glad_lib STATIC ${glad_src})
set_target_properties(glad_lib PROPERTIES FOLDER "External")
target_include_directories(glad_lib SYSTEM
    PUBLIC
        glad/include
)
target_link_libraries(glad_lib
    PUBLIC
        ${CMAKE_DL_LIBS}
)
add_library(external::glad ALIAS glad_lib)



# -----------------------------------------------------------------------------
# sdl
if(EU_SDL_SOURCE_FETCH)
    set(SDL_EXTERN_DIR ${CMAKE_CURRENT_BINARY_DIR}/sdl)
    set(SDL_INSTALL_DIR "${SDL_EXTERN_DIR}/install")
    message(STATUS "Will download and build ${SDL_VERSION_STRING} to ${SDL_EXTERN_DIR} and install to ${SDL_INSTALL_DIR} when building")

    ExternalProject_Add(
        SDL2_external
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION_STRING}/SDL2-${SDL_VERSION_STRING}.zip
        PREFIX "${SDL_EXTERN_DIR}/${SDL_VERSION_STRING}"
        LOG_DIR "${SDL_EXTERN_DIR}/log"
        INSTALL_DIR ${SDL_INSTALL_DIR}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DLIBC=ON
            -DSDL_STATIC=ON
            -DSDL_SHARED=OFF
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
    )
    set_target_properties(SDL2_external PROPERTIES FOLDER "External")

    add_library(external_sdl2 INTERFACE)
    add_dependencies(external_sdl2 SDL2_external)
    target_include_directories(external_sdl2 SYSTEM
        INTERFACE
            ${SDL_INSTALL_DIR}/include/SDL2
    )
    if(WIN32)
        target_link_libraries(external_sdl2 INTERFACE
            optimized ${SDL_INSTALL_DIR}/lib/SDL2main.lib
            optimized ${SDL_INSTALL_DIR}/lib/SDL2-static.lib
            debug ${SDL_INSTALL_DIR}/lib/SDL2maind.lib
            debug ${SDL_INSTALL_DIR}/lib/SDL2-staticd.lib
            version.lib imm32.lib winmm.lib Setupapi.lib
        )
    else()
        # todo(Gustav): specify debug flag?
        target_link_libraries(external_sdl2 INTERFACE ${SDL_INSTALL_DIR}/lib/libSDL2.a)
    endif()
    add_library(external::sdl2 ALIAS external_sdl2)
endif()

if(EU_SDL_SOURCE_FIND)
    find_package(SDL2 REQUIRED)
    add_library(external_sdl2 INTERFACE)
    target_include_directories(external_sdl2 SYSTEM
        INTERFACE
            ${SDL2_INCLUDE_DIR}
    )
    target_link_libraries(external_sdl2
        INTERFACE
            ${SDL2_LIBRARY}
            ${CMAKE_DL_LIBS}
    )
    if(WIN32)
    target_link_libraries(external_sdl2
        INTERFACE
        Setupapi.lib
    )
    endif()
    add_library(external::sdl2 ALIAS external_sdl2)
endif()




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
        OpenGL::GL
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
        external::sdl2
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
