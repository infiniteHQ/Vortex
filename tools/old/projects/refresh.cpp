#include "../../main/include/vortex.h"
#include "../../main/include/vortex_internals.h"


/*
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
    // if (action["type"].get<std::string>() == "command")
    //{
    std::shared_ptr<VXPackage_Action> newAction = std::make_shared<VXPackage_Action>();
    newAction->type = action["type"].get<std::string>();
    newAction->priority = action["priority"].get<int>();
    newAction->executionSequence = action["sequence"].get<std::string>();
    newAction->command = action["command"].get<std::string>();
    this->actions.push_back(newAction);
    // }
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
*/

void VxHost::RefreshCurrentWorkingHost()
{
  // Check if this->workingPath + "/working_host.config" exists
  if (!std::filesystem::exists(this->workingPath + "/working_host.config"))
  {
    std::cerr << "Error : " << this->workingPath + "/working_host.config"
              << " does not exists." << std::endl;
    this->haveCurrentSys = false;
    return;
  }

  nlohmann::json data = VortexMaker::DumpJSON(this->workingPath + "/working_host.config");
    this->currentLoadedSystem.parent = std::make_shared<VxHost>(*this);
  this->currentLoadedSystem.Populate(data);
  this->haveCurrentSys = true;
}

void VxToolchain::RefreshCurrentWorkingToolchain()
{
  // Check if this->workingPath + "/working_host.config" exists
  if (!std::filesystem::exists(this->workingPath + "/working_host.config"))
  {
    std::cerr << "Error : " << this->workingPath + "/working_host.config"
              << " does not exists." << std::endl;
    this->haveCurrentSys = false;
    return;
  }

  nlohmann::json data = VortexMaker::DumpJSON(this->workingPath + "/working_host.config");
  this->currentLoadedSystem.parent = std::make_shared<VxToolchain>(*this);
  this->currentLoadedSystem.Populate(data);
  this->haveCurrentSys = true;
}

