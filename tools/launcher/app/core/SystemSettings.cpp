#include "SystemSettings.hpp"

static std::string current_section = "mm"; // mm / pm tm cv
static std::shared_ptr<ModuleInterface> selected_module;
static std::shared_ptr<TemplateInterface> selected_template;
static std::string default_project_avatar = "/usr/local/include/Vortex/1.1/imgs/base_vortex.png";
static std::string _parent;
    static bool open_module_deletion_modal = false;
    static bool open_import_module = false;
    static bool open_import_all_module = false;
    static bool open_import_all_templates = false;

static std::shared_ptr<ModuleInterface> single_module_to_add;

static std::vector<std::string> available_versions;

bool TestVortexExecutable(const std::string &path)
{
    std::array<char, 128> buffer;
    std::string result;
    std::string command = path + " -test";

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        std::cerr << "popen() failed!" << std::endl;
        return false;
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    int return_code = pclose(pipe.release());

    return (result.find("ok") != std::string::npos) && (return_code == 0);
}

void RegisterAvailableVersions()
{
    const std::string base_path = "/usr/local/bin/Vortex";
    available_versions.clear();
    for (const auto &entry : std::filesystem::directory_iterator(base_path))
    {
        if (entry.is_directory())
        {
            std::string version = entry.path().filename().string();
            std::string vortex_executable = entry.path().string() + "/vortex";

            if (TestVortexExecutable(vortex_executable))
            {
                available_versions.push_back(version);
            }
        }
    }
}

enum class ModulesStates
{
    ALREADY_INSTALLED_WITH_CORRECT_VERSION,
    ALREADY_INSTALLED_WITH_ANOTHER_VERSION,
    NOT_INSTALLED,
};

enum class TemplatesStates
{
    SAME_NAME_DETECTED,
    NOT_INSTALLED,
};

static std::unordered_map<std::string, std::pair<ModulesStates, std::shared_ptr<ModuleInterface>>> all_modules_to_add;
static std::vector<std::pair<TemplatesStates, std::shared_ptr<TemplateInterface>>> all_templates_to_add;

static void CheckAllTemplatesStates(std::vector<std::shared_ptr<TemplateInterface>> finded_templates)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::cout << finded_templates.size() << std::endl;
    std::cout << finded_templates[100] << std::endl;
    all_templates_to_add.clear();

    for (auto template_ : finded_templates)
    {
        bool resolved = false;

        for (auto sys_modules : ctx->IO.sys_templates)
        {
            if (resolved)
            {
                continue;
            }

            if (sys_modules->m_name == template_->m_name)
            {
                // Already installed
                std::pair<TemplatesStates, std::shared_ptr<TemplateInterface>> module_to_add;
                module_to_add.second = template_;
                module_to_add.first = TemplatesStates::SAME_NAME_DETECTED;
                all_templates_to_add.push_back(module_to_add);
                resolved = true;
            }
        }

        if (!resolved)
        {
            // Already installed, but not this version
            std::pair<TemplatesStates, std::shared_ptr<TemplateInterface>> module_to_add;
            module_to_add.second = template_;
            module_to_add.first = TemplatesStates::NOT_INSTALLED;
            all_templates_to_add.push_back(module_to_add);
            resolved = true;
        }
    }
}

static void InsertModule(std::pair<ModulesStates, std::shared_ptr<ModuleInterface>> module_to_add)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::string tag = module_to_add.second->m_name + module_to_add.second->m_version;

    bool finded = false;

    for (auto sys_modules : ctx->IO.sys_em)
    {
        if (finded)
            continue;

        if (sys_modules->m_name == module_to_add.second->m_name && sys_modules->m_version == module_to_add.second->m_version)
        {
            module_to_add.first = ModulesStates::ALREADY_INSTALLED_WITH_CORRECT_VERSION;
            finded = true;
        }

        if (sys_modules->m_name == module_to_add.second->m_name && !finded)
        {
            module_to_add.first = ModulesStates::ALREADY_INSTALLED_WITH_ANOTHER_VERSION;
        }
    }

    for (auto it = all_modules_to_add.begin(); it != all_modules_to_add.end(); it++)
    {
        if (it->second.second->m_name == module_to_add.second->m_name && it->second.second->m_version == module_to_add.second->m_version)
        {
            it->second.first = ModulesStates::ALREADY_INSTALLED_WITH_CORRECT_VERSION;
        }
    }

    all_modules_to_add.insert({tag, module_to_add});
}

