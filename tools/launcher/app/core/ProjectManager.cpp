
#include "ProjectManager.hpp"

static std::vector<std::string> labels = {"Open a project", "Create a new project"};
static int selected = 0;
static int selected_template = 0;
static bool template_is_selected = false;
static bool project_creation = false;
static std::shared_ptr<TemplateInterface> selected_template_object;
static std::shared_ptr<EnvProject> selected_envproject;
static std::string title = "none";
static std::string default_project_avatar = "/usr/local/include/Vortex/imgs/base_vortex.png";
static std::string operating_system_banner = "/usr/local/include/Vortex/1.1/imgs/operating_system_banner.png";

static std::vector<uint8_t> loadPngHex(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "Erreur lors de l'ouverture du fichier. -> " + filePath << std::endl;
        return {};
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> hexContent(fileSize);

    file.read(reinterpret_cast<char *>(hexContent.data()), fileSize);

    return hexContent;
}
static std::vector<std::shared_ptr<UIKit::Image>> logos;
static std::vector<ImTextureID> textures;
static std::vector<std::shared_ptr<std::pair<std::string, int>>> labels_indexes;
static std::vector<std::string> syslabels;

static std::vector<std::pair<std::string, ImTextureID>> associated_textures;

void addTexture(const std::string &name, const std::string &path)
{
    uint32_t w = 0, h = 0;

    if (path.empty())
    {
        std::cerr << "Chemin de fichier invalide." << std::endl;
        return;
    }

    if (associated_textures.empty())
    {
        std::vector<uint8_t> hexTable = loadPngHex(path);
        const uint8_t *hexData = hexTable.data();

        if (!hexTable.empty())
        {
            void *data = UIKit::Image::Decode(hexData, hexTable.size(), w, h);
            std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
            logos.push_back(_icon);
            VX_FREE(data);
            if (data)
            {
                ImTextureID addIcon = _icon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); 
                associated_textures.push_back(std::make_pair(name, addIcon));                
                std::cout << "Premiere Texture ajoutée : " << name << std::endl;
            }
            else
            {
                std::cerr << "Échec du décodage de l'image." << std::endl;
            }
        }
    }
    else
    {
        for (auto &texture : associated_textures)
        {
            if (texture.first == name)
            {
                std::cout << "Une texture avec le nom \"" << name << "\" existe déjà." << std::endl;
                return;
            }
        }
        std::vector<uint8_t> hexTable = loadPngHex(path);
        const uint8_t *hexData = hexTable.data();

        if (!hexTable.empty())
        {

            void *data = UIKit::Image::Decode(hexData, hexTable.size(), w, h);
            std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
            logos.push_back(_icon);
            VX_FREE(data);
            if (data)
            {
                ImTextureID addIcon = _icon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); 
                associated_textures.push_back(std::make_pair(name, addIcon));                             
                std::cout << "Enieme Texture ajoutée : " << name << std::endl;
            }
            else
            {
                std::cerr << "Échec du décodage de l'image." << std::endl;
            }
        }
    }
}

ImTextureID getTexture(const std::string &name, ImDrawList *drawList, ImVec2 pos, ImVec2 size)
{
    for (const auto &texture : associated_textures)
    {
        if (texture.first == name)
        {
            drawList->AddImage(texture.second, pos, ImVec2(pos.x + size.x, pos.y + size.y));
            return texture.second;
        }
    }
    std::cerr << "Texture non trouvée pour le nom : " << name << std::endl;
    return nullptr;
}