void VxToolchain::RefreshSnapshots()
{
  // Create snapshot folder
  VxContext *ctx = VortexMaker::GetCurrentContext();
  std::string envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;
  std::string baseDir = envPath + "/" + this->name;
  std::string snapshotsDir = baseDir + "/" + "snapshots";
  if (mkdir(snapshotsDir.c_str(), 0777) == -1)
  {
    perror("Error while creating folder");
  }

  this->path_hostsnapshots = snapshotsDir;

  this->snapshots.clear();
  // Dist Toolchains
  for (const auto &file : VortexMaker::SearchFiles(this->path_hostsnapshots, "snapshot.config"))
  {
    try
    {
      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      VxToolchainSnapshot newsnapshot;

      std::string path = file;
      size_t position = path.find("/snapshot.config");
      if (position != std::string::npos)
      {
        path.erase(position, 16);
      }

      newsnapshot.name = filecontent["snapshot"]["name"].get<std::string>();
      newsnapshot.path = path;

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

void VxHost::RefreshSnapshots()
{
  // Create snapshot folder
  VxContext *ctx = VortexMaker::GetCurrentContext();
  std::string envPath = ctx->projectPath / ctx->paths.hostDistFolder;
  std::string baseDir = envPath + "/" + this->name;
  std::string snapshotsDir = baseDir + "/" + "snapshots";
  if (mkdir(snapshotsDir.c_str(), 0777) == -1)
  {
    perror("Error while creating folder");
  }

  this->path_hostsnapshots = snapshotsDir;

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

std::chrono::time_point<std::chrono::system_clock> stringToTimePoint(const std::string &timeString)
{
  // Création du flux de chaînes pour la conversion
  std::istringstream ss(timeString);

  // Déclaration et initialisation de la structure tm pour stocker les composants du temps
  std::tm tm = {};

  // Utilisation de std::get_time pour extraire les composants du temps de la chaîne
  ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");

  // Vérification si l'extraction a réussi
  if (ss.fail())
  {
    throw std::runtime_error("Failed to parse the time string.");
  }

  // Convertir la structure tm en un point de temps
  std::time_t time = std::mktime(&tm);
  if (time == -1)
  {
    throw std::runtime_error("Failed to convert time structure.");
  }

  // Création du point de temps à partir du temps depuis l'époque
  return std::chrono::system_clock::from_time_t(time);
}

void VxToolchainCurrentSystem::Populate(nlohmann::json jsonData)
{
  this->executedTasks.clear();
  this->size = jsonData["size"].get<std::string>();

  // Get list of packages and all reports associated
  for (auto packageReport : jsonData["taskList"])
  {
    std::shared_ptr<Task> task = std::make_shared<Task>();

    task->id = packageReport["t_id"].get<std::string>();
    task->tasktype = packageReport["t_tasktype"].get<std::string>();
    task->priority = packageReport["t_priority"].get<int>();
    task->component = packageReport["t_component"].get<std::string>();
    task->state = packageReport["t_state"].get<std::string>();
    task->m_TotalTime = packageReport["t_duration"].get<double>();
    task->m_StartTime = stringToTimePoint(packageReport["t_time"].get<std::string>());

    for (auto created_variables : packageReport["t_created_variables"])
    {
      task->created_variables.push_back(std::make_tuple(created_variables["variable"].get<std::string>(), created_variables["owner"].get<std::string>(), created_variables["value"].get<std::string>()));
      this->variables.push_back(std::make_tuple(created_variables["variable"].get<std::string>(), created_variables["owner"].get<std::string>(), created_variables["value"].get<std::string>()));
    }
    for (auto created_variables : packageReport["t_used_variables"])
    {
      task->created_variables.push_back(std::make_tuple(created_variables["variable"].get<std::string>(), created_variables["owner"].get<std::string>(), created_variables["value"].get<std::string>()));
      this->variables.push_back(std::make_tuple(created_variables["variable"].get<std::string>(), created_variables["owner"].get<std::string>(), created_variables["value"].get<std::string>()));
    }

    for (auto checks : packageReport["t_checklist"])
    {
      std::shared_ptr<Check> check = std::make_shared<Check>();
      check->checkResult = checks["result"].get<std::string>();
      check->checkDirective = checks["directive"].get<std::string>();
      check->checkID = checks["id"].get<std::string>();
      check->checkLog = checks["log"].get<std::string>();
      if (check->checkResult == "success")
      {
        task->successCounter++;
      };
      if (check->checkResult == "failed")
      {
        task->failCounter++;
      };
      if (check->checkResult == "warning")
      {
        task->warningCounter++;
      };
      if (check->checkResult == "unknow")
      {
        task->unknowCounter++;
      };
      task->checkList.push_back(check);
    }

    this->executedTasks.push_back(task);
  }

    for (auto packageReport : jsonData["packageList"])
    {
      for (auto package : this->parent->packages)
      {
        if (package->name == packageReport["p_name"].get<std::string>())
        {
          for (auto task : this->executedTasks)
          {
            for (auto taskid : packageReport["p_tasklist"])
            {
              if (task->id == taskid.get<std::string>())
              {
                package->allTasks.push_back(task);
              }
              if (task->id == packageReport["p_latest_taskid"].get<std::string>())
              {
                package->latestTask = task;
              }
            }
          }
        }
      }
    }
  // Get filesystem informations

  // Get list of all render assets (actions, scirpts, skeletons, etc)
} // from working_host.config

void VxHostCurrentSystem::Populate(nlohmann::json jsonData)
{
  this->executedTasks.clear();
  this->size = jsonData["size"].get<std::string>();

  // Get list of packages and all reports associated
  for (auto packageReport : jsonData["taskList"])
  {
    std::shared_ptr<Task> task = std::make_shared<Task>();

    task->id = packageReport["t_id"].get<std::string>();
    task->tasktype = packageReport["t_tasktype"].get<std::string>();
    task->priority = packageReport["t_priority"].get<int>();
    task->component = packageReport["t_component"].get<std::string>();
    task->state = packageReport["t_state"].get<std::string>();
    task->m_TotalTime = packageReport["t_duration"].get<double>();
    task->m_StartTime = stringToTimePoint(packageReport["t_time"].get<std::string>());

    for (auto created_variables : packageReport["t_created_variables"])
    {
      task->created_variables.push_back(std::make_tuple(created_variables["variable"].get<std::string>(), created_variables["owner"].get<std::string>(), created_variables["value"].get<std::string>()));
      this->variables.push_back(std::make_tuple(created_variables["variable"].get<std::string>(), created_variables["owner"].get<std::string>(), created_variables["value"].get<std::string>()));
    }
    for (auto created_variables : packageReport["t_used_variables"])
    {
      task->created_variables.push_back(std::make_tuple(created_variables["variable"].get<std::string>(), created_variables["owner"].get<std::string>(), created_variables["value"].get<std::string>()));
      this->variables.push_back(std::make_tuple(created_variables["variable"].get<std::string>(), created_variables["owner"].get<std::string>(), created_variables["value"].get<std::string>()));
    }

    for (auto checks : packageReport["t_checklist"])
    {
      std::shared_ptr<Check> check = std::make_shared<Check>();
      check->checkResult = checks["result"].get<std::string>();
      check->checkDirective = checks["directive"].get<std::string>();
      check->checkID = checks["id"].get<std::string>();
      check->checkLog = checks["log"].get<std::string>();
      if (check->checkResult == "success")
      {
        task->successCounter++;
      };
      if (check->checkResult == "failed")
      {
        task->failCounter++;
      };
      if (check->checkResult == "warning")
      {
        task->warningCounter++;
      };
      if (check->checkResult == "unknow")
      {
        task->unknowCounter++;
      };
      task->checkList.push_back(check);
    }

    this->executedTasks.push_back(task);
  }

    for (auto packageReport : jsonData["packageList"])
    {
      for (auto package : this->parent->packages)
      {
        if (package->name == packageReport["p_name"].get<std::string>())
        {
          for (auto task : this->executedTasks)
          {
            for (auto taskid : packageReport["p_tasklist"])
            {
              if (task->id == taskid.get<std::string>())
              {
                package->allTasks.push_back(task);
              }
              if (task->id == packageReport["p_latest_taskid"].get<std::string>())
              {
                package->latestTask = task;
              }
            }
          }
        }
      }
    }
  // Get filesystem informations

  // Get list of all render assets (actions, scirpts, skeletons, etc)
} // from working_host.config

nlohmann::json VxToolchainCurrentSystem::Extract()
{
  VxContext &ctx = *CVortexMaker;
  nlohmann::json jsonData;

  jsonData["size"] = this->size;
  jsonData["taskList"] = nlohmann::json::array();
  jsonData["packageList"] = nlohmann::json::array();
  jsonData["actionsReportsList"] = nlohmann::json::array();

      std::cout << "Package : " << std::endl;

    for (auto package : this->parent->packages)
    {
      std::cout << "Package : " << package->name << std::endl;
      nlohmann::json report;
      report["p_name"] = package->name;
      std::cout << "Package : " << package->name << std::endl;
      report["p_latest_taskid"] = package->latestTask->id;
      std::cout << "Package : " << package->name << std::endl;
      report["p_tasklist"] = nlohmann::json::array();
      std::cout << "Package : " << package->name << std::endl;
      for (auto task : package->allTasks)
      {
      std::cout << "Package : " << package->name << std::endl;
        report["p_tasklist"].push_back(task->id);
      }
      std::cout << "Package : " << package->name << std::endl;
      jsonData["packageList"].push_back(report);
    }

      std::cout << "00 : " << std::endl;
  // Fix : All tasks features and after : make all basic task of toolchain
  for (auto task : this->executedTasks)
  {
    std::string def = "unknow";

    nlohmann::json report;
    report["t_id"] = task->id;
    report["t_tasktype"] = task->tasktype;
    report["t_priority"] = task->priority;
    report["t_state"] = task->state;

    report["t_duration"] = task->m_TotalTime;
    report["t_time"] = task->startTime();

    report["t_component"] = task->component;

    report["t_checklist"] = nlohmann::json::array();
    report["t_used_variables"] = nlohmann::json::array();
    report["t_created_variables"] = nlohmann::json::array();

    // Variable Name // Required By (Task) // Value
    for (auto var : task->used_variables)
    {
      nlohmann::json c;
      c["variable"] = std::get<0>(var);
      c["owner"] = std::get<1>(var);
      c["value"] = std::get<2>(var);
      report["t_used_variables"].push_back(c);
    }

    // Variable Name // Required By (Task) // Value
    for (auto var : task->created_variables)
    {
      nlohmann::json c;
      c["variable"] = std::get<0>(var);
      c["owner"] = std::get<1>(var);
      c["value"] = std::get<2>(var);
      report["t_created_variables"].push_back(c);
    }

    // TODO: Add props to checklist

    for (auto check : task->checkList)
    {
      nlohmann::json c;
      c["result"] = check->checkResult;
      c["id"] = check->checkID;
      c["log"] = check->checkLog;
      c["directive"] = check->checkDirective;
      report["t_checklist"].push_back(c);
    }

    jsonData["taskList"].push_back(report);
  }

  return jsonData;
}

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

    report["t_duration"] = task->m_TotalTime;
    report["t_time"] = task->startTime();

    report["t_component"] = task->component;

    report["t_checklist"] = nlohmann::json::array();

    for (auto check : task->checkList)
    {
      nlohmann::json c;
      c["result"] = check->checkResult;
      c["id"] = check->checkID;
      c["log"] = check->checkLog;
      report["t_checklist"].push_back(c);
    }

    jsonData["taskList"].push_back(report);
  }

  return jsonData;
}

// TODO : Split to little refresh functions, create a RefreshAll function.
// TODO : This need to go in toolchains core folder

void VxToolchain::RefreshDistConfig()
{

  VxContext *ctx = VortexMaker::GetCurrentContext();

  std::string distPath = ctx->projectPath;
  distPath += "/" + ctx->paths.toolchainDistFolder + "/" + this->name + "/toolchain.dist.config";

  nlohmann::json toolchainData = VortexMaker::DumpJSON(distPath);

  this->distToolchain.AR = toolchainData["configs"]["AR"].get<std::string>();
  this->distToolchain.AS = toolchainData["configs"]["AS"].get<std::string>();
  this->distToolchain.CC = toolchainData["configs"]["CC"].get<std::string>();
  this->distToolchain.CXX = toolchainData["configs"]["CXX"].get<std::string>();
  this->distToolchain.LD = toolchainData["configs"]["LD"].get<std::string>();
  this->distToolchain.RANLIB = toolchainData["configs"]["RANLIB"].get<std::string>();
  this->distToolchain.STRIP = toolchainData["configs"]["STRIP"].get<std::string>();
}

void VxToolchain::Refresh()
{
  VortexMaker::LogInfo("Core", "Refreshing toolchain " + this->name + " from " + this->configFilePath);
  nlohmann::json toolchainData = VortexMaker::DumpJSON(this->configFilePath);

  VortexMaker::LogInfo("Core", "Getting toolchain \"toolchain\" datas from " + this->configFilePath);
  this->name = toolchainData["toolchain"]["name"].get<std::string>();
  this->author = toolchainData["toolchain"]["author"].get<std::string>();
  this->description = toolchainData["toolchain"]["description"].get<std::string>();
  this->version = toolchainData["toolchain"]["version"].get<std::string>();
  this->type = toolchainData["toolchain"]["type"].get<std::string>();
  this->state = toolchainData["toolchain"]["state"].get<std::string>();

  VortexMaker::LogInfo("Core", "Getting toolchain \"configs\" datas from " + this->configFilePath);
  this->builder_vendor = toolchainData["configs"]["builder_vendor"].get<std::string>();
  this->builder_arch = toolchainData["configs"]["builder_arch"].get<std::string>();
  this->builder_platform = toolchainData["configs"]["builder_platform"].get<std::string>();
  this->builder_cpu = toolchainData["configs"]["builder_cpu"].get<std::string>();
  this->builder_fpu = toolchainData["configs"]["builder_fpu"].get<std::string>();

  this->target_vendor = toolchainData["configs"]["target_vendor"].get<std::string>();
  this->target_arch = toolchainData["configs"]["target_arch"].get<std::string>();
  this->target_platform = toolchainData["configs"]["target_platform"].get<std::string>();
  this->target_cpu = toolchainData["configs"]["target_cpu"].get<std::string>();
  this->target_fpu = toolchainData["configs"]["target_fpu"].get<std::string>();

  this->host_vendor = toolchainData["configs"]["host_vendor"].get<std::string>();
  this->host_arch = toolchainData["configs"]["host_arch"].get<std::string>();
  this->host_platform = toolchainData["configs"]["host_platform"].get<std::string>();
  this->host_cpu = toolchainData["configs"]["host_cpu"].get<std::string>();
  this->host_fpu = toolchainData["configs"]["host_fpu"].get<std::string>();

  this->toolchain_type = toolchainData["configs"]["toolchain_type"].get<std::string>();

  this->compressionMode = toolchainData["configs"]["compression"].get<std::string>();

  VortexMaker::LogInfo("Core", "Getting toolchain \"data\" informations from " + this->configFilePath);
  this->localPackagesPath = toolchainData["data"]["packages"].get<std::string>();
  this->localPatchsPath = toolchainData["data"]["patchs"].get<std::string>();
  this->localScriptsPath = toolchainData["data"]["scripts"].get<std::string>();

  VortexMaker::LogInfo("Core", "Refreshing packages asset of " + this->name);
  registeredPackages.clear();
  nlohmann::json packages = toolchainData["content"]["packages"];
  for (auto &pkg : packages)
  {
    this->RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }

  VortexMaker::LogInfo("Core", "Finding packages asset of " + this->name);
  this->FindPackages();

  VortexMaker::LogInfo("Core", "Refreshing tasklists asset of " + this->name);
  registeredTasklists.clear();
  nlohmann::json tasklists = toolchainData["content"]["tasklists"];
  for (auto &t : tasklists)
  {
    this->RegisterTasklist(t["label"].get<std::string>());
  }
  VortexMaker::LogInfo("Core", "Finding tasklists asset of " + this->name);
  this->FindTasklists();

  VortexMaker::LogInfo("Core", "Refreshing toolchain " + this->name + " is finish !");
  // this->Init();
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

  this->host_arch = toolchainData["configs"]["host_arch"].get<std::string>();
  this->target_arch = toolchainData["configs"]["target_arch"].get<std::string>();

  this->toolchainToUse = toolchainData["build"]["use_toolchain"].get<std::string>();

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

void VxGPOSystem::Refresh()
{
  nlohmann::json gposData = VortexMaker::DumpJSON(this->configFilePath);

  this->name = gposData["gpos"]["name"].get<std::string>();
  this->author = gposData["gpos"]["author"].get<std::string>();
  this->type = gposData["gpos"]["type"].get<std::string>();
  this->state = gposData["gpos"]["state"].get<std::string>();
  this->vendor = gposData["gpos"]["vendor"].get<std::string>();
  this->platform = gposData["gpos"]["platform"].get<std::string>();
  this->target_arch = gposData["configs"]["target_arch"].get<std::string>();

  this->localPackagesPath = gposData["data"]["packages"].get<std::string>();
  this->localPatchsPath = gposData["data"]["patchs"].get<std::string>();
  this->localScriptsPath = gposData["data"]["scripts"].get<std::string>();
  registeredPackages.clear();
  nlohmann::json packages = gposData["content"]["packages"];
  for (auto &pkg : packages)
  {
    this->RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }
  this->FindPackages();

  registeredTasklists.clear();
  nlohmann::json tasklists = gposData["content"]["tasklists"];
  for (auto &t : tasklists)
  {
    this->RegisterTasklist(t["label"].get<std::string>());
  }
  this->FindTasklists();

  // this->Init();
}

/*
TODO apres-midi :

- Asset script, execution, variables export, etc...
- Full build overview (executing TL_Main, get working_host infos, & test)
- Testing tools (toolchain executable test, lib checks, functions (strip, ld, etc...))
- After : Make a monolithic P_GCC package
- After After : Host, GPOS/Embedded.

*/

VORTEX_API void VortexMaker::RefreshToolchains()
{
  VxContext &ctx = *CVortexMaker;

  // clear existing dist toolchains
  // ctx.IO.toolchains.clear();

  // Toolchains
  for (const auto &file : SearchFiles(ctx.toolchainsPath, "toolchain.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      std::shared_ptr<VxToolchain> toolchain = std::make_shared<VxToolchain>();

      toolchain->configFilePath = file;
      toolchain->path = file;

      size_t position = toolchain->path.find("/toolchain.config");
      if (position != std::string::npos)
      {
        toolchain->path.erase(position, 17);
      }

      bool alreadyExist = false;

      for (auto alreadyRegistered : ctx.IO.toolchains)
      {
        if (alreadyRegistered->name == filecontent["toolchain"]["name"].get<std::string>())
        {
          VortexMaker::LogError("Core", alreadyRegistered->name + " is already registered.");
          alreadyExist = true;
        }
      }

      if (alreadyExist == true)
      {
        continue;
      }

      RegisterToolchain(toolchain, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}

void VortexMaker::RefreshGpos()
{

  VxContext &ctx = *CVortexMaker;

  // Hosts
  for (const auto &file : SearchFiles(ctx.gposPath, "gpos.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      std::shared_ptr<VxGPOSystem> gpos = std::make_shared<VxGPOSystem>();

      gpos->configFilePath = file;
      bool alreadyExist = false;

      for (auto alreadyRegistered : ctx.IO.gpoSystems)
      {
        if (alreadyRegistered->name == filecontent["gpos"]["name"].get<std::string>())
        {
          VortexMaker::LogError("Core", alreadyRegistered->name + " is already registered.");
          alreadyExist = true;
        }
      }

      if (alreadyExist == true)
      {
        continue;
      }

      RegisterGPOS(gpos, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}

VORTEX_API void VortexMaker::RefreshScripts()
{
  VxContext &ctx = *CVortexMaker;

  // Hosts
  for (const auto &file : SearchFiles(ctx.scriptsPath, "script.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      std::shared_ptr<VxScript> newScript = std::make_shared<VxScript>();

      newScript->path = file;
      newScript->configFilePath = file;

      bool alreadyExist = false;

      for (auto alreadyRegistered : ctx.IO.scripts)
      {
        if (alreadyRegistered->name == filecontent["script"]["name"].get<std::string>())
        {
          VortexMaker::LogError("Core", alreadyRegistered->name + " is already registered.");
          alreadyExist = true;
        }
      }

      if (alreadyExist == true)
      {
        continue;
      }

      RegisterScript(newScript, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}
/*
VORTEX_API void VortexMaker::RefreshPackages()
{
  VxContext &ctx = *CVortexMaker;

  // Hosts
  for (const auto &file : SearchFiles(ctx.packagesPath, "package.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      std::shared_ptr<VxPackage> newPackage = std::make_shared<VxPackage>();

      newPackage->path = file;
      newPackage->configFilePath = file;

      bool alreadyExist = false;

      for (auto alreadyRegistered : ctx.IO.hosts)
      {
        if (alreadyRegistered->name == filecontent["package"]["name"].get<std::string>())
        {
          VortexMaker::LogError("Core", alreadyRegistered->name + " is already registered.");
          alreadyExist = true;
        }
      }

      if (alreadyExist == true)
      {
        continue;
      }

      RegisterPackage(file, newPackage, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}
*/
VORTEX_API void VortexMaker::RefreshHosts()
{
  VxContext &ctx = *CVortexMaker;

  // Hosts
  for (const auto &file : SearchFiles(ctx.hostsPath, "host.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      std::shared_ptr<VxHost> host = std::make_shared<VxHost>();

      host->configFilePath = file;
      bool alreadyExist = false;

      for (auto alreadyRegistered : ctx.IO.hosts)
      {
        if (alreadyRegistered->name == filecontent["host"]["name"].get<std::string>())
        {
          alreadyExist = true;
        }
      }

      if (alreadyExist == true)
      {
        continue;
      }

      RegisterHost(host, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
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
