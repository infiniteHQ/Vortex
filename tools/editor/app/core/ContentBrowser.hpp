
#include "../../../../lib/uikit/Source/editor/Application.h"
#include "../../../../lib/uikit/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../../../lib/uikit/Platform/GUI/editor/Image.h"
#include "../../../../lib/uikit/Platform/GUI/editor/UI/UI.h"
#include "../../../../lib/uikit/Assets/icons.h"

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