static void logo(const std::string &path, int index, int total)
{
    sleep(0.1);
    uint32_t w = 0, h = 0;
    
    if (path.empty())
    {
        std::cerr << "Chemin de fichier invalide." << std::endl;
        return;
    }

    std::vector<uint8_t> hexTable = loadPngHex(path);
    const uint8_t *hexData = hexTable.data();

    if (total > logos.size() && !hexTable.empty())
    {
        void *data = UIKit::Image::Decode(hexData, hexTable.size(), w, h);
        if (data)
        {
            std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
            logos.push_back(_icon);
            VX_FREE(data);
            ImTextureID addIcon = _icon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            textures.push_back(addIcon);
        }
        else
        {
            std::cerr << "Échec du décodage de l'image." << std::endl;
        }
    }

    if (index >= 0 && index < textures.size())
    {
        ImGui::Image(textures[index], ImVec2(50, 50));
    }
    else
    {
        std::cerr << "Index hors limites." << std::endl;
    }
}
static void logo(const std::string &path, int index, int total, ImDrawList *drawList, ImVec2 pos, ImVec2 size)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    uint32_t w = 0, h = 0;

    if (path.empty())
    {
        std::cerr << "Chemin de fichier invalide." << std::endl;
        return;
    }

    std::vector<uint8_t> hexTable = loadPngHex(path);
    const uint8_t *hexData = hexTable.data();

    if (total > logos.size() && !hexTable.empty())
    {
        void *data = UIKit::Image::Decode(hexData, hexTable.size(), w, h);
        if (data)
        {
            std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
            logos.push_back(_icon);
            VX_FREE(data);
            ImTextureID addIcon = _icon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); // Utiliser 0 pour la simulation
            textures.push_back(addIcon);
            std::cout << textures.size() << std::endl;
        }
        else
        {
            std::cerr << "Échec du décodage de l'image." << std::endl;
        }
    }

    if (index >= 0 && index < textures.size())
    {
        drawList->AddImage(textures[index], pos, ImVec2(pos.x + size.x, pos.y + size.y));
    }
    else
    {
        std::cerr << "Index hors limites." << std::endl;
    }
}

ProjectManager::ProjectManager(VxContext *_ctx)
{
    this->ctx = _ctx;

    this->Refresh();

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_project, icons::i_project_size, w, h);
        m_ProjectIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_open, icons::i_open_size, w, h);
        m_OpenIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
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


void MyButton(const std::string& name, int w, int h)
{
    ImVec2 squareSize(w, h);
    ImVec2 totalSize(squareSize.x, squareSize.y + 5); 
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    std::string button_id = name + "squareButtonWithText";
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    addTexture(name, default_project_avatar);
    getTexture(name, drawList, cursorPos, squareSize);

    ImVec2 smallRectSize(40, 20);                                                                                           
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5); 

    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

void MyButton(const std::shared_ptr<EnvProject> envproject)
{
    ImVec2 squareSize(100, 100);

    const char *originalText = envproject->name.c_str();
    char truncatedText[12]; 

    if (strlen(originalText) > 8)
    {
        strncpy(truncatedText, originalText, 8);
        strcpy(truncatedText + 8, "...");
    }
    else
    {
        strcpy(truncatedText, originalText);
    }

    ImVec2 textSize = ImGui::CalcTextSize(truncatedText);
    ImVec2 totalSize(squareSize.x, squareSize.y + textSize.y + 5); 

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    std::string button_id = envproject->name + "squareButtonWithText" + envproject->lastOpened;
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
        selected_envproject = envproject;
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    addTexture("test", default_project_avatar);
    getTexture("test", drawList, cursorPos, squareSize);

    ImVec2 smallRectSize(40, 20);                                                                               
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5); 

    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));
    ImVec2 versionTextPos = ImVec2(smallRectPos.x + (smallRectSize.x - ImGui::CalcTextSize(envproject->version.c_str()).x) / 2, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize("version").y) / 2);
    drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), envproject->version.c_str());

    ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), truncatedText);

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

void MyBanner(const std::string& path)
{
    addTexture(path, path);
    ImVec2 squareSize(300, 70);

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    if (ImGui::InvisibleButton("##squ5684areButtonWithText2", squareSize))
    {
        std::cout << "Hello World 2" << std::endl;
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    getTexture(path, drawList, cursorPos, squareSize);

    ImVec2 smallRectSize(40, 20);                                                                                            
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5); 
}

