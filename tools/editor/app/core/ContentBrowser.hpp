
#ifndef CONTENTBROWSER_H
#define CONTENTBROWSER_H

#include "../include/instanceFactory.h"

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		
		std::shared_ptr<UIKit::Image> m_FileIcon;
		std::shared_ptr<UIKit::Image> m_DirectoryIcon;


		 ImTextureID m_DirectoryIconTexture;
		 ImTextureID m_FileIconTexture;
	};

#endif // CONTENTBROWSER_H