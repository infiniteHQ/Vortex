#include "toolchain.h"

#include "tasks/CheckCompiler.h"
#include "tasks/CompilePackage.h"
#include "tasks/ConfigurePackage.h"
#include "tasks/CreateBuildEnv.h"
#include "tasks/CreateTemporaryUser.h"
#include "tasks/DeleteTemporaryUser.h"
#include "tasks/GiveToolchainToTemporaryUser.h"
#include "tasks/InstallPackage.h"
#include "tasks/MovePackageToDist.h"
#include "tasks/SetupDistEnvironment.h"
#include "tasks/UncompressDistPackage.h"
#include "tasks/ExecuteTasklist.h"


template<typename T>
void Toolchain::AddTaskType(const std::string& name){
    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    args->add<std::vector<std::shared_ptr<Task>>*>("taskarray", &this->tasks);
    std::shared_ptr<T> task = std::make_shared<T>();
    args->add<const char*>("pool_name", this->pool_name);
    args->add<std::shared_ptr<Task>>("task", task);
    task->tasktype = name;
    VortexMaker::CallModuleEvent(args, "AddTaskToPool", "vortex.modules.builtin.tasks");

}

void Toolchain::InitTasks(){

    this->pool_name = 'toolchains.' + this->name.c_str();

    {
      std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
      args->add("processor_name", this->pool_name);
      VortexMaker::CallModuleEvent(args, "CreateTaskProcessor", "vortex.modules.builtin.tasks");
      VortexMaker::CallModuleEvent(args, "StartTaskProcessor", "vortex.modules.builtin.tasks");

    }

    {
      std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
      args->add("pool_name", this->pool_name);
      VortexMaker::CallModuleEvent(args, "CreateTaskPool", "vortex.modules.builtin.tasks");

    }

    this->AddTaskType<CreateTemporaryUser>("CreateTemporaryUser");
    this->AddTaskType<DeleteTemporaryUser>("DeleteTemporaryUser");
    this->AddTaskType<CreateBuildEnv>("CreateBuildEnv");
    this->AddTaskType<GiveToolchainToTemporaryUser>("GiveToolchainToTemporaryUser");
    this->AddTaskType<MovePackageToDist>("MovePackageToDist");
    this->AddTaskType<UncompressDistPackage>("UncompressDistPackage");
    this->AddTaskType<ConfigurePackage>("ConfigurePackage");
    this->AddTaskType<CompilePackage>("CompilePackage");
    this->AddTaskType<InstallPackage>("InstallPackage");
    this->AddTaskType<SetupDistEnvironment>("SetupDistEnvironment");
    this->AddTaskType<CheckCompiler>("CheckCompiler");
    this->AddTaskType<ExecuteTasklist>("ExecuteTasklist");

}

static std::chrono::time_point<std::chrono::system_clock> stringToTimePoint(const std::string &timeString)
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

