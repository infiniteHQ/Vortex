#include "../../include/vortex.h"
#include "./src/module.h"
#include "./src/instances/packageInstance/PackageRenderInstance.h"



/**
 * @brief Find packages from a registered package interface list
 * @param ["args"] "list" The target package we wan't to open in a interface. (type: std::vector<PackageInterface>)
*/
void FindPackages(std::shared_ptr<hArgs> args){
    VxContext &ctx = *CVortexMaker;

    if(args == nullptr)
    return;

    std::string path = args->get<std::string>("path", "unknow");


    std::vector<std::shared_ptr<Package>> packages_list;
    packages_list = args->get<std::vector<std::shared_ptr<Package>>>("packages", packages_list);

    std::vector<std::shared_ptr<PackageInterface>> interfaces_list;
    interfaces_list = args->get<std::vector<std::shared_ptr<PackageInterface>>>("list", interfaces_list);


  // Register all finded local packages
  for (const auto &file : VortexMaker::SearchFiles(CPackagesModule->m_interface->m_datapath, "/package.config"))
  {
    try
    {
      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      Package newPackage;

      // Get packages infos

      for (auto registeredPackage : interfaces_list)
      {
        if (registeredPackage->emplacement == "local")
        {
          if (registeredPackage->label == filecontent["package"]["label"].get<std::string>())
          {
            bool already_registered = false;
            for (auto registered_package : packages_list)
            {
              if (filecontent["package"]["label"].get<std::string>() == registered_package->label)
              {
                already_registered = true;
              }
            }

            if (!already_registered)
            {
              std::shared_ptr<Package> newPackage = std::make_shared<Package>();

              newPackage->path = file;
              size_t position = newPackage->path.find("/package.config");
              newPackage->configFilePath = file; 

              if (position != std::string::npos)
              {
                newPackage->path.erase(position, 15);
              }

              newPackage->label = filecontent["package"]["label"].get<std::string>();
              newPackage->fileName = filecontent["package"]["filename"].get<std::string>();
              newPackage->description = filecontent["package"]["description"].get<std::string>();
              newPackage->name = filecontent["package"]["name"].get<std::string>();
              newPackage->compressed = filecontent["package"]["compressed"].get<std::string>();
              newPackage->priority = filecontent["package"]["priority"].get<int>();
              newPackage->compilation.useCompilationOptimization = filecontent["parameters"]["useCompilationOptimization"].get<bool>();
              newPackage->compilation.exclusiveCustomCompilationProcess = filecontent["parameters"]["useOnlyCustomCompilationProcess"].get<std::string>();
              newPackage->compilation.exclusiveCustomConfigProcess = filecontent["parameters"]["useOnlyCustomConfigurationProcess"].get<std::string>();
              newPackage->compilation.exclusiveCustomInstallationProcess = filecontent["parameters"]["useOnlyCustomInstallationProcess"].get<std::string>();

              for (auto arch : filecontent["package"]["target_archs"])
              {
                newPackage->archs.push_back(arch);
              }

              for (auto configSuffixes : filecontent["compilation"]["configurationSuffixes"])
              {
                for (auto it = configSuffixes.begin(); it != configSuffixes.end(); ++it)
                {
                  newPackage->compilation.configurationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto action : filecontent["actions"])
              {
                if (action["type"].get<std::string>() == "command")
                {
                  std::shared_ptr<PackageAction> newAction = std::make_shared<PackageAction>();
                  newAction->type = action["type"].get<std::string>();
                  newAction->priority = action["priority"].get<int>();
                  newAction->executionSequence = action["sequence"].get<std::string>();
                  newAction->command = action["command"].get<std::string>(); // TODO : if type == command
                  newPackage->actions.push_back(newAction);
                }
              }

              std::sort(newPackage->actions.begin(), newPackage->actions.end(), [](const std::shared_ptr<PackageAction> &a, const std::shared_ptr<PackageAction> &b)
                        { return a->priority < b->priority; });

              for (auto installSuffixes : filecontent["compilation"]["installationSuffixes"])
              {
                for (auto it = installSuffixes.begin(); it != installSuffixes.end(); ++it)
                {
                  newPackage->compilation.installationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto compileSuffixes : filecontent["compilation"]["compilationSuffixes"])
              {
                for (auto it = compileSuffixes.begin(); it != compileSuffixes.end(); ++it)
                {
                  newPackage->compilation.compilationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto configParameters : filecontent["compilation"]["configurationPrefixes"])
              {
                for (auto it = configParameters.begin(); it != configParameters.end(); ++it)
                {
                  newPackage->compilation.configurationPrefixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto installParameters : filecontent["compilation"]["installationPrefixes"])
              {
                for (auto it = installParameters.begin(); it != installParameters.end(); ++it)
                {
                  newPackage->compilation.installationPrefixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto compileParameters : filecontent["compilation"]["compilationPrefixes"])
              {
                for (auto it = compileParameters.begin(); it != compileParameters.end(); ++it)
                {
                  newPackage->compilation.compilationPrefixes.emplace_back(it.key(), it.value());
                }
              }

              std::shared_ptr<Task> newTask = std::make_shared<Task>();
              newTask->tasktype = "unknow";
              newPackage->latestTask = newTask;

              packages_list.push_back(newPackage);
              registeredPackage->resolved = true;
            }
          }
        }
        
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }

  std::sort(packages_list.begin(), packages_list.end(), [](const std::shared_ptr<Package> &a, const std::shared_ptr<Package> &b)
            { return a->priority < b->priority; });


      for (auto registeredPackage : interfaces_list)
      {


        if (registeredPackage->emplacement == "global")
        {
          // Recupérer les packages du ctx.

          for(auto package : CPackagesModule->m_packages){
            if(package->label == registeredPackage->label){

            bool already_registered = false;
            for (auto registered_package : packages_list)
            {
              if (registeredPackage->label == registered_package->label)
              {
                already_registered = true;
              }
            }

            if (!already_registered)
            {
              std::shared_ptr<Package> newPackage = package;

              packages_list.push_back(newPackage);
              registeredPackage->resolved = true;
            }

            }
          }

        }
      

      }

   args->add<std::vector<std::shared_ptr<Package>>>("packages", packages_list);

}


/**
 * @brief 
 * @param
*/
void RegisterPackage(std::shared_ptr<hArgs> args){

}


class PackagesModule : public ModuleInterface
{
public:
    /**
     * @brief When the module is turning on, this function is executed. In this, all components are declared, all contents are loaded
     */
    void execute() override
    {
        // Create the context ptr of this module
        PackageModule::CreatePackageContext();

        // Get the interface ptr (for GUI launcher, from others modules)
        CPackagesModule->m_interface = ModuleInterface::GetEditorModuleByName(this->m_name);

        // Add main args
        this->AddArg<const char *>("chainsModule.name", "PackagesModule");

        // Add logo
        this->AddLogo(icons::_i, icons::_i_size);

        // Adding functions
        this->AddFunction(Register, "RegisterPackages");

        // Adding events
        this->AddInputEvent(PackageModule::LaunchPackageInterface, "LaunchPackageInterface");
        this->AddInputEvent(FindPackages,           "FindPackages");
        this->AddInputEvent(RegisterPackage,        "RegisterPackage");

        // Execute main functions 
        this->ExecFunction("RegisterPackages");
    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
     */
    void render() override
    {

        ImGui::Begin("Packages module 1.0.0");
        for (auto package : CPackagesModule->m_packages)
        {
            ImGui::Text(package->name.c_str());
            ImGui::SameLine();
            std::string label = "Open ###" + package->name;
            if (ImGui::Button(label.c_str()))
            {
                VxContext *ctx = VortexMaker::GetCurrentContext();
                std::shared_ptr<PackageRenderInstance> instance = std::make_shared<PackageRenderInstance>(ctx, package);
                // factory->SpawnInstance(instance);
                instance->name = package->name;
                this->AddModuleRenderInstance(instance);
            }
        }
        ImGui::End();
        // "Launcher" of regitered toolchains
    }
    /*
        void openToolchain(){ // Make a function
            // Get instance, open it and create a render instance

            // Ctx (from module), Toolchain
            ToolchainRenderInstance instance(VortexMaker::GetCurrentContext(), nullptr);
            this->AddModuleRenderInstance(instance);
        }*/
};

extern "C" ModuleInterface *create_em()
{
    return new PackagesModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
