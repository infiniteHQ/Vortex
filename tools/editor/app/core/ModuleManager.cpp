
#include "ModuleManager.hpp"

static int item_current = 0;

// Fonction pour charger le contenu hexadécimal d'un fichier .png
static std::vector<uint8_t> loadPngHex(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "Erreur lors de l'ouverture du fichier. -> " + filePath << std::endl;
        return {};
    }

    // Déterminez la taille du fichier
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Créez un vecteur pour stocker le contenu hexadécimal
    std::vector<uint8_t> hexContent(fileSize);

    // Lisez le contenu hexadécimal du fichier
    file.read(reinterpret_cast<char *>(hexContent.data()), fileSize);

    return hexContent;
}
static std::vector<std::shared_ptr<UIKit::Image>> logos;
static std::vector<ImTextureID> textures;

static void logo(const std::string &path, int index, int total)
{
    uint32_t w, h;
    // Chargez le contenu hexadécimal du fichier .png
    std::vector<uint8_t> hexTable = loadPngHex(path);
    const uint8_t *hexData = hexTable.data();

    if (total > logos.size())
    {
        void *data = UIKit::Image::Decode(hexData, hexTable.size(), w, h);
        std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data); // ML
        logos.push_back(_icon);
        VX_FREE(data);
        ImTextureID addIcon = logos[index]->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        textures.push_back(addIcon);
    }

    ImGui::Image(textures[index], ImVec2(50, 50));
}

ModuleManager::ModuleManager(VxContext *_ctx, InstanceFactory *_factory)
{
    this->ctx = _ctx;
    this->factory = _factory;
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
}