nlohmann::json ToolchainCurrentSystem::Extract()
{
  VxContext &ctx = *CVortexMaker;
  nlohmann::json jsonData;

  jsonData["size"] = this->size;
  jsonData["taskList"] = nlohmann::json::array();
  jsonData["packageList"] = nlohmann::json::array();
  jsonData["actionsReportsList"] = nlohmann::json::array();

  for (auto package : this->parent->packages)
  {
    nlohmann::json report;
    report["p_name"] = package->name;
    report["p_latest_taskid"] = package->latestTask->id;
    report["p_tasklist"] = nlohmann::json::array();
    for (auto task : package->allTasks)
    {
      report["p_tasklist"].push_back(task->id);
    }
    jsonData["packageList"].push_back(report);
  }

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

void ToolchainCurrentSystem::Populate(nlohmann::json jsonData)
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

// Task succeded = last task
// Plutot le faire dans le current system
bool Toolchain::TaskSuccedded(std::string label){
  for(auto succededTask : this->currentLoadedSystem.executedTasks){
    if(succededTask->tasktype == label){
      if(succededTask->state == "success"){
        return true;
      }
      else{
        return false;
      }
    }
  }
  return false;
}

std::string Toolchain::GetTriplet(std::string triplet_type)
{
  std::string NativeTriplet;
  FILE *pipe = popen("gcc -dumpmachine", "r");
  if (!pipe)
  {
    perror("Error while try to get triplet with gcc !");
    return "unknow";
  }
  char buffer[128];
  while (fgets(buffer, 128, pipe) != NULL)
  {
    NativeTriplet += buffer;
  }
  pclose(pipe);
  NativeTriplet.pop_back();

  if (triplet_type == "target")
  {

   
      std::string triplet;

      triplet += this->target_arch;
      triplet += "-";
      triplet += this->target_vendor;
      triplet += "-";
      triplet += this->target_platform;

      return triplet;
  }
  else if (triplet_type == "builder")
  {
      std::string triplet;

      triplet += this->builder_arch;
      triplet += "-";
      triplet += this->builder_vendor;
      triplet += "-";
      triplet += this->builder_platform;

      return triplet;
  }

  if (triplet_type == "host")
  {

      std::string triplet;

      triplet += this->host_arch;
      triplet += "-";
      triplet += this->host_vendor;
      triplet += "-";
      triplet += this->host_platform;

      return triplet;
  }

  return "unknow";
}

void Toolchain::MakeSnapshot(std::string label)
{
    // Create snapshot folder
    VxContext *ctx = VortexMaker::GetCurrentContext();
    std::string envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;
    std::string baseDir = envPath + "/" + this->name;
    std::string snapshotsDir = baseDir + "/" + "snapshots";

    this->RefreshSnapshots();
    for (auto snapshot : this->snapshots)
    {
        if (snapshot.name == label)
        {
            VortexMaker::LogError("Core", "Snapshot with label " + label + " already exists.");

            // détecter si le dernier charactere de label est un nombre, si oui ajouter +1 sinon ajouter _1 a la fin
            std::string lastChar = label.substr(label.size() - 1);
            int number = 1;

            bool flag = true; // We initialise flag as true.
            for (int i = 0; i < lastChar.length(); i++)
            {
                if (isdigit(lastChar[i]) == false)
                {
                    flag = false;
                    number = isdigit(lastChar[i]);
                    break;
                }
            }

            if (flag == true)
            { // It means that the previous for loop found each character to be a digit.
                label.pop_back();
                number++;
                label += std::to_string(number);
            }
            else
            {
                label += "-1";
            }
        }
    }


    if (mkdir(snapshotsDir.c_str(), 0777) == -1)
    {
        perror("Error while creating folder");
    }

    this->path_hostsnapshots = snapshotsDir;

    std::string path = this->path_hostsnapshots + "/" + label;
    std::string SnapshotFolderCreation = "sudo mkdir " + path;
    system(SnapshotFolderCreation.c_str());

    // Create base snapshot config file
    nlohmann::json distToolchainJson;
    distToolchainJson["snapshot"]["name"] = label;
    std::ofstream outputFile(path + "/snapshot.config");

    if (outputFile.is_open())
    {
        outputFile << std::setw(4) << distToolchainJson << std::endl;
        outputFile.close();
    }
    else
    {
        std::cerr << "Error while creating the toolchain dist config file." << std::endl;
    }

    // Compress current working_host into snapshot folder
    std::string CopyConfig = "sudo cp " + this->workingPath + "/working_host.config " + path;
    system(CopyConfig.c_str());

    std::string Compress = " cd " + this->workingPath + "/../ && sudo tar -czvf " + path + "/working_host.tar.gz working_host";
    system(Compress.c_str());
}

void Toolchain::RetakeSnapshot(std::string snapshot_label)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();
    std::string envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;
    std::string baseDir = envPath + "/" + this->name;
    std::string snapshotsDir = baseDir + "/" + this->GetTriplet("target");

    this->DeleteCurrentToolchainSystem();

    // Recréer un dossier workingPaht
    std::string CreateWorkingHost = "sudo mkdir " + this->workingPath;
    system(CreateWorkingHost.c_str());

    // Find snapshot by label
    for (auto snapshot : this->snapshots)
    {
        if (snapshot.name == snapshot_label)
        {
            // Decompresser le tarball présent dans snapshot.path et le mettre dans workingPath
            std::string Decompress = " cd " + snapshot.path + " && sudo tar -xzvf " + "working_host.tar.gz -C " + this->workingPath + "/../";
            system(Decompress.c_str());
        }
    }

    this->RefreshCurrentWorkingToolchain();

    // Uncompress working_host tarball into main host folder
    // Set working host instance into current working host (refresh)
}