void ProjectManager::OnImGuiRender()
{
    static ImTextureID projectIcon = this->m_ProjectIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::Begin("Project managers", &projectIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        this->menubar();

    if (!project_creation)
    {
        float columnWidths[2] = {345.0f, 100.0f};

        ImVec2 windowSize = ImGui::GetWindowSize();

        float availableWidth = windowSize.x - ImGui::GetStyle().ItemSpacing.x * 2;

        float totalColumnWidths = 0.0f;
        for (int i = 0; i < 2; ++i)
            totalColumnWidths += columnWidths[i];

        float columnProportions[2];
        for (int i = 0; i < 2; ++i)
            columnProportions[i] = columnWidths[i] / totalColumnWidths;

        ImGui::Columns(2, nullptr, false);

        ImVec4 columnColors[2] = {ImVec4(1.0f, 0.0f, 0.0f, 0.0f),  
                                  ImVec4(0.0f, 0.0f, 1.0f, 0.0f)}; 

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        for (int i = 0; i < 2; ++i)
        {
            float columnWidth = availableWidth * columnProportions[i];
            ImGui::SetColumnWidth(i, columnWidth);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, columnColors[i]);
            ImGui::BeginChild(ImGui::GetID((void *)(intptr_t)i), ImVec2(columnWidth, 0), true);
            if (i == 0)
            {
                for (int row = 0; row < this->ctx->IO.sys_projects.size(); row++)
                {
                    MyButton(this->ctx->IO.sys_projects[row]);
                }
            }
            else if (i == 1)
            {
                if (!selected_envproject)
                {
                    ImGui::Text("Select a project");
                }
                else
                {
                    {
                        ImGui::BeginChild("LOGO_", ImVec2(70, 70), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                        if (selected_envproject->logoPath.empty())
                        {
                            MyButton("tezt", 60, 60);
                        }
                        ImGui::EndChild();
                        ImGui::SameLine();
                    }

                    {
                        ImGuiID _id = ImGui::GetID("TEXT_");
                        ImGui::BeginChild(_id, ImVec2(0, 70), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                        float oldsize = ImGui::GetFont()->Scale;
                        ImGui::GetFont()->Scale *= 1.3;
                        ImGui::PushFont(ImGui::GetFont());

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_envproject->name.c_str());

                        ImGui::GetFont()->Scale = oldsize;
                        ImGui::PopFont();

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Last opened ");
                        ImGui::SameLine();
                        ImGui::Text(selected_envproject->lastOpened.c_str());

                        ImGui::EndChild();
                    }

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) ");
                    ImGui::SameLine();
                    ImGui::Text(selected_envproject->author.c_str());

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description ");
                    ImGui::SameLine();
                    ImGui::Text(selected_envproject->description.c_str());

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Version ");
                    ImGui::SameLine();
                    ImGui::Text(selected_envproject->version.c_str());

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Compatible with ");
                    ImGui::SameLine();
                    ImGui::Text(selected_envproject->version.c_str());

                    ImVec2 windowSize = ImGui::GetWindowSize();
                    ImVec2 contentSize = ImGui::GetContentRegionAvail();
                    ImVec2 buttonSize = ImVec2(100, 30); 

                    float firstButtonPosX = windowSize.x - buttonSize.x * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

                    float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - 5;

                    ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

                    if (ImGui::Button("Settings", buttonSize))
                    {
                    }

                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 1.0f, 0.8f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 1.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.0f, 1.8f));
                    if (ImGui::Button("Open Project", buttonSize))
                    {

                        std::string projectPath = selected_envproject->path;

                        std::thread([projectPath]()
                                    {
        std::string cmd = "cd " + projectPath + " && vortex -e";
        system(cmd.c_str()); })
                            .detach();
                    }
                    ImGui::PopStyleColor(3);
                }
            }

            ImGui::EndChild();
            ImGui::PopStyleColor();

            if (i < 1)
                ImGui::NextColumn();
        }

        ImGui::PopStyleVar(4);
        // Vous pouvez ajouter le reste de votre code ImGui ici
    }
    else
    {
        float columnWidths[3] = {100.0f, 300.0f, 100.0f};

        ImVec2 windowSize = ImGui::GetWindowSize();

        float availableWidth = windowSize.x - ImGui::GetStyle().ItemSpacing.x * 2;

        float totalColumnWidths = 0.0f;
        for (int i = 0; i < 3; ++i)
            totalColumnWidths += columnWidths[i];

        float columnProportions[3];
        for (int i = 0; i < 3; ++i)
            columnProportions[i] = columnWidths[i] / totalColumnWidths;

        ImGui::Columns(3, nullptr, false);

        ImVec4 columnColors[3] = {ImVec4(1.0f, 0.0f, 0.0f, 0.0f),  
                                  ImVec4(0.0f, 0.0f, 1.0f, 0.0f),  
                                  ImVec4(0.0f, 1.0f, 0.0f, 0.0f)};

        for (int i = 0; i < 3; ++i)
        {
            float columnWidth = availableWidth * columnProportions[i];
            ImGui::SetColumnWidth(i, columnWidth);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, columnColors[i]);
            ImGui::BeginChild(ImGui::GetID((void *)(intptr_t)i), ImVec2(columnWidth, 0), true);
            if (i == 0)
            {
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_all_bases.png");
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_operating_systems.png");
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_app_svc.png");
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_tools_utils.png");
            }
            else if (i == 1)
            {
                for (int row = 0; row < this->ctx->IO.sys_projects.size(); row++)
                {
                }
            }
            else if (i == 2)
            {
                ImGui::Text("Defs");
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();

            if (i < 2)
                ImGui::NextColumn();
        }

    }
    ImGui::End();
}

