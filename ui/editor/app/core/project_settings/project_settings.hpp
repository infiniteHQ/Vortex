#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#include "../utils.hpp"

#ifndef PROJECT_SETTINGS_APPWINDOW_H
#define PROJECT_SETTINGS_APPWINDOW_H

struct ProjectSettingsSave
{
	char name[128] = "unknow";
	char author[128] = "unknow";
	char version[128] = "unknow";
	char description[128] = "unknow";
	char type[128] = "unknow";
	bool include_system_templates;
};

using namespace Cherry;

namespace VortexEditor
{
	struct ProjectSettingsChild
	{
		std::string m_Parent;
		std::string m_ChildName;
		std::function<void()> m_Callback;

		ProjectSettingsChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child) : m_Parent(parent_name),
																																		  m_ChildName(child_name),
																																		  m_Callback(child)
		{
			//
		}
	};

	class ProjectSettingsAppWindow
	{
	public:
		ProjectSettingsAppWindow(const std::string &name);

        void AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);
        void RefreshRender(const std::shared_ptr<ProjectSettingsAppWindow>& instance);

		void menubar();
		void addModuleModal();

		void Refresh();
		void Update();       
		
		std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
        {
            return m_AppWindow;
        }


		/**
		 * @brief Menu items
		 */
		void mainButtonsMenuItem();
		void filterMenuItem();
		void createMenuItem();
		void searchMenuItem();

		std::shared_ptr<ProjectSettingsSave> current_save;

	private:
		bool opened;
        std::vector<ProjectSettingsChild> m_Childs;
        std::shared_ptr<Cherry::AppWindow> m_AppWindow;

        std::string m_SelectedChildName;
	};
}
#endif // PROJECTSETTINGSAppWindow_H