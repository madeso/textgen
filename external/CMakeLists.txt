Include(FetchContent)
include(ExternalProject)

set(SDL_VERSION_STRING "2.32.10") # tar.gz name on sdl github release page

###################################################################################################
# glad
add_library(external_glad STATIC
    glad/src/glad.c
    glad/include/glad/glad.h
)
set_target_properties(external_glad PROPERTIES FOLDER "External")
target_include_directories(external_glad SYSTEM
    PUBLIC
        glad/include
)
target_link_libraries(external_glad
    PUBLIC
        ${CMAKE_DL_LIBS}
)
add_library(external::glad ALIAS external_glad)



###################################################################################################
# SDL2
###################################################################################################
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



###################################################################################################
# OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
add_library(external_opengl INTERFACE)
target_link_libraries(external_opengl INTERFACE OpenGL::GL)
add_library(external::opengl ALIAS external_opengl)



###################################################################################################
# fast noise lite
add_library(fastnoiselite_lib INTERFACE
)
target_include_directories(fastnoiselite_lib SYSTEM
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseLite/Cpp/
)
add_library(external::fast_noise_lite ALIAS fastnoiselite_lib)




###################################################################################################
# dear_imgui
set(dear_imgui_src
    imgui/imconfig.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h

    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_sdl2.h
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
)
add_library(dear_imgui STATIC ${dear_imgui_src})
set_target_properties(dear_imgui PROPERTIES FOLDER "External")
source_group("" FILES ${dear_imgui_src})
target_include_directories(dear_imgui SYSTEM
    PUBLIC
        imgui/
        imgui/backends/
)
target_link_libraries(dear_imgui
    PUBLIC
        external::glad
        external::opengl
        external::sdl2
)
target_compile_definitions(dear_imgui
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD
)
add_library(external::dear_imgui ALIAS dear_imgui)



###################################################################################################
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




###################################################################################################
# stb image
add_library(stb_image_lib STATIC
    stb/stb-image.cc
)
target_include_directories(stb_image_lib
    PUBLIC
        stb/include
)
add_library(external::stb_image ALIAS stb_image_lib)