/*
float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.3;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), title.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 0.9;
    ImGui::PushFont(ImGui::GetFont());

    std::string label = "Vortex version ";
    label += ctx->version.c_str();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
    ImGui::SameLine();
    ImGui::Text(this->ctx->version.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    ImGui::Separator();

    {
        ImGui::BeginChild("left pane", ImVec2(230, -1), true);

        for (int i = 0; i < labels.size(); i++)
        {
            if (ImGui::Selectable(labels[i].c_str(), selected == i))
                selected = i;
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();
    static int _i;
    if (selected == 0)
    {
        title = "Open a project";

        ImGuiID _id = ImGui::GetID("_main");
        ImGui::BeginChildFrame(_id, ImVec2(0, 0), true);
        for (int row = 0; row < this->ctx->IO.sys_projects.size(); row++)
        {
            std::string label = this->ctx->IO.sys_projects[row]->name + "###" + std::to_string(row);
            ImGuiID id = ImGui::GetID(label.c_str());
            ImGui::BeginChild(id, ImVec2(0, 220), true);

            {
                ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                if (this->ctx->IO.sys_projects[row]->logoPath.empty())
                {
                    logo(default_project_avatar, row, this->ctx->IO.sys_projects.size());
                }
                else
                {
                    logo(this->ctx->IO.sys_projects[row]->logoPath, row, this->ctx->IO.sys_projects.size());
                }

                ImGui::EndChild();
                ImGui::SameLine();
            }

            {
                ImGuiID _id = ImGui::GetID("TEXT_");
                ImGui::BeginChild(_id, ImVec2(0, 70), true);
                float oldsize = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.3;
                ImGui::PushFont(ImGui::GetFont());

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), this->ctx->IO.sys_projects[row]->name.c_str());

                ImGui::GetFont()->Scale = oldsize;
                ImGui::PopFont();

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Last opened ");
                ImGui::SameLine();
                ImGui::Text(this->ctx->IO.sys_projects[row]->lastOpened.c_str());

                ImGui::EndChild();
            }

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) ");
            ImGui::SameLine();
            ImGui::Text(this->ctx->IO.sys_projects[row]->author.c_str());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description ");
            ImGui::SameLine();
            ImGui::Text(this->ctx->IO.sys_projects[row]->description.c_str());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Version ");
            ImGui::SameLine();
            ImGui::Text(this->ctx->IO.sys_projects[row]->version.c_str());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Compatible with ");
            ImGui::SameLine();
            ImGui::Text(this->ctx->IO.sys_projects[row]->version.c_str());

            if (ImGui::ImageButtonWithText(openIcon, "Open"))
            {
                std::string projectPath = this->ctx->IO.sys_projects[row]->path;

                std::thread([projectPath]()
                            {
        std::string cmd = "cd " + projectPath + " && vortex -e";
        system(cmd.c_str()); })
                    .detach();
            }

            ImGui::EndChildFrame();
        }
    }
    else if (selected == 1 && template_is_selected == false)
    {

        // Appel de votre fonction pour dessiner le bouton
        MyButton();
        MuBanner();
        title = "Create a new project";
        ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);

        ImGui::Columns(2, NULL);

        for (int project = 0; project < project_templates.size(); project++)
        {
            if (project_templates[project] != NULL)
            {
                {
                    ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                    try
                    {
                        logo(project_templates[project]->m_logo_path, project, project_templates.size());
                    }
                    catch (std::exception e)
                    {
                    }
                    ImGui::EndChild();
                    ImGui::SameLine();
                }

                {
                    ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                    float oldsize = ImGui::GetFont()->Scale;
                    ImGui::GetFont()->Scale *= 1.3;
                    ImGui::PushFont(ImGui::GetFont());

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), project_templates[project]->m_proper_name.c_str());

                    ImGui::GetFont()->Scale = oldsize;
                    ImGui::PopFont();

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                    ImGui::SameLine();
                    ImGui::Text(project_templates[project]->m_author.c_str());
                    ImGui::EndChild();
                }

                ImGui::TextWrapped(project_templates[project]->m_description.c_str());

                if (ImGui::Button("Create"))
                {
                    template_is_selected = true;
                    selected_template_object = project_templates[project];
                }

                ImGui::NextColumn();
            }
        }
        ImGui::EndChild();
    }
    else if (selected == 1 && template_is_selected == true)
    {
        title = "Create a new project based on " + selected_template_object->m_proper_name;
        ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);

        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static char buf[255]{};
        std::string s{VortexMaker::getHomeDirectory() + "/VortexProjects/"};

        strncpy(buf, s.c_str(), sizeof(buf) - 1);

        static char name[128] = "UnknowName";
        static char desc[128] = "My project description !";
        static char version[128] = "1.0.0";
        static char auth[128] = "you";
        static bool open = true;
        {
            ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
            if (!selected_template_object->m_logo_path.empty())
            {
                logo(selected_template_object->m_logo_path, _i++, project_templates.size());
            }

            ImGui::EndChild();
            ImGui::SameLine();
        }

        {
            ImGui::BeginChild("TEXT_", ImVec2(0, 68), true);
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.3;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), name);
            ImGui::SameLine();

            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();
            static std::string _name = " based on " + selected_template_object->m_proper_name;
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), _name.c_str());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "from");
            ImGui::SameLine();
            ImGui::Text(auth);
            ImGui::EndChild();
        }

        ImGui::InputText("Path", buf, sizeof(buf));
        ImGui::InputText("Name", name, 128);
        ImGui::InputText("Description", desc, 128);
        ImGui::InputText("Authors", auth, 128);
        ImGui::InputText("Version", version, 128);

        ImGui::Separator();
        float oldsize = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= 1.2;
        ImGui::PushFont(ImGui::GetFont());
        ImGui::Text("About the template used : ");
        ImGui::GetFont()->Scale = oldsize;
        ImGui::PopFont();
        ImGui::Text("Name & Version : ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), selected_template_object->m_proper_name.c_str());
        ImGui::Text("Description : ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), selected_template_object->m_description.c_str());
        ImGui::Separator();

        ImGui::Checkbox("Open the project after creation", &open);
        std::string path = s + name;

        ImGui::Text("This new project will be install in "); // Fix path and allow project creation + fix core dumped
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), path.c_str());

        if (ImGui::Button("Back"))
        {
            template_is_selected = false;
        }
        ImGui::SameLine();

        if (ImGui::ImageButtonWithText(addIcon, "Create", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
            VortexMaker::CreateProject(name, auth, version, desc, path, selected_template_object->m_name);
        }
        ImGui::EndChild();
    }

*/

