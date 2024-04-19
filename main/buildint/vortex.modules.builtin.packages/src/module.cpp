#include "module.h"
#include "../src/instances/packageInstance/PackageRenderInstance.h"

#ifndef CPackagesModule
PackagesModuleCTX *CPackagesModule = NULL;
#endif

void PackageModule::CreatePackageContext()
{
    PackagesModuleCTX *ctx = VX_NEW(PackagesModuleCTX);
    CPackagesModule = ctx;
}


/**
 * @brief Launch package interface
 * @param ["args"] "package" The target package we wan't to open in a interface. (type: std::shared_ptr<Package>)
*/
void PackageModule::LaunchPackageInterface(std::shared_ptr<hArgs> args)
{
    if (args != NULL)
    {
        std::shared_ptr<Package> package = args->get<std::shared_ptr<Package>>("package", nullptr);
        if (package != nullptr)
        {
            VxContext *ctx = VortexMaker::GetCurrentContext();
            std::shared_ptr<PackageRenderInstance> instance = std::make_shared<PackageRenderInstance>(ctx, package);
            instance->name = package->name;
            std::cout << "Add to" << CPackagesModule->m_interface << std::endl;
            CPackagesModule->m_interface->AddModuleRenderInstance(instance);
            VortexMaker::LogWarn("..", "package sdfdsf");
        }
        else{
            VortexMaker::LogError("..", "package null");
        }
    }
    else{

            VortexMaker::LogError("..", "arg null");
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