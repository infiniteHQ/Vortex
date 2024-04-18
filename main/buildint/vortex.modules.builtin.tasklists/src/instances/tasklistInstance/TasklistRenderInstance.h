#include "../../../../../../lib/uikit/uikit.h"
#include "../../../../../../main/include/vortex.h"
#include "../../../assets/icons.h"
#include "../../../../../../main/include/vortex_internals.h"
#include "../../module.h"

#ifndef TasklistRenderInstance_H
#define TasklistRenderInstance_H


using namespace VortexMaker;
class InstanceFactory;


class TasklistRenderInstance : public ModuleRenderInstance
{
public:
    TasklistRenderInstance(VxContext *ctx, std::shared_ptr<TaskList_> t);
    
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
    std::shared_ptr<TaskList_> tasklist;
    std::shared_ptr<TaskList_Save> m_currentSave;
    ImGuiID dockspaceID;

    std::shared_ptr<VxHost> parentHost;
    std::shared_ptr<VxToolchain> parentToolchain;
    std::string parentType;

    std::shared_ptr<Walnut::Image> m_HostIcon;
    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_SaveIcon;
    std::shared_ptr<Walnut::Image> m_TaskList_Icon;
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

#endif // TasklistRenderInstance_H