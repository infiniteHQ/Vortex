
#include "../../../../../../lib/uikit/Source/editor/Application.h"
#include "../../../../../../lib/uikit/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../../../../../lib/uikit/Platform/GUI/editor/Image.h"
#include "../../../../../../lib/uikit/Platform/GUI/editor/UI/UI.h"
#include "../../../../../../lib/uikit/Assets/icons.h"
#include "../../../src/instanceFactory.h"
#include "../../../../../../main/include/vortex.h"

#ifndef TasklistInstance_H
#define TasklistInstance_H


using namespace VortexMaker;
class InstanceFactory;


class TasklistInstance
{
public:
    TasklistInstance(VxContext *ctx, std::shared_ptr<TaskList> t, std::shared_ptr<VxHost> parentHost);
    TasklistInstance(VxContext *ctx, std::shared_ptr<TaskList> t, std::shared_ptr<VxToolchain> parentToolchain);

    // Content Managment 
    void Refresh();
    void Save();

    // UI Managment
    std::string render();
    void close();
    void menubar();

    void UI_MainSettings();

    bool opened;
    bool show_UI_MainSettings = true;


    std::string name;
    VxContext *m_ctx;
    std::shared_ptr<TaskList> tasklist;
    std::shared_ptr<TaskListSave> m_currentSave;
    ImGuiID dockspaceID;

    std::shared_ptr<VxHost> parentHost;
    std::shared_ptr<VxToolchain> parentToolchain;
    std::string parentType;

    std::shared_ptr<Walnut::Image> m_HostIcon;
    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_SaveIcon;
    std::shared_ptr<Walnut::Image> m_TaskListIcon;
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

#endif // TasklistInstance_H