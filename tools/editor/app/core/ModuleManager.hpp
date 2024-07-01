#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H


#include "../include/instanceFactory.h"

class ModuleManager
{
public:
	ModuleManager(VxContext *_ctx, InstanceFactory* _factory, const std::string &parent);

	void OnImGuiRender(const std::string& parent, std::function<void(ImGuiWindow*)> controller);	
	void menubar();
	void addModuleModal();


	VxContext *ctx;
	InstanceFactory* factory;
	std::string m_Parent;

private:
	bool opened;

	std::shared_ptr<VxToolchain> latest_toolchain;

    std::shared_ptr<UIKit::Image> m_StartIcon;
    std::shared_ptr<UIKit::Image> m_StopIcon;
    std::shared_ptr<UIKit::Image> m_LogsIcon;
    std::shared_ptr<UIKit::Image> m_ListIcon;
    std::shared_ptr<UIKit::Image> m_RefreshIcon;
    std::shared_ptr<UIKit::Image> m_ModuleIcon;
    std::shared_ptr<UIKit::Image> m_AddIcon;
    std::shared_ptr<UIKit::Image> m_TrashIcon;
	bool CollapseAll = false;
};

#endif // MODULEMANAGER_H