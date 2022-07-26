#include "app/app.h"

// #include <iostream>

#include "imgui.h"
// #define IMGUI_DEFINE_MATH_OPERATORS
// #include <imgui_internal.h>
#include "imgui_node_editor.h"
#include "glad/glad.h"

#include "textgen/textgen.h"




namespace imgui
{
    void begin_column()
    {
        ImGui::BeginGroup();
    }

    void next_column()
    {
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
    }

    void end_column()
    {
        ImGui::EndGroup();
    }
}




namespace ed = ax::NodeEditor;


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


ImColor get_icon_color(textgen::PinType type)
{
    switch (type)
    {
        case textgen::PinType::image:    return ImColor( 68, 201, 156);
        default:
            assert(false && "missing case for pintype in get_icon_color(...)");
            return ImColor(255, 255, 255);
        /*
        ImColor(220,  48,  48)
        ImColor(147, 226,  74)
        ImColor(124,  21, 153)
        ImColor( 51, 150, 215)
        ImColor(218,   0, 183)
        ImColor(255,  48,  48)
        */
    }
}


template<typename T, typename F>
void erase_remove_if(std::vector<T>& vec, F&& f)
{
    vec.erase
    (
        std::remove_if
        (
            vec.begin(),
            vec.end(),
            std::move(f)
        ), 
        vec.end()
    );
}

void submit_single_link(const textgen::Link& link)
{
    ed::Link(link.id, link.start_pin, link.end_pin);
}

struct TextGen : App
{
    textgen::TextGen textgen;
    ed::EditorContext* node_editor_context = nullptr;

    TextGen()
    {
        ed::Config config;
        config.SettingsFile = "textgen-nodes.json";
        node_editor_context = ed::CreateEditor(&config);
        textgen.make_native_image_fun = [](const textgen::Image& img){ return std::make_unique<Texture>(img); };
        textgen.nodes.emplace_back(std::make_unique<textgen::NoiseNode>());
        textgen.nodes.emplace_back(std::make_unique<textgen::DummyNode>());
    }

    ~TextGen()
    {
        ed::DestroyEditor(node_editor_context);
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

        ed::SetCurrentEditor(node_editor_context);
        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        submit_nodes();
        submit_links();
        handle_interaction();

        ed::End();
        ed::SetCurrentEditor(nullptr);
        
        ImGui::End();

        //ImGui::ShowMetricsWindow();
    }


    void submit_nodes()
    {
        for(auto& node: textgen.nodes)
        {
            if(node->id == 0) { continue; }

            ed::BeginNode(node->id);
            ImGui::TextUnformatted(node->get_name().c_str());
            if(node->native_image)
            {
                auto* texture = static_cast<Texture*>(node->native_image.get());
                ImGui::Image(reinterpret_cast<ImTextureID>(texture->id), ImVec2{128,128});
            }

            imgui::begin_column();
            for(auto& p: node->inputs)
            {
                ed::BeginPin(p.id, ed::PinKind::Input);
                ImGui::Text("-> %s", p.name.c_str());
                ed::EndPin();
            }
            imgui::next_column();
            for(auto& p: node->outputs)
            {
                ed::BeginPin(p.id, ed::PinKind::Output);
                ImGui::Text("%s ->", p.name.c_str());
                ed::EndPin();
            }
            imgui::end_column();
            
            ed::EndNode();
        }
    }


    void submit_links()
    {
        for (auto& link : textgen.links)
        {
            submit_single_link(*link.get());
        }
    }


    void handle_interaction()
    {
        handle_link_creation();
        handle_link_deletion();
    }

    void handle_link_creation()
    {
        if (ed::BeginCreate())
        {
            ed::PinId input_pin_id_id;
            ed::PinId output_pin_id_id;
            if (ed::QueryNewLink(&input_pin_id_id, &output_pin_id_id))
            {
                unsigned int input_pin_id = static_cast<unsigned int>(input_pin_id_id.Get());
                unsigned int output_pin_id = static_cast<unsigned int>(output_pin_id_id.Get());

                auto* input_pin = textgen.find_pin(input_pin_id);
                auto* output_pin = textgen.find_pin(output_pin_id);
                if (textgen.can_create_link(input_pin, output_pin))
                {
                    if (ed::AcceptNewItem())
                    {
                        textgen.links.emplace_back
                        (
                            std::make_unique<textgen::Link>(textgen::Link{textgen.create_new_id(), input_pin_id, output_pin_id})
                        );

                        submit_single_link(*textgen.links.back().get());
                    }
                }
                else
                {
                    ed::RejectNewItem();
                }
                
            }
        }
        ed::EndCreate(); // Wraps up object creation action handling.
    }

    void handle_link_deletion()
    {
        if (ed::BeginDelete())
        {
            ed::LinkId deleted_link_id_id;
            while (ed::QueryDeletedLink(&deleted_link_id_id))
            {
                unsigned int deleted_link_id = static_cast<unsigned int>(deleted_link_id_id.Get());
                if (ed::AcceptDeletedItem())
                {
                    erase_remove_if(textgen.links, [deleted_link_id](std::unique_ptr<textgen::Link>& link) -> bool
                    {
                        return link->id == deleted_link_id;
                    });
                }
            }
        }
        ed::EndDelete();
    }
};


std::unique_ptr<App> make_app()
{
    return std::make_unique<TextGen>();
}