void ModuleManager::OnImGuiRender()
{
    static ImTextureID listIcon = this->m_ListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::Begin("Modules manager", &listIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        this->menubar();

    float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.3;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module of project(s)");
    ImGui::SameLine();
    ImGui::Text(this->ctx->name.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 0.9;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Vortex version");
    ImGui::SameLine();
    ImGui::Text(this->ctx->version.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    ImGui::Separator();

    // Left
    static int selected = 0;
    static std::vector<std::string> labels;
    static std::vector<std::string> syslabels;

    for (auto em : ctx->IO.em)
    {
        bool add = true;

        for (auto label : labels)
        {
            if (label == em->m_group)
            {
                add = false;
            }
        }

        if (add)
        {
            labels.push_back(em->m_group);
        }
    }

    for (auto em : ctx->IO.sys_em)
    {
        bool add = true;

        for (auto label : syslabels)
        {
            if (label == em->m_group)
            {
                add = false;
            }
        }

        if (add)
        {
            syslabels.push_back(em->m_group);
        }
    }

    {
        ImGui::BeginChild("left pane", ImVec2(230, -1), true);
        for (int i = 0; i < labels.size(); i++)
        {
            if (i == 0)
            {
                std::string label = "All modules (" + std::to_string(this->ctx->IO.em.size()) + ")";
                ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), label.c_str());
            }

            std::string label;

            int number = 0;
            for (auto em : this->ctx->IO.em)
            {
                if (em->m_group == labels[i])
                {
                    number++;
                }
            }
            label = labels[i];
            label += " (";
            label += std::to_string(number);
            label += ")";

            label += "##";
            label += labels[i];

            if (ImGui::Selectable(label.c_str(), selected == i))
                selected = i;
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();

    std::string label = "packhhqsdsdageView###";
    ImGuiID id = ImGui::GetID(label.c_str());
    ImGui::BeginChildFrame(id, ImVec2(0, 0), true);

    for (int i = 0; i < ctx->IO.em.size(); i++)
    {
        if (ctx->IO.em[i]->m_group == labels[selected])
        {
            std::string childLabel = "module##" + ctx->IO.em[i]->m_name;

            ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 250), true);

            {
                ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                logo(ctx->IO.em[i]->m_logo_path, i, this->ctx->IO.em.size());

                ImGui::EndChild();
                ImGui::SameLine();
            }

            {
                ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                float oldsize = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.3;
                ImGui::PushFont(ImGui::GetFont());

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ctx->IO.em[i]->m_proper_name.c_str());

                ImGui::GetFont()->Scale = oldsize;
                ImGui::PopFont();

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                ImGui::SameLine();
                ImGui::Text(ctx->IO.em[i]->m_version.c_str());
                ImGui::EndChild();
            }

            if (ctx->IO.em[i]->m_state == "running")
            {
                ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 0.8f), "Running");
            }
            if (ctx->IO.em[i]->m_state == "failed")
            {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 0.8f), "Failed");
            }
            if (ctx->IO.em[i]->m_state == "unknow" || ctx->IO.em[i]->m_state == "stopped")
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Not running");
            }

            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Name : ");
                ImGui::SameLine();
                ImGui::Text(ctx->IO.em[i]->m_name.c_str());
            }

            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) : ");
                ImGui::SameLine();
                ImGui::Text(ctx->IO.em[i]->m_author.c_str());
            }

            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description : ");
                ImGui::SameLine();
                ImGui::Text(ctx->IO.em[i]->m_description.c_str());
            }

            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Contributors : ");
                int i = 0;
                for (auto contributor : ctx->IO.em[i]->m_contributors)
                {
                    i++;
                    if (i <= 3)
                    {
                        if (i < ctx->IO.em[i]->m_contributors.size())
                        {
                            ImGui::SameLine();
                            std::string contri = contributor + ",";
                            ImGui::Text(contri.c_str());
                        }
                        else
                        {
                            ImGui::SameLine();
                            ImGui::Text(contributor.c_str());
                        }
                    }
                    else
                    {
                        ImGui::SameLine();
                        int counter = ctx->IO.em[i]->m_contributors.size() - 3;
                        std::string label = " and " + std::to_string(counter) + " other...";
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
                        break;
                    }
                }
            }

            if (ctx->IO.em[i]->m_state == "running")
            {
                if (ImGui::ImageButtonWithText(listIcon, "Stop", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    ctx->IO.em[i]->Stop();
                }
            }

            if (ctx->IO.em[i]->m_state == "failed")
            {
                if (ImGui::ImageButtonWithText(listIcon, "Retry to launch", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(ctx, ctx->IO.em[i]);
                    factory->SpawnInstance(instance);
                }
                ImGui::SameLine();

                if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                // Behavior
                }
            }
            if (ctx->IO.em[i]->m_state == "unknow" || ctx->IO.em[i]->m_state == "stopped")
            {
                if (ImGui::ImageButtonWithText(listIcon, "Launch", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    ctx->IO.em[i]->Start();
                }
                ImGui::SameLine();

                if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                // Behavior
                }
            }

            ImGui::EndChild();
        }
    }

    ImGui::EndChildFrame();
    ImGui::End();
}


// Helper functions for menu items

void handleRefresh() {
    // Behavior
}

void handleAddToProject() {
    // Behavior
}

void handleFilterBuildRebuild() {
    // Behavior
}

void handleGlobalBuild() {
    // Behavior
}

void handleCreateModule() {
    // Behavior
}

void handleSearch() {
    // Behavior
}

// Main menu function

