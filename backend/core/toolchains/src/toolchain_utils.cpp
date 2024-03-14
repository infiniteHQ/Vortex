#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"

// Import tasks
#include "../tasks/t_toolchain_SetupSkeleton.inl"
#include "../tasks/t_toolchain_CreateTemporaryUser.inl"
#include "../tasks/t_toolchain_DeleteTemporaryUser.inl"
#include "../tasks/t_toolchain_BuildDistPackage.inl"
#include "../tasks/t_toolchain_ClearDistPackage.inl"
#include "../tasks/t_toolchain_MovePackageToDist.inl"
#include "../tasks/t_toolchain_GiveToolchainToTemporaryUser.inl"
#include "../tasks/t_toolchain_UncompressDistPackage.inl"




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

std::pair<std::string, int> VxToolchain::exec_cmd(const std::string& cmd) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  std::string output;
  int returnCode = -1;

  std::string _cmd = cmd;

  _cmd += " 2>";
  _cmd += ctx->projectPath;
  _cmd += "/.vx/temp/_o.txt";

      std::string path = ctx->projectPath;
      path += "/.vx/temp/_o.txt";

      returnCode = system((char *)_cmd.c_str());

      std::ifstream outputFile(path);
      output.clear();

        if (outputFile.is_open())
        {
          output.assign(std::istreambuf_iterator<char>(outputFile), std::istreambuf_iterator<char>());
          outputFile.close();
          std::string clearFile = "rm ";
          clearFile +=  ctx->projectPath;
          clearFile += "/.vx/temp/_o.txt";


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
    std::shared_ptr<SetupSkeleton> task = std::make_shared<SetupSkeleton>();
    task->tasktype = "SetupSkeleton";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<BuildDistPackage> task = std::make_shared<BuildDistPackage>();
    task->tasktype = "BuildDistPackage";
    this->tasks.push_back(task);
  }
  {
    std::shared_ptr<CreateTemporaryUser> task = std::make_shared<CreateTemporaryUser>();
    task->tasktype = "CreateTemporaryUser";
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

    if (this->type == "native")
    {
      return NativeTriplet;
    }
    else if (this->type == "cross")
    {
      return NativeTriplet;
    }
    else if (this->type == "cross-native" || this->type == "canadian" || this->type == "custom")
    {
      std::string SpecifiedTriplet;

      SpecifiedTriplet += this->target_arch;
      SpecifiedTriplet += "-";
      SpecifiedTriplet += this->vendor;
      SpecifiedTriplet += "-";
      SpecifiedTriplet += this->platform;

      return SpecifiedTriplet;
    }
    else
    {
      return "unknow";
    }
  }
  else if (triplet_type == "builder")
  {

    if (this->type == "native")
    {
      return NativeTriplet;
    }
    else if (this->type == "cross")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "cross-native")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "canadian")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "custom")
    {
      // -> Return Custom triplet
    }
    else
    {
      return "unknow";
    }
  }

  if (triplet_type == "host")
  {

    if (this->type == "native")
    {
      return NativeTriplet;
    }
    else if (this->type == "cross")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "cross-native")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "canadian")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "custom")
    {
      // -> Return Custom triplet
    }
    else
    {
      return "unknow";
    }
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