void Toolchain::RefreshCurrentWorkingToolchain()
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
  this->currentLoadedSystem.parent = std::make_shared<Toolchain>(*this);
  this->currentLoadedSystem.Populate(data);
  this->haveCurrentSys = true;
}

void Toolchain::RefreshSnapshots()
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
      ToolchainSnapshot newsnapshot;

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

void Toolchain::RefreshDistConfig()
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


void Toolchain::DeleteCurrentToolchainSystem()
{
  // Delete working_host directory and recreate a new one
  std::string DeleteWorkingHost = "sudo rm -rf " + this->workingPath;
  ToolchainCurrentSystem newCurrentSystem; // To erase
  this->currentLoadedSystem = newCurrentSystem;
  
  system(DeleteWorkingHost.c_str());
}

void Toolchain::CreateCurrentToolchainSystem()
{
  this->DeleteCurrentToolchainSystem();
  // Recréer un dossier workingPaht
  std::string CreateWorkingHost = "sudo mkdir " + this->workingPath;
  system(CreateWorkingHost.c_str());

  // Create working_host.config into WorkingPath

  ToolchainCurrentSystem newCurrentSystem;
  newCurrentSystem.parent = std::make_shared<Toolchain>(*this);
  nlohmann::json working_host_config = newCurrentSystem.Extract();

  std::ofstream outputFile(this->workingPath + "/working_host.config");

  // Populate file with working_host_config
  if (outputFile.is_open())
  {
    outputFile << std::setw(4) << working_host_config << std::endl;
    outputFile.close();
  }
  else
  {
    std::cerr << "Error while creating the working_host.config file." << std::endl;
  }

  this->haveCurrentSys = true;
}


std::pair<std::string, int> Toolchain::exec_cmd_quote(const std::string& cmd) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  std::string output;
  int returnCode = -1;

  std::string _cmd = cmd;
  std::string uid = VortexMaker::gen_random(8);

  _cmd += "' 2>";
  _cmd += ctx->projectPath;
  _cmd += "/.vx/temp/" + uid + ".txt";
  _cmd += "";

      std::string path = ctx->projectPath;
      path += "/.vx/temp/"+uid+".txt";

      returnCode = system((char *)_cmd.c_str());

      std::ifstream outputFile(path);
      output.clear();

        if (outputFile.is_open())
        {
          output.assign(std::istreambuf_iterator<char>(outputFile), std::istreambuf_iterator<char>());
          outputFile.close();
          std::string clearFile = "rm ";
          clearFile +=  ctx->projectPath;
          clearFile += "/.vx/temp/" + uid + ".txt";


          system((char *)clearFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return{"Unknow log(s)", returnCode};
        
      }
      return{output, returnCode};
}


std::pair<std::string, int> Toolchain::exec_cmd(const std::string& cmd) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  std::string output;
  int returnCode = -1;

  std::string uid = VortexMaker::gen_random(8);
  std::string _cmd = cmd;

  _cmd += " 2>";
  _cmd += ctx->projectPath;
  _cmd += "/.vx/temp/" + uid + ".txt";

      std::string path = ctx->projectPath;
      path += "/.vx/temp/"+uid+".txt";

      returnCode = system((char *)_cmd.c_str());

      std::ifstream outputFile(path);
      output.clear();

        if (outputFile.is_open())
        {
          output.assign(std::istreambuf_iterator<char>(outputFile), std::istreambuf_iterator<char>());
          outputFile.close();
          std::string clearFile = "rm ";
          clearFile +=  ctx->projectPath;
          clearFile += "/.vx/temp/" + uid + ".txt";


          system((char *)clearFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return{"null", returnCode};
        
      }
      return{output, returnCode};
}

void Toolchain::RegisterTasklist(const std::string label)
{
  std::shared_ptr<TasklistInterface> newTasklistInterface = std::make_shared<TasklistInterface>();
  newTasklistInterface->label = label;
  registeredTasklists.push_back(newTasklistInterface);
}

