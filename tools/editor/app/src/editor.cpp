#include "editor.h"

void ApplicationLayer::menubar(const std::shared_ptr<ApplicationLayer>& exampleLayer, UIKit::Application* app)
{
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("Exit"))
    {
      app->Close();
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Edit"))
  {
    if (ImGui::MenuItem("Project Settings", "Main configurations of this project"))
    {
      // Behavior
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Manage plugins", "Add, remove, edit plugins of this project"))
    {
      // Behavior
    }
    if (ImGui::MenuItem("Manage modules", "Project file manager", &exampleLayer->ShowModulesManager))
    {
      // Behavior
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Window"))
  {
    if (ImGui::MenuItem("Show bottom toolbar", "Get some usefull tools in a bottom bar."))
    {
      // Behavior
    }
    if (ImGui::MenuItem("Show simplified header", "Reduce the size of header"))
    {
      app->m_Specification.CustomTitlebar = !app->m_Specification.CustomTitlebar;
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Tools"))
  {
    if (ImGui::MenuItem("Content Browser", "Project file manager", &exampleLayer->ShowContentBrowser))
    {
      // Behavior
    }
    if (ImGui::MenuItem("Project Viewer", "Project component manager", &exampleLayer->ShowProjectViewer))
    {
      // Behavior
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Help"))
  {
    if (ImGui::MenuItem("News", "Get latest Vortex news"))
    {
      // Behavior
    }
    if (ImGui::MenuItem("Community", "Join a community of creators"))
    {
      // Behavior
    }
    if (ImGui::MenuItem("Tutorials", "Get bunch of tutorials"))
    {
      // Behavior
    }
    if (ImGui::MenuItem("Documentation", "See official documentation of Vortex Maker"))
    {
      // Behavior
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Exit"))
    {
      app->Close();
    }
    ImGui::EndMenu();
  }
}