#include "logs_utility.hpp"

#include <iostream>

static bool ErrorFilter = true;
static bool WarnFilter = true;
static bool FatalFilter = true;
static bool InfoFilter = true;

namespace VortexEditor
{
    LogsUtilityAppWindow::LogsUtilityAppWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<AppWindow>(name, name);
        m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
        m_AppWindow->SetInternalPaddingX(8.0f);
        m_AppWindow->SetInternalPaddingY(8.0f);
        std::shared_ptr<AppWindow> win = m_AppWindow;
        m_CmdInputValue = std::make_shared<std::string>("");

        m_AppWindow->m_Closable = true;
        m_AppWindow->SetCloseCallback([this]()
                                      { Cherry::DeleteAppWindow(m_AppWindow); });


        m_AppWindow->SetLeftMenubarCallback([]()
                                            {
                                                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));

                                                {
                                                    std::shared_ptr<ImageTextButtonSimple> btn = Application::Get().CreateComponent<ImageTextButtonSimple>("add_button", "Clear", Application::CookPath("resources/imgs/icons/misc/icon_add.png"));
                                                    btn->SetScale(0.85f);
                                                    btn->SetInternalMarginX(10.0f);
                                                    btn->SetLogoSize(15, 15);

                                                    if (btn->Render())
                                                    {
                                                    }
                                                }
                                                ImGui::PopStyleColor(); });

        m_AppWindow->SetLeftBottombarCallback([this]()
                                              {
                                                {
                                                    std::shared_ptr<ImageTextButtonSimple> btn = Application::Get().CreateComponent<ImageTextButtonSimple>("add_button", "Options.add", Application::CookPath("resources/imgs/icons/misc/icon_add.png"));
                                                    btn->SetScale(0.85f);
                                                    btn->SetInternalMarginX(10.0f);
                                                    btn->SetLogoSize(15, 15);

                                                    if (btn->Render())
                                                    {
                                                    }
                                                }
                                                  {
                                                      ImGui::SetNextItemWidth(300.0f);
                                                      auto input = Application::Get().CreateComponent<SimpleStringInput>("keyvaldouble_1", m_CmdInputValue, "Simple string value");
                                                      input->Render("input");
                                                  }
                                              });

        this->ctx = VortexMaker::GetCurrentContext();
    }

    std::shared_ptr<Cherry::AppWindow> &LogsUtilityAppWindow::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<LogsUtilityAppWindow> LogsUtilityAppWindow::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<LogsUtilityAppWindow>(new LogsUtilityAppWindow(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void LogsUtilityAppWindow::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void LogsUtilityAppWindow::Render()
    {
        // Configuration de la couleur de fond pour le "terminal"
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

        // Crée un Child scrollable
        if (ImGui::BeginChild("TerminalChild", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar))
        {
            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoBordersInBody;

            const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

            // Début du tableau
            if (ImGui::BeginTable("LogsTable", 4, flags))
            {
                // Configuration des colonnes
                ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 4.0f);
                ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
                ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);

                // Pas de header
                // ImGui::TableHeadersRow(); <-- Supprimé

                // Parcourir les logs
                for (auto log : VortexMaker::GetCurrentContext()->registered_logs)
                {
                    if ((log->m_level == spdlog::level::critical && !FatalFilter) ||
                        (log->m_level == spdlog::level::err && !ErrorFilter) ||
                        (log->m_level == spdlog::level::warn && !WarnFilter) ||
                        (log->m_level == spdlog::level::info && !InfoFilter))
                    {
                        continue;
                    }

                    ImGui::TableNextRow();

                    // Gestion du survol et de la copie
                    bool is_hovered = ImGui::IsItemHovered();
                    if (is_hovered)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Texte surbrillant
                    }

                    if (ImGui::IsMouseClicked(0) && is_hovered)
                    {
                        std::string content_to_copy = log->m_timestamp + " | " + log->m_filter + " | " + log->m_message;
                        ImGui::SetClipboardText(content_to_copy.c_str());
                    }

                    // Colonnes du tableau
                    for (int i = 0; i <= 3; i++)
                    {
                        ImGui::TableSetColumnIndex(i);
                        if (i == 0) // Niveau avec pastille
                        {
                            ImVec4 color;
                            switch (log->m_level)
                            {
                            case spdlog::level::critical:
                                color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                                break; // Rouge
                            case spdlog::level::err:
                                color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
                                break; // Rouge clair
                            case spdlog::level::warn:
                                color = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);
                                break; // Jaune
                            case spdlog::level::info:
                                color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
                                break; // Vert
                            default:
                                color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
                                break; // Gris par défaut
                            }

                            // Récupérer la position actuelle
                            ImVec2 pos = ImGui::GetCursorScreenPos();
                            float radius = ImGui::GetFontSize() * 0.2f;             // Taille du cercle
                            ImVec2 center = ImVec2(pos.x + radius, pos.y + radius); // Calculer le centre du cercle

                            // Dessiner le cercle coloré
                            ImGui::GetWindowDrawList()->AddCircleFilled(center, radius, ImGui::GetColorU32(color));

                            // Ajuster le curseur pour laisser de l'espace après le cercle
                            ImGui::Dummy(ImVec2(radius * 2.5f, radius * 2.0f)); // Espace réservé (largeur et hauteur)
                        }

                        else if (i == 1) // Message (coloré par niveau + texte wrappé)
                        {
                            ImVec4 message_color;
                            if (log->m_level == spdlog::level::critical)
                            {
                                message_color = ImVec4(0.5f, 0.0f, 0.0f, 1.0f); // Rouge foncé
                            }
                            else if (log->m_level == spdlog::level::err)
                            {
                                message_color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f); // Rouge clair
                            }
                            else if (log->m_level == spdlog::level::warn)
                            {
                                message_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Jaune
                            }
                            else
                            {
                                message_color = ImVec4(1.0f, 1.0f, 1.0f, 0.8f); // Blanc (par défaut)
                            }

                            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetColumnWidth());
                            ImGui::TextColored(message_color, log->m_message.c_str());
                            ImGui::PopTextWrapPos();
                        }
                        else if (i == 2) // Origin (aligné à droite)
                        {
                            float column_width = ImGui::GetColumnWidth();
                            float text_width = ImGui::CalcTextSize(log->m_filter.c_str()).x;
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + column_width - text_width);
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), log->m_filter.c_str());
                        }
                        else if (i == 3) // Timestamp (aligné à droite)
                        {
                            float column_width = ImGui::GetColumnWidth();
                            float text_width = ImGui::CalcTextSize(log->m_timestamp.c_str()).x;
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + column_width - text_width);
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), log->m_timestamp.c_str());
                        }
                    }

                    // Nettoyage des styles
                    if (is_hovered)
                    {
                        ImGui::PopStyleColor();
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor(); // Restaurer la couleur de fond
    }

    void LogsUtilityAppWindow::OnImGuiRender()
    {
    }

    void LogsUtilityAppWindow::menubar()
    {
        //
    }

}