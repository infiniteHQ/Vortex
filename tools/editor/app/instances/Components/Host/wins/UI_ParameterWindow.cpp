#include "../HostInstance.h"

void HostInstance::UI_ParametersWindow(){
        if (this->show_UI_ParametersWindow)
        {

             std::string label = this->name + " - Parameters Window###" +  this->name + "parameters";
            ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);
            if (ImGui::Begin(label.c_str()))
            {
                ImGui::TextUnformatted("Test");
                if (ImGui::CollapsingHeader("Properties"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Actions"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Scripts"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Patchs"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Console"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                if (ImGui::CollapsingHeader("Build"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
            }
            ImGui::End();
        }

        
    }
