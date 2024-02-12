

#include <filesystem>
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Assets/icons.h"

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		
		std::shared_ptr<Walnut::Image> m_FileIcon;
		std::shared_ptr<Walnut::Image> m_DirectoryIcon;


		 ImTextureID m_DirectoryIconTexture;
		 ImTextureID m_FileIconTexture;
	};
