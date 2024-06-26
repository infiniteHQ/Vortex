#include "ContentBrowser.hpp"

/*
TODO:

Selection (dont multiple),
Cpy, Past, etc...


*/

static std::string _parent;
static char ProjectSearch[256];
static float threshold = 0.4;

static ImU32 folder_color = IM_COL32(255, 255, 75, 255);

static bool isOnlySpacesOrEmpty(const char *str)
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

static std::string toLowerCase(const std::string &str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

static int levenshteinDistance(const std::string &s1, const std::string &s2)
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

static bool hasCommonLetters(const std::string &s1, const std::string &s2)
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

static bool areStringsSimilar(const std::string &s1, const std::string &s2, double threshold)
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

static const std::string File_LICENSE_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_license_file.png";
static const std::string File_ASM_Logo = "/usr/local/include/Vortex/1.1/imgs/file_asm_logo.png";
static const std::string File_GIT_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_git_file.png";
static const std::string File_C_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_c_file.png";
static const std::string File_C_H_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_c_h_file.png";
static const std::string File_CPP_H_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_cpp_h_file.png";
static const std::string File_CPP_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_cpp_file.png";
static const std::string File_UNKNOW_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_unknow_file.png";
static const std::string File_PICTURE_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_picture_file.png";
static const std::string Folder_Logo = "/usr/local/include/Vortex/1.1/imgs/icon_folder.png";

static const std::string Icon_Left_Arrow_Enabled = "/usr/local/include/Vortex/1.1/imgs/icon_arrow_l_enabled.png";
static const std::string Icon_Left_Arrow_Disabled = "/usr/local/include/Vortex/1.1/imgs/icon_arrow_l_disabled.png";
static const std::string Icon_Right_Arrow_Enabled = "/usr/local/include/Vortex/1.1/imgs/icon_arrow_r_enabled.png";
static const std::string Icon_Right_Arrow_Disabled = "/usr/local/include/Vortex/1.1/imgs/icon_arrow_r_disabled.png";

static std::string default_project_avatar = "/usr/local/include/Vortex/imgs/base_vortex.png";

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

static ImTextureID getTexture(const std::string &name)
{
	for (const auto &texture : associated_textures)
	{
		if (texture.first == name)
		{
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

// Fonction pour dessiner du texte avec des correspondances mises en surbrillance
static void DrawHighlightedText(ImDrawList *drawList, ImVec2 textPos, const char *text, const char *search, ImU32 highlightColor, ImU32 textColor, ImU32 highlightTextColor)
{
	if (!text || !search || !*search)
	{
		drawList->AddText(textPos, textColor, text);
		return;
	}

	const char *start = text;
	const char *found = strstr(start, search);
	while (found)
	{
		// Dessiner le texte avant la correspondance
		if (found > start)
		{
			std::string preText(start, found);
			drawList->AddText(textPos, textColor, preText.c_str());
			textPos.x += ImGui::CalcTextSize(preText.c_str()).x;
		}

		// Dessiner la correspondance mise en surbrillance avec un texte noir
		ImVec2 highlightPos = textPos;
		ImVec2 highlightSize = ImGui::CalcTextSize(search);
		drawList->AddRectFilled(highlightPos, ImVec2(highlightPos.x + highlightSize.x, highlightPos.y + highlightSize.y), highlightColor);
		drawList->AddText(textPos, highlightTextColor, search);
		textPos.x += highlightSize.x;

		// Passer à la partie suivante du texte
		start = found + strlen(search);
		found = strstr(start, search);
	}

	// Dessiner le texte restant après la dernière correspondance
	if (*start)
	{
		drawList->AddText(textPos, textColor, start);
	}
}

bool MyButton(const std::string &name, const std::string &description, const std::string &size, bool selected, const std::string &logo, ImU32 bgColor = IM_COL32(100, 100, 100, 255), ImU32 borderColor = IM_COL32(150, 150, 150, 255), ImU32 lineColor = IM_COL32(255, 255, 0, 255), float maxTextWidth = 100.0f, float borderRadius = 5.0f)
{
	bool pressed = false;

	// Configuration des tailles et positions
	float logoSize = 60.0f;				// Taille du logo
	float extraHeight = 80.0f;			// Hauteur supplémentaire pour le bouton global
	float padding = 10.0f;				// Espace autour des éléments
	float separatorHeight = 2.0f;		// Hauteur du séparateur
	float textOffsetY = 5.0f;			// Offset vertical pour le texte
	float versionBoxWidth = 10.0f;		// Largeur de la boîte de version
	float versionBoxHeight = 20.0f;		// Hauteur de la boîte de version
	float thumbnailIconOffsetY = 30.0f; // Hauteur de la boîte de version

	if (selected)
	{
		bgColor = IM_COL32(80, 80, 240, 255);
		borderColor = IM_COL32(150, 150, 255, 255);
	}

	ImVec2 squareSize(logoSize, logoSize);

	// Déterminer le texte à afficher
	const char *originalText = name.c_str();
	std::string truncatedText = name;

	// Tronquer le texte si nécessaire
	if (ImGui::CalcTextSize(originalText).x > maxTextWidth)
	{
		// Tronquer à 20 caractères pour la première ligne
		truncatedText = name.substr(0, 20);
		// Ajouter une nouvelle ligne si nécessaire
		if (ImGui::CalcTextSize(truncatedText.c_str()).x > maxTextWidth)
		{
			// Tronquer à 10 caractères pour la deuxième ligne
			truncatedText = name.substr(0, 10) + "\n" + name.substr(10, 10);
		}
	}
	else
	{
		truncatedText = name + "\n";
	}

	// Utiliser une taille fixe pour le composant
	ImVec2 fixedSize(maxTextWidth + padding * 2, logoSize + extraHeight + padding * 2);

	ImVec2 cursorPos = ImGui::GetCursorScreenPos();

	std::string button_id = name + "squareButtonWithText" + name;
	if (ImGui::InvisibleButton(button_id.c_str(), fixedSize))
	{
		pressed = true;
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}

	ImDrawList *drawList = ImGui::GetWindowDrawList();

	drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), bgColor, borderRadius);
	drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + thumbnailIconOffsetY + squareSize.y), IM_COL32(26, 26, 26, 255), borderRadius, ImDrawFlags_RoundCornersTop);
	drawList->AddRect(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), borderColor, borderRadius, 0, 1.0f);

	// Ajouter votre code pour dessiner le logo ici
	ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2, cursorPos.y + padding); // Centrer le logo
	addTexture(logo, logo);
	getTexture(logo, drawList, logoPos, squareSize);

	ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY - 20 + textOffsetY);
	ImGui::SetCursorScreenPos(sizePos);

	ImGui::GetFont()->Scale *= 0.7;
	ImGui::PushFont(ImGui::GetFont());
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Couleur grise pour le texte
	ImGui::PushItemWidth(maxTextWidth);
	ImGui::TextWrapped(size.c_str()); // Utilisation de TextWrapped pour que le texte s'ajuste automatiquement
	ImGui::PopItemWidth();
	ImGui::PopStyleColor();

	ImGui::GetFont()->Scale = 1.0f;
	ImGui::PopFont();

	ImVec2 lineStart = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);			 // Déplacer la ligne vers le bas
	ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight); // Déplacer la ligne vers le bas
	drawList->AddLine(lineStart, lineEnd, lineColor, separatorHeight);

	ImGui::GetFont()->Scale *= 0.9;
	ImGui::PushFont(ImGui::GetFont());

	ImVec2 textPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
	ImGui::SetCursorScreenPos(textPos);
	ImGui::PushItemWidth(maxTextWidth);
	ImU32 textColor = IM_COL32(255, 255, 255, 255);
	ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
	ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

	DrawHighlightedText(drawList, textPos, truncatedText.c_str(), ProjectSearch, highlightColor, textColor, highlightTextColor);

	ImGui::PopItemWidth();

	ImGui::GetFont()->Scale = 1.0f;
	ImGui::PopFont();

	ImVec2 descriptionPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + 35 + textOffsetY);
	ImGui::SetCursorScreenPos(descriptionPos);

	ImGui::GetFont()->Scale *= 0.7;
	ImGui::PushFont(ImGui::GetFont());
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Couleur grise pour le texte
	ImGui::PushItemWidth(maxTextWidth);
	ImGui::TextWrapped(description.c_str()); // Utilisation de TextWrapped pour que le texte s'ajuste automatiquement
	ImGui::PopItemWidth();
	ImGui::PopStyleColor();

	ImGui::GetFont()->Scale = 1.0f;
	ImGui::PopFont();

	ImVec2 smallRectPos = ImVec2(cursorPos.x + fixedSize.x - versionBoxWidth - padding, cursorPos.y + fixedSize.y - versionBoxHeight - padding);
	drawList->AddRectFilled(smallRectPos, ImVec2(smallRectPos.x + versionBoxWidth, smallRectPos.y + versionBoxHeight), IM_COL32(0, 0, 0, 255), borderRadius);

	float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	if (cursorPos.x + fixedSize.x < windowVisibleX2)
		ImGui::SameLine();

	// Ajouter un espacement vertical après chaque bouton pour éviter le chevauchement
	ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

	return pressed;
}

