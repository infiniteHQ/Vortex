// UI (with imgui)
#include "../../backend/Source/editor/Application.h"
#include "../../backend/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"
#include "../../backend/Platform/GUI/editor/UI/Notification.h"
#include "../../backend/Platform/GUI/editor/UI/IconsFontAwesome6.h"

#include "../../vendor/imgui/imgui.h"
#include "../../vendor/stb-image/stb_image.h"

#include "../../backend/Assets/icons.h"

#include "../../../../src/vortex.h"
#include "../../../../src/vortex_core.h"

#ifndef InstanceWindow_H
#define InstanceWindow_H

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
        this->dockspaceID = ImGui::GetID(this->host->name.c_str());
        static ImGuiIO &io = ImGui::GetIO();

        // Mainwindow with dockspace
        if (ImGui::Begin(this->host->name.c_str(), &this->opened, ImGuiWindowFlags_MenuBar))
        {
            this->menubar();
            
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
        }
        else
        {
            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_KeepAliveOnly);
        }

        ImGui::End();

        // All Windows of this instances :
        this->UI_ParametersWindow();
        this->UI_ContentWindow();


    }

    void menubar(){

            if (ImGui::BeginMenuBar())
            {
                static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                static ImTextureID folderIcon = this->m_FolderIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);



                if(ImGui::ImageButtonWithText(saveIcon, "Save", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    //Save behavior
                } 
                
                if(ImGui::ImageButtonWithText(folderIcon, "Open", ImVec2(this->m_FolderIcon->GetWidth(), this->m_FolderIcon->GetHeight()))){
                    //Save behavior
                }

                ImGui::Separator();

                if (ImGui::BeginMenu("Pannels"))
                {
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Tools"))
                {
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Build"))
                {
                    if (ImGui::MenuItem("Build/Rebuild single parts"))
                    {
                    }
                    if (ImGui::MenuItem("Global build"))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Debug"))
                {
                    if (ImGui::MenuItem("Parameters window", NULL, &this->show_UI_ParametersWindow))
                    {
                    }
                    if (ImGui::MenuItem("Content window", NULL, &this->show_UI_ContentWindow))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Preview"))
                {
                    if (ImGui::MenuItem("Build/Rebuild single parts", NULL, &this->show_UI_ContentWindow))
                    {
                    }
                    if (ImGui::MenuItem("Global build"))
                    {
                    }
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if(ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight()))){
                    //Save behavior
                }
                if (ImGui::Button("Preview"))
                {
                }
                ImGui::Separator();
               

                if(ImGui::ImageButtonWithText(settingsIcon, "Settings", ImVec2(this->m_SettingsIcon->GetWidth(), this->m_SettingsIcon->GetHeight()))){
                    //Save behavior
                }
                ImGui::EndMenuBar();
            }
            
    }

    void UI_ContentWindow(){
        if (this->show_UI_ContentWindow)
        {

            static std::string label = this->host->name + " - Content Window";
            ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
            if (ImGui::Begin(label.c_str()))
            {
                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("table1", 3, flags))
                {
                    ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("CCC", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    for (int row = 0; row < 5; row++)
                    {
                        ImGui::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            ImGui::TableSetColumnIndex(column);
                            ImGui::Text("%s %d,%d", (column == 2) ? "Stretch" : "Fixed", column, row);
                        }
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::End();
        }
    

    }
    
    void UI_ParametersWindow(){
        if (this->show_UI_ParametersWindow)
        {

            static std::string label = this->host->name + " - Parameters Window";
            ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);
            if (ImGui::Begin(label.c_str()))
            {
                ImGui::TextUnformatted("Test");
                if (ImGui::CollapsingHeader("Properties"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Actions"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Scripts"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Patchs"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Console"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Build"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
            }
            ImGui::End();
        }

        
    }

    void setup(std::shared_ptr<VxHost> _host)
    {
        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_save, icons::i_save_size, w, h);
            m_SaveIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }
        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_add, icons::i_add_size, w, h);
            m_AddIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }
        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_folder, icons::i_folder_size, w, h);
            m_FolderIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }
        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_settings, icons::i_settings_size, w, h);
            m_SettingsIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }

        this->host = _host;
    }

    bool opened;
    bool show_UI_ContentWindow = false;
    bool show_UI_ParametersWindow = false;

    std::string name;
    VxContext *m_ctx;
    std::shared_ptr<VxHost> host;
    ImGuiID dockspaceID;

    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_SaveIcon;
    std::shared_ptr<Walnut::Image> m_SettingsIcon;
    std::shared_ptr<Walnut::Image> m_AddIcon;
    std::shared_ptr<Walnut::Image> m_FolderIcon;
};

#endif