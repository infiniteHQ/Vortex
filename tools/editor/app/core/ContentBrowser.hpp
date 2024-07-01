#include <stack>
#include <algorithm>
#include <cctype>
#include <unordered_set>

#ifndef CONTENTBROWSER_H
#define CONTENTBROWSER_H

#include "../include/instanceFactory.h"

enum class FileTypes
{
	// Very low level
	File_ASM,
	File_BIN,

	// Programming languages
	File_C,
	File_H,
	File_CPP,
	File_HPP,
	File_INL,
	File_RUST,
	File_ZIG,
	File_GO,
	File_JAVA,
	File_JAVASCRIPT,
	File_COBOL,
	File_PASCAL,
	File_CARBON,

	// Misc
	File_CFG,
	File_JSON,
	File_PICTURE,
	File_TXT,
	File_MD,
	File_YAML,
	File_INI,
	File_GIT,

	// Vortex
	File_VORTEX_CONFIG,

	// Other
	File_UNKNOW,

};

struct FileHandler
{
	FileTypes for_type;
	std::string file_handler_name;
	std::string handler_module_name;
	std::string handler_plugin_name;
};

class ContentBrowserPanel
{
public:
	ContentBrowserPanel(VxContext *_ctx, const std::string &parent);

	void OnImGuiRender(const std::string &parent, std::function<void(ImGuiWindow *)> controller);

    void ChangeDirectory(const std::filesystem::path &newDirectory);
    void GoBack();
    void GoForward();

void DrawFolderIcon(ImVec2 pos, ImVec2 size, ImU32 color);
void MyFolderButton(const char* id, ImVec2 size, ImU32 color, const std::string& path);

	VxContext *ctx;
	std::string parent;
	void menubar();

private:
	bool opened;

	bool m_ThumbnailMode = true;
	bool m_ListMode = false;

	bool m_ShowTypes = false;
	bool m_ShowSizes = false;

	std::filesystem::path m_BaseDirectory;

    std::filesystem::path m_CurrentDirectory;
    std::stack<std::filesystem::path> m_BackHistory;
    std::stack<std::filesystem::path> m_ForwardHistory;

    std::vector<std::filesystem::path> m_Selected;

    std::vector<std::filesystem::path> m_Favorites;

	std::shared_ptr<UIKit::Image> m_ProjectIcon;
	std::shared_ptr<UIKit::Image> m_FileIcon;
	std::shared_ptr<UIKit::Image> m_DirectoryIcon;
};

#endif // CONTENTBROWSER_H