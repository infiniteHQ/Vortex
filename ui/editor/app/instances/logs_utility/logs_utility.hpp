
#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#include "../../../../../lib/cherry/kit/v1/components/buttons/image_buttons.hpp"
#include "../../../../../lib/cherry/kit/v1/components/buttons/image_text_buttons.hpp"
#include "../../../../../lib/cherry/kit/v1/components/buttons/dropdown_buttons.hpp"

#ifndef VORTEX_EDITOR_LOGS_UTILITY_APPWINDOW_H
#define VORTEX_EDITOR_LOGS_UTILITY_APPWINDOW_H

using namespace Cherry;

#include <stack>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_set>

namespace VortexEditor
{
	class LogsUtilityAppWindow
	{
	public:
		LogsUtilityAppWindow(const std::string &name);

		void OnImGuiRender();
		void menubar();

		VxContext *ctx;
		void RefreshRender(const std::shared_ptr<LogsUtilityAppWindow> &instance);

		std::shared_ptr<AppWindow> &GetAppWindow()
		{
			return m_AppWindow;
		}

	private:
		bool opened;
		std::shared_ptr<VxToolchain> latest_toolchain;
		std::shared_ptr<AppWindow> m_AppWindow;

		std::shared_ptr<Cherry::Image> m_JournalIcon;
		std::shared_ptr<Cherry::Image> m_ListIcon;
		std::shared_ptr<Cherry::Image> m_RefreshIcon;
		std::shared_ptr<Cherry::Image> m_AddIcon;
	};
}

#endif // LOGUTILITY_H
