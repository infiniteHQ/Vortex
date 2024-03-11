#include "../../../vortex.h"
#include "../../../vortex_internals.h"

void VxHost::Init()
{
  VxContext &ctx = *CVortexMaker;
  std::string envPath = ctx.projectPath / ctx.paths.hostDistFolder;

  this->targetTriplet = this->GetTriplet("target");
  this->builderTriplet = this->GetTriplet("builder");
  this->hostTriplet = this->GetTriplet("host");

  std::string baseDir = envPath + "/" + this->name;
  if (mkdir(baseDir.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string crosstoolsDir = baseDir + "/" + "working_host";
  if (mkdir(crosstoolsDir.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string snapshotsDir = baseDir + "/" + "snapshots";
  if (mkdir(snapshotsDir.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string factory = baseDir + "/factory";
  if (mkdir(factory.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string packages_data = baseDir + "/factory/packages";
  if (mkdir(packages_data.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string patchs_data = baseDir + "/factory/patchs";
  if (mkdir(patchs_data.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string scripts_data = baseDir + "/factory/scripts";
  if (mkdir(scripts_data.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string toolchain_data = baseDir + "/factory/toolchains";
  if (mkdir(toolchain_data.c_str(), 0777) == -1){perror("Error while creating folder");}



  // Apply skeleton of host (when prebuild)
  std::string usrDir = crosstoolsDir + "/usr";
  if (mkdir(usrDir.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string usrLibDir = usrDir + "/lib";
  if (mkdir(usrLibDir.c_str(), 0777) == -1){perror("Error while creating folder");}



  this->path_hostroot = crosstoolsDir;
  this->path_hostsnapshots = snapshotsDir;
  this->path_hostfactory = factory;

}

void VxHost::PreBuild()
{
  VxContext &ctx = *CVortexMaker;
  std::string envPath = ctx.projectPath / ctx.paths.hostDistFolder;

  this->targetTriplet = this->GetTriplet("target");
  this->builderTriplet = this->GetTriplet("builder");
  this->hostTriplet = this->GetTriplet("host");

  std::string baseDir = envPath + "/" + this->name;
  if (mkdir(baseDir.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier baseDir");
  }

  std::string crosstoolsDir = baseDir + "/" + this->GetTriplet("target");
  if (mkdir(crosstoolsDir.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  this->host = crosstoolsDir;

  std::string usrDir = crosstoolsDir + "/usr";
  if (mkdir(usrDir.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }
  std::string usrLibDir = usrDir + "/lib";
  if (mkdir(usrLibDir.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  // this->crosstoolsPath = crosstoolsDir;

  std::string data = baseDir + "/data";
  if (mkdir(data.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  this->path_datapackages = baseDir + "/data/packages";
  if (mkdir(this->path_datapackages.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  std::string patchs_data = baseDir + "/data/patchs";
  if (mkdir(patchs_data.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  std::string scripts_data = baseDir + "/data/scripts";
  if (mkdir(scripts_data.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  std::string toolchain_data = baseDir + "/data/toolchains";
  if (mkdir(toolchain_data.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  // other assets

  std::string CMD_AddToGroup = "groupadd vortex";
  std::string CMD_AddUser = "useradd -s /bin/bash -g vortex -m -k /dev/null vortex"; // + " -p " + user.vxHostUser_Crypto;
  std::string CMD_CreateUserDirectory = "mkdir -pv /home/vortex";
  std::string CMD_GiveUserDirectoryToUser = "sudo chown -v vortex:vortex  /home/vortex";
  std::string CMD_AddToSudoers = "sudo usermod -aG root vortex";

  system((char *)CMD_AddToGroup.c_str());
  system((char *)CMD_AddUser.c_str());
  system((char *)CMD_CreateUserDirectory.c_str());
  system((char *)CMD_GiveUserDirectoryToUser.c_str());
  system((char *)CMD_AddToSudoers.c_str());

  // Give toolchain to user
  std::string cmd = "sudo chown -v -R vortex " + baseDir + "/*";
  system((char *)cmd.c_str());
  VortexMaker::RefreshDistToolchains();

  for (auto toolchain : ctx.IO.distToolchains)
  {
    if (this->toolchainToUse == toolchain.name)
    {
      std::string Bashrc = "sudo touch /home/vortex/.bashrc";    
      std::string CC = "sudo echo \"CC=" + toolchain.CC + "\" >> /home/vortex/.bashrc";
      std::string CXX = "sudo echo \"CXX=" + toolchain.CXX + "\" >> /home/vortex/.bashrc";
      std::string AR = "sudo echo \"AR=" + toolchain.AR + "\" >> /home/vortex/.bashrc";
      std::string AS = "sudo echo \"AS=" + toolchain.AS + "\" >> /home/vortex/.bashrc";
      std::string RANLIB = "sudo echo \"RANLIB=" + toolchain.RANLIB + "\" >> /home/vortex/.bashrc";
      std::string LD = "sudo echo \"LD=" + toolchain.LD + "\" >> /home/vortex/.bashrc";
      std::string STRIP = "sudo echo \"STRIP=" + toolchain.STRIP + "\" >> /home/vortex/.bashrc";

        std::unordered_map<std::string, std::string> replacements = {
        {"${DistToolchain}", toolchain.path + "/" + toolchain.target + "/"}
        };

        CC = VortexMaker::replacePlaceholders(CC, replacements);
        CC = VortexMaker::replacePlaceholders(CC, replacements);
        CXX = VortexMaker::replacePlaceholders(CXX, replacements);
        AR = VortexMaker::replacePlaceholders(AR, replacements);
        AS = VortexMaker::replacePlaceholders(AS, replacements);
        LD = VortexMaker::replacePlaceholders(LD, replacements);
        STRIP = VortexMaker::replacePlaceholders(STRIP, replacements);
        RANLIB = VortexMaker::replacePlaceholders(RANLIB, replacements);

        std::cout << system(Bashrc.c_str());
        std::cout << system(CC.c_str());
        std::cout << system(CXX.c_str());
        std::cout << system(AR.c_str());
        std::cout << system(AS.c_str());
        std::cout << system(RANLIB.c_str());
        std::cout << system(LD.c_str());
        std::cout << system(STRIP.c_str());
        std::cout << Bashrc << std::endl;


        // FAIRE PLUTOT UN PATH QUE LES ELEMERNTS

    }
  }

  for (auto package : this->packages)
  {
    if(!package->useChroot){

    package->AddDiag("decompression");
    package->AddDiag("configuration");
    package->AddDiag("compilation");
    package->AddDiag("installation");

    for (auto action : package->actions)
    {
      if (action->type == "command")
      {
        std::string label = "action-" + action->type + "-" + action->executionSequence + "-";
        label += action->priority;

        package->AddDiag(label);
      }
    }

    // TODO: envoi des package a compiler dans le temp puis les traiter en chroot
    // Puis, se reabser dans l'host pour compiler le système final (dans tmp/system)
    // Faire les utilitaire de reprise eet de compilation optimisée

    std::string cmd = "cp -r " + package->path + " " + this->path_datapackages;

    size_t posDernierSlash = package->path.find_last_of('/');
    if (posDernierSlash != std::string::npos)
    {
      std::string resultat = package->path.substr(posDernierSlash + 1);
      package->distPath = this->path_datapackages + "/" + resultat;
    }

    int returnCode = system(cmd.c_str());
    package->SetDiagCode("Prepare", returnCode);

    if (returnCode == 0)
    {
      // package->SetDiagStatus("Prepare", "Success");
    }
    else
    {
      // package->SetDiagStatus("Prepare", "Fail");
    }
    }
  }

  // other assets



  // Give toolchain to user
  // std::string ghj = "sudo chown -v -R vortex " + ctx.projectPath.c_str();
  // system((char *)ghj.c_str());

  // Add the toolchain PATH to env

  std::string cuser = "chown vortex:vortex -R " + envPath + "/*";
  system(cuser.c_str());
}
