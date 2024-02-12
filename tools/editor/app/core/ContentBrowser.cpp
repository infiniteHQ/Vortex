#include "ContentBrowser.hpp"
#include "../../vendor/imgui/imgui.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"


#include <iostream>
	
    
    ContentBrowserPanel::ContentBrowserPanel()
		: m_BaseDirectory("../../"), m_CurrentDirectory(m_BaseDirectory)
	{

		{
			uint32_t w, h;
			void* data = Walnut::Image::Decode(icons::i_FolderIcon, icons::i_FolderIcon_size, w, h);
			this->m_DirectoryIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
		 	this->m_DirectoryIconTexture = m_DirectoryIcon->GetImGuiTextureID();
			free(data);
		}
		{
			uint32_t w, h;
		  	void* data = Walnut::Image::Decode(icons::i_file, icons::i_file_size, w, h);
			this->m_FileIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
			this->m_FileIconTexture = m_FileIcon->GetImGuiTextureID();
			free(data);
		}
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 94.0f;
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
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));


			static ImTextureID texture;
			if(directoryEntry.is_directory()){
				texture = this->m_DirectoryIconTexture;
			 }
			 else{ 
				texture = this->m_FileIconTexture;
			 }

			ImGui::ImageButton(texture, { thumbnailSize, thumbnailSize }, { 1, 0 }, { 0, 1 });



			std::filesystem::path relativePath(path);
			static std::string itemPath = relativePath.c_str();
            


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