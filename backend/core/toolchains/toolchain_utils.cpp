#include "../../../vortex.h"
#include "../../../vortex_internals.h"

// Import tasks
#include "./tasks/t_toolchain_SetupSkeleton.hpp"
#include "./tasks/t_toolchain_CreateTemporaryUser.hpp"

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
      ctx.IO.tasksToProcess.push_back(task);
      ctx.IO.tasks.push_back(task);

      this->parent->tasks.push_back(task);
    }
    else
    {
      // error api : event not exist
    }
  }
}



void VxToolchain::Init()
{
  VxContext &ctx = *CVortexMaker;

  // Add tasks types
  std::shared_ptr<SetupSkeleton> newSetupSkeleton = std::make_shared<SetupSkeleton>();
  newSetupSkeleton->tasktype = "SetupSkeleton";
  this->tasks.push_back(newSetupSkeleton);

  std::shared_ptr<CreateTemporaryUser> newCreateTemporaryUser = std::make_shared<CreateTemporaryUser>();
  newCreateTemporaryUser->tasktype = "CreateTemporaryUser";
  this->tasks.push_back(newCreateTemporaryUser);

  // Get dist working path (for CurrentWorkingToolchain)
  std::string envPath = ctx.projectPath / ctx.paths.toolchainDistFolder;
  std::string baseDir = envPath + "/" + this->name;
  std::string crosstoolsDir = baseDir + "/" + this->GetTriplet("target");
  this->workingPath = crosstoolsDir;
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
