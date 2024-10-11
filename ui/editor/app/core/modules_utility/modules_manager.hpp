#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../../../../main/include/modules/load.h"
#include "../../../../../main/include/modules/interface.h"

#ifndef MODULE_MANAGER_APP_WINDOW_H
#define MODULE_MANAGER_APP_WINDOW_H

using namespace Cherry;

namespace VortexEditor
{
	class ModuleManagerAppWindow
	{
	public:
		ModuleManagerAppWindow(const std::string &name);

		void menubar();
		void addModuleModal();
		void RefreshRender(const std::shared_ptr<ModuleManagerAppWindow> &instance);

		std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
		{
			return m_AppWindow;
		}

	private:
		bool opened;

		std::shared_ptr<Cherry::AppWindow> m_AppWindow;

		bool CollapseAll = false;
	};
} // namespace VortexEditor

#endif // MODULE_MANAGER_APP_WINDOW_H