// Helper function to get file extension
std::string get_extension(const std::string &path)
{
	size_t dot_pos = path.find_last_of('.');
	if (dot_pos == std::string::npos)
		return "";
	return path.substr(dot_pos + 1);
}

// Function to detect file type based on extension
FileTypes detect_file(const std::string &path)
{
	static const std::unordered_map<std::string, FileTypes> extension_map = {
		{"asm", FileTypes::File_ASM},
		{"bin", FileTypes::File_BIN},
		{"c", FileTypes::File_C},
		{"h", FileTypes::File_H},
		{"cpp", FileTypes::File_CPP},
		{"hpp", FileTypes::File_HPP},
		{"inl", FileTypes::File_INL},
		{"rs", FileTypes::File_RUST},
		{"zig", FileTypes::File_ZIG},
		{"go", FileTypes::File_GO},
		{"cfg", FileTypes::File_CFG},
		{"json", FileTypes::File_JSON},
		{"txt", FileTypes::File_TXT},
		{"md", FileTypes::File_MD},
		{"yaml", FileTypes::File_YAML},
		{"ini", FileTypes::File_INI},
		{"gitignore", FileTypes::File_GIT},
		{"gitmodules", FileTypes::File_GIT},
		{"git", FileTypes::File_GIT},
		{"png", FileTypes::File_PICTURE},
		{"jpg", FileTypes::File_PICTURE},
		{"jpeg", FileTypes::File_PICTURE},
	};

	std::string extension = get_extension(path);
	auto it = extension_map.find(extension);
	if (it != extension_map.end())
	{
		return it->second;
	}
	else
	{
		return FileTypes::File_UNKNOW;
	}
}
ContentBrowserPanel::ContentBrowserPanel(VxContext *_ctx, const std::string &parentwindow)
	: m_BaseDirectory("/"), m_CurrentDirectory(m_BaseDirectory)
{
	this->ctx = _ctx;
	parent = parentwindow;
	_parent = parentwindow;

	{
		uint32_t w, h;
		void *data = UIKit::Image::Decode(icons::i_FolderIcon, icons::i_FolderIcon_size, w, h);
		m_DirectoryIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
		free(data);
	}
	{
		uint32_t w, h;
		void *data = UIKit::Image::Decode(icons::i_project, icons::i_project_size, w, h);
		m_FileIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
		free(data);
	}
	{
		uint32_t w, h;
		void *data = UIKit::Image::Decode(icons::i_project, icons::i_project_size, w, h);
		m_ProjectIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
		free(data);
	}
}

