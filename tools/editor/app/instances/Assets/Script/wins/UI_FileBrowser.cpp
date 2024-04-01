#include "../ScriptInstance.h"
#include <array>
#include <filesystem>

namespace fs = std::filesystem;



class NodeTree {
public:
    NodeTree(const std::string& rootDirectory, std::shared_ptr<ScriptInstance> instance) 
        :m_RootDirectory(rootDirectory), m_CurrentDirectory(rootDirectory), m_Instance(instance) 
        {};

    void OnImGuiRender();
    std::string m_SelectedDirectory;

private:
    void RenderNode(const std::filesystem::path& nodePath);
    
    std::filesystem::path m_RootDirectory;
    std::filesystem::path m_CurrentDirectory;

    std::shared_ptr<ScriptInstance> m_Instance;
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
                    m_SelectedDirectory =  m_CurrentDirectory;
                    m_SelectedDirectory += "/" + entryName;
                    std::cout << "Selected: " << m_SelectedDirectory << std::endl;

				    std::shared_ptr<TextEditorInstance> instance = std::make_shared<TextEditorInstance>(this->m_Instance->m_ctx, m_SelectedDirectory);
				    this->m_Instance->factory->SpawnInstance(instance);	

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


void ScriptInstance::UI_FileBrowser()
{

    if (this->show_UI_FileBrowser)
    {

        std::string label = this->name + " - Script Settings###" + this->name + " - Compilations Arguments";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &editIcon, &this->show_UI_FileBrowser, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save", "CTRL+S"))
                {
                }
                if (ImGui::MenuItem("Save As.."))
                {
                }
                if (ImGui::MenuItem("Close"))
                {
                    this->show_UI_FileBrowser = false;
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            ImGui::Text("Opened editors :");

            if (ImGui::ImageButtonWithText(editIcon, "MyScript.sh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
            }
            if (ImGui::ImageButtonWithText(editIcon, "Tes.sh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
            }
            if (ImGui::ImageButtonWithText(editIcon, "sdfs.sh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
            }
            


            ImGui::EndMenuBar();
        }

        static std::string currentPath = this->script->path;
        static std::string old = "?";

        {
            ImGui::BeginChild("left pane", ImVec2(0, 0), true);
            static std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(*this);
            static NodeTree tree(currentPath, instance);


            tree.OnImGuiRender();
            

            ImGui::EndChild();
        }

        ImGui::End();
    }
}
