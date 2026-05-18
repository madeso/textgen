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
// #include "glm/glm.hpp"
// #include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/type_ptr.hpp"

// imgui
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// custom/local headers
#include "app/debug_opengl.h"
#include "app/app.h"

#include "app/enable_high_performance_graphics.h"

ENABLE_HIGH_PERFORMANCE_GRAPHICS

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
	////////////////////////////////////////////////////////////////////////////////
	// sdl config

	constexpr Uint32 flags = SDL_INIT_VIDEO;
	if (SDL_Init(flags) != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL: %s", SDL_GetError());
		return -1;
	}

    {
		SDL_version compiled;
		SDL_version linked;

		SDL_VERSION(&compiled);
		SDL_GetVersion(&linked);

		SDL_Log("Compiled against SDL version %u.%u.%u ...\n", compiled.major, compiled.minor, compiled.patch);
		SDL_Log("Linking against SDL version %u.%u.%u.\n", linked.major, linked.minor, linked.patch);
    }

#if defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);	// Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);  // was 0 in dear imgui example??
#endif

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);


    int width = 1280;
    int height = 720;

    SDL_Window* sdl_window = SDL_CreateWindow
    (
        "textgen",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if(sdl_window == nullptr)
    {
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s", SDL_GetError());
		return -1;
    }

    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_window);

	if (sdl_gl_context == nullptr)
    {
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not create gl context: %s", SDL_GetError());

		SDL_DestroyWindow(sdl_window);
		sdl_window = nullptr;

        return -1;
    }

	SDL_GL_MakeCurrent(sdl_window, sdl_gl_context);
	SDL_GL_SetSwapInterval(1);	// Enable vsync
	
	if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to load OpenGL");

		SDL_GL_DeleteContext(sdl_gl_context);
		sdl_gl_context = nullptr;

		SDL_DestroyWindow(sdl_window);
		sdl_window = nullptr;

		return -1;
	}

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loaded OpenGL %d.%d", GLVersion.major, GLVersion.minor);

	///////////////////////////////////////////////////////////////
	// load opengl debug
	setup_opengl_debug();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    
    ImGui_ImplSDL2_InitForOpenGL(sdl_window, sdl_gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    const auto update_viewport = [&]()
    {
        glViewport(0, 0, width, height);
    };

    update_viewport();

    auto app = make_app();

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
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            app->on_gui();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_GL_SwapWindow(sdl_window);
    }

    app.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    SDL_GL_DeleteContext(sdl_gl_context);
    SDL_DestroyWindow(sdl_window);

    SDL_Quit();
    return 0;
}

