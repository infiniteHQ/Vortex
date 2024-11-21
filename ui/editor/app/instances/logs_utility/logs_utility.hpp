
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
	class LogsUtilityAppWindow : public std::enable_shared_from_this<LogsUtilityAppWindow>
	{
	public:
		LogsUtilityAppWindow(const std::string &name);

		void OnImGuiRender();
		void menubar();

		std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
		static std::shared_ptr<LogsUtilityAppWindow> Create(const std::string &name);
		void SetupRenderCallback();
		void Render();

	private:
		VxContext *ctx;
		bool opened;
		std::shared_ptr<std::string> m_CmdInputValue;
		std::shared_ptr<AppWindow> m_AppWindow;
	};
}

#endif // LOGUTILITY_H
