#include "../../../../../../lib/newuikit/uikit.h"
#include "../../../../../../main/include/vortex.h"
#include "../../../assets/icons.h"
#include "../../../../../../main/include/vortex_internals.h"
#include "../../module.h"

#include "../../../../vortex.modules.builtin.toolchains/src/toolchain.h"

#ifndef TasklistRenderInstance_H
#define TasklistRenderInstance_H


using namespace VortexMaker;
class InstanceFactory;


class TasklistRenderInstance : public ModuleRenderInstance
{
public:
    TasklistRenderInstance(VxContext *ctx, std::shared_ptr<TaskList> t, std::shared_ptr<Toolchain> toolchain);
    
    // Content Managment 
    void Refresh();
    void Save();

    // UI Managment
    void render();
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

    //std::shared_ptr<VxHost> parentHost;
    std::shared_ptr<Toolchain> parentToolchain;
    std::string parentType;

    std::shared_ptr<UIKit::Image> m_HostIcon;
    std::shared_ptr<UIKit::Image> m_Icon;
    std::shared_ptr<UIKit::Image> m_SaveIcon;
    std::shared_ptr<UIKit::Image> m_TaskList_Icon;
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

#endif // TasklistRenderInstance_H