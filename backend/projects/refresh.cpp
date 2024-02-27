#include "../../vortex.h"
#include "../../vortex_internals.h"

void TaskList::Refresh()
{
  nlohmann::json filecontent = VortexMaker::DumpJSON(this->configFilePath);

  this->label = filecontent["label"].get<std::string>();

  this->list.clear();
  nlohmann::json tasks = filecontent["tasks"];
  for (auto &t : tasks)
  {
    std::shared_ptr<Task> task = std::make_shared<Task>();
    task->tasktype = t["task"].get<std::string>();
    //task.component = t["component"].get<std::string>();
    task->priority = t["priority"].get<int>();
    this->list.push_back(task);
  }
}

void VxPackage::Refresh()
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
    if (action["type"].get<std::string>() == "command")
    {
      std::shared_ptr<VXPackage_Action> newAction = std::make_shared<VXPackage_Action>();
      newAction->type = action["type"].get<std::string>();
      newAction->priority = action["priority"].get<int>();
      newAction->executionSequence = action["sequence"].get<std::string>();
      newAction->command = action["command"].get<std::string>();
      this->actions.push_back(newAction);
    }
  }

  std::sort(this->actions.begin(), this->actions.end(), [](const std::shared_ptr<VXPackage_Action> &a, const std::shared_ptr<VXPackage_Action> &b)
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

void VxHost::RefreshCurrentWorkingHost()
{
  nlohmann::json data = VortexMaker::DumpJSON(this->path_hostroot + "/working_host.config");
  this->currentLoadedSystem.Populate(data);
  
}

void VxHost::RefreshSnapshots()
{
  this->snapshots.clear();
  // Dist Toolchains
  for (const auto &file : VortexMaker::SearchFiles(this->path_hostsnapshots, "snapshot.config"))
  {
    try
    {
      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      VxHostSnapshot newsnapshot;

      std::string path = file;
      size_t position = path.find("/snapshot.config");
      if (position != std::string::npos)
      {
        path.erase(position, 16);
      }

      newsnapshot.name = filecontent["snapshot"]["name"].get<std::string>();

      nlohmann::json working_host = VortexMaker::DumpJSON(path + "/working_host.config");
      newsnapshot.snapshotSystem.Populate(working_host); // working_host file data

      this->snapshots.push_back(newsnapshot);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}

void VxHostCurrentSystem::Populate(nlohmann::json jsonData)
{
  this->size = jsonData["size"].get<std::string>();

  // Get list of packages and all reports associated
  for (auto packageReport : jsonData["taskList"])
  {
    std::shared_ptr<Task> task = TaskFactory::getInstance().createInstance(packageReport["t_tasktype"].get<std::string>().c_str());



    task->id = packageReport["t_id"].get<std::string>();
    task->tasktype = packageReport["t_tasktype"].get<std::string>();
    task->priority = packageReport["t_priority"].get<int>();
    task->state = packageReport["t_state"].get<std::string>();

    this->executedTasks.push_back(task);
  }


  // Get filesystem informations

  // Get list of all render assets (actions, scirpts, skeletons, etc)
} // from working_host.config

nlohmann::json VxHostCurrentSystem::Extract()
{
  VxContext &ctx = *CVortexMaker;
  nlohmann::json jsonData;
  jsonData["size"] = this->size;
  jsonData["taskList"] = nlohmann::json::array();
  jsonData["actionsReportsList"] = nlohmann::json::array();

  for (auto task : this->executedTasks)
  {
    std::string def = "unknow";

    nlohmann::json report;
    report["t_id"] = task->id;
    report["t_tasktype"] = task->tasktype;
    report["t_priority"] = task->priority;
    report["t_state"] = task->state;

    //report["t_component"] = task->result_props->get("component", def);

    
    jsonData["taskList"].push_back(report);
  }
  
  return jsonData;
}

// TODO : Split to little refresh functions, create a RefreshAll function.
void VxHost::Refresh()
{
  nlohmann::json toolchainData = VortexMaker::DumpJSON(this->configFilePath);

  this->name = toolchainData["host"]["name"].get<std::string>();
  this->author = toolchainData["host"]["author"].get<std::string>();
  this->description = toolchainData["host"]["description"].get<std::string>();
  this->version = toolchainData["host"]["version"].get<std::string>();
  this->type = toolchainData["host"]["type"].get<std::string>();
  this->state = toolchainData["host"]["state"].get<std::string>();
  this->vendor = toolchainData["host"]["vendor"].get<std::string>();
  this->platform = toolchainData["host"]["platform"].get<std::string>();
  this->host_arch = toolchainData["host"]["host_arch"].get<std::string>();
  this->target_arch = toolchainData["host"]["target_arch"].get<std::string>();
  this->toolchainToUse = toolchainData["build"]["useToolchain"].get<std::string>();

  this->localPackagesPath = toolchainData["data"]["packages"].get<std::string>();
  this->localPatchsPath = toolchainData["data"]["patchs"].get<std::string>();
  this->localScriptsPath = toolchainData["data"]["scripts"].get<std::string>();

  registeredPackages.clear();
  nlohmann::json packages = toolchainData["content"]["packages"];
  for (auto &pkg : packages)
  {
    this->RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }
  this->FindPackages();

  registeredTasklists.clear();
  nlohmann::json tasklists = toolchainData["content"]["tasklists"];
  for (auto &t : tasklists)
  {
    this->RegisterTasklist(t["label"].get<std::string>());
  }
  this->FindTasklists();

  this->Init();
}

VORTEX_API void VortexMaker::RefreshDistToolchains() // Rename to RefreshDistHostsList
{
  VxContext &ctx = *CVortexMaker;

  // clear existing dist toolchains
  ctx.IO.distToolchains.clear();

  // Dist Toolchains
  for (const auto &file : SearchFiles(ctx.paths.toolchainDistFolder, "toolchain.dist.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);

      VxDistToolchain toolchain;

      toolchain.path = file;

      size_t position = toolchain.path.find("/toolchain.dist.config");
      if (position != std::string::npos)
      {
        toolchain.path.erase(position, 22);
      }

      VortexMaker::RegisterDistToolchain(toolchain, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}

VORTEX_API void VortexMaker::RefreshDistHosts() // Rename to RefreshDistHostsList
{
  VxContext &ctx = *CVortexMaker;

  // clear existing dist toolchains
  ctx.IO.distHosts.clear();

  // Dist Toolchains
  for (const auto &file : SearchFiles(ctx.paths.hostDistFolder, "host.dist.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);

      VxDistHost host;

      host.path = file;

      size_t position = host.path.find("/host.dist.config");
      if (position != std::string::npos)
      {
        host.path.erase(position, 17);
      }

      VortexMaker::RegisterDistHost(host, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}
