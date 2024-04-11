
#include "../../../../lib/uikit/uikit.h"
#include "../../../../main/include/vortex.h"
#include "../assets/icons.h"
#include "../../../../main/include/vortex_internals.h"

#ifndef ToolchainModulesInstance_H
#define ToolchainModulesInstance_H

using namespace VortexMaker;

class ToolchainInstance
{
public:
    ToolchainInstance(VxContext *ctx, std::shared_ptr<VxToolchain> _toolchain);

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
    void UI_VolatileTasks();
    void UI_FullBuild();
    void UI_AssetsViewer();
    void UI_MainSettings();

    bool opened;
    bool show_UI_ContentWindow = false;
    bool show_UI_ParametersWindow = false;
    bool show_UI_TasksEditor = false;
    bool show_UI_CurrentToolchainPreview = false;
    bool show_UI_SnapshotUtility = false;
    bool show_UI_ManualBuild = false;
    bool show_UI_FullBuild = false;
    bool show_UI_VolatileTasks = false;
    bool show_UI_AssetsViewer = false;
    bool show_UI_MainSettings = false;

    //InstanceFactory* factory;

    std::string name;
    VxContext *m_ctx;
    std::shared_ptr<VxToolchain> toolchain;
    std::shared_ptr<ToolchainSave> m_currentSave;
    std::shared_ptr<VxDistToolchainSave> m_currentDistSave;
    ImGuiID dockspaceID;

    std::shared_ptr<Walnut::Image> m_ToolchainIcon;
    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_EyeIcon;
    std::shared_ptr<Walnut::Image> m_FlipBookIcon;
    std::shared_ptr<Walnut::Image> m_WarningIcon;
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

#endif // ToolchainModulesInstance_H