void Toolchain::FindTasklists()
{
  VxContext &ctx = *CVortexMaker;

  // Register all finded local packages
  for (const auto &file : VortexMaker::SearchFiles(ctx.toolchainsPath, "tasklist.config"))
  {
    try
    {

      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      // VxPackage newPackage;

      // newPackage.configFilePath = file;

      // Get packages infos

      for (auto registeredTasklist : this->registeredTasklists)
      {
        if (registeredTasklist->label == filecontent["label"].get<std::string>())
        {
          bool already_registered = false;
          for (auto tasklist : this->tasklists)
          {
            if (filecontent["label"].get<std::string>() == tasklist->label)
            {
              already_registered = true;
            }
          }

          if (!already_registered)
          {
            std::shared_ptr<TaskList> newTasklist = std::make_shared<TaskList>();

              newTasklist->configFilePath = file;

            newTasklist->label = filecontent["label"].get<std::string>();

            //this->tasks.clear();
            nlohmann::json tasks = filecontent["tasks"];
            for (auto &t : tasks)
            {
              std::shared_ptr<Task> task = std::make_shared<Task>();
              task->tasktype = t["task"].get<std::string>();
              task->component = t["component"].get<std::string>();

              for(auto env_props : t["env_props"]){
                task->env_props.push_back({env_props["type"].get<std::string>(), env_props["prop"].get<std::string>()});
              }

              task->priority = t["priority"].get<int>();

              newTasklist->list.push_back(task);
            }

            this->tasklists.push_back(newTasklist);
            registeredTasklist->resolved = true;
          }
        }
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }

  std::sort(this->packages.begin(), this->packages.end(), [](const std::shared_ptr<Package> &a, const std::shared_ptr<Package> &b)
            { return a->priority < b->priority; });

  // Register global packages
  for (auto registeredPackage : this->registeredPackages)
  {
    if (registeredPackage->emplacement == "global")
    {
      // Recupérer les packages du ctx.
    }
  }
}


void Toolchain::Refresh()
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

    std::shared_ptr<PackageInterface> newPackageInterface = std::make_shared<PackageInterface>();
    newPackageInterface->label = pkg["label"].get<std::string>();
    newPackageInterface->emplacement = pkg["origin"].get<std::string>();
    newPackageInterface->resolved = false;
    registeredPackages.push_back(newPackageInterface);
  }

  VortexMaker::LogInfo("Core", "Finding packages asset of " + this->name);

  // this->FindPackages();
  {
    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    args->add("packages", this->packages);
    args->add("list", this->registeredPackages);

    VortexMaker::CallModuleEvent(args, "FindPackages", "vortex.modules.builtin.packages");

    this->packages = args->get<std::vector<std::shared_ptr<Package>>>("packages", this->packages);
    this->registeredPackages = args->get<std::vector<std::shared_ptr<PackageInterface>>>("list", this->registeredPackages);
  }

  this->InitTasks();

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

void ToolchainCurrentSystem::Save(std::shared_ptr<Toolchain> parent)
{
  nlohmann::json data = this->Extract();
  std::ofstream file(parent->workingPath + "/working_host.config");
  if (file.is_open())
  {
    file << std::setw(4) << data << std::endl;
    VortexMaker::LogInfo("Core", "Object saved to " + parent->workingPath + "/working_host.config");
    file.close();
  }
  else
  {
    VortexMaker::LogError("Core", "Unable to open file " + parent->workingPath + "/working_host.config" + " for writing!");
  }
}



void Toolchain::PushSave(std::shared_ptr<ToolchainSave> save)
{
  VxContext &ctx = *CVortexMaker;
  nlohmann::json toolchainData;
  toolchainData["toolchain"]["name"] = save->name;
  toolchainData["toolchain"]["author"] = save->author;
  toolchainData["toolchain"]["type"] = save->type;
  toolchainData["toolchain"]["state"] = save->state;
  toolchainData["toolchain"]["version"] = save->version;
  toolchainData["toolchain"]["description"] = save->description;

  toolchainData["configs"]["toolchain_type"] = save->toolchain_type;

  toolchainData["configs"]["target_arch"] = save->target_arch;
  toolchainData["configs"]["target_vendor"] = save->target_vendor;
  toolchainData["configs"]["target_platform"] = save->target_platform;
  toolchainData["configs"]["target_cpu"] = save->target_cpu;
  toolchainData["configs"]["target_fpu"] = save->target_fpu;

  toolchainData["configs"]["builder_arch"] = save->builder_arch;
  toolchainData["configs"]["builder_vendor"] = save->builder_vendor;
  toolchainData["configs"]["builder_platform"] = save->builder_platform;
  toolchainData["configs"]["builder_cpu"] = save->builder_cpu;
  toolchainData["configs"]["builder_fpu"] = save->builder_fpu;

  toolchainData["configs"]["host_arch"] = save->host_arch;
  toolchainData["configs"]["host_vendor"] = save->host_vendor;
  toolchainData["configs"]["host_platform"] = save->host_platform;
  toolchainData["configs"]["host_cpu"] = save->host_cpu;
  toolchainData["configs"]["host_fpu"] = save->host_fpu;

  toolchainData["configs"]["compression"] = save->compression;

  toolchainData["data"]["packages"] = save->localPackagePath;
  toolchainData["data"]["patchs"] = save->localPatchsPath;
  toolchainData["data"]["scripts"] = save->localScriptsPath;

  nlohmann::json registeredPackagesJson;
  for (const auto &package : save->registeredPackages)
  {
    nlohmann::json packageJson;
    packageJson["label"] = std::string(package.first);
    packageJson["origin"] = std::string(package.second);
    registeredPackagesJson.push_back(packageJson);
  }

  nlohmann::json tasklist;
  for (const auto &list : save->registeredTasklists)
  {
    nlohmann::json packageJson;
    packageJson["label"] = list.first;
    tasklist.push_back(packageJson);
  }

  nlohmann::json contents;
  contents["packages"] = registeredPackagesJson;
  contents["tasklists"] = tasklist;

  toolchainData["content"] = contents;

  std::ofstream file(this->configFilePath);
  if (file.is_open())
  {
    file << std::setw(4) << toolchainData << std::endl;
    VortexMaker::LogInfo("Core", "Object saved to " + this->configFilePath);
    file.close();
  }
  else
  {
    VortexMaker::LogError("Core", "Unable to open file " + this->configFilePath + " for writing!");
  }
}

void Toolchain::PushDistSave(std::shared_ptr<VxDistToolchainSave> save)
{
  VxContext *ctx = VortexMaker::GetCurrentContext();
  // TODO Also push metrics, actual packages and states, and all usefull information about a toolchain build
  nlohmann::json toolchainData;
  toolchainData["configs"]["AR"] = save->AR_value;
  toolchainData["configs"]["AS"] = save->AS_value;
  toolchainData["configs"]["CC"] = save->CC_value;
  toolchainData["configs"]["CXX"] = save->CXX_value;
  toolchainData["configs"]["LD"] = save->LD_value;
  toolchainData["configs"]["RANLIB"] = save->RANLIB_value;
  toolchainData["configs"]["STRIP"] = save->STRIP_value;

  std::string distPath = ctx->projectPath;
  distPath += "/" + ctx->paths.toolchainDistFolder + "/" + this->name + "/toolchain.dist.config";

  std::ofstream file(distPath);
  if (file.is_open())
  {
    file << std::setw(4) << toolchainData << std::endl;
    VortexMaker::LogInfo("Core", "Object saved to " + distPath);
    file.close();
  }
  else
  {
    VortexMaker::LogError("Core", "Unable to open file " + distPath + " for writing!");
  }
}

void Toolchain::Init()
{
  VxContext &ctx = *CVortexMaker;

  // TODO: Take all tasks at root, and check if a task has toolchain type, if yes, include it.

  // Add tasks types

  // Load all custom tasks from plugins

  // Get dist working path (for CurrentWorkingToolchain)
  std::string envPath = ctx.projectPath / ctx.paths.toolchainDistFolder;
  std::string baseDir = envPath + "/" + this->name;
  std::string crosstoolsDir = baseDir + "/working_host";
  this->workingPath = crosstoolsDir;

  // this->taskProcessor = std::make_shared<TaskProcessor>();
  // this->taskProcessor->startWorker();
}

TOOLCHAIN_MODULE_API bool ToolchainModule::RegisterNewToolchain(std::shared_ptr<Toolchain> toolchain, nlohmann::json toolchainData)
{
  VxContext &ctx = *CVortexMaker;

  toolchain->name = toolchainData["toolchain"]["name"].get<std::string>();
  toolchain->author = toolchainData["toolchain"]["author"].get<std::string>();
  toolchain->type = toolchainData["toolchain"]["type"].get<std::string>();
  toolchain->description = toolchainData["toolchain"]["description"].get<std::string>();
  toolchain->version = toolchainData["toolchain"]["version"].get<std::string>();
  toolchain->state = toolchainData["toolchain"]["state"].get<std::string>();

  toolchain->builder_vendor = toolchainData["configs"]["builder_vendor"].get<std::string>();
  toolchain->builder_arch = toolchainData["configs"]["builder_arch"].get<std::string>();
  toolchain->builder_platform = toolchainData["configs"]["builder_platform"].get<std::string>();
  toolchain->builder_cpu = toolchainData["configs"]["builder_cpu"].get<std::string>();
  toolchain->builder_fpu = toolchainData["configs"]["builder_fpu"].get<std::string>();

  toolchain->target_vendor = toolchainData["configs"]["target_vendor"].get<std::string>();
  toolchain->target_arch = toolchainData["configs"]["target_arch"].get<std::string>();
  toolchain->target_platform = toolchainData["configs"]["target_platform"].get<std::string>();
  toolchain->target_cpu = toolchainData["configs"]["target_cpu"].get<std::string>();
  toolchain->target_fpu = toolchainData["configs"]["target_fpu"].get<std::string>();

  toolchain->host_vendor = toolchainData["configs"]["host_vendor"].get<std::string>();
  toolchain->host_arch = toolchainData["configs"]["host_arch"].get<std::string>();
  toolchain->host_platform = toolchainData["configs"]["host_platform"].get<std::string>();
  toolchain->host_cpu = toolchainData["configs"]["host_cpu"].get<std::string>();
  toolchain->host_fpu = toolchainData["configs"]["host_fpu"].get<std::string>();

  // toolchain->compressionMode = toolchainData["configs"]["compression"].get<std::string>();

  nlohmann::json packages = toolchainData["content"]["packages"];
  for (auto &pkg : packages)
  {
    std::shared_ptr<PackageInterface> newPackageInterface = std::make_shared<PackageInterface>();
    newPackageInterface->label = pkg["label"].get<std::string>();
    newPackageInterface->emplacement = pkg["origin"].get<std::string>();
    newPackageInterface->resolved = false;
    toolchain->registeredPackages.push_back(newPackageInterface);
  }
  // this->FindPackages();

  {
    VortexMaker::LogInfo("Core", "Finding packages asset of " + toolchain->name);
    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    args->add("packages", toolchain->packages);
    args->add("list", toolchain->registeredPackages);

    VortexMaker::CallModuleEvent(args, "FindPackages", "vortex.modules.builtin.packages");

    toolchain->packages = args->get<std::vector<std::shared_ptr<Package>>>("packages", toolchain->packages);
    toolchain->registeredPackages = args->get<std::vector<std::shared_ptr<PackageInterface>>>("list", toolchain->registeredPackages);
  };



  nlohmann::json tasklists = toolchainData["content"]["tasklists"];
  for (auto &tasklist : tasklists)
  {
    std::shared_ptr<TasklistInterface> newtasklistInterface = std::make_shared<TasklistInterface>();
    newtasklistInterface->label = tasklist["label"].get<std::string>();
    newtasklistInterface->emplacement = tasklist["origin"].get<std::string>();
    newtasklistInterface->resolved = false;
    toolchain->registeredTasklists.push_back(newtasklistInterface);
  }
  
  {
    VortexMaker::LogInfo("Core", "Finding tasklists asset of " + toolchain->name);
    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    args->add("path", toolchain->path);
    args->add("tasklists", toolchain->tasklists);
    args->add("list", toolchain->registeredPackages);

    VortexMaker::CallModuleEvent(args, "FindTasklists", "vortex.modules.builtin.tasklists");

    toolchain->tasklists = args->get<std::vector<std::shared_ptr<TaskList>>>("tasklists", toolchain->tasklists);
    //toolchain->registeredTasklists = args->get<std::vector<std::shared_ptr<TasklistInterface>>>("list", toolchain->registeredTasklists);
  };


  toolchain->Init();

  CToolchainModule->m_toolchains.push_back(toolchain);

  return true;
}

// MODULE FUNCTION