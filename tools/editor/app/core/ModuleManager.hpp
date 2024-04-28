
#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H


#include "../include/instanceFactory.h"

class ModuleManager
{
public:
	ModuleManager(VxContext *_ctx, InstanceFactory* _factory);

	void OnImGuiRender();
	void menubar();
	void addModuleModal();


	VxContext *ctx;
	InstanceFactory* factory;

private:
	bool opened;

	std::shared_ptr<VxToolchain> latest_toolchain;

    std::shared_ptr<UIKit::Image> m_ListIcon;
    std::shared_ptr<UIKit::Image> m_RefreshIcon;
    std::shared_ptr<UIKit::Image> m_AddIcon;
    std::shared_ptr<UIKit::Image> m_TrashIcon;
	bool CollapseAll = false;
};

#endif // MODULEMANAGER_H