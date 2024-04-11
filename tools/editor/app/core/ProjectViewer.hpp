
#include "../../../../lib/uikit/Source/editor/Application.h"
#include "../../../../lib/uikit/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../../../lib/uikit/Platform/GUI/editor/Image.h"
#include "../../../../lib/uikit/Platform/GUI/editor/UI/UI.h"
#include "../../../../lib/uikit/Assets/icons.h"

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
				std::string hostName = "Open###" + node->Name + "Open";
				if(ImGui::Button(hostName.c_str())){
					for(auto host : ctx->IO.hosts){
						if(node->Name == host->name){
							std::shared_ptr<HostInstance> instance = std::make_shared<HostInstance>(ctx, host, factory);
							factory->SpawnInstance(instance);	
						}
					}

				}
				ImGui::SameLine();
			}
			if(node->Type == "GPOS"){
				std::string gposName = "Open###" + node->Name + "Open";
				if(ImGui::Button(gposName.c_str())){
					for(auto gpos: ctx->IO.gpoSystems){
						if(node->Name == gpos->name){
							std::shared_ptr<GPOSInstance> instance = std::make_shared<GPOSInstance>(ctx, gpos, factory);
							factory->SpawnInstance(instance);	
						}
					}

				}
				ImGui::SameLine();
			}


			if(node->Type == "Package"){
				std::string gposName = "Open###" + node->Name + "Open";
				if(ImGui::Button(gposName.c_str())){
					for(auto package: ctx->IO.packages){
						if(node->Name == package->name){
							std::shared_ptr<PackageInstance> instance = std::make_shared<PackageInstance>(ctx, package);
							factory->SpawnInstance(instance);	
						}
					}

				}
				ImGui::SameLine();
			}

			if(node->Type == "Script"){
				std::string gposName = "Open###" + node->Name + "Open";
				if(ImGui::Button(gposName.c_str())){
					for(auto script: ctx->IO.scripts){
						if(node->Name == script->name){
							std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(ctx, script, factory);
							factory->SpawnInstance(instance);	
						}
					}

				}
				ImGui::SameLine();
			}
			

			if(node->Type == "Toolchain"){
				std::string toolchainName = "Open###" + node->Name + "Open";
				if(ImGui::Button(toolchainName.c_str())){
					for(auto toolchain : ctx->IO.toolchains){
						if(node->Name == toolchain->name){
							std::shared_ptr<ToolchainInstance> instance = std::make_shared<ToolchainInstance>(ctx, toolchain, factory);
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
			nodeHost.Name = host->name.c_str();
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
			nodeToolchain.Name = (char*)toolchain->name.c_str();
			nodeToolchain.Size = 1024;
			nodeToolchain.Type = "Toolchain";
			nodeToolchain.ChildCount = -1;
			nodeToolchain.ChildIdx = -1;
			Toolchains.push_back(nodeToolchain);
		}
		int toolchains_size = Toolchains.size();


		std::vector<MyTreeNode> Gpos = {};
		for(auto gpos : ctx->IO.gpoSystems){
			MyTreeNode nodeGPOS;
			nodeGPOS.Name = (char*)gpos->name.c_str();
			nodeGPOS.Size = 1024;
			nodeGPOS.Type = "GPOS";
			nodeGPOS.ChildCount = -1;
			nodeGPOS.ChildIdx = -1;
			Gpos.push_back(nodeGPOS);
		}
		int gpos_size = Gpos.size();


		std::vector<MyTreeNode> Packages = {};
		for(auto package : ctx->IO.packages){
			MyTreeNode nodePackage;
			nodePackage.Name = (char*)package->name.c_str();
			nodePackage.Size = 1024;
			nodePackage.Type = "Package";
			nodePackage.ChildCount = -1;
			nodePackage.ChildIdx = -1;
			Packages.push_back(nodePackage);
		}
		int package_size = Packages.size();


		std::vector<MyTreeNode> Scripts = {};
		for(auto script : ctx->IO.scripts){
			MyTreeNode nodePackage;
			nodePackage.Name = (char*)script->name.c_str();
			nodePackage.Size = 1024;
			nodePackage.Type = "Script";
			nodePackage.ChildCount = -1;
			nodePackage.ChildIdx = -1;
			Scripts.push_back(nodePackage);
		}
		int scripts_size = Scripts.size();


		std::vector<MyTreeNode> nodes = {
			{this->ctx->name.c_str(), "Project", 	-1, 1, 2}, // 0
			{"Components", "...", 					-1, 3, 3},				// 1
			{"Assets", "...", 						-1, 6, 3},						// 2

			{"Hosts", "...", 						-1, 9, 									hosts_size},						// 1
			{"Toolchains", "...", 					-1, 9 + hosts_size, 					toolchains_size},				// 1
			{"GPOS", "...", 						-1, 9 + toolchains_size + hosts_size,  	gpos_size},				// 1

			{"Packages", "...", 					-1, 9 + toolchains_size + hosts_size + gpos_size, 					package_size},						// 2
			{"Scripts", "...", 						-1, 9 + package_size + toolchains_size + hosts_size + gpos_size, 	scripts_size},						// 2
			{"Patchs", "..." 						-1, 1, 1},						// 2

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

		for(auto systems : Gpos){
			nodes.push_back({systems.Name, systems.Type, systems.Size, systems.ChildIdx, systems.ChildCount});
		}

		for(auto package : Packages){
			nodes.push_back({package.Name, package.Type, package.Size, package.ChildIdx, package.ChildCount});
		}

		for(auto script : Scripts){
			nodes.push_back({script.Name, script.Type, script.Size, script.ChildIdx, script.ChildCount});
		}

		this->nodeInfos = nodes;
	}

	VxContext *ctx;
	InstanceFactory* factory;

private:
	bool opened;


	std::vector<MyTreeNode> nodeInfos;

	std::shared_ptr<VxToolchain> latest_toolchain;

    std::shared_ptr<Walnut::Image> m_ListIcon;
    std::shared_ptr<Walnut::Image> m_RefreshIcon;
    std::shared_ptr<Walnut::Image> m_AddIcon;

	bool CollapseAll = false;
};
