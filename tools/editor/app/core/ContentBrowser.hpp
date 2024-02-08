

#include <filesystem>
#include "../../backend/Platform/GUI/editor/Image.h"

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
	};
