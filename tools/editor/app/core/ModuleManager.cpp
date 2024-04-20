#include "ModuleManager.hpp"

#include <iostream>

static int item_current = 0;

// Fonction pour charger le contenu hexadécimal d'un fichier .png
static std::vector<uint8_t> loadPngHex(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
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
static std::vector<std::shared_ptr<Walnut::Image>> logos;
static std::vector<ImTextureID> textures;

static void logo(const std::string &path, int index, int total)
{

    uint32_t w, h;
    // Chargez le contenu hexadécimal du fichier .png
    std::vector<uint8_t> hexTable = loadPngHex(path);
    const uint8_t *hexData = hexTable.data();

    if(total > logos.size()){
        void *data = Walnut::Image::Decode(hexData, hexTable.size(), w, h);
        std::shared_ptr<Walnut::Image> _icon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data); // ML
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
        void *data = Walnut::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
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

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module of ");
    ImGui::SameLine();
    ImGui::Text(this->ctx->name.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    ImGui::Separator();

    // Left
    static int selected = 0;
    static std::array<char[128], 3> labels = {"Instances", "Utils", "Workers"};

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

            switch (i)
            {
            case 0:
            {

                int number = 0;
                for (auto em : this->ctx->IO.em)
                {
                    if (em->m_type == "em_instanciable")
                    {
                        number++;
                    }
                }
                label = labels[i];
                label += " (";
                label += std::to_string(number);
                label += ")";
                break;
            }
            case 1:
            {

                int number = 0;
                for (auto em : this->ctx->IO.em)
                {
                    if (em->m_type == "em_instanciable")
                    {
                        number++;
                    }
                }
                label = labels[i];
                label += " (";
                label += std::to_string(number);
                label += ")";
                break;
            }
            case 2:
            {

                int number = 0;
                for (auto em : this->ctx->IO.em)
                {
                    if (em->m_type == "other..")
                    {
                        number++;
                    }
                }
                label = labels[i];
                label += " (";
                label += std::to_string(number);
                label += ")";
                break;
            }
            }

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
    for (int row = 0; row < this->ctx->IO.em.size(); row++)
    {
        switch (selected)
        {
        case 1:
        {
            if (this->ctx->IO.em[row]->m_type == "em_instanciable")
            {

                std::string childLabel = "module##" + this->ctx->IO.em[row]->m_name;

                ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 220), true);

                {
                ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                    logo(this->ctx->IO.em[row]->m_logo_path, row, this->ctx->IO.em.size());
                   
            ImGui::EndChild();
             ImGui::SameLine();
                }

                {
                ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                    float oldsize = ImGui::GetFont()->Scale;
                    ImGui::GetFont()->Scale *= 1.3;
                    ImGui::PushFont(ImGui::GetFont());

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), this->ctx->IO.em[row]->m_proper_name.c_str());

                    ImGui::GetFont()->Scale = oldsize;
                    ImGui::PopFont();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                    ImGui::SameLine();
                    ImGui::Text(this->ctx->IO.em[row]->m_version.c_str());
            ImGui::EndChild();
                }

                {

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Name : ");
                    ImGui::SameLine();
                    ImGui::Text(this->ctx->IO.em[row]->m_name.c_str());
                }


                {

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) : ");
                    ImGui::SameLine();
                    ImGui::Text(this->ctx->IO.em[row]->m_author.c_str());
                }

                {

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description : ");
                    ImGui::SameLine();
                    ImGui::Text(this->ctx->IO.em[row]->m_description.c_str());
                }

                {

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Contributors : ");
                    int i = 0;
                    for (auto contributor : this->ctx->IO.em[row]->m_contributors)
                    {
                        i++;
                        if (i <= 3)
                        {
                            if (i < this->ctx->IO.em[row]->m_contributors.size())
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
                            int counter = this->ctx->IO.em[row]->m_contributors.size() - 3;
                            std::string label = " and " + std::to_string(counter) + " other...";
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
                            break;
                        }
                    }
                }

                if (ImGui::ImageButtonWithText(listIcon, "Details", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {

                    std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(ctx, this->ctx->IO.em[row]);
                    factory->SpawnInstance(instance);
                }
                ImGui::SameLine();

                if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                }
            }
            ImGui::EndChild();
        }
        }
    }
    ImGui::EndChildFrame();
    ImGui::End();
}

void ModuleManager::menubar()
{

    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::BeginMenuBar())
    {

        const char *items[] = {"Treev iew", "Cards view"};

        ImGui::Combo("View", &item_current, items, IM_ARRAYSIZE(items));

        ImGui::Separator();

        if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
        }
        if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
        {
            ImGui::OpenPopup("CreationMenu");
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Filters"))
        {

            if (ImGui::MenuItem("Build/Rebuild single parts"))
            {
            }
            if (ImGui::MenuItem("Global build"))
            {
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}