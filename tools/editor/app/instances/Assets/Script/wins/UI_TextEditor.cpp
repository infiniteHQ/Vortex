#include "../ScriptInstance.h"
#include <array>
#include <filesystem>

namespace fs = std::filesystem;



class NodeTree {
public:
    NodeTree(const std::filesystem::path& rootDirectory) 
        : m_RootDirectory(rootDirectory), m_CurrentDirectory(rootDirectory) {}

    void OnImGuiRender();
    std::filesystem::path m_SelectedDirectory;

private:
    void RenderNode(const std::filesystem::path& nodePath);
    
    std::filesystem::path m_RootDirectory;
    std::filesystem::path m_CurrentDirectory;
};

void NodeTree::RenderNode(const std::filesystem::path& nodePath) {
    const std::string nodeName = nodePath.filename().string();
    if (ImGui::TreeNode(nodeName.c_str())) {
        for (const auto& entry : std::filesystem::directory_iterator(nodePath)) {
            const auto& path = entry.path();
            const std::string entryName = path.filename().string();
            if (entry.is_directory()) {
                RenderNode(path);
            } else {
                ImGui::Bullet();
                if(ImGui::Selectable(entryName.c_str())){
                    m_SelectedDirectory =  m_CurrentDirectory.c_str() + '/';
                    m_SelectedDirectory += entryName;
                    std::cout << "Selected: " << m_SelectedDirectory << std::endl;
                }
            }
        }
        ImGui::TreePop();
    }
}

void NodeTree::OnImGuiRender() {

    if (m_CurrentDirectory != m_RootDirectory) {
        if (ImGui::Button("<-")) {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }
    }

    RenderNode(m_CurrentDirectory);

}


void ScriptInstance::UI_TextEditor()
{

    if (this->show_UI_TextEditor)
    {

        std::string label = this->name + " - Script Settings###" + this->name + " - Compilations Arguments";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &editIcon, &this->show_UI_TextEditor, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(saveIcon, "Save", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                this->Save();
            }
            if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                this->Refresh();
            }

            const char *items[] = {"Packages", "Scripts", "Patchs", "Automations"};
            static int item_current = 0;
            ImGui::Combo("Type", &item_current, items, IM_ARRAYSIZE(items));

            ImGui::Separator();
            if (ImGui::BeginMenu("Pannels"))
            {
                if (ImGui::MenuItem("Options Editor"))
                {
                }
                if (ImGui::MenuItem("Contents Window"))
                {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Left
        static int selected = 0;
        static std::array<char[128], 4> labels = {"Project Settings", "Configuration", "Production", "Maintenance"};

        static std::string currentPath = this->script->path;
        static std::string old = "?";

        if (ImGui::GetIO().MouseClicked[1])
        {
            ImGuiContext &g = *GImGui;
            if (g.OpenPopupStack.size() > 0)
                g.OpenPopupStack.pop_back();

            ImGui::OpenPopup("test");
        }

        {
            ImGui::BeginChild("left pane", ImVec2(500, 0), true);

            static NodeTree tree(currentPath);
            old = currentPath;

            if(tree.m_SelectedDirectory != "" && tree.m_SelectedDirectory != currentPath){
                currentPath = tree.m_SelectedDirectory;
            }


            this->editor->SwitchToFile(currentPath);

            tree.OnImGuiRender();
            



            ImGui::EndChild();
        }

        ImGui::SameLine();

        // ImGui::BeginChild("Right pane", ImVec2(0, 0), true);
        this->editor->Render("##Editor");
        
        // ImGui::EndChild();

        // Right
        {
        }

        ImGui::End();
    }
}