static void CheckAllModulesStates(std::vector<std::shared_ptr<ModuleInterface>> finded_modules)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();

    all_modules_to_add.clear();

    for (auto module_ : finded_modules)
    {
        std::pair<ModulesStates, std::shared_ptr<ModuleInterface>> module_to_add;
        module_to_add.second = module_;
        module_to_add.first = ModulesStates::NOT_INSTALLED;
        InsertModule(module_to_add);
    }
}

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

static void addTexture(const std::string &name, const std::string &path)
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
            std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, _parent, data);
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
                return;
            }
        }
        std::vector<uint8_t> hexTable = loadPngHex(path);
        const uint8_t *hexData = hexTable.data();

        if (!hexTable.empty())
        {

            void *data = UIKit::Image::Decode(hexData, hexTable.size(), w, h);
            std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, _parent, data);
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

static ImTextureID getTexture(const std::string &name, ImDrawList *drawList, ImVec2 pos, ImVec2 size)
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
            std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, _parent, data);
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
            std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, _parent, data);
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

static void MyButton(const std::string &name, int w, int h)
{
    ImVec2 squareSize(w, h);
    ImVec2 totalSize(squareSize.x, squareSize.y + 5);
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    std::string button_id = name + "squareButtonWithText";
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
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

SystemSettings::SystemSettings(VxContext *_ctx, const std::string &parent)
{
    this->ctx = _ctx;

    this->Refresh();

    _parent = parent;

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_project, icons::i_project_size, w, h);
        m_ProjectIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_open, icons::i_open_size, w, h);
        m_OpenIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }

    RegisterAvailableVersions();
}

bool MyParamBanner(const std::string &path)
{
    bool pressed = false;
    addTexture(path, path);
    ImVec2 squareSize(300, 51);

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    std::string id = "## " + path;
    if (ImGui::InvisibleButton(id.c_str(), squareSize))
    {
        pressed = true;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    getTexture(path, drawList, cursorPos, squareSize);

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);
    return pressed;
}

void SystemSettings::OnImGuiRender(const std::string &parent, std::function<void(ImGuiWindow *)> controller)
{
    static ImTextureID projectIcon = this->m_ProjectIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    ImGui::SetNextWindowDockID(ImGui::GetID("MainDockspace"), ImGuiCond_FirstUseEver);
    if (ImGui::UIKit_BeginLogoWindow("System Settings", &projectIcon, &this->opened, ImGuiWindowFlags_MenuBar))
    {
        this->menubar();

        static ImGuiWindow *win = ImGui::GetCurrentWindow();
        this->parent = parent;
        std::cout << "[" << win->Name << "] -> " << this->parent << std::endl;

        controller(win);

        float columnWidths[3] = {100.0f, 250.0f, 150.0f};

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

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        for (int i = 0; i < 3; ++i)
        {
            float columnWidth = availableWidth * columnProportions[i];
            ImGui::SetColumnWidth(i, columnWidth);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, columnColors[i]);
            ImGui::BeginChild(ImGui::GetID((void *)(intptr_t)i), ImVec2(columnWidth, 0), true);
            if (i == 0)
            {
                if (MyParamBanner("/usr/local/include/Vortex/1.1/imgs/b_modules_managment.png"))
                {
                    current_section = "mm";
                }

                if (MyParamBanner("/usr/local/include/Vortex/1.1/imgs/b_plugins_managment.png"))
                {
                    current_section = "pm";
                }

                if (MyParamBanner("/usr/local/include/Vortex/1.1/imgs/b_templates_managment.png"))
                {
                    current_section = "tm";
                }

                if (MyParamBanner("/usr/local/include/Vortex/1.1/imgs/b_core_versions.png"))
                {
                    current_section = "cv";
                }
            }
            else if (i == 1)
            {
                if (current_section == "mm")
                {
                    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

                    if (ImGui::BeginTable("tablhjke_", 6, flags))
                    {

                        ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        ImGui::PopStyleVar(4);

                        for (int row = 0; row < this->ctx->IO.sys_em.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 6; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    std::string deleteButtonID = "Select###" + std::to_string(row) + "-" + std::to_string(column);
                                    if (ImGui::ImageButtonWithText(projectIcon, deleteButtonID.c_str(), ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
                                    {
                                        selected_module = this->ctx->IO.sys_em[row];
                                    }
                                }
                                else if (column == 1)
                                {
                                    ImGui::Text(this->ctx->IO.sys_em[row]->m_proper_name.c_str());
                                }
                                else if (column == 2)
                                {
                                    ImGui::Text(this->ctx->IO.sys_em[row]->m_name.c_str());
                                }
                                else if (column == 3)
                                {
                                    ImGui::Text(this->ctx->IO.sys_em[row]->m_version.c_str());
                                }
                                else if (column == 4)
                                {
                                    ImGui::Text(this->ctx->IO.sys_em[row]->m_path.c_str());
                                }
                                else if (column == 5)
                                {
                                    ImGui::Text(this->ctx->IO.sys_em[row]->m_type.c_str());
                                }
                            }
                        }
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                        ImGui::EndTable();
                    }
                }
                else if (current_section == "tm")
                {

                    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

                    if (ImGui::BeginTable("tablhjke_", 5, flags))
                    {

                        ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        ImGui::PopStyleVar(4);

                        for (int row = 0; row < this->ctx->IO.sys_templates.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 5; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    std::string deleteButtonID = "Select###" + std::to_string(row) + "-" + std::to_string(column);
                                    if (ImGui::ImageButtonWithText(projectIcon, deleteButtonID.c_str(), ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
                                    {
                                        selected_template = this->ctx->IO.sys_templates[row];
                                    }
                                }
                                else if (column == 1)
                                {
                                    ImGui::Text(this->ctx->IO.sys_templates[row]->m_proper_name.c_str());
                                }
                                else if (column == 2)
                                {
                                    ImGui::Text(this->ctx->IO.sys_templates[row]->m_name.c_str());
                                }
                                else if (column == 3)
                                {
                                    ImGui::Text(this->ctx->IO.sys_templates[row]->m_path.c_str());
                                }
                                else if (column == 4)
                                {
                                    ImGui::Text(this->ctx->IO.sys_templates[row]->m_type.c_str());
                                }
                            }
                        }
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                        ImGui::EndTable();
                    }
                }
                else if (current_section == "cv")
                {
                    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

                    if (ImGui::BeginTable("tablhjke_", 2, flags))
                    {

                        ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Prop", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        ImGui::PopStyleVar(4);

                        for (int row = 0; row < available_versions.size(); row++)
                        {
                            ImGui::TableNextRow();
                            for (int column = 0; column < 1; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    std::string label = "Vortex version " + available_versions[row];
                                    ImGui::Text(label.c_str());
                                }
                                else if (column == 1)
                                {
                                    ImGui::Text(available_versions[row].c_str());
                                }
                            }
                        }
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                        ImGui::EndTable();
                    }
                }
            }
            else if (i == 2)
            {
                if (current_section == "mm")
                {
                    if (!selected_module)
                    {
                        ImGui::Text("Select a module");
                    }
                    else
                    {
                        ImVec2 windowSize = ImGui::GetWindowSize();
                        ImVec2 contentSize = ImGui::GetContentRegionAvail();
                        ImVec2 buttonSize = ImVec2(100, 30);
                        ImVec2 bitButtonSize = ImVec2(150, 30);
                        {
                            ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);

                            MyButton("tezt", 60, 60);

                            ImGui::EndChild();
                            ImGui::SameLine();
                        }

                        {
                            ImGui::BeginChild("TEXT_", ImVec2(0, 68), true);
                            float oldsize = ImGui::GetFont()->Scale;
                            ImGui::GetFont()->Scale *= 1.3;
                            ImGui::PushFont(ImGui::GetFont());

                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_module->m_proper_name.c_str());

                            ImGui::GetFont()->Scale = oldsize;
                            ImGui::PopFont();
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_module->m_name.c_str());
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_module->m_version.c_str());
                            ImGui::EndChild();
                        }

                        float ysection = windowSize.y - 280;
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ysection));

                        float firstButtonPosX = windowSize.x - buttonSize.x - bitButtonSize.y - 75 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

                        float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - bitButtonSize.y;

                        ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

                        if (ImGui::Button("Rebuild", buttonSize))
                        {
                        }

                        ImGui::SameLine();

                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.2f, 0.2f, 0.8f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.2f, 0.2f, 1.8f));
                        if (ImGui::Button("Delete module", bitButtonSize))
                        {
                            open_module_deletion_modal = true;
                        }
                        ImGui::PopStyleColor(3);
                    }
                }

                if (current_section == "tm")
                {
                    if (!selected_template)
                    {
                        ImGui::Text("Select a template");
                    }
                    else
                    {
                        ImVec2 windowSize = ImGui::GetWindowSize();
                        ImVec2 contentSize = ImGui::GetContentRegionAvail();
                        ImVec2 buttonSize = ImVec2(100, 30);
                        ImVec2 bitButtonSize = ImVec2(150, 30);
                        {
                            ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);

                            MyButton("tezt", 60, 60);

                            ImGui::EndChild();
                            ImGui::SameLine();
                        }

                        {
                            ImGui::BeginChild("TEXT_", ImVec2(0, 68), true);
                            float oldsize = ImGui::GetFont()->Scale;
                            ImGui::GetFont()->Scale *= 1.3;
                            ImGui::PushFont(ImGui::GetFont());

                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_template->m_proper_name.c_str());

                            ImGui::GetFont()->Scale = oldsize;
                            ImGui::PopFont();
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), selected_template->m_name.c_str());
                            ImGui::EndChild();
                        }

                        float ysection = windowSize.y - 280;
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ysection));

                        float firstButtonPosX = windowSize.x - buttonSize.x - bitButtonSize.y - 75 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

                        float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - bitButtonSize.y;

                        ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

                        if (ImGui::Button("Rebuild", buttonSize))
                        {
                        }

                        ImGui::SameLine();

                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.2f, 0.2f, 0.8f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.2f, 0.2f, 1.8f));
                        if (ImGui::Button("Delete template", bitButtonSize))
                        {
                            // beh
                        }
                        ImGui::PopStyleColor(3);
                    }
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();

            if (i < 2)
                ImGui::NextColumn();
        }
        ImGui::PopStyleVar(4);

        ImGui::End();
    }
}

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

void SystemSettings::addModuleModal()
{
}

void SystemSettings::mainButtonsMenuItem()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void SystemSettings::filterMenuItem()
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

void SystemSettings::createMenuItem()
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

void SystemSettings::searchMenuItem()
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

void SystemSettings::menubar()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    if (open_import_all_templates)
    {
        static ImGuiTableFlags window_flags = ImGuiWindowFlags_MenuBar;
        if (ImGui::BeginPopupModal("Search templates in folder", NULL, window_flags))
        {
            static char path_input_template_all[512];

            if (ImGui::BeginMenuBar())
            {
                ImGui::Text("Please chose a folder");

                if (ImGui::ImageButtonWithText(refreshIcon, "", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
                {
                }
                ImGui::InputText("###Path", path_input_template_all, IM_ARRAYSIZE(path_input_template_all));
                std::string label = "Find###templates";
                if (ImGui::Button(label.c_str()))
                {
                    CheckAllTemplatesStates(VortexMaker::FindTemplatesInDirectory(path_input_template_all));
                }
                ImGui::EndMenuBar();
            }

            if (!all_templates_to_add.empty())
            {
                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

                if (ImGui::BeginTable("ModulesToAddTable", 4, flags))
                {

                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();

                    for (int row = 0; row < all_templates_to_add.size(); row++)
                    {
                        static std::pair<char[128], char[128]> newItem;
                        static char label[128];

                        ImGui::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                if (all_templates_to_add[row].first == TemplatesStates::SAME_NAME_DETECTED)
                                {
                                    std::string label = "Install another";
                                    if (ImGui::Button(label.c_str()))
                                    {
                                        VxContext *ctx = VortexMaker::GetCurrentContext();
                                        VortexMaker::InstallTemplateOnSystem(all_templates_to_add[row].second->m_path);
                                        VortexMaker::LoadSystemTemplates(ctx->IO.sys_templates);
                                        CheckAllTemplatesStates(VortexMaker::FindTemplatesInDirectory(path_input_template_all));
                                    }
                                }
                                else if (all_templates_to_add[row].first == TemplatesStates::NOT_INSTALLED)
                                {
                                    if (ImGui::Button("Install"))
                                    {
                                        VxContext *ctx = VortexMaker::GetCurrentContext();
                                        VortexMaker::InstallTemplateOnSystem(all_templates_to_add[row].second->m_path);
                                        VortexMaker::LoadSystemTemplates(ctx->IO.sys_templates);
                                        CheckAllTemplatesStates(VortexMaker::FindTemplatesInDirectory(path_input_template_all));
                                    }
                                }
                            }
                            else if (column == 1)
                            {
                                ImGui::Text(all_templates_to_add[row].second->m_proper_name.c_str());
                            }
                            else if (column == 2)
                            {
                                ImGui::Text(all_templates_to_add[row].second->m_name.c_str());
                            }
                            else if (column == 3)
                            {
                            }
                        }
                    }

                    ImGui::EndTable();
                }
            }

            // static int unused_i = 0;
            // ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 buttonSize = ImVec2(100, 30);
            ImVec2 bitButtonSize = ImVec2(150, 30);
            float ysection = windowSize.y - 280;
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ysection));

            float firstButtonPosX = windowSize.x - buttonSize.x - 75 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

            float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - 10;

            ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

            if (ImGui::Button("Done", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                open_import_all_templates = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    if (open_module_deletion_modal)
    {

        ImGui::SetNextWindowSize(ImVec2(300, 200));

        static ImGuiTableFlags window_flags = ImGuiWindowFlags_NoResize;
        if (ImGui::BeginPopupModal("Delete a system module", NULL, window_flags))
        {
            // Set the size of the modal to 200x75 pixels the first time it is created

            // 3 text inputs
            static char path_input_all[512];
            // inputs widget
            ImGui::TextWrapped("WARNING, if you click on the Delete button, the project will be erase forever.");
            ImGui::SetItemDefaultFocus();


            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                open_module_deletion_modal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
            if (ImGui::Button("Delete", ImVec2(120, 0)))
            {
                // Delete
                VortexMaker::DeleteSystemModule(selected_module->m_name, selected_module->m_version);
                VortexMaker::LoadSystemModules(this->ctx->IO.sys_em);

                open_module_deletion_modal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(3);
            ImGui::EndPopup();
        }
    }

    if (open_import_all_module)
    {
        static ImGuiTableFlags window_flags = ImGuiWindowFlags_MenuBar;
        static bool first_time = true;


        if (first_time)
        {
            ImGui::SetNextWindowSize(ImVec2(820, 420));
        }

        if (ImGui::BeginPopupModal("Search modules in folder", NULL, window_flags))
        {
            if (first_time)
            {
                first_time = false;
            }

            static char path_input_all[512];

            if (ImGui::BeginMenuBar())
            {
                ImGui::Text("Please chose a folder");

                if (ImGui::ImageButtonWithText(refreshIcon, "", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
                {
                }
                ImGui::InputText("###Path", path_input_all, IM_ARRAYSIZE(path_input_all));
                std::string label = "Find###templates";
                if (ImGui::Button(label.c_str()))
                {
                    CheckAllModulesStates(VortexMaker::FindModulesInDirectory(path_input_all));
                }
                ImGui::EndMenuBar();
            }

            if (!all_modules_to_add.empty())
            {
                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

                if (ImGui::BeginTable("ModulesToAddTable", 4, flags))
                {

                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Proper Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();

                    for (auto it = all_modules_to_add.begin(); it != all_modules_to_add.end(); it++)
                    {
                        static std::pair<char[128], char[128]> newItem;
                        static char label[128];

                        ImGui::TableNextRow();
                        for (int column = 0; column < 4; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                if (it->second.first == ModulesStates::ALREADY_INSTALLED_WITH_CORRECT_VERSION)
                                {
                                    ImGui::Text("Already installed");
                                }
                                else if (it->second.first == ModulesStates::ALREADY_INSTALLED_WITH_ANOTHER_VERSION)
                                {
                                    std::string label = "Install v" + it->second.second->m_version;
                                    if (ImGui::Button(label.c_str()))
                                    {
                                        VxContext *ctx = VortexMaker::GetCurrentContext();
                                        VortexMaker::InstallModuleToSystem(it->second.second->m_path);
                                        VortexMaker::LoadSystemModules(ctx->IO.sys_em);
                                        CheckAllModulesStates(VortexMaker::FindModulesInDirectory(path_input_all));
                                    }
                                }
                                else if (it->second.first == ModulesStates::NOT_INSTALLED)
                                {
                                    if (ImGui::Button("Install"))
                                    {
                                        VxContext *ctx = VortexMaker::GetCurrentContext();
                                        VortexMaker::InstallModuleToSystem(it->second.second->m_path);
                                        VortexMaker::LoadSystemModules(ctx->IO.sys_em);
                                        CheckAllModulesStates(VortexMaker::FindModulesInDirectory(path_input_all));
                                    }
                                }
                            }
                            else if (column == 1)
                            {
                                ImGui::Text(it->second.second->m_proper_name.c_str());
                            }
                            else if (column == 2)
                            {
                                ImGui::Text(it->second.second->m_name.c_str());
                            }
                            else if (column == 3)
                            {
                                ImGui::Text(it->second.second->m_version.c_str());
                            }
                        }
                    }

                    ImGui::EndTable();
                }
            }

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 buttonSize = ImVec2(100, 30);
            ImVec2 bitButtonSize = ImVec2(150, 30);
            float ysection = windowSize.y - 280;
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ysection));

            float firstButtonPosX = windowSize.x - buttonSize.x - 75 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

            float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - 10;

            ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

            if (ImGui::Button("Done", ImVec2(120, 0)))
            {

                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                open_import_all_module = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    if (open_import_module)
    {

        if (ImGui::BeginPopupModal("Import a module", NULL, NULL))
        {
            // Set the size of the modal to 200x75 pixels the first time it is created

            // 3 text inputs
            static char path_input[512];
            // inputs widget
            ImGui::TextWrapped("Please provide the path of your module, you need to include the module folder like \"/path/to/your/module/ModuleFolder\"");
            ImGui::InputText("Module path", path_input, IM_ARRAYSIZE(path_input));
            if (ImGui::Button("Find"))
            {
                single_module_to_add = VortexMaker::FindModuleInDirectory(path_input);
            }

            if (single_module_to_add)
            {
                ImGui::Text(single_module_to_add->m_name.c_str());
            }

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 buttonSize = ImVec2(100, 30);
            ImVec2 bitButtonSize = ImVec2(150, 30);
            float ysection = windowSize.y - 280;
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ysection));

            float firstButtonPosX = windowSize.x - buttonSize.x - 75 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

            float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - 10;

            ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

            if (ImGui::Button("Done", ImVec2(120, 0)))
            {

                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                open_import_module = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    if (open_import_module)
        ImGui::OpenPopup("Import a module");

    if (open_import_all_module)
        ImGui::OpenPopup("Search modules in folder");

    if (open_import_all_templates)
        ImGui::OpenPopup("Search templates in folder");

    if (open_module_deletion_modal)
        ImGui::OpenPopup("Delete a system module");

    if (ImGui::BeginMenuBar())
    {
        if (current_section == "mm")
        {
            if (ImGui::ImageButtonWithText(addIcon, "Import a module", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
            {
                open_import_module = true;
            }
            if (ImGui::ImageButtonWithText(addIcon, "Search modules in a folder", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
            {
                open_import_all_module = true;
            }
        }
        if (current_section == "tm")
        {
            if (ImGui::ImageButtonWithText(addIcon, "Search modules in a folder", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
            {
                open_import_all_templates = true;
            }
        }
        if (current_section == "pm")
        {
            if (ImGui::ImageButtonWithText(addIcon, "Install a plugin", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
            {
            }
        }
        if (current_section == "cv")
        {

            if (ImGui::ImageButtonWithText(addIcon, "Refresh versions", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
            {
                RegisterAvailableVersions();
            }
            if (ImGui::ImageButtonWithText(addIcon, "Download a newer version", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
            {
            }
            if (ImGui::ImageButtonWithText(addIcon, "Upgrade a project", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
            {
            }
            if (ImGui::ImageButtonWithText(addIcon, "Upgrade a module", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
            {
            }
        }
        ImGui::EndMenuBar();
    }
}

void SystemSettings::Refresh()
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