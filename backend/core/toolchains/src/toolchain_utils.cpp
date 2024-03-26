#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"

// Import tasks
#include "../tasks/t_toolchain_SetupDistEnvironment.inl"
#include "../tasks/t_toolchain_CreateTemporaryUser.inl"
#include "../tasks/t_toolchain_DeleteTemporaryUser.inl"
#include "../tasks/t_toolchain_ClearDistPackage.inl"
#include "../tasks/t_toolchain_MovePackageToDist.inl"
#include "../tasks/t_toolchain_GiveToolchainToTemporaryUser.inl"
#include "../tasks/t_toolchain_UncompressDistPackage.inl"

// Packages Building in toolchjain
#include "../tasks/t_toolchain_CreateBuildEnv.inl"
#include "../tasks/t_toolchain_BuildDistPackage.inl"
#include "../tasks/t_toolchain_ConfigureDistPackage.inl"
#include "../tasks/t_toolchain_CompileDistPackage.inl"
#include "../tasks/t_toolchain_InstallDistPackage.inl"


#include "../tasks/t_toolchain_ExecuteTaskList.inl"


void VxToolchainCurrentSystem::CreateTask(std::string tasktype, std::string component, std::string uniqueID, int priority, std::shared_ptr<hArgs> props)
{
  VxContext &ctx = *CVortexMaker;
  for (auto task : this->parent->tasks)
  {
    if (task->tasktype == tasktype)
    {

      task->id = uniqueID;
      task->component = component;
      task->priority = priority;
      task->props = props;
      task->state = "not_started";

      // Ajout de la tâche aux listes appropriées
      this->parent->taskProcessor->tasksToProcess.push_back(task);
      ctx.IO.tasks.push_back(task);

      this->parent->tasks.push_back(task);
    }
    else
    {
      // error api : event not exist
    }
  }
}


void VxToolchain::ExecuteTask(Task task, hArgs args){

}



// Task succeded = last task
// Plutot le faire dans le current system
bool VxToolchain::TaskSuccedded(std::string label){
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

void VxToolchain::DeleteCurrentToolchainSystem()
{
  // Delete working_host directory and recreate a new one
  std::string DeleteWorkingHost = "sudo rm -rf " + this->workingPath;
  VxToolchainCurrentSystem newCurrentSystem; // To erase
  this->currentLoadedSystem = newCurrentSystem;
  
  system(DeleteWorkingHost.c_str());
}

void VxToolchain::CreateCurrentToolchainSystem()
{
  this->DeleteCurrentToolchainSystem();
  // Recréer un dossier workingPaht
  std::string CreateWorkingHost = "sudo mkdir " + this->workingPath;
  system(CreateWorkingHost.c_str());

  // Create working_host.config into WorkingPath

  VxToolchainCurrentSystem newCurrentSystem;
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

std::pair<std::string, int> VxToolchain::exec_cmd_quote(const std::string& cmd) {
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


std::pair<std::string, int> VxToolchain::exec_cmd(const std::string& cmd) {
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

void VxToolchain::Init()
{
  VxContext &ctx = *CVortexMaker;

  // Add tasks types
  {
    std::shared_ptr<SetupDistEnvironment> task = std::make_shared<SetupDistEnvironment>();
    task->tasktype = "SetupDistEnvironment";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<BuildDistPackage> task = std::make_shared<BuildDistPackage>();
    task->tasktype = "BuildDistPackage";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<ExecuteTaskList> task = std::make_shared<ExecuteTaskList>();
    task->tasktype = "ExecuteTaskList";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<CreateTemporaryUser> task = std::make_shared<CreateTemporaryUser>();
    task->tasktype = "CreateTemporaryUser";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<CreateBuildEnv> task = std::make_shared<CreateBuildEnv>();
    task->tasktype = "CreateBuildEnv";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<InstallDistPackage> task = std::make_shared<InstallDistPackage>();
    task->tasktype = "InstallDistPackage";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<CompileDistPackage> task = std::make_shared<CompileDistPackage>();
    task->tasktype = "CompileDistPackage";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<ConfigureDistPackage> task = std::make_shared<ConfigureDistPackage>();
    task->tasktype = "ConfigureDistPackage";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<DeleteTemporaryUser> task = std::make_shared<DeleteTemporaryUser>();
    task->tasktype = "DeleteTemporaryUser";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<MovePackageToDist> task = std::make_shared<MovePackageToDist>();
    task->tasktype = "MovePackageToDist";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<UncompressDistPackage> task = std::make_shared<UncompressDistPackage>();
    task->tasktype = "UncompressDistPackage";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<ClearDistPackage> task = std::make_shared<ClearDistPackage>();
    task->tasktype = "ClearDistPackage";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<GiveToolchainToTemporaryUser> task = std::make_shared<GiveToolchainToTemporaryUser>();
    task->tasktype = "GiveToolchainToTemporaryUser";
    this->tasks.push_back(task);
  }


  // Load all custom tasks from plugins


  // Get dist working path (for CurrentWorkingToolchain)
  std::string envPath = ctx.projectPath / ctx.paths.toolchainDistFolder;
  std::string baseDir = envPath + "/" + this->name;
  std::string crosstoolsDir = baseDir + "/working_host";
  this->workingPath = crosstoolsDir;


  this->taskProcessor = std::make_shared<TaskProcessor>();
  this->taskProcessor->startWorker();
}

std::string VxToolchain::GetTriplet(std::string triplet_type)
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

void VxToolchain::RegisterPackage(const std::string label, const std::string emplacemement)
{
  std::shared_ptr<VxPackageInterface> newPackageInterface = std::make_shared<VxPackageInterface>();
  newPackageInterface->label = label;
  newPackageInterface->emplacement = emplacemement;
  newPackageInterface->resolved = false;
  registeredPackages.push_back(newPackageInterface);
}


void VxToolchain::CreateTasklist(std::string name, std::shared_ptr<ToolchainSave> save){
  VxContext *ctx = VortexMaker::GetCurrentContext();


    std::string path = ctx->projectPath.c_str();
    path += "/.vx/data/toolchains/" + this->name + "/data/tasklists/" + name;

  // Create the tasklist folder
  {
    std::string cmd = "mkdir " + path;
    system(cmd.c_str());
  }

  {
    // Create the tasklist config file
    std::string cmd = "touch " + path + "/tasklist.config";
    system(cmd.c_str());
  }

  // Populate config file
  {
    nlohmann::json tasklist_config;
    tasklist_config["label"] = name;
    tasklist_config["tasks"] = nlohmann::json::array();

      // Store this into toolchain.config
      std::ofstream o(path + "/tasklist.config");
      o << std::setw(4) << tasklist_config << std::endl;
      o.close();

    //VortexMaker::RefreshToolchains();
  }

}

void VxToolchain::DeleteTasklist(std::string name){

  VxContext *ctx = VortexMaker::GetCurrentContext();


    std::string path = ctx->projectPath.c_str();
    path += "/.vx/data/toolchain/" + this->name + "/data/tasklists/" + name;

  // Create the tasklist folder
  {
    std::string cmd = "rm -rf " + path;
    system(cmd.c_str());
  }
  
  VortexMaker::RefreshToolchains();


}
