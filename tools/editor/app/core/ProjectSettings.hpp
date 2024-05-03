
#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H


#include "../include/instanceFactory.h"

struct ProjectSettingsSave
{
    char name[128] = "unknow";
    char author[128] = "unknow";
    char version[128] = "unknow";
    char description[128] = "unknow";
    char type[128] = "unknow";
	bool include_system_templates;
};


class ProjectSettings
{
public:
	ProjectSettings(VxContext *_ctx, InstanceFactory* _factory);

	void OnImGuiRender();
	void menubar();
	void addModuleModal();

	void Refresh();
	void Update();

	/**
	 * @brief Menu items
	*/
	void mainButtonsMenuItem();
	void filterMenuItem();
	void createMenuItem();
	void searchMenuItem();

	VxContext *ctx;
	InstanceFactory* factory;

	std::shared_ptr<ProjectSettingsSave> current_save;

private:
	bool opened;

	std::shared_ptr<VxToolchain> latest_toolchain;

    std::shared_ptr<UIKit::Image> m_ListIcon;
    std::shared_ptr<UIKit::Image> m_RefreshIcon;
    std::shared_ptr<UIKit::Image> m_AddIcon;
    std::shared_ptr<UIKit::Image> m_TrashIcon;
	bool CollapseAll = false;
};

#endif // PROJECTSETTINGS_H