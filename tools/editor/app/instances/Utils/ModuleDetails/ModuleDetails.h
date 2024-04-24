
#include "../../../src/instanceFactory.h"
#include "../../../../../../lib/uikit/uikit.h"

#include "../../../../../../main/include/modules/interface.h"

#ifndef ModuleDetails_H
#define ModuleDetails_H

using namespace VortexMaker;

class ModuleDetails
{
public:
    ModuleDetails(VxContext *ctx, std::shared_ptr<ModuleInterface> module);

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

    std::string name;
    VxContext *m_ctx;
    std::shared_ptr<ModuleInterface> m_module;

    std::shared_ptr<UIKit::Image> m_ToolchainIcon;
    std::shared_ptr<UIKit::Image> m_Icon;
    std::shared_ptr<UIKit::Image> m_EyeIcon;
    std::shared_ptr<UIKit::Image> m_WarningIcon;
    std::shared_ptr<UIKit::Image> m_FlipBookIcon;
    std::shared_ptr<UIKit::Image> m_ErrorIcon;
    std::shared_ptr<UIKit::Image> m_SuccessIcon;
    std::shared_ptr<UIKit::Image> m_UnknowIcon;
    std::shared_ptr<UIKit::Image> m_TaskListIcon;
    std::shared_ptr<UIKit::Image> m_SaveIcon;
    std::shared_ptr<UIKit::Image> m_RefreshIcon;
    std::shared_ptr<UIKit::Image> m_DatabaseIcon;
    std::shared_ptr<UIKit::Image> m_TrashIcon;
    std::shared_ptr<UIKit::Image> m_PackageIcon;
    std::shared_ptr<UIKit::Image> m_EditIcon;
    std::shared_ptr<UIKit::Image> m_BuildIcon;
    std::shared_ptr<UIKit::Image> m_SettingsIcon;
    std::shared_ptr<UIKit::Image> m_AddIcon;
    std::shared_ptr<UIKit::Image> m_FolderIcon;
};

#endif // ModuleDetails_H