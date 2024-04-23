#include "module.h"
#include "../src/instances/packageInstance/PackageRenderInstance.h"



void PackageModule::CreatePackageContext()
{
    PackagesModuleCTX *ctx = VX_NEW(PackagesModuleCTX);
    CPackagesModule = ctx;
}


/**
 * @brief Finds and registers packages based on configuration files.
 *
 * @param args Pointer to the arguments containing necessary data.
 */
void PackageModule::FindPackages(const std::shared_ptr<hArgs>& args)
{
    VxContext& ctx = *CVortexMaker;

    if (args == nullptr)
    {
        return;
    }

    // Retrieve necessary parameters from the arguments
    std::string path = args->get<std::string>("path", "unknown");

    std::vector<std::shared_ptr<Package>> packages_list;
    packages_list = args->get<std::vector<std::shared_ptr<Package>>>("packages", packages_list);

    std::vector<std::shared_ptr<PackageInterface>> interfaces_list;
    interfaces_list = args->get<std::vector<std::shared_ptr<PackageInterface>>>("list", interfaces_list);

    // Register all found local packages
    for (const auto& file : VortexMaker::SearchFiles(CPackagesModule->m_interface->m_datapath, "/package.config"))
    {
        try
        {
            nlohmann::json filecontent = VortexMaker::DumpJSON(file);
            Package newPackage;

            // Check against registered package interfaces
            for (auto registeredPackage : interfaces_list)
            {
                if (registeredPackage->emplacement == "local" && registeredPackage->label == filecontent["package"]["label"].get<std::string>())
                {
                    bool already_registered = false;
                    for (auto registered_package : packages_list)
                    {
                        if (filecontent["package"]["label"].get<std::string>() == registered_package->label)
                        {
                            already_registered = true;
                            break;
                        }
                    }

                    if (!already_registered)
                    {
                        std::shared_ptr<Package> newPackage = std::make_shared<Package>();

                        // Parse package information from the configuration file
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

                        // Parse target architectures
                        for (auto arch : filecontent["package"]["target_archs"])
                        {
                            newPackage->archs.push_back(arch);
                        }

                        // Parse compilation configuration suffixes
                        for (auto configSuffixes : filecontent["compilation"]["configurationSuffixes"])
                        {
                            for (auto it = configSuffixes.begin(); it != configSuffixes.end(); ++it)
                            {
                                newPackage->compilation.configurationSuffixes.emplace_back(it.key(), it.value());
                            }
                        }

                        // Parse package actions
                        for (auto action : filecontent["actions"])
                        {
                            if (action["type"].get<std::string>() == "command")
                            {
                                std::shared_ptr<PackageAction> newAction = std::make_shared<PackageAction>();
                                newAction->type = action["type"].get<std::string>();
                                newAction->priority = action["priority"].get<int>();
                                newAction->executionSequence = action["sequence"].get<std::string>();
                                newAction->command = action["command"].get<std::string>();
                                newPackage->actions.push_back(newAction);
                            }
                        }

                        // Sort actions by priority
                        std::sort(newPackage->actions.begin(), newPackage->actions.end(), [](const std::shared_ptr<PackageAction>& a, const std::shared_ptr<PackageAction>& b)
                                  { return a->priority < b->priority; });

                        // Parse installation configuration suffixes
                        for (auto installSuffixes : filecontent["compilation"]["installationSuffixes"])
                        {
                            for (auto it = installSuffixes.begin(); it != installSuffixes.end(); ++it)
                            {
                                newPackage->compilation.installationSuffixes.emplace_back(it.key(), it.value());
                            }
                        }

                        // Parse compilation configuration prefixes
                        for (auto compileSuffixes : filecontent["compilation"]["compilationSuffixes"])
                        {
                            for (auto it = compileSuffixes.begin(); it != compileSuffixes.end(); ++it)
                            {
                                newPackage->compilation.compilationSuffixes.emplace_back(it.key(), it.value());
                            }
                        }

                        // Parse compilation configuration prefixes
                        for (auto configParameters : filecontent["compilation"]["configurationPrefixes"])
                        {
                            for (auto it = configParameters.begin(); it != configParameters.end(); ++it)
                            {
                                newPackage->compilation.configurationPrefixes.emplace_back(it.key(), it.value());
                            }
                        }

                        // Parse installation configuration prefixes
                        for (auto installParameters : filecontent["compilation"]["installationPrefixes"])
                        {
                            for (auto it = installParameters.begin(); it != installParameters.end(); ++it)
                            {
                                newPackage->compilation.installationPrefixes.emplace_back(it.key(), it.value());
                            }
                        }

                        // Parse compilation configuration prefixes
                        for (auto compileParameters : filecontent["compilation"]["compilationPrefixes"])
                        {
                            for (auto it = compileParameters.begin(); it != compileParameters.end(); ++it)
                            {
                                newPackage->compilation.compilationPrefixes.emplace_back(it.key(), it.value());
                            }
                        }

                        // Create a default task
                        std::shared_ptr<Task> newTask = std::make_shared<Task>();
                        newTask->tasktype = "unknown";
                        newPackage->latestTask = newTask;

                        // Add the new package to the list
                        packages_list.push_back(newPackage);
                        registeredPackage->resolved = true;
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error : " << e.what() << std::endl;
        }
    }

    // Sort the packages by priority
    std::sort(packages_list.begin(), packages_list.end(), [](const std::shared_ptr<Package>& a, const std::shared_ptr<Package>& b)
              { return a->priority < b->priority; });

    // Add global packages to the list
    for (auto registeredPackage : interfaces_list)
    {
        if (registeredPackage->emplacement == "global")
        {
            for (auto package : CPackagesModule->m_packages)
            {
                if (package->label == registeredPackage->label)
                {
                    bool already_registered = false;
                    for (auto registered_package : packages_list)
                    {
                        if (registeredPackage->label == registered_package->label)
                        {
                            already_registered = true;
                            break;
                        }
                    }

                    if (!already_registered)
                    {
                        std::shared_ptr<Package> newPackage = package;

                        // Add the new package to the list
                        packages_list.push_back(newPackage);
                        registeredPackage->resolved = true;
                    }
                }
            }
        }
    }

    // Update the arguments with the new list of packages
    args->add<std::vector<std::shared_ptr<Package>>>("packages", packages_list);
}


/**
 * @brief Launches the package interface.
 *
 * @param args Pointer to the arguments containing the package to launch.
 */
void PackageModule::LaunchPackageInterface(const std::shared_ptr<hArgs>& args)
{
    if (args != nullptr)
    {
        // Retrieve the package from arguments
        std::shared_ptr<Package> package = args->get<std::shared_ptr<Package>>("package", nullptr);
        
        if (package != nullptr)
        {
            // Create a render instance for the package
            VxContext* ctx = VortexMaker::GetCurrentContext();
            std::shared_ptr<PackageRenderInstance> instance = std::make_shared<PackageRenderInstance>(ctx, package);
            instance->name = package->name;

            // Add the render instance to the packages module interface
            if (CPackagesModule != nullptr && CPackagesModule->m_interface != nullptr)
            {
                CPackagesModule->m_interface->AddModuleRenderInstance(instance);
                VortexMaker::LogWarn("PackageModule", "Added package render instance to interface.");
            }
            else
            {
                VortexMaker::LogError("PackageModule", "Packages module or interface is null.");
            }
        }
        else
        {
            VortexMaker::LogError("PackageModule", "Package pointer is null.");
        }
    }
    else
    {
        VortexMaker::LogError("PackageModule", "Arguments pointer is null.");
    }
}


static void fillArray(const std::vector<std::pair<char[128], char[128]>> &pairs, nlohmann::json &jsonArray)
{
    for (const auto &pair : pairs)
    {
        nlohmann::json parameter;
        parameter[pair.first] = pair.second;
        jsonArray.push_back(parameter);
    }
}

void Package::PushSave(std::shared_ptr<PackageSave> save)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();
    nlohmann::json packageData;

    std::string tempPath;
    size_t lastSlashPos = path.find_last_of('/');
    
    if (lastSlashPos != std::string::npos) {
        tempPath = this->path.substr(0, lastSlashPos);
    } 

    packageData["package"]["name"] = save->name;
    packageData["package"]["author"] = save->author;
    packageData["package"]["description"] = save->description;
    packageData["package"]["label"] = save->label;

    packageData["package"]["priority"] = save->priority;
    packageData["package"]["compressed"] = save->compressed;
    packageData["package"]["filename"] = save->filename;

    packageData["parameters"]["useOnlyCustomConfigurationProcess"] = save->configurationExclusiveCommand;
    packageData["parameters"]["useOnlyCustomCompilationProcess"] = save->compilationExclusiveCommand;
    packageData["parameters"]["useOnlyCustomInstallationProcess"] = save->installationExclusiveCommand;
    packageData["parameters"]["useCompilationOptimization"] = save->useCompilationOptimization;

    packageData["actions"] = nlohmann::json::array();
    
    for(auto action : save->actions){
        nlohmann::json actionJson;
        actionJson["type"] = action.type;
        actionJson["priority"] = action.priority;
        actionJson["sequence"] = action.sequence;
        actionJson["command"] = action.command;
        packageData["actions"].push_back(actionJson);
    }   

    // Initialize arrays
    nlohmann::json objJson;
    objJson["configurationPrefixes"] = nlohmann::json::array();
    objJson["configurationSuffixes"] = nlohmann::json::array();
    objJson["compilationPrefixes"] = nlohmann::json::array();
    objJson["compilationSuffixes"] = nlohmann::json::array();
    objJson["installationPrefixes"] = nlohmann::json::array();
    objJson["installationSuffixes"] = nlohmann::json::array();

    // Fill arrays with pairs
    fillArray(save->configurationPrefixes, objJson["configurationPrefixes"]);
    fillArray(save->configurationSuffixes, objJson["configurationSuffixes"]);
    fillArray(save->compilationPrefixes, objJson["compilationPrefixes"]);
    fillArray(save->compilationSuffixes, objJson["compilationSuffixes"]);
    fillArray(save->installationPrefixes, objJson["installationPrefixes"]);
    fillArray(save->installationSuffixes, objJson["installationSuffixes"]);

    packageData["compilation"] = objJson;

    std::ofstream file(this->configFilePath);
    if (file.is_open())
    {
        file << std::setw(4) << packageData << std::endl;
          VortexMaker::LogInfo("Core", "Object saved to " + this->configFilePath);
        file.close();
    }
    else
    {
          VortexMaker::LogError("Core", "Unable to open file " + this->configFilePath + " for writing!");
    }


    // change name of folder 
    std::string cmd = "mv " + this->path + " " + tempPath + "/" + save->name;
    system(cmd.c_str());
}

void Package::ExecuteActions(std::string sequence, std::shared_ptr<Package> package)
{
  for (auto action : package->actions)
  {
    if (sequence == action->executionSequence)
    {
      if (action->type == "command")
      {

        std::string cmd = "";
        cmd += "cd " + package->distPath + "/build && " + action->command;

        std::string label = "action-" + action->type + "-" + action->executionSequence + "-";
        label += action->priority;

        package->SetDiagCode(label, system(cmd.c_str()));
      }
    }
  }
}

void Package::Refresh()
{
  nlohmann::json filecontent = VortexMaker::DumpJSON(this->configFilePath);

  this->label = filecontent["package"]["label"].get<std::string>();
  this->fileName = filecontent["package"]["filename"].get<std::string>();
  this->description = filecontent["package"]["description"].get<std::string>();
  this->name = filecontent["package"]["name"].get<std::string>();
  this->compressed = filecontent["package"]["compressed"].get<std::string>();
  this->priority = filecontent["package"]["priority"].get<int>();
  this->useChroot = false;
  try
  {
    this->useChroot = filecontent["parameters"]["useChroot"].get<bool>();
  }
  catch (std::exception e)
  {
    e.what();
  };

  this->compilation.useCompilationOptimization = filecontent["parameters"]["useCompilationOptimization"].get<bool>();
  this->compilation.exclusiveCustomCompilationProcess = filecontent["parameters"]["useOnlyCustomCompilationProcess"].get<std::string>();
  this->compilation.exclusiveCustomConfigProcess = filecontent["parameters"]["useOnlyCustomConfigurationProcess"].get<std::string>();
  this->compilation.exclusiveCustomInstallationProcess = filecontent["parameters"]["useOnlyCustomInstallationProcess"].get<std::string>();

  for (auto arch : filecontent["package"]["target_archs"])
  {
    this->archs.push_back(arch);
  }

  this->actions.clear();
  for (auto action : filecontent["actions"])
  {
    // if (action["type"].get<std::string>() == "command")
    //{
    std::shared_ptr<PackageAction> newAction = std::make_shared<PackageAction>();
    newAction->type = action["type"].get<std::string>();
    newAction->priority = action["priority"].get<int>();
    newAction->executionSequence = action["sequence"].get<std::string>();
    newAction->command = action["command"].get<std::string>();
    this->actions.push_back(newAction);
    // }
  }

  std::sort(this->actions.begin(), this->actions.end(), [](const std::shared_ptr<PackageAction> &a, const std::shared_ptr<PackageAction> &b)
            { return a->priority < b->priority; });

  this->compilation.installationSuffixes.clear();
  for (auto installSuffixes : filecontent["compilation"]["installationSuffixes"])
  {
    for (auto it = installSuffixes.begin(); it != installSuffixes.end(); ++it)
    {
      this->compilation.installationSuffixes.emplace_back(it.key(), it.value());
    }
  }

  this->compilation.configurationSuffixes.clear();
  for (auto configurationSuffixes : filecontent["compilation"]["configurationSuffixes"])
  {
    for (auto it = configurationSuffixes.begin(); it != configurationSuffixes.end(); ++it)
    {
      this->compilation.configurationSuffixes.emplace_back(it.key(), it.value());
    }
  }

  this->compilation.compilationSuffixes.clear();
  for (auto compileSuffixes : filecontent["compilation"]["compilationSuffixes"])
  {
    for (auto it = compileSuffixes.begin(); it != compileSuffixes.end(); ++it)
    {
      this->compilation.compilationSuffixes.emplace_back(it.key(), it.value());
    }
  }

  this->compilation.configurationPrefixes.clear();
  for (auto configurationPrefixes : filecontent["compilation"]["configurationPrefixes"])
  {
    for (auto it = configurationPrefixes.begin(); it != configurationPrefixes.end(); ++it)
    {
      this->compilation.configurationPrefixes.emplace_back(it.key(), it.value());
    }
  }

  this->compilation.installationPrefixes.clear();
  for (auto installationPrefixes : filecontent["compilation"]["installationPrefixes"])
  {
    for (auto it = installationPrefixes.begin(); it != installationPrefixes.end(); ++it)
    {
      this->compilation.installationPrefixes.emplace_back(it.key(), it.value());
    }
  }

  this->compilation.compilationPrefixes.clear();
  for (auto compilationPrefixes : filecontent["compilation"]["compilationPrefixes"])
  {
    for (auto it = compilationPrefixes.begin(); it != compilationPrefixes.end(); ++it)
    {
      this->compilation.compilationPrefixes.emplace_back(it.key(), it.value());
    }
  }
}


bool PackageModule::RegisterPackage(std::string filepath, std::shared_ptr<Package> newPackage, nlohmann::json filecontent)
{
    VxContext* ctx = VortexMaker::GetCurrentContext(); 

    newPackage->path = filepath;
    size_t position = newPackage->path.find("/package.config");
    newPackage->configFilePath = filepath;

    if (position != std::string::npos)
    {
        newPackage->path.erase(position, 17);
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
            newAction->command = action["command"].get<std::string>();
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

    CPackagesModule->m_packages.push_back(newPackage);

    return true;
}


// MODULE FUNCTION