void ModuleManager::menubar() {
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static bool open_ADDMODULE = false;

    // Left
    static int selected = 0;
    static std::vector<std::string> syslabels;

    for (auto em : ctx->IO.sys_em) {
        bool add = true;

        for (auto label : syslabels) {
            if (label == em->m_group) {
                add = false;
            }
        }

        if (add) {
            syslabels.push_back(em->m_group);
        }
    }

    if (open_ADDMODULE) {
        if (ImGui::BeginPopupModal("Add module(s)", NULL, ImGuiWindowFlags_MenuBar)) {
            if (ImGui::BeginMenuBar()) {
                if (ImGui::Button("Close")) {
                    open_ADDMODULE = false;
                }
                if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight()))) {
                    handleRefresh();
                }
                ImGui::Separator();
                if (ImGui::ImageButtonWithText(addIcon, "Browse public modules", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight()))) {
                    // Behavior
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Filters")) {
                    if (ImGui::MenuItem("Build/Rebuild single parts")) {
                        handleFilterBuildRebuild();
                    }
                    if (ImGui::MenuItem("Global build")) {
                        handleGlobalBuild();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::BeginChild("left pane", ImVec2(230, -1), true);
            for (int i = 0; i < syslabels.size(); i++) {
                if (i == 0) {
                    std::string label = "All modules (" + std::to_string(this->ctx->IO.sys_em.size()) + ")";
                    ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), label.c_str());
                }

                std::string label;

                int number = 0;
                for (auto sysem : this->ctx->IO.sys_em) {
                    if (sysem->m_group == syslabels[i]) {
                        number++;
                    }
                }
                label = syslabels[i];
                label += " (";
                label += std::to_string(number);
                label += ")";

                label += "##";
                label += syslabels[i];

                if (ImGui::Selectable(label.c_str(), selected == i))
                    selected = i;
            }
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::Separator();
            ImGui::SameLine();

            std::string label = "packhhqsdsdageView###";
            ImGuiID id = ImGui::GetID(label.c_str());
            ImGui::BeginChildFrame(id, ImVec2(0, 0), true);

            for (int i = 0; i < ctx->IO.sys_em.size(); i++) {
                if (ctx->IO.sys_em[i]->m_group == syslabels[selected]) {
                    std::string childLabel = "module##" + ctx->IO.sys_em[i]->m_name;

                    ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 250), true);

                    {
                        ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                        logo(ctx->IO.sys_em[i]->m_logo_path, i, this->ctx->IO.sys_em.size());

                        ImGui::EndChild();
                        ImGui::SameLine();
                    }

                    {
                        ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                        float oldsize = ImGui::GetFont()->Scale;
                        ImGui::GetFont()->Scale *= 1.3;
                        ImGui::PushFont(ImGui::GetFont());

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ctx->IO.sys_em[i]->m_proper_name.c_str());

                        ImGui::GetFont()->Scale = oldsize;
                        ImGui::PopFont();

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                        ImGui::SameLine();
                        ImGui::Text(ctx->IO.sys_em[i]->m_version.c_str());
                        ImGui::EndChild();
                    }

                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Name : ");
                        ImGui::SameLine();
                        ImGui::Text(ctx->IO.sys_em[i]->m_name.c_str());
                    }
                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) : ");
                        ImGui::SameLine();
                        ImGui::Text(ctx->IO.sys_em[i]->m_author.c_str());
                    }
                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description : ");
                        ImGui::SameLine();
                        ImGui::Text(ctx->IO.sys_em[i]->m_description.c_str());
                    }
                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Contributors : ");
                        int i = 0;
                        for (auto contributor : ctx->IO.sys_em[i]->m_contributors) {
                            i++;
                            if (i <= 3) {
                            }
                            else {
                                ImGui::SameLine();
                                int counter = ctx->IO.sys_em[i]->m_contributors.size() - 3;
                                std::string label = " and " + std::to_string(counter) + " other...";
                                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
                                break;
                            }
                        }
                    }

                    if (ImGui::ImageButtonWithText(addIcon, "Add to the current project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight()))) {
                        handleAddToProject();
                    }
                    ImGui::EndChild();
                }
                ImGui::EndChildFrame();
            }
            ImGui::EndPopup();
        }
    }

    if (open_ADDMODULE)
        ImGui::OpenPopup("Add module(s)");

    if (ImGui::BeginMenuBar()) {
        if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight()))) {
            open_ADDMODULE = true;
        }

        if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight()))) {
            handleRefresh();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Filters")) {
            if (ImGui::MenuItem("Build/Rebuild single parts")) {
                handleFilterBuildRebuild();
            }
            if (ImGui::MenuItem("Global build")) {
                handleGlobalBuild();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Create a module")) {
            if (ImGui::MenuItem("Build/Rebuild single parts")) {
                handleCreateModule();
            }
            if (ImGui::MenuItem("Global build")) {
                handleGlobalBuild();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Search")) {
            if (ImGui::MenuItem("Build/Rebuild single parts")) {
                handleSearch();
            }
            if (ImGui::MenuItem("Global build")) {
                handleGlobalBuild();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