// Helper functions for menu items

static void handleRefresh()
{
    // Behavior
}

static void handleAddToProject(const std::string &name, const std::string &version)
{
    // Behavior
}

static void handleFilterBuildRebuild()
{
    // Behavior
}

static void handleGlobalBuild()
{
    // Behavior
}

static void handleCreateModule()
{
    // Behavior
}

static void handleSearch()
{
    // Behavior
}

void ProjectManager::addModuleModal()
{
}

void ProjectManager::mainButtonsMenuItem()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    if (!project_creation)
    {
        if (ImGui::ImageButtonWithText(addIcon, "Create a project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
            project_creation = true;
        }
    }
    else
    {
        if (ImGui::ImageButtonWithText(addIcon, "Open a project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
            project_creation = false;
        }
    }
}

void ProjectManager::filterMenuItem()
{
    ImGui::Separator();
    if (ImGui::BeginMenu("Filters"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleFilterBuildRebuild();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectManager::createMenuItem()
{
    if (ImGui::BeginMenu("Create a module"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleCreateModule();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectManager::searchMenuItem()
{
    if (ImGui::BeginMenu("Search"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleSearch();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectManager::menubar()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::BeginMenuBar())
    {
        this->mainButtonsMenuItem();
        ImGui::EndMenuBar();
    }
}

void ProjectManager::Refresh()
{
    project_templates.clear();
    for (auto tem : this->ctx->IO.sys_templates)
    {
        if (tem->m_type == "project")
        {
            project_templates.push_back(tem);
        }
    }
};
