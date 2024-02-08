// UI (with imgui)
#include "../../backend/Source/editor/Application.h"
#include "../../backend/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../backend/Source/editor/EntryPoint.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"
#include "../../vendor/imgui/imgui.h"
#include "../../vendor/stb-image/stb_image.h"

#include "./icons.embed"

using namespace VortexMaker;

class InstanceWindow
{
public:
    InstanceWindow(VxContext *ctx)
    {

        this->m_ctx = ctx;
    };

    void render()
    {
            // Espace de docking ici!!
            ImGuiID dockspaceID = ImGui::GetID(this->name.c_str());
            ImGuiIO &io = ImGui::GetIO();

        if (ImGui::Begin(name.c_str(), &this->opened, ImGuiWindowFlags_MenuBar))
        {

            ImGui::SetWindowSize(ImVec2(ImGui::GetWindowSize().x, 500.0f));
            // Barre d'outils

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::Button("Save"))
                {
                }
                ImGui::Separator();


    if (ImGui::BeginMenu("Pannels")) {
      if (ImGui::MenuItem("Contents Window")) {
        
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools")) {
      if (ImGui::MenuItem("Contents Window")) {
        
      }
      ImGui::EndMenu();
    }
                ImGui::Separator();

                if (ImGui::Button("Add"))
                {
                }
                if (ImGui::Button("Preview"))
                {
                }
                ImGui::Separator();
                if (ImGui::Button("|>"))
                {
                }
                if (ImGui::Button(">"))
                {
                }
                if (ImGui::Button("||"))
                {
                }
                ImGui::Separator();
                if (ImGui::Button("Settings"))
                {
                }

                ImGui::EndMenuBar();
            }
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            
        }
        else{
                ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_KeepAliveOnly);
            
        }

        ImGui::End();

        std::string label = this->name + " - Dockable Window";
        if (ImGui::Begin(label.c_str()))
        {
            ImGui::TextUnformatted("Test");
        }
        ImGui::End();
    }

    bool opened;
    std::string name;
    VxContext *m_ctx;

    std::shared_ptr<Walnut::Image> m_Icon;
};
