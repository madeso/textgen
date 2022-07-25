#include "app/app.h"

#include <imgui.h>
// #define IMGUI_DEFINE_MATH_OPERATORS
// #include <imgui_internal.h>
#include "imgui_node_editor.h"
#include "glad/glad.h"

#include "textgen/textgen.h"



namespace node_editor = ax::NodeEditor;


unsigned int create_texture()
{
    unsigned int texture;
    glGenTextures(1, &texture);
    return texture;
}


struct Texture : textgen::NativeImage
{
    unsigned int id;

    Texture(const textgen::Image& img)
        : id(create_texture())
    {
        // make options?
        constexpr bool include_transparency = false;
        constexpr bool render_pixels = false;
        constexpr bool clamp = true;

        // transform img to pixel_data
        std::vector<unsigned char> pixel_data;
        {
            const unsigned int channels = include_transparency ? 4 : 3;
            pixel_data.resize(img.width * img.height * channels);
            constexpr auto float_to_byte = [](float f) -> unsigned char
                { return static_cast<unsigned char>(f * 255.0f); };
            for(unsigned int y=0; y<img.height; y+=1)
            {
                for(unsigned int x=0; x<img.width; x+=1)
                {
                    const auto c = img.get(x, y);
                    const auto base_index = (y * img.height + x) * channels;
                    pixel_data[base_index + 0] = float_to_byte(c.r);
                    pixel_data[base_index + 1] = float_to_byte(c.g);
                    pixel_data[base_index + 2] = float_to_byte(c.b);
                    if(include_transparency)
                    {
                        // todo(Gustav): get transparency from image?
                        pixel_data[base_index + 3] = 255;
                    }
                }
            }
        }

        // send pixel_data to opengl
        {
            glBindTexture(GL_TEXTURE_2D, id);
            const auto wrap = clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
            const auto min_filter = render_pixels ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
            const auto mag_filter = render_pixels ? GL_NEAREST : GL_LINEAR;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
            glTexImage2D
            (
                GL_TEXTURE_2D,
                0,
                include_transparency ? GL_RGBA : GL_RGB,
                img.width, img.height,
                0,
                include_transparency ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
                &pixel_data[0]
            );
            if(render_pixels == false)
            {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        }
    }

    ~Texture()
    {
        glDeleteTextures(1, &id);
    }

    Texture(Texture&&) = delete;
    void operator=(Texture&&) = delete;
    Texture(const Texture&) = delete;
    void operator=(const Texture&) = delete;
};


struct TextGen : App
{
    textgen::TextGen textgen;
    node_editor::EditorContext* node_editor_context = nullptr;

    TextGen()
    {
        node_editor::Config config;
        config.SettingsFile = "textgen-nodes.json";
        node_editor_context = node_editor::CreateEditor(&config);
        textgen.make_native_image_fun = [](const textgen::Image& img){ return std::make_unique<Texture>(img); };
        textgen.nodes.emplace_back(std::make_unique<textgen::NoiseNode>());
    }

    ~TextGen()
    {
        node_editor::DestroyEditor(node_editor_context);
    }
    void on_gui() override
    {
        textgen.work();
        /*{
            auto& io = ImGui::GetIO();
            ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate>0.0f : 0.0f);
        }*/

        bool main_window = true;

        if(main_window)
        {
            auto& io = ImGui::GetIO();
            ImGui::SetNextWindowPos({0, 0});
            ImGui::SetNextWindowSize(io.DisplaySize);

            const auto flags =
              ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoCollapse
            ;

            ImGui::Begin("window", nullptr, flags);
        }
        else
        {
            ImGui::Begin("window");
        }

        node_editor::SetCurrentEditor(node_editor_context);
        node_editor::Begin("My Editor", ImVec2(0.0, 0.0f));
        // unsigned int uniqueId = 1;

        for(auto& node: textgen.nodes)
        {
            if(node->id == 0) { continue; }

            node_editor::BeginNode(node->id);
            ImGui::TextUnformatted(node->get_name().c_str());
            if(node->native_image)
            {
                auto* texture = static_cast<Texture*>(node->native_image.get());
                ImGui::Image(reinterpret_cast<ImTextureID>(texture->id), ImVec2{128,128});
            }
            /*
            node_editor::BeginPin(uniqueId++, node_editor::PinKind::Input);
                ImGui::Text("-> In");
            node_editor::EndPin();
            ImGui::SameLine();
            node_editor::BeginPin(uniqueId++, node_editor::PinKind::Output);
                ImGui::Text("Out ->");
            node_editor::EndPin();
            */
            node_editor::EndNode();
        }
        node_editor::End();
        node_editor::SetCurrentEditor(nullptr);
        
        ImGui::End();

        //ImGui::ShowMetricsWindow();
    }
};


std::unique_ptr<App> make_app()
{
    return std::make_unique<TextGen>();
}
