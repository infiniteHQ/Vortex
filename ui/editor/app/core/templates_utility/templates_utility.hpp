#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#include "../utils.hpp"

#ifndef TEMPLATES_UTILITY_APPWINDOW_H
#define TEMPLATES_UTILITY_APPWINDOW_H

using namespace Cherry;

namespace VortexEditor
{
    struct TemplatesUtilityAppWindowChild
    {
        std::string m_Parent;
        std::string m_ChildName;
        std::function<void()> m_Callback;

        TemplatesUtilityAppWindowChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child) : m_Parent(parent_name),
                                                                                                                                m_ChildName(child_name),
                                                                                                                                m_Callback(child)
        {
        }
    };

    class TemplatesUtilityAppWindow
    {
    public:
        TemplatesUtilityAppWindow(const std::string &name);

        void AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);
        void RefreshRender(const std::shared_ptr<TemplatesUtilityAppWindow>& instance);

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
        {
            return m_AppWindow;
        }

        std::vector<TemplatesUtilityAppWindowChild> m_Childs;

        std::string m_SelectedChildName;

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };
}

#endif // TEMPLATES_UTILITY_APPWINDOW_H