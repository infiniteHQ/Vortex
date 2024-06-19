
#include "ProjectManager.hpp"

#include <algorithm>
#include <cctype>
#include <unordered_set>

static std::vector<std::string> labels = {"Open a project", "Create a new project"};
static int selected = 0;
static int selected_template = 0;
static bool template_is_selected = false;
static bool project_creation = false;
static std::shared_ptr<TemplateInterface> selected_template_object;
static std::shared_ptr<EnvProject> selected_envproject;
static std::shared_ptr<EnvProject> selected_envproject_to_remove;
static std::string title = "none";
static std::string default_project_avatar = "/usr/local/include/Vortex/imgs/base_vortex.png";
static std::string operating_system_banner = "/usr/local/include/Vortex/1.1/imgs/operating_system_banner.png";
static std::string _parent;

static char ProjectSearch[256];
static float threshold = 0.4;

bool isOnlySpacesOrEmpty(const char *str)
{
    if (str == nullptr || std::strlen(str) == 0)
    {
        return true;
    }

    for (size_t i = 0; i < std::strlen(str); ++i)
    {
        if (str[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

std::string toLowerCase(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

int levenshteinDistance(const std::string &s1, const std::string &s2)
{
    const size_t m = s1.size();
    const size_t n = s2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (size_t i = 0; i <= m; ++i)
    {
        for (size_t j = 0; j <= n; ++j)
        {
            if (i == 0)
            {
                dp[i][j] = j;
            }
            else if (j == 0)
            {
                dp[i][j] = i;
            }
            else
            {
                int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
                dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost});
            }
        }
    }
    return dp[m][n];
}

bool hasCommonLetters(const std::string &s1, const std::string &s2)
{
    std::unordered_set<char> set1(s1.begin(), s1.end());
    for (char c : s2)
    {
        if (set1.find(c) != set1.end())
        {
            return true;
        }
    }
    return false;
}

bool areStringsSimilar(const std::string &s1, const std::string &s2, double threshold)
{
    std::string lower_s1 = toLowerCase(s1);
    std::string lower_s2 = toLowerCase(s2);

    int dist = levenshteinDistance(lower_s1, lower_s2);
    int maxLength = std::max(lower_s1.size(), lower_s2.size());
    double similarity = 1.0 - (static_cast<double>(dist) / maxLength);

    if (std::strlen(ProjectSearch) < 5)
    {
        return similarity >= threshold || hasCommonLetters(lower_s1, lower_s2);
    }

    return similarity >= threshold;
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
            ImGuiWindow *win = ImGui::GetCurrentWindow();
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
            ImGuiWindow *win = ImGui::GetCurrentWindow();

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
        ImGuiWindow *win = ImGui::GetCurrentWindow();
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
        ImGuiWindow *win = ImGui::GetCurrentWindow();
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

ProjectManager::ProjectManager(VxContext *_ctx, const std::string &parent)
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
    std::cout << "8" << std::endl;
}

void MyButton(const std::string &name, int w, int h)
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

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
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

void MyButton(const std::shared_ptr<TemplateInterface> templateinterface)
{
    ImVec2 squareSize(100, 100);

    const char *originalText = templateinterface->m_proper_name.c_str();
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

    std::string button_id = templateinterface->m_name + "squareButtonWithText";
    if (ImGui::InvisibleButton(button_id.c_str(), totalSize))
    {
        selected_template_object = templateinterface;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    addTexture("test", default_project_avatar);
    getTexture("test", drawList, cursorPos, squareSize);

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);

    drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + smallRectSize.x, smallRectPos.y + smallRectSize.y), IM_COL32(0, 0, 0, 255));
    ImVec2 versionTextPos = ImVec2(smallRectPos.x + (smallRectSize.x - ImGui::CalcTextSize(templateinterface->m_group.c_str()).x) / 2, smallRectPos.y + (smallRectSize.y - ImGui::CalcTextSize("version").y) / 2);
    drawList->AddText(versionTextPos, IM_COL32(255, 255, 255, 255), templateinterface->m_group.c_str());

    ImVec2 textPos = ImVec2(cursorPos.x + (squareSize.x - textSize.x) / 2, cursorPos.y + squareSize.y + 5);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), truncatedText);

    float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + totalSize.x < windowVisibleX2)
        ImGui::SameLine();
}

