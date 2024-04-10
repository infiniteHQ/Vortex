#include "../../../main/include/vortex.h"
#include "../../../main/include/vortex_internals.h"
#include "./tasks/utils/H_U_LoadToolchain.inl"

void VxHost::ExecuteTask(Task t, hArgs args){
/*

  // Creaate Task Report
  std::shared_ptr<VxHostTaskReport> report = std::make_shared<VxHostTaskReport>();
  this->currentLoadedSystem.reports.push_back(report);

  report->parent = t;
  report->state = "execution";
  

  if(t.task == "fullBuildPackage"){
    std::shared_ptr<VxPackage> package = args.get<std::shared_ptr<VxPackage>>("pacakge", nullptr);
    // PreparePackage
    // ConfigurePackage
    // CompilePackage
    // install package

  }
  else if(t.task == "testpackage"){
    report->parent.uniqueID = report->parent.component + "-testpackage";
    report->state = "finish";
    report->description = "Everything is ok";
    report->result = "success";

  }
  else if(t.task == "test"){
    report->state = "finish";
    report->description = "Everything is ok";
    report->result = "success";

  }
  else{
    //custom task...
  }


  // Update working_host.config with this report (for future initialization)*/
}


void VxHost::RegisterPackage(const std::string label, const std::string emplacemement)
{
  std::shared_ptr<VxPackageInterface> newPackageInterface = std::make_shared<VxPackageInterface>();
  newPackageInterface->label = label;
  newPackageInterface->emplacement = emplacemement;
  newPackageInterface->resolved = false;
  registeredPackages.push_back(newPackageInterface);
}

void VxHost::RegisterTasklist(const std::string label)
{
  std::shared_ptr<VxTasklistInterface> newTasklistInterface = std::make_shared<VxTasklistInterface>();
  newTasklistInterface->label = label;
  registeredTasklists.push_back(newTasklistInterface);
}


void VxHost::DeleteCurrentHostSystem()
{
  // Delete working_host directory and recreate a new one
  std::string DeleteWorkingHost = "sudo rm -rf " + this->workingPath;
  VxHostCurrentSystem newCurrentSystem; // To erase
  this->currentLoadedSystem = newCurrentSystem;
  
  system(DeleteWorkingHost.c_str());
}

void VxHost::CreateCurrentHostSystem()
{
  this->DeleteCurrentHostSystem();
  // Recréer un dossier workingPaht
  std::string CreateWorkingHost = "sudo mkdir " + this->workingPath;
  system(CreateWorkingHost.c_str());

  // Create working_host.config into WorkingPath

  VxHostCurrentSystem newCurrentSystem;
  newCurrentSystem.parent = std::make_shared<VxHost>(*this);
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


void VxHost::Init()
{
  VxContext &ctx = *CVortexMaker;

  // TODO: Take all tasks at root, and check if a task has toolchain type, if yes, include it.

  // Add tasks types
  {
    std::shared_ptr<H_U_LoadToolchain> task = std::make_shared<H_U_LoadToolchain>();
    task->tasktype = "H_U_LoadToolchain";
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

int VxHost::ExecuteCmdInChroot(std::string cmd)
{

  std::string path = this->host;

  std::string CMD_CreateDirectories = "mkdir -pv " + path + "/{dev,proc,sys,run}";
  std::string CMD_Console = "mknod -m 600 " + path + "/dev/console c 5 1";
  std::string CMD_Null = "mknod -m 666 " + path + "/dev/null c 1 3";
  std::string CMD_MountDev = "mount -v --bind /dev " + path + "/dev";
  std::string CMD_MountDevPTS = "mount -v --bind /dev/pts " + path + "/dev/pts";
  std::string CMD_MountProc = "mount -vt proc proc " + path + "/proc";
  std::string CMD_MountSysfs = "mount -vt sysfs sysfs " + path + "/sys";
  std::string CMD_MountTmpFs = "mount -vt tmpfs tmpfs " + path + "/run";

  system((char *)CMD_CreateDirectories.c_str());
  system((char *)CMD_Console.c_str());
  system((char *)CMD_Null.c_str());
  system((char *)CMD_MountDev.c_str());
  system((char *)CMD_MountDevPTS.c_str());
  system((char *)CMD_MountProc.c_str());
  system((char *)CMD_MountSysfs.c_str());
  system((char *)CMD_MountTmpFs.c_str());

  std::string CMD_Simlink = "if [ -h " + path + "/dev/shm ]; then \n mkdir -pv " + path + "/$(readlink " + path + "/dev/shm) else mount -t tmpfs -o nosuid,nodev tmpfs " + path + "/dev/shm \n fi";
  std::cout << CMD_Simlink << std::endl;
  system((char *)CMD_Simlink.c_str());

  std::string CMD_Chroot = "chroot \"" + path + "\" /usr/bin/env -i HOME=/root TERM=\"$TERM\" PS1='(vortex chroot) \\u:\\w\\$ ' PATH=/usr/bin:/usr/sbin:" + path + "\" " + "/bin " + "\" " + "/usr/bin/bash --login";

  CMD_Chroot += " -c '" + cmd + "'";

  std::cout << CMD_Chroot << std::endl;
  int result = system((char *)CMD_Chroot.c_str());

  std::string CMD_MountPoint = "mountpoint -q " + path + "/dev/shm && umount " + this->host + "/dev/shm";
  std::string CMD_UnmountDev = "umount " + path + "/dev/pts";
  std::string CMD_Unmount = "umount " + path + "/{sys,proc,run,dev}";

  system((char *)CMD_MountPoint.c_str());
  system((char *)CMD_UnmountDev.c_str());
  system((char *)CMD_Unmount.c_str());

  return result;
}

std::string VxHost::GetTriplet(std::string triplet_type)
{
  
        std::cout << "Yan3" << std::endl;
  std::string NativeTriplet;
        std::cout << "Yan3" + triplet_type<< std::endl;
  FILE *pipe = popen("gcc -dumpmachine", "r");
  if (!pipe)
  {
    perror("Error while try to get triplet with gcc !");
    return "unknow";
  }
        std::cout << "Yan3" << std::endl;
  char buffer[128];
  while (fgets(buffer, 128, pipe) != NULL)
  {
    NativeTriplet += buffer;
  }
        std::cout << "Yan3" << std::endl;
  pclose(pipe);
  NativeTriplet.pop_back();

        std::cout << "Yan3" << std::endl;
  if (triplet_type == "target")
  {

        std::cout << "Yan3" << std::endl;
    if (this->type == "native")
    {
        std::cout << "Yan3" << std::endl;
      return NativeTriplet;
    }
    else if (this->type == "cross")
    {
        std::cout << "Yan3" << std::endl;
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
        std::cout << "Yan3" << std::endl;
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
        std::cout << "Yan3" << std::endl;
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
