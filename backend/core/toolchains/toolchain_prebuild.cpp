#include "../../../vortex.h"
#include "../../../vortex_internals.h"


void VxToolchain::PreBuild()
{
  VxContext &ctx = *CVortexMaker;
  std::string envPath = ctx.projectPath / ctx.paths.toolchainDistFolder;

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

  this->crosstoolsPath = crosstoolsDir;

  std::string data = baseDir + "/data";
  if (mkdir(data.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  std::string packages_data = baseDir + "/data/packages";
  if (mkdir(packages_data.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  std::string patchs_data = baseDir + "/data/patchs";
  if (mkdir(packages_data.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  std::string scripts_data = baseDir + "/data/scripts";
  if (mkdir(packages_data.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier crosstoolsDir");
  }

  std::string sysrootDir = crosstoolsDir + "/sysroot";
  this->sysrootPath = sysrootDir;
  if (mkdir(sysrootDir.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier sysrootDir");
  }

  std::string debugrootDir = crosstoolsDir + "/debugroot";
  if (mkdir(debugrootDir.c_str(), 0777) == -1)
  {
    perror("Erreur lors de la création du dossier debugrootDir");
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

  std::string bashprofile = "cat > ~/.bash_profile << \"EOF\" \n exec env -i HOME=$HOME TERM=$TERM PS1='\\u:\\w\\$ ' /bin/bash \n EOF";

  std::string bashrc = "cat > ~/.bashrc << \"EOF\" set +h \n umask 022 \n LC_ALL=POSIX \n PATH=/usr/bin \n if [ ! -L /bin ]; then PATH=/bin:" + this->crosstoolsPath + "; fi \n PATH=" + this->crosstoolsPath + "/tools/bin:$PATH \n CONFIG_SITE=" + this->crosstoolsPath + "/usr/share/config.site \n export LFS LC_ALL LFS_TGT PATH CONFIG_SITE \n EOF";

  std::string save_bashprofile = "[ ! -e /etc/bash.bashrc ] || mv -v /etc/bash.bashrc /etc/bash.bashrc.NOUSE";
  std::string reload_bashprofile = "source ~/.bash_profile";

  std::string as_bashprofile = "su - vortex -c \"" + bashprofile + "\"";
  std::string as_bashrc = "su - vortex -c \"" + bashrc + "\"";
  std::string as_save_bashprofile = "su - vortex -c \"" + save_bashprofile + "\"";
  std::string as_reload_bashprofile = "su - vortex -c \"" + reload_bashprofile + "\"";

  system("touch /home/vortex/test.txt");

  for (auto package : this->packages)
  {
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

    std::string cmd = "cp -r " + package->path + " " + packages_data;

    size_t posDernierSlash = package->path.find_last_of('/');
    if (posDernierSlash != std::string::npos)
    {
      std::string resultat = package->path.substr(posDernierSlash + 1);
      package->distPath = packages_data + "/" + resultat;
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

  std::string cuser = "chown vortex:vortex -R " + envPath + "/*";
  system(cuser.c_str());
}
