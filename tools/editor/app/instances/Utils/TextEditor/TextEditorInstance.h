
#include "../../../src/instanceFactory.h"
#include "../../../../../../lib/uikit/Source/editor/Application.h"
#include "../../../../../../lib/uikit/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../../../../../lib/uikit/Platform/GUI/editor/Image.h"
#include "../../../../../../lib/uikit/Platform/GUI/editor/UI/UI.h"
#include "../../../../../../lib/uikit/Assets/icons.h"
#include "./TextEditor.hpp"

class TextEditor;

#ifndef TextEditorInstance_H
#define TextEditorInstance_H

using namespace VortexMaker;

class TextEditorInstance
{
public:
    TextEditorInstance(VxContext *ctx, const std::string& path);

    std::string render();
    void close();

    void menubar();


    // Content Managment 
    void Refresh();
    void Save();

    void UI_ContentWindow();
    void UI_TasksEditor();
    void UI_ParametersWindow();
    void UI_CurrentToolchainPreview();
    void UI_SnapshotUtility();
    void UI_ManualBuild();

    bool opened;
    bool show_UI_ContentWindow = false;
    bool show_UI_ParametersWindow = false;
    bool show_UI_TasksEditor = false;
    bool show_UI_CurrentToolchainPreview = false;
    bool show_UI_SnapshotUtility = false;
    bool show_UI_ManualBuild = false;

    std::shared_ptr<TextEditor> editor;

    std::string name;
    VxContext *m_ctx;
    
    std::string textFilePath;
    std::string filename;
    std::string id;

    std::shared_ptr<Walnut::Image> m_ToolchainIcon;
    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_EyeIcon;
    std::shared_ptr<Walnut::Image> m_WarningIcon;
    std::shared_ptr<Walnut::Image> m_FlipBookIcon;
    std::shared_ptr<Walnut::Image> m_ErrorIcon;
    std::shared_ptr<Walnut::Image> m_SuccessIcon;
    std::shared_ptr<Walnut::Image> m_UnknowIcon;
    std::shared_ptr<Walnut::Image> m_TaskListIcon;
    std::shared_ptr<Walnut::Image> m_SaveIcon;
    std::shared_ptr<Walnut::Image> m_RefreshIcon;
    std::shared_ptr<Walnut::Image> m_DatabaseIcon;
    std::shared_ptr<Walnut::Image> m_TrashIcon;
    std::shared_ptr<Walnut::Image> m_PackageIcon;
    std::shared_ptr<Walnut::Image> m_EditIcon;
    std::shared_ptr<Walnut::Image> m_BuildIcon;
    std::shared_ptr<Walnut::Image> m_SettingsIcon;
    std::shared_ptr<Walnut::Image> m_AddIcon;
    std::shared_ptr<Walnut::Image> m_FolderIcon;
};

#endif // TextEditorInstance_H