void ContentBrowserPanel::menubar()
{
	static ImTextureID refreshIcon = this->m_ProjectIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	addTexture(Icon_Left_Arrow_Enabled, Icon_Left_Arrow_Enabled);
	addTexture(Icon_Right_Arrow_Enabled, Icon_Right_Arrow_Enabled);
	addTexture(Icon_Left_Arrow_Disabled, Icon_Left_Arrow_Disabled);
	addTexture(Icon_Right_Arrow_Disabled, Icon_Right_Arrow_Disabled);

	ImGui::SetNextWindowSize(ImVec2(200, 300));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 200, 300));
	if (ImGui::BeginMenuBar())
	{
		float oldsize = ImGui::GetFont()->Scale;
		ImGui::GetFont()->Scale *= 0.85;
		ImGui::PushFont(ImGui::GetFont());

		if (ImGui::UIKit_ImageButtonWithText(refreshIcon, "Add", ImVec2(15, 15)))
		{
		}
		ImGui::GetFont()->Scale = oldsize;
		ImGui::PopFont();
		if (ImGui::UIKit_ImageButtonWithText(refreshIcon, "Import", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
		{
		}

		ImGui::Separator();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		if (m_BackHistory.empty())
		{
			if (ImGui::UIKit_ImageButtonWithText(getTexture(Icon_Left_Arrow_Disabled), "##LEFT_ARROW", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
			{
				//
			}
		}
		else
		{
			if (ImGui::UIKit_ImageButtonWithText(getTexture(Icon_Left_Arrow_Enabled), "##LEFT_ARROW", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
			{
				GoBack();
			}

			if (ImGui::IsMouseClicked(3))
			{
				GoBack();
			}
		}

		if (m_ForwardHistory.empty())
		{
			if (ImGui::UIKit_ImageButtonWithText(getTexture(Icon_Right_Arrow_Disabled), "##RIGHT_ARROW", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
			{
				//
			}
		}
		else
		{
			if (ImGui::UIKit_ImageButtonWithText(getTexture(Icon_Right_Arrow_Enabled), "##RIGHT_ARROW", ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
			{
				GoForward();
			}

			if (ImGui::IsMouseClicked(4))
			{
				GoForward();
			}
		}
		ImGui::PopStyleColor();

		ImGui::Separator();

		ImGui::Text("All / Main / Sources");

		// Calculer la largeur du texte du bouton
		float buttonWidth = ImGui::CalcTextSize("RightButton").x;
		float spacing = 30.0f;

		// Placer le bouton à droite de la barre de menu
		ImGui::SameLine(ImGui::GetWindowWidth() - buttonWidth - spacing);

		// Utilisation de ImageButton ou de votre bouton personnalisé
		if (ImGui::ImageButton(refreshIcon, ImVec2(this->m_ProjectIcon->GetWidth(), this->m_ProjectIcon->GetHeight())))
		{
			ImGui::OpenPopup("SettingPopup");
		}

		// Ouvrir le popup contextuel si l'utilisateur a cliqué sur le bouton
		if (ImGui::BeginPopup("SettingPopup"))
		{
			// Ajouter une entrée de menu pour "Change folder color"
			if (ImGui::BeginMenu("Change folder color"))
			{
				static bool alpha_preview = true;
				static bool alpha_half_preview = false;
				static bool drag_and_drop = true;
				static bool options_menu = true;
				static bool hdr = false;
				ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) |
												 (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) |
												 (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) |
												 (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

				ImGui::ColorEdit3("MyColor##1", (float *)&folder_color, misc_flags);
				ImGui::EndMenu(); // Fin du sous-menu "Change folder color"
			}
			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();

		ImGui::EndMenuBar();
	}
}

static std::uintmax_t getDirectorySize(const fs::path &directoryPath)
{
	std::uintmax_t size = 0;

	for (const auto &entry : fs::recursive_directory_iterator(directoryPath))
	{
		if (fs::is_regular_file(entry.path()))
		{
			size += fs::file_size(entry.path());
		}
	}

	return size;
}

static std::string formatFileSize(size_t size)
{
	const char *units[] = {"o", "ko", "Mo", "Go", "To"};
	int unitIndex = 0;
	double displaySize = static_cast<double>(size);

	while (displaySize >= 1024 && unitIndex < 4)
	{
		displaySize /= 1024;
		++unitIndex;
	}

	char formattedSize[20];
	snprintf(formattedSize, sizeof(formattedSize), "%.2f %s", displaySize, units[unitIndex]);
	return std::string(formattedSize);
}

void ContentBrowserPanel::GoBack()
{
	if (!m_BackHistory.empty())
	{
		m_ForwardHistory.push(m_CurrentDirectory);
		m_CurrentDirectory = m_BackHistory.top();
		m_BackHistory.pop();
	}
}

void ContentBrowserPanel::GoForward()
{
	if (!m_ForwardHistory.empty())
	{
		m_BackHistory.push(m_CurrentDirectory);
		m_CurrentDirectory = m_ForwardHistory.top();
		m_ForwardHistory.pop();
	}
}

void ContentBrowserPanel::ChangeDirectory(const std::filesystem::path &newDirectory)
{
	if (newDirectory != m_CurrentDirectory)
	{
		if (!m_CurrentDirectory.empty())
		{
			m_BackHistory.push(m_CurrentDirectory);
			// Clear forward history when a new directory is selected
			while (!m_ForwardHistory.empty())
			{
				m_ForwardHistory.pop();
			}
		}
		m_CurrentDirectory = newDirectory;
	}
}

void Splitter(bool split_vertically, float thickness, float *size1, float *size2, float min_size1, float min_size2)
{
	ImVec2 backup_pos = ImGui::GetCursorPos();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));		   // Gris
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f)); // Gris clair pour hover
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));  // Gris plus clair pour actif

	if (split_vertically)
	{
		ImGui::SetCursorPosX(backup_pos.x + *size1);
		ImGui::InvisibleButton("##Splitter", ImVec2(thickness, ImGui::GetContentRegionAvail().y));
	}
	else
	{
		ImGui::SetCursorPosY(backup_pos.y + *size1);
		ImGui::InvisibleButton("##Splitter", ImVec2(ImGui::GetContentRegionAvail().x, thickness));
	}

	if (ImGui::IsItemActive())
	{
		float mouse_delta = (split_vertically ? ImGui::GetIO().MouseDelta.x : ImGui::GetIO().MouseDelta.y);
		if (mouse_delta < min_size1 - *size1)
			mouse_delta = min_size1 - *size1;
		if (mouse_delta > *size2 - min_size2)
			mouse_delta = *size2 - min_size2;
		*size1 += mouse_delta;
		*size2 -= mouse_delta;
	}

	ImGui::SetCursorPos(backup_pos);
	ImGui::PopStyleColor(3); // Restaurer les couleurs précédentes
}

void ContentBrowserPanel::DrawFolderIcon(ImVec2 pos, ImVec2 size, ImU32 color)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Calcul des positions des différentes parties du dossier
    float folderFlapHeight = size.y * 0.2f;
    float flapSlopeWidth = size.x * 0.15f;
    float borderRadius = size.y * 0.1f;

    ImVec2 flapTopLeft = pos;
    ImVec2 flapBottomRight = ImVec2(pos.x + size.x * 0.6f, pos.y + folderFlapHeight);
    ImVec2 flapSlopeEnd = ImVec2(flapBottomRight.x + flapSlopeWidth, flapBottomRight.y);

    ImVec2 bodyTopLeft = ImVec2(pos.x, pos.y + folderFlapHeight);
    ImVec2 bodyBottomRight = ImVec2(pos.x + size.x, pos.y + size.y);

    // Couleur pour le dégradé
    ImU32 darkColor = IM_COL32(130, 180, 75, 255); // Couleur plus foncée

    // Dessiner le corps du dossier avec coins arrondis et dégradé
    drawList->AddRectFilledMultiColor(bodyTopLeft, bodyBottomRight, color, color, darkColor, darkColor);

    // Dessiner un rectangle blanc au centre
    ImVec2 centerRectTopLeft = ImVec2(pos.x + size.x * 0.2f, pos.y + 0.2f + size.y * 0.2f);
    ImVec2 centerRectBottomRight = ImVec2(pos.x + size.x * 0.8f, pos.y + size.y * 0.8f);
    drawList->AddRectFilled(centerRectTopLeft, centerRectBottomRight, IM_COL32_WHITE, 0.0f, 0);

    ImVec2 secondBodyTopLeft = ImVec2(pos.x, pos.y - 0.8f);
    ImVec2 secondBodyBottomRight = ImVec2(pos.x + size.x, pos.y + size.y - 0.8f);
	
    // Dessiner la languette du dossier avec pente et coins arrondis
    drawList->AddRectFilled(flapTopLeft, flapBottomRight, color, borderRadius, ImDrawFlags_RoundCornersTopLeft);
    drawList->AddTriangleFilled(flapBottomRight, flapSlopeEnd, ImVec2(flapBottomRight.x - 3, flapTopLeft.y), color);
}




void ContentBrowserPanel::MyFolderButton(const char *id, ImVec2 size, ImU32 color, const std::string &path)
{
	ImVec2 pos = ImGui::GetCursorScreenPos();

	ImGui::InvisibleButton(id, size);
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		ChangeDirectory(path);
	}

	// Dessiner l'icône du dossier
	DrawFolderIcon(pos, size, color);

	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
}

// Fonction pour afficher un en-tête avec une icône
bool CollapsingHeaderWithIcon(const char *label, ImTextureID icon)
{
	ImGui::PushID(label);

	bool open = ImGui::CollapsingHeader("##hidden", ImGuiTreeNodeFlags_AllowItemOverlap);

	// Positionnez l'image avant le texte
	ImVec2 textPos = ImGui::GetCursorPos();
	ImGui::SameLine();
	ImGui::SetCursorPosX(textPos.x + ImGui::GetStyle().FramePadding.x);

	ImGui::Image(icon, ImVec2(16, 16)); // Ajustez la taille selon vos besoins
	ImGui::SameLine();

	ImGui::SetCursorPosY(textPos.y);
	ImGui::TextUnformatted(label);

	ImGui::PopID();

	return open;
}

bool MyCollapsingHeader(const char *label, ImTextureID my_texture)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
	bool *p_open = ImGui::GetStateStorage()->GetBoolRef(ImGui::GetID(label), false);

	ImGuiStyle &style = ImGui::GetStyle();

	// Calculate the size of the texture and padding
	float texture_size = ImGui::GetFontSize(); // Adjust the size of the texture as needed
	float padding = style.ItemInnerSpacing.x;

	// Create a horizontal layout
	ImGui::BeginGroup();
	if (ImGui::UIKit_ImageButtonWithText(my_texture, label, ImVec2(-FLT_MIN, 0.0f)))
		*p_open ^= 1;

	// Adjust arrow position considering the texture
	ImVec2 arrow_pos = ImVec2(ImGui::GetItemRectMax().x - style.FramePadding.x - ImGui::GetFontSize(), ImGui::GetItemRectMin().y + style.FramePadding.y);
	ImGui::RenderArrow(ImGui::GetWindowDrawList(), arrow_pos, ImGui::GetColorU32(ImGuiCol_Text), *p_open ? ImGuiDir_Down : ImGuiDir_Right);
	ImGui::EndGroup();

	ImGui::PopStyleVar();
	return *p_open;
}

static std::vector<std::pair<std::shared_ptr<ContenBrowserItem>, std::string>> recognized_modules_items;

void ContentBrowserPanel::OnImGuiRender(const std::string &parent, std::function<void(ImGuiWindow *)> controller)
{
	static ImTextureID projectIcon = this->m_ProjectIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowDockID(ImGui::GetID("MainDockspace"), ImGuiCond_FirstUseEver);

	if (ImGui::UIKit_BeginLogoWindow("Content Browser", &projectIcon, &this->opened, window_flags))
	{
		// Render the menubar with proper paddin
		this->menubar();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	static ImGuiWindow *win = ImGui::GetCurrentWindow();
	this->parent = parent;
	controller(win);

	ImGui::PopStyleVar(); // Pop the ItemSpacing style

	static float size1 = 200.0f;
	static float size2 = 200.0f;
	float min_size1 = 200.0f;
	float min_size2 = 200.0f;
	float splitter_thickness = 4.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);

	ImGui::BeginChild("Child1", ImVec2(size1, 0), true);
	if (MyCollapsingHeader("Favorites", projectIcon))
	{
	}
	if (MyCollapsingHeader("All", projectIcon))
	{
	}

	ImGui::EndChild();
	ImGui::PopStyleVar(4);

	ImGui::SameLine();

	// Sauvegarder les styles actuels
	ImGuiStyle &style = ImGui::GetStyle();
	ImVec4 originalChildBgColor = style.Colors[ImGuiCol_ChildBg];
	ImVec4 originalBorderColor = style.Colors[ImGuiCol_Border];
	ImVec4 originalBorderShadowColor = style.Colors[ImGuiCol_BorderShadow];

	// Modifier la couleur de fond et les bordures pour les rendre transparentes
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
	style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

	ImGui::BeginChild("Child2", ImVec2(0, 0), true);

	static float padding = 16.0f;
	static float thumbnailSize = 94.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::InputText("Search", ProjectSearch, sizeof(ProjectSearch));
	ImGui::Columns(columnCount, 0, false);

	std::vector<std::filesystem::directory_entry> directories;
	std::vector<std::filesystem::directory_entry> files;

	recognized_modules_items.clear();

	for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		bool isItem = false;
		for (auto item : this->ctx->IO.contentbrowser_items)
		{
			std::string path = directoryEntry.path().string();
			if (item->f_Detect(path))
			{
				recognized_modules_items.push_back({item, path});
				isItem = true;
			}
		}

		if(isItem)
		{
			continue;
		}

		if (directoryEntry.is_directory())
		{
			directories.push_back(directoryEntry);
		}
		else
		{
			files.push_back(directoryEntry);
		}
	}

	std::sort(directories.begin(), directories.end(), [](const auto &a, const auto &b)
			  { return a.path().filename().string() < b.path().filename().string(); });

	for (auto &directoryEntry : directories)
	{
		const auto &path = directoryEntry.path();
		std::string filenameString = path.filename().string();

		if (areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
		{
			ImGui::PushID(filenameString.c_str());

			// Réduire légèrement la taille du dossier
			float reducedThumbnailSize = thumbnailSize * 0.9f;

			// Calculer les positions pour centrer l'image du dossier et le texte
			float availableWidth = ImGui::GetContentRegionAvail().x;
			float imageOffsetX = (availableWidth - reducedThumbnailSize) * 0.5f;

			// Enlever le fond gris, les contours et la bordure
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

			// Old folder logo
			/*ImGui::SetCursorPosX(ImGui::GetCursorPosX() + imageOffsetX);
			addTexture(Folder_Logo, Folder_Logo);
			ImGui::ImageButton(getTexture(Folder_Logo), {reducedThumbnailSize, reducedThumbnailSize}, {-1, 0}, {0, 1});

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				ChangeDirectory(path);
			}*/

			// Définir la taille et la couleur de l'icône du dossier
			ImVec2 folderSize(reducedThumbnailSize, reducedThumbnailSize); // Taille de l'icône
			// ImU32 folderColor = ;				   // Jaune

			// Chemin du dossier à ouvrir
			std::string folderPath = "path/to/folder";

			// Dessiner le bouton dossier
			MyFolderButton("folder_icon", folderSize, folder_color, path);

			float oldsize = ImGui::GetFont()->Scale;
			if (ImGui::BeginPopupContextItem("ContextPopup"))
			{

				// Appliquer le nouveau scale
				ImGui::GetFont()->Scale *= 0.9;
				ImGui::PushFont(ImGui::GetFont());

				// Ajouter un espace au-dessus
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f); // Ajustez la valeur selon vos besoins

				// Changer la couleur du texte en gris
				ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);			// Gris (50% blanc)
				ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f); // Gris (50% blanc)
				ImGui::PushStyleColor(ImGuiCol_Text, grayColor);

				ImGui::Text("Main");

				// Restaurer la couleur du texte précédente
				ImGui::PopStyleColor();

				// Changer la couleur du separator en gris
				ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
				ImGui::Separator();
				ImGui::PopStyleColor(); // Restaurer la couleur du separator précédente

				// Restaurer l'ancien scale de la police
				ImGui::GetFont()->Scale = oldsize;
				ImGui::PopFont();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f); // Ajustez la valeur selon vos besoins

				if (ImGui::MenuItem("Open", "Ctrl + O"))
				{
					ChangeDirectory(path);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Copy folder", "Ctrl + C"))
				{
					ChangeDirectory(path);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Cut folder", "Ctrl + X"))
				{
					ChangeDirectory(path);
					ImGui::CloseCurrentPopup();
				}

				// Appliquer le nouveau scale
				ImGui::GetFont()->Scale *= 0.9;
				ImGui::PushFont(ImGui::GetFont());

				// Ajouter un espace au-dessus
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f); // Ajustez la valeur selon vos besoins

				// Changer la couleur du texte en gris
				ImGui::PushStyleColor(ImGuiCol_Text, grayColor);

				ImGui::Text("Main");

				// Restaurer la couleur du texte précédente
				ImGui::PopStyleColor();

				// Changer la couleur du separator en gris
				ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
				ImGui::Separator();
				ImGui::PopStyleColor(); // Restaurer la couleur du separator précédente

				// Restaurer l'ancien scale de la police
				ImGui::GetFont()->Scale = oldsize;
				ImGui::PopFont();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f); // Ajustez la valeur selon vos besoins

				if (ImGui::MenuItem("Change color"))
				{
				}
				if (ImGui::MenuItem("Mark as favorite"))
				{
				}
				// Ajouter d'autres options de menu ici...

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar(2); // Pop FrameBorderSize and FramePadding
			ImGui::PopStyleColor(3);

			// Centrer le texte
			float textWidth = ImGui::CalcTextSize(filenameString.c_str()).x;
			float textOffsetX = (availableWidth - textWidth) * 0.5f;

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffsetX);
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::PopID();
			ImGui::NextColumn();
		}
	}

	for (auto &itemEntry : recognized_modules_items)
	{
		const auto &path = itemEntry.second;
		fs::path fsPath(path);
		std::string filenameString = fsPath.filename().string();

		bool selected = false;

		if (std::find(m_Selected.begin(), m_Selected.end(), path) != m_Selected.end())
		{
			selected = true;
		}

		if (areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
		{
			std::uintmax_t folderSize = getDirectorySize(path);
			std::string folderSizeString = formatFileSize(folderSize);
			ImGui::PushID(filenameString.c_str());

			if (MyButton(filenameString, itemEntry.first->m_Description, folderSizeString, selected, File_PICTURE_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(itemEntry.first->m_LineColor.x, itemEntry.first->m_LineColor.y, itemEntry.first->m_LineColor.z, itemEntry.first->m_LineColor.w)))
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					itemEntry.first->f_Execute(path);
					VXERROR("te", "tyr");
				}

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
				{
					m_Selected.push_back(path);
				}
				else
				{
					m_Selected.clear();
					m_Selected.push_back(path);
				}
			}

			ImGui::PopID();
			ImGui::NextColumn();
		}
	}

	for (auto &fileEntry : files)
	{
		const auto &path = fileEntry.path();
		std::string filenameString = path.filename().string();

		bool selected = false;

		if (std::find(m_Selected.begin(), m_Selected.end(), path) != m_Selected.end())
		{
			selected = true;
		}

		if (areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
		{
			size_t fileSize = std::filesystem::file_size(path);
			std::string fileSizeString = formatFileSize(fileSize);
			ImGui::PushID(filenameString.c_str());

			FileTypes fileType = detect_file(path.string());

			switch (fileType)
			{
			case FileTypes::File_PICTURE:
			{
				if (MyButton(filenameString, "Picture file", fileSizeString, selected, File_PICTURE_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(255, 100, 150, 255)))
				{
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						m_Selected.push_back(path);
					}
					else
					{
						m_Selected.clear();
						m_Selected.push_back(path);
					}
				}
				break;
			}
			case FileTypes::File_GIT:
			{
				if (MyButton(filenameString, "Git File", fileSizeString, selected, File_GIT_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(100, 100, 255, 255)))
				{
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						m_Selected.push_back(path);
					}
					else
					{
						m_Selected.clear();
						m_Selected.push_back(path);
					}
				}
				break;
			}
			case FileTypes::File_H:
			{
				if (MyButton(filenameString, "C Header File", fileSizeString, selected, File_C_H_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(220, 100, 220, 255)))
				{
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						m_Selected.push_back(path);
					}
					else
					{
						m_Selected.clear();
						m_Selected.push_back(path);
					}
				}
				break;
			}
			case FileTypes::File_C:
			{
				if (MyButton(filenameString, "C Source File", fileSizeString, selected, File_C_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(100, 100, 255, 255)))
				{
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						m_Selected.push_back(path);
					}
					else
					{
						m_Selected.clear();
						m_Selected.push_back(path);
					}
				}
				break;
			}
			case FileTypes::File_HPP:
			{
				if (MyButton(filenameString, "C++ Header File", fileSizeString, selected, File_CPP_H_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(100, 100, 255, 255)))
				{
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						m_Selected.push_back(path);
					}
					else
					{
						m_Selected.clear();
						m_Selected.push_back(path);
					}
				}
				break;
			}
			case FileTypes::File_CPP:
			{
				if (MyButton(filenameString, "C++ Source File", fileSizeString, selected, File_CPP_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(100, 100, 255, 255)))
				{
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						m_Selected.push_back(path);
					}
					else
					{
						m_Selected.clear();
						m_Selected.push_back(path);
					}
				}
				break;
			}
			case FileTypes::File_INI:
			{
				if (MyButton(filenameString, "Init File", fileSizeString, selected, File_CPP_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(150, 150, 150, 255)))
				{
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						m_Selected.push_back(path);
					}
					else
					{
						m_Selected.clear();
						m_Selected.push_back(path);
					}
				}
				break;
			}
			default:
			{
				if (MyButton(filenameString, "File", fileSizeString, selected, File_UNKNOW_Logo, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), IM_COL32(150, 150, 150, 255)))
				{
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						m_Selected.push_back(path);
					}
					else
					{
						m_Selected.clear();
						m_Selected.push_back(path);
					}
				}
				break;
			}

				ImGui::PopStyleVar(2); // Pop FrameBorderSize and FramePadding
				ImGui::PopStyleColor(3);
			}
			float oldsize = ImGui::GetFont()->Scale;

			if (ImGui::BeginPopupContextItem("ItemContextPopup"))
			{

				// Appliquer le nouveau scale
				ImGui::GetFont()->Scale *= 0.9;
				ImGui::PushFont(ImGui::GetFont());

				// Ajouter un espace au-dessus
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f); // Ajustez la valeur selon vos besoins

				// Changer la couleur du texte en gris
				ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);			// Gris (50% blanc)
				ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f); // Gris (50% blanc)
				ImGui::PushStyleColor(ImGuiCol_Text, grayColor);

				ImGui::Text("Main");

				// Restaurer la couleur du texte précédente
				ImGui::PopStyleColor();

				// Changer la couleur du separator en gris
				ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
				ImGui::Separator();
				ImGui::PopStyleColor(); // Restaurer la couleur du separator précédente

				// Restaurer l'ancien scale de la police
				ImGui::GetFont()->Scale = oldsize;
				ImGui::PopFont();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f); // Ajustez la valeur selon vos besoins

				if (ImGui::MenuItem("Open", "Ctrl + O"))
				{
					ChangeDirectory(path);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Copy folder", "Ctrl + C"))
				{
					ChangeDirectory(path);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Cut folder", "Ctrl + X"))
				{
					ChangeDirectory(path);
					ImGui::CloseCurrentPopup();
				}

				// Appliquer le nouveau scale
				ImGui::GetFont()->Scale *= 0.9;
				ImGui::PushFont(ImGui::GetFont());

				// Ajouter un espace au-dessus
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f); // Ajustez la valeur selon vos besoins

				// Changer la couleur du texte en gris
				ImGui::PushStyleColor(ImGuiCol_Text, grayColor);

				ImGui::Text("Main");

				// Restaurer la couleur du texte précédente
				ImGui::PopStyleColor();

				// Changer la couleur du separator en gris
				ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
				ImGui::Separator();
				ImGui::PopStyleColor(); // Restaurer la couleur du separator précédente

				// Restaurer l'ancien scale de la police
				ImGui::GetFont()->Scale = oldsize;
				ImGui::PopFont();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f); // Ajustez la valeur selon vos besoins

				if (ImGui::MenuItem("Change color"))
				{
				}
				if (ImGui::MenuItem("Mark as favorite"))
				{
				}
				// Ajouter d'autres options de menu ici...

				ImGui::EndPopup();
			}

			ImGui::PopID();
			ImGui::NextColumn();
		}
	}

	ImGui::EndChild();

	// Restaurer les couleurs originales
	style.Colors[ImGuiCol_ChildBg] = originalChildBgColor;
	style.Colors[ImGuiCol_Border] = originalBorderColor;
	style.Colors[ImGuiCol_BorderShadow] = originalBorderShadowColor;
	ImGui::Columns(1);

	ImGui::End();
}