void MyBanner(const std::string &path)
{
    addTexture(path, path);
    ImVec2 squareSize(300, 70);

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    if (ImGui::InvisibleButton("##squ5684areButtonWithText2", squareSize))
    {
        std::cout << "Hello World 2" << std::endl;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    getTexture(path, drawList, cursorPos, squareSize);

    ImVec2 smallRectSize(40, 20);
    ImVec2 smallRectPos(cursorPos.x + squareSize.x - smallRectSize.x - 5, cursorPos.y + squareSize.y - smallRectSize.y - 5);
}

void ProjectManager::OnImGuiRender(const std::string &parent, std::function<void(ImGuiWindow *)> controller)
{
    static ImTextureID projectIcon = this->m_ProjectIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

    ImGui::SetNextWindowDockID(ImGui::GetID("MainDockspace"), ImGuiCond_FirstUseEver);

    if (ImGui::UIKit_BeginLogoWindow("Project managers", &projectIcon, &this->opened, window_flags))
        this->menubar();

    static ImGuiWindow *win = ImGui::GetCurrentWindow();
    this->parent = parent;
    std::cout << "[" << win->Name << "] -> " << this->parent << std::endl;

    controller(win);

    static bool open_deletion_modal = false;
    static bool user_string_validation = false;
    static char string_validation[256] = "";

    if (open_deletion_modal)
    {

        if (ImGui::BeginPopupModal("Delete a project", NULL, NULL))
        {
            // Set the size of the modal to 200x75 pixels the first time it is created

            // 3 text inputs
            static char path_input_all[512];
            // inputs widget
            ImGui::TextWrapped("WARNING, if you click on the Delete button, the project will be erase forever.");
            ImGui::InputText("Please", string_validation, sizeof(string_validation));
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                open_deletion_modal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            std::cout << string_validation << selected_envproject_to_remove->name.c_str() << std::endl;

            if (strcmp(string_validation, selected_envproject_to_remove->name.c_str()) != 0)
            {
                ImGui::BeginDisabled();
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
            if (ImGui::Button("Delete", ImVec2(120, 0)))
            {
                // Delete
                VortexMaker::DeleteProject(selected_envproject_to_remove->path);

                open_deletion_modal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(3);
            if (strcmp(string_validation, selected_envproject_to_remove->name.c_str()) != 0)
            {
                ImGui::EndDisabled();
            }
            ImGui::EndPopup();
        }
    }

    if (open_deletion_modal)
        ImGui::OpenPopup("Delete a project");

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
                    if (areStringsSimilar(this->ctx->IO.sys_projects[row]->name, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
                    {
                        MyButton(this->ctx->IO.sys_projects[row]);
                    }
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

                    if (ImGui::Button("Delete", buttonSize))
                    {
                        selected_envproject_to_remove = selected_envproject;
                        open_deletion_modal = true;
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
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_all_bases.png");
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_operating_systems.png");
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_app_svc.png");
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_tools_utils.png");
                MyBanner("/usr/local/include/Vortex/1.1/imgs/b_assets_components.png");
            }

            else if (i == 1)
            {
                for (int project = 0; project < project_templates.size(); project++)
                {
                    if (project_templates[project] != NULL)
                    {
                        MyButton(project_templates[project]);
                    }
                }
            }
            else if (i == 2)
            {
                if (!selected_template_object)
                {
                    ImGui::Text("Select a template");
                }
                else
                {
                    ImVec2 windowSize = ImGui::GetWindowSize();
                    ImVec2 contentSize = ImGui::GetContentRegionAvail();
                    ImVec2 buttonSize = ImVec2(100, 30);
                    ImVec2 bitButtonSize = ImVec2(150, 30);

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
                            MyButton("tezt", 60, 60);
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

                    ImGui::PopStyleVar(4);
                    ImGui::InputText("Path", buf, sizeof(buf));
                    ImGui::InputText("Name", name, 128);
                    ImGui::InputText("Description", desc, 128);
                    ImGui::InputText("Authors", auth, 128);
                    ImGui::InputText("Version", version, 128);

                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                    float ysection = windowSize.y - 280;
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ysection));

                    ImGui::Text("Configuration of the project :");
                    ImGui::Checkbox("Open the project after creation", &open);
                    std::string path = s + name;

                    float firstButtonPosX = windowSize.x - buttonSize.x - bitButtonSize.y - 75 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

                    float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - bitButtonSize.y;

                    ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

                    if (ImGui::Button("Settings", buttonSize))
                    {
                    }

                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 1.0f, 0.8f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 1.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.0f, 1.8f));
                    if (ImGui::Button("Create new project", bitButtonSize))
                    {

                        VortexMaker::CreateProject(name, auth, version, desc, path, selected_template_object->m_name);

                        std::string projectPath = path;

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

            if (i < 2)
                ImGui::NextColumn();
        }
        ImGui::PopStyleVar(4);
    }
    ImGui::End();
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

void ProjectManager::addModuleModal()
{
}

void ProjectManager::mainButtonsMenuItem()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    if (!project_creation)
    {
        if (ImGui::UIKit_ImageButtonWithText(addIcon, "Create a project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
            project_creation = true;
        }
        ImGui::Separator();

        if (ImGui::UIKit_ImageButtonWithText(addIcon, "Import project(s)", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
        }
        ImGui::PushItemWidth(200);
        ImGui::InputText("", ProjectSearch, sizeof(ProjectSearch));
        ImGui::PopItemWidth();
    }
    else
    {
        if (ImGui::UIKit_ImageButtonWithText(addIcon, "Open a project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
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
