#include "ContentBrowser.hpp"
#include "../../vendor/imgui/imgui.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"

#include "icons.embed"

#include <iostream>
	
    
    ContentBrowserPanel::ContentBrowserPanel()
		: m_BaseDirectory("../../"), m_CurrentDirectory(m_BaseDirectory)
	{

	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		{
			uint32_t w, h;
			void* data = Walnut::Image::Decode(g_WindowMinimizeIcon, sizeof(g_WindowMinimizeIcon), w, h);
			m_DirectoryIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Walnut::Image::Decode(g_WindowMinimizeIcon, sizeof(g_WindowMinimizeIcon), w, h);
			m_FileIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
			free(data);
		}
		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();
			
			ImGui::PushID(filenameString.c_str());
			std::shared_ptr<Walnut::Image> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			//ImGui::ImageButton((ImTextureID)icon->, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });



			std::filesystem::path relativePath(path);
			static std::string itemPath = relativePath.c_str();
			Walnut::UI::DrawButtonImage(icon, 50, 50, 50, ImRect(ImVec4(1.0f,1.0f,1.0f,1.0f)));
            


			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		// TODO: status bar
		ImGui::End();
	}