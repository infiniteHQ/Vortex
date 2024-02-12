

#include <filesystem>
#include "../../../../../vortex.h"
#include "../../backend/Platform/GUI/editor/Image.h"

	class ProjectViewer
	{
	public:
		ProjectViewer(VxContext* _ctx);

		void OnImGuiRender();
		void menubar();

	private:
		VxContext* ctx;

		bool CollapseAll = false; 
	};
