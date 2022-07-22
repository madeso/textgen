#include "app/app.h"

#include <imgui.h>
// #define IMGUI_DEFINE_MATH_OPERATORS
// #include <imgui_internal.h>
#include "imgui_node_editor.h"

namespace node_editor = ax::NodeEditor;



struct TextGen : App
{
    node_editor::EditorContext* node_editor_context = nullptr;

    TextGen()
    {
        node_editor::Config config;
        config.SettingsFile = "textgen-nodes.json";
        node_editor_context = node_editor::CreateEditor(&config);
    }

    ~TextGen()
    {
        node_editor::DestroyEditor(node_editor_context);
    }
    void on_gui() override
    {
        /*{
            auto& io = ImGui::GetIO();
            ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate>0.0f : 0.0f);
        }*/
        ImGui::Begin("window");

        node_editor::SetCurrentEditor(node_editor_context);
        node_editor::Begin("My Editor", ImVec2(0.0, 0.0f));
        unsigned int uniqueId = 1;
        
        // Start drawing nodes.
        node_editor::BeginNode(uniqueId++);
            ImGui::Text("Node A");
            node_editor::BeginPin(uniqueId++, node_editor::PinKind::Input);
                ImGui::Text("-> In");
            node_editor::EndPin();
            ImGui::SameLine();
            node_editor::BeginPin(uniqueId++, node_editor::PinKind::Output);
                ImGui::Text("Out ->");
            node_editor::EndPin();
        node_editor::EndNode();
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
