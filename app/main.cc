// standard headers
#include <string>
#include <string_view>
#include <cassert>
#include <vector>
#include <numeric>
#include <functional>
#include <array>
#include <map>
#include <set>

// dependency headers
#include "glad/glad.h"
#include "SDL.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// imgui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// custom/local headers
#include "app/debug_opengl.h"


///////////////////////////////////////////////////////////////////////////////
// common "header"


int
Csizet_to_int(std::size_t t)
{
    return static_cast<int>(t);
}


std::size_t
Cint_to_sizet(int i)
{
    return static_cast<std::size_t>(i);
}

int
Cunsigned_int_to_int(unsigned int ui)
{
    return static_cast<int>(ui);
}


int
main(int, char**)
{
    ///////////////////////////////////////////////////////////////////////////
    // setup
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int width = 1280;
    int height = 720;

    SDL_Window* window = SDL_CreateWindow
    (
        "textgen",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if(window == nullptr)
    {
        SDL_Log("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        SDL_Log("Failed to initialize OpenGL context");
        return -1;
    }

    const auto* renderer = glGetString(GL_RENDERER); // get renderer string
    const auto* version = glGetString(GL_VERSION); // version as a string
    SDL_Log("Renderer: %s\n", renderer);
    SDL_Log("Version: %s\n", version);

    // imgui setup
    const char* glsl_version = "#version 130";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsLight();
    
    ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    const auto update_viewport = [&]()
    {
        glViewport(0, 0, width, height);
    };

    update_viewport();

    

    ///////////////////////////////////////////////////////////////////////////
    // main

    bool running = true;
    

    while(running)
    {
        

        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(false == ImGui_ImplSDL2_ProcessEvent(&e))
            {
                switch(e.type)
                {
                case SDL_WINDOWEVENT:
                    if(e.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        width = e.window.data1;
                        height = e.window.data2;
                        update_viewport();
                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    const bool down = e.type == SDL_KEYDOWN;

                    switch(e.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        if(!down)
                        {
                            running = false;
                        }
                        break;
                    default:
                        // ignore other keys
                        break;
                    }
                }
                break;
                default:
                    // ignore other events
                    break;
                }
            }
        }

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            // ImGui::ShowDemoWindow();

            if(ImGui::Begin("Debug"))
            {
                if(ImGui::Button("Quit")) { running = false; }
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

