

#include <filesystem>
#include "../../../../../vortex.h"
#include "../../../../../vortex_internals.h"
#include "../../backend/Platform/GUI/editor/Image.h"

#include "../instances/Components/Host/HostInstance.h"
#include "../instances/Components/Toolchain/ToolchainInstance.h"
#include "../src/instanceFactory.h"

// Simple storage to output a dummy file-system.
struct MyTreeNode
{
	std::string Name;
	const char *Type;
	int Size;
	int ChildIdx;
	int ChildCount;
	static void DisplayNode(const MyTreeNode *node, std::vector<MyTreeNode> all_nodes, InstanceFactory* factory, VxContext *ctx)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		const bool is_folder = (node->ChildCount > 0);
		if (is_folder)
		{
			bool open = ImGui::TreeNodeEx(node->Name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
			ImGui::TableNextColumn();
			ImGui::TextDisabled("--");
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(node->Type);
			if (open)
			{
				for (int child_n = 0; child_n < node->ChildCount; child_n++)
					DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes, factory, ctx);

				ImGui::TreePop();
			}
		}
		else
		{

			if(node->Type == "Host"){
				if(ImGui::Button("Open")){
					for(auto host : ctx->IO.hosts){
						if(node->Name == host.name){
							std::shared_ptr<HostInstance> instance = std::make_shared<HostInstance>(ctx, &host);
							factory->SpawnInstance(instance);	
						}
					}

				}
				ImGui::SameLine();
			}
			if(node->Type == "Toolchain"){
				if(ImGui::Button("Open")){
					for(auto toolchain : ctx->IO.toolchains){
						if(node->Name == toolchain.name){
							std::shared_ptr<ToolchainInstance> instance = std::make_shared<ToolchainInstance>(ctx, &toolchain);
							factory->SpawnInstance(instance);	
						}
					}

				}
				ImGui::SameLine();
			}
			ImGui::TreeNodeEx(node->Name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
			
			ImGui::TableNextColumn();
			ImGui::Text("%d", node->Size);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(node->Type);
		}
	}
};

class ProjectViewer
{
public:
	ProjectViewer(VxContext *_ctx, InstanceFactory* _factory);

	void OnImGuiRender();
	void menubar();

	void refreshContents()
	{
		// Tout faire ici dans le vecteur
		std::vector<MyTreeNode> _nodeInfos = {
			{"Root", "Folder", -1, 1, 4},									 // 0
			{"Music", "Folder", -1, 4, 2},									 // 1
			{"Copi", "Folder", -1, 10, 3},									 // 1
			{"Textures", "Folder", -1, 6, 3},								 // 2
			{"desktop.ini", "System file", 1024, -1, -1},					 // 3
			{"File1_a.wav", "Audio file", 123000, -1, -1},					 // 4
			{"File1_b.wav", "Audio file", 456000, -1, -1},					 // 5
			{"Image001.png", "Image file", 203128, -1, -1},					 // 6
			{"Copy of Image001.png", "Image file", 203256, -1, -1},			 // 7
			{"Copy of Image001 (Final2).png", "Image file", 203512, -1, -1}, // 8
			{"Copyqs", "Image file", 203512, -1, -1},						 // 8
			{"Cofg", "Image file", 203512, -1, -1},							 // 8
			{"Copgng", "Image file", 203512, -1, -1},						 // 8

			// Ajoutez le reste des données ici...
		};


		std::vector<MyTreeNode> Hosts = {};
		for(auto host : ctx->IO.hosts){
			MyTreeNode nodeHost;
			nodeHost.Name = host.name.c_str();
			nodeHost.Type = "Host";
			nodeHost.Size = 1024;
			nodeHost.ChildCount = -1;
			nodeHost.ChildIdx = -1;
			Hosts.push_back(nodeHost);
		}
		int hosts_size = Hosts.size();



		std::vector<MyTreeNode> Toolchains = {};
		for(auto toolchain : ctx->IO.toolchains){
			MyTreeNode nodeToolchain;
			nodeToolchain.Name = (char*)toolchain.name.c_str();
			nodeToolchain.Size = 1024;
			nodeToolchain.Type = "Toolchain";
			nodeToolchain.ChildCount = -1;
			nodeToolchain.ChildIdx = -1;
			Toolchains.push_back(nodeToolchain);
		}
		int toolchains_size = Toolchains.size();

		std::cout << hosts_size << std::endl;
		std::cout << toolchains_size << std::endl;


		std::vector<MyTreeNode> nodes = {
			{this->ctx->name.c_str(), "Project", 	-1, 1, 2}, // 0
			{"Components", "...", 					-1, 3, 3},				// 1
			{"Assets", "...", 						-1, 6, 3},						// 2

			{"Systems", "Group", 					-1, 1, 1},					// 1
			{"Hosts", "Group", 						9, 9, 				hosts_size},						// 1
			{"Toolchains", "Group", 				9 + hosts_size, 9 + hosts_size, 	toolchains_size},				// 1

			{"Packages", "Group" 					-1, 1, 1},						// 2
			{"Scripts", "Group" 					-1, 1, 1},						// 2
			{"Patchs", "Group" 						-1, 1, 1},						// 2

			// Host 1
			// Toolchain 1

			// Ajoutez le reste des données ici...
		};

		for(auto hostnode : Hosts){
			nodes.push_back({hostnode.Name, hostnode.Type, hostnode.Size, hostnode.ChildIdx, hostnode.ChildCount});
		}


		for(auto toolchainnode : Toolchains){
			nodes.push_back({toolchainnode.Name, toolchainnode.Type, toolchainnode.Size, toolchainnode.ChildIdx, toolchainnode.ChildCount});
		}

		this->nodeInfos = nodes;
	}

	VxContext *ctx;
	InstanceFactory* factory;

private:
	bool opened;


	std::vector<MyTreeNode> nodeInfos;

    std::shared_ptr<Walnut::Image> m_ListIcon;

	bool CollapseAll = false;
};
