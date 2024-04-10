// UI (with imgui)
#include "../../backend/Source/editor/Application.h"
#include "../../backend/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"
#include "../../vendor/imgui/imgui.h"
#include "../../vendor/stb-image/stb_image.h"

#include "../../backend/Assets/icons.h"

#include "../../../../main/include/vortex.h"
#include "../../../../main/include/vortex_internals.h"

#include "../../../src/instanceFactory.h"

#ifndef HostInstance_H
#define HostInstance_H

using namespace VortexMaker;
class InstanceFactory;


class HostInstance
{
public:
    HostInstance(VxContext *ctx, std::shared_ptr<VxHost> _host, InstanceFactory* factory);
    ~HostInstance(){this->receiveThread.join();}

    // Content Managment 
    void Refresh();
    void Save();

    // UI Managment
    std::string render();
    void close();
    void menubar();

    void UI_ContentWindow();
    void UI_AssetsViewer();
    void UI_ParametersWindow();
    void UI_MainSettings();
    void UI_FullBuild();
    void UI_SnapshotUtility();
    void UI_CurrentHostPreview();
    void UI_TasksEditor();
    void UI_VolatileTasks();



    bool opened;
    bool show_UI_ContentWindow = false;
    bool show_UI_ParametersWindow = false;
    bool show_UI_AssetsViewer = false;
    bool show_UI_MainSettings = false;
    bool show_UI_FullBuild = false;
    bool show_UI_SnapshotUtility = false;
    bool show_UI_CurrentHostPreview = false;
    bool show_UI_TasksEditor = false;
    bool show_UI_VolatileTasks = false;

    std::thread receiveThread;

    InstanceFactory* factory;

    std::string name;
    VxContext *m_ctx;
    std::shared_ptr<VxHost> host;
    std::shared_ptr<HostSave> m_currentSave;
    ImGuiID dockspaceID;

    std::shared_ptr<Walnut::Image> m_HostIcon;
    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_EyeIcon;
    std::shared_ptr<Walnut::Image> m_WarningIcon;
    std::shared_ptr<Walnut::Image> m_ErrorIcon;
    std::shared_ptr<Walnut::Image> m_SuccessIcon;
    std::shared_ptr<Walnut::Image> m_UnknowIcon;
    std::shared_ptr<Walnut::Image> m_FlipBookIcon;
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

#endif // HostInstance_H