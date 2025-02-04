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

	class ProjectSettingsAppWindow : public std::enable_shared_from_this<ProjectSettingsAppWindow>
	{
	public:
		ProjectSettingsAppWindow(const std::string &name);

		void AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child);
		void RemoveChild(const std::string &child_name);
		std::function<void()> GetChild(const std::string &child_name);
		void RefreshRender(const std::shared_ptr<ProjectSettingsAppWindow> &instance);

		std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
		static std::shared_ptr<ProjectSettingsAppWindow> Create(const std::string &name);
		void SetupRenderCallback();
		void Render();

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

		// Menubar
		void RenderRightMenubar(); 

		// Pannels
		void RenderProjectInformations();

		// Update detection
		bool IsProjectInformationsUpdated();

		// Update / Refresh
		void UpdateProjectInformations();
		void RefreshProjectInformations();

		std::shared_ptr<ProjectSettingsSave> current_save;

		std::string v_ProjectNameInitial;
		std::string v_ProjectDescriptionInitial;
		std::string v_ProjectAuthorInitial;
		std::string v_ProjectVersionInitial;
		std::shared_ptr<std::string> v_ProjectName;
		std::shared_ptr<std::string> v_ProjectDescription;
		std::shared_ptr<std::string> v_ProjectAuthor;
		std::shared_ptr<std::string> v_ProjectVersion;

		std::shared_ptr<DoubleKeyValString> cp_ProjectNameDoubleString;
		std::shared_ptr<DoubleKeyValString> cp_ProjectDescriptionDoubleString;
		std::shared_ptr<DoubleKeyValString> cp_AuthorsDoubleString;
		std::shared_ptr<DoubleKeyValString> cp_VersionDoubleString;

	private:
		bool opened;
		std::vector<ProjectSettingsChild> m_Childs;
		std::shared_ptr<Cherry::AppWindow> m_AppWindow;

		std::string m_SelectedChildName;
	};
}
#endif // PROJECTSETTINGSAppWindow_H