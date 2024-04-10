#include "../../../main/include/vortex.h"
#include "../../../main/include/vortex_internals.h"
#include "./tasks_host.h"

void fullBuildPackage::exec() {
    this->start();

        std::cout << "Yan" << std::endl;
    this->addIdleCheck("deployement");    
    this->addIdleCheck("preparation");    
    this->addIdleCheck("decompression");    
    this->addIdleCheck("configuration");
    this->addIdleCheck("compilation");
    this->addIdleCheck("installation");
    this->addIdleCheck("post_process");
        std::cout << "Yanpm" << std::endl;


  VxContext &ctx = *CVortexMaker;
  std::string envPath = ctx.projectPath / ctx.paths.hostDistFolder;

        std::cout << "Yan2" << std::endl;
  this->parent->targetTriplet = this->parent->GetTriplet("target");
  this->parent->builderTriplet = this->parent->GetTriplet("builder");
  this->parent->hostTriplet = this->parent->GetTriplet("host");
        std::cout << "Yan3" << std::endl;

  std::string baseDir = envPath + "/" + this->parent->name;
  if (mkdir(baseDir.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string crosstoolsDir = baseDir + "/" + "working_host";
  if (mkdir(crosstoolsDir.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string snapshotsDir = baseDir + "/" + "snapshots";
  if (mkdir(snapshotsDir.c_str(), 0777) == -1){perror("Error while creating folder");}

        std::cout << "Yan" << std::endl;
  std::string factory = baseDir + "/factory";
  if (mkdir(factory.c_str(), 0777) == -1){perror("Error while creating folder");}

  this->parent->path_datapackages = baseDir + "/factory/packages";
  if (mkdir(this->parent->path_datapackages.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string patchs_data = baseDir + "/factory/patchs";
  if (mkdir(patchs_data.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string scripts_data = baseDir + "/factory/scripts";
  if (mkdir(scripts_data.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string toolchain_data = baseDir + "/factory/toolchains";
  if (mkdir(toolchain_data.c_str(), 0777) == -1){perror("Error while creating folder");}


        std::cout << "Yan" << std::endl;

  // Apply skeleton of host (when prebuild)
  std::string usrDir = crosstoolsDir + "/usr";
  if (mkdir(usrDir.c_str(), 0777) == -1){perror("Error while creating folder");}

  std::string usrLibDir = usrDir + "/lib";
  if (mkdir(usrLibDir.c_str(), 0777) == -1){perror("Error while creating folder");}


        std::cout << "Yan" << std::endl;

  this->parent->path_hostroot = crosstoolsDir;
  this->parent->path_hostsnapshots = snapshotsDir;
  this->parent->path_hostfactory = factory;
        std::cout << "Yan" << std::endl;

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
  {

  std::string cmd = "sudo chown -v -R vortex " + baseDir + "/*";
  system((char *)cmd.c_str());
  }
  VortexMaker::RefreshDistToolchains();

  for (auto toolchain : ctx.IO.distToolchains)
  {
    if (this->parent->toolchainToUse == toolchain.name)
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


        // FAIRE PLUTOT UN PATH QUE LES ELEMERNTS

    }
  }

    std::shared_ptr<VxPackage> component;
    bool component_finded = false;

    for(auto package : this->parent->packages){
      std::cout << this->component << " " << package->name << std::endl;
      if(this->component == package->name){
    std::cout << "Finded !" << std::endl;
        component = package;
        component_finded = true;
      }
    }

    if(!component_finded){
      std::string log = "Error : The \"" + this->component + "\" package not exist in \"" + this->parent->name + "\" host ! ";
      this->addCheckVerdict("deployement", "failed", log);    
      std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
      this->finish("unknow", args);
      return;
    }

    std::cout << "FLKJ" << std::endl;
    std::cout << component << std::endl;
    std::cout << component->name << std::endl;




    std::cout << "FLKJ" << std::endl;

    size_t posDernierSlash = component->path.find_last_of('/');
    std::cout << "FLKJ" << std::endl;
    if (posDernierSlash != std::string::npos)
    {
    std::cout << "FLKJ" << std::endl;
      std::string resultat = component->path.substr(posDernierSlash + 1);
    std::cout << "FLKJ" << std::endl;
      component->distPath = this->parent->path_datapackages + "/" + resultat;
    }
    std::cout << "FLKJ" << std::endl;

    std::cout << "FLKJ" << std::endl;
    std::shared_ptr<hArgs> finishprops = std::make_shared<hArgs>();
    std::string state = "finished";

    std::cout << "FLKJ" << std::endl;

  std::string cuser = "chown vortex:vortex -R " + envPath + "/*";
  system(cuser.c_str());

    std::cout << "FLKJ" << std::endl;

        std::cout << "Yan2" << std::endl;
    this->addCheckVerdict("deployement", "success", "Ok");    
    this->addCheckVerdict("preparation", "success", "Ok");    

        std::cout << "Yan" << std::endl;

std::unordered_map<std::string, std::string> replacements = {
        {"${Target}", this->parent->GetTriplet("target")},
        {"${Build}", this->parent->GetTriplet("builder")},
        {"${Host}", this->parent->GetTriplet("host")},
        {"${HostSystem}", this->parent->host},
        {"${PackageFolder}", component->path},
        {"${DistPackageFolder}", component->distPath}};


    if (component->useChroot)
    {
      std::string mkdir = "sudo mkdir " + this->parent->host + "/tmp";
      std::string mkdirSecond = "sudo mkdir " + this->parent->host + "/tmp/packages";
      std::string mkdirThird = "sudo mkdir " + this->parent->host + "/tmp/packages/" + component->label;
      system(mkdir.c_str());
      system(mkdirSecond.c_str());
      system(mkdirThird.c_str());

      std::string cmd = "cp -r " + component->path + "/* " + this->parent->host + "/tmp/packages/" + component->label;



    component->AddDiag("decompression");
    component->AddDiag("configuration");
    component->AddDiag("compilation");
    component->AddDiag("installation");

    for (auto action : component->actions)
    {
      if (action->type == "command")
      {
        std::string label = "action-" + action->type + "-" + action->executionSequence + "-";
        label += action->priority;

        component->AddDiag(label);
      }
    }

    size_t posDernierSlash = component->path.find_last_of('/');
    if (posDernierSlash != std::string::npos)
    {
      std::string resultat = component->path.substr(posDernierSlash + 1);
      component->distPath =  this->parent->host + "/tmp/packages/" + component->label + "/" + resultat;
    }

    int returnCode = system(cmd.c_str());
    //packageToBuild->SetDiagCode("Prepare", returnCode);


      component->distPath = this->parent->host + "/tmp/packages/" + component->label;

      // If décompréssé = decompression
      std::string path;
      if (component->compressed == ".tar.xz" || component->compressed == ".tar.gz" || component->compressed == ".tar.bz2")
      {
        path = VortexMaker::ExtractPackageWithTar(component->distPath, component->fileName);
        component->SetDiagCode("decompression", 0);

        this->addCheckVerdict("decompression", "success", "Ok");    
      }

      std::string moveToBuildFolder = "cd " + component->distPath + "/" + path + " && mkdir build && cd build";
      component->path = path;

      component->distPath = "/tmp/packages/" + component->label;

      system((char *)moveToBuildFolder.c_str());

      std::string cmdf = "chown vortex -R " + component->distPath;
      system(cmdf.c_str());

      // Configure
      std::string configuration;
      if (component->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        std::string cmd = "";

        cmd += "cd " + component->distPath + "/" + path + "/build && ";

        for (auto suffix : component->compilation.configurationSuffixes)
        {
          if (suffix.first == "all" || suffix.first == this->parent->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += "../configure ";

        for (auto parameter : component->compilation.configurationPrefixes)
        {
          if (parameter.first == "all" || parameter.first == this->parent->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        configuration = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        configuration += "cd " + component->distPath + "/" + path + "/build && ";
        configuration += VortexMaker::replacePlaceholders(component->compilation.exclusiveCustomConfigProcess, replacements);
      }

      component->ExecuteActions("preconfig", component);
      std::cout << "Configuration : " << configuration << std::endl;

      std::string errortxt = "cd " +  this->parent->host + "/tmp/packages/" + component->label + "/" + path + "/build && touch error_output.txt'";
      configuration += " 2>error_output.txt";


{

        int result = this->parent->ExecuteCmdInChroot(configuration);
}

      std::ifstream errorFile( this->parent->host + "/tmp/packages/" + component->label + "/" + path + "/build/error_output.txt");
      std::string errorMessage;

      if (component->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        if (errorFile.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile), std::istreambuf_iterator<char>());
          errorFile.close();
          component->SetDiagOutput("configuration", errorMessage);


          this->addCheckVerdict("configuration", "success", errorMessage);    

          std::string rmOutputFile = "rm " + this->parent->host + "/tmp/packages/" + component->label + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }



      std::string compilation;
      if (component->compilation.exclusiveCustomCompilationProcess == "not specified")
      {
        std::string cmd = "";
        cmd += "cd " + component->distPath + "/" + path + "/build && ";

        for (auto suffix : component->compilation.compilationSuffixes)
        {
          if (suffix.first == "all" || suffix.first == this->parent->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        if (component->compilation.useCompilationOptimization)
        {
          cmd += " make -j6";
        }
        else
        {
          cmd += " make";
        }

        for (auto parameter : component->compilation.compilationPrefixes)
        {
          if (parameter.first == "all" || parameter.first == this->parent->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        compilation = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        compilation += "cd " + component->distPath + "/" + path + "/build && ";
        compilation += VortexMaker::replacePlaceholders(component->compilation.exclusiveCustomCompilationProcess, replacements);
      }
      component->ExecuteActions("precompile", component);
      errortxt.clear();
      errortxt = "cd " + this->parent->host + "/tmp/packages/" + component->label + "/" + path + "/build && touch error_output.txt'";
      compilation += " 2>error_output.txt";
      std::cout << "Compilation : " << compilation << std::endl;
      {

      int result = this->parent->ExecuteCmdInChroot(compilation);
      }


      std::ifstream errorFile2(this->parent->host + "/tmp/packages/" + component->label + "/" + path + "/build/error_output.txt");
      errorMessage.clear();

      if (component->compilation.exclusiveCustomCompilationProcess == "not specified")
      {
        if (errorFile2.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile2), std::istreambuf_iterator<char>());



          errorFile2.close();
          
          this->addCheckVerdict("compilation", "success", errorMessage);    

          std::string rmOutputFile = "rm " + this->parent->host + "/tmp/packages/" + component->label + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());

        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }


      std::string installatiobn;
      if (component->compilation.exclusiveCustomInstallationProcess == "not specified")
      {
        std::string cmd = "";
        cmd += "cd " + component->distPath + "/" + path + "/build && ";

        for (auto suffix : component->compilation.installationSuffixes)
        {
          if (suffix.first == "all" || suffix.first == this->parent->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += " make install ";

        for (auto parameter : component->compilation.installationPrefixes)
        {
          if (parameter.first == "all" || parameter.first == this->parent->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        installatiobn = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        installatiobn += "cd " + component->distPath + "/" + path + "/build && ";
        installatiobn += VortexMaker::replacePlaceholders(component->compilation.exclusiveCustomInstallationProcess, replacements);
      }
      component->ExecuteActions("preinstall", component);
      std::cout << "Installation : " << installatiobn << std::endl;

      errortxt.clear();
      errortxt = "cd " + component->distPath + "/" + path + "/build && touch error_output.txt";
      installatiobn += " 2>error_output.txt";{

      int result = this->parent->ExecuteCmdInChroot(installatiobn);
      }

      std::ifstream errorFile3(this->parent->host + "/tmp/packages/" + component->label + "/" + path + "/build/error_output.txt");

      if (component->compilation.exclusiveCustomInstallationProcess == "not specified")
      {
        errorMessage.clear();
        if (errorFile3.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile3), std::istreambuf_iterator<char>());
          errorFile3.close();
          this->addCheckVerdict("installation", "success", errorMessage);    
          std::string rmOutputFile = "rm " + this->parent->host + "/tmp/packages/" + component->label + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }

      component->ExecuteActions("postinstall", component);
    
    }
    else
    {
    std::string cmd = "cp -r " + component->path + " " + this->parent->path_datapackages;
    system(cmd.c_str());

      // If décompréssé = decompression
      std::string path;
      if (component->compressed == ".tar.xz" || component->compressed == ".tar.gz" || component->compressed == ".tar.bz2")
      {
        path = VortexMaker::ExtractPackageWithTar(component->distPath, component->fileName);
        component->SetDiagCode("decompression", 0);
      }
      // else, rentrer dans le package->path

      std::string moveToBuildFolder = "cd " + component->distPath + "/" + path + " && mkdir build && cd build";
      component->path = path;

      system((char *)moveToBuildFolder.c_str());

      std::string cmdf = "chown vortex -R " + component->distPath;
      system(cmdf.c_str());

      // Configure
      std::string configuration;
      if (component->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        std::string cmd = "";

        cmd += "sudo -u vortex -i sh -c 'cd " + component->distPath + "/" + path + "/build && ";

        for (auto suffix : component->compilation.configurationPrefixes)
        {
          if (suffix.first == "all" || suffix.first == this->parent->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += "../configure ";

        for (auto parameter : component->compilation.configurationSuffixes)
        {
          if (parameter.first == "all" || parameter.first == this->parent->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        configuration = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        configuration += "sudo -u vortex -i sh -c 'cd " + component->distPath + "/" + path + "/build && ";
        configuration += VortexMaker::replacePlaceholders(component->compilation.exclusiveCustomConfigProcess, replacements);
      }

      component->ExecuteActions("preconfig", component);
      std::cout << "Configuration : " << configuration << std::endl;

      std::string errortxt = "sudo -u vortex -i sh -c 'cd " + component->distPath + "/" + path + "/build && touch error_output.txt'";
      configuration += " 2>error_output.txt";

      configuration += "'";{

      int result = system((char *)configuration.c_str());
      }

      std::ifstream errorFile(component->distPath + "/" + path + "/build/error_output.txt");
      std::string errorMessage;

      if (component->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        if (errorFile.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile), std::istreambuf_iterator<char>());
          errorFile.close();  
          this->addCheckVerdict("configuration", "success", errorMessage);  

          std::string rmOutputFile = "rm " + component->distPath + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }

      std::string compilation;
      if (component->compilation.exclusiveCustomCompilationProcess == "not specified")
      {
        std::string cmd = "";
        cmd += "sudo -u vortex -i sh -c 'cd " + component->distPath + "/" + path + "/build && ";

        for (auto suffix : component->compilation.compilationPrefixes)
        {
          if (suffix.first == "all" || suffix.first == this->parent->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        if (component->compilation.useCompilationOptimization)
        {
          cmd += " make -j6";
        }
        else
        {
          cmd += " make";
        }

        for (auto parameter : component->compilation.compilationSuffixes)
        {
          if (parameter.first == "all" || parameter.first == this->parent->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        compilation = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        compilation += "sudo -u vortex -i sh -c 'cd " + component->distPath + "/" + path + "/build && ";
        compilation += VortexMaker::replacePlaceholders(component->compilation.exclusiveCustomCompilationProcess, replacements);
      }
      component->ExecuteActions("precompile", component);
      errortxt.clear();
      errortxt = "sudo -u vortex -i sh -c 'cd " + component->distPath + "/" + path + "/build && touch error_output.txt'";
      compilation += " 2>error_output.txt";
      std::cout << "Compilation : " << compilation << std::endl;
      compilation += "'";
      int resutl = system((char *)compilation.c_str());

      std::ifstream errorFile2(component->distPath + "/" + path + "/build/error_output.txt");
      errorMessage.clear();

      if (component->compilation.exclusiveCustomCompilationProcess == "not specified")
      {
        if (errorFile2.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile2), std::istreambuf_iterator<char>());
          errorFile2.close();
          this->addCheckVerdict("compilation", "success", errorMessage); 
          std::string rmOutputFile = "rm " + component->distPath + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }

      std::string installatiobn;
      if (component->compilation.exclusiveCustomInstallationProcess == "not specified")
      {
        std::string cmd = "";
        cmd += "sudo -u vortex -i sh -c 'cd " + component->distPath + "/" + path + "/build && ";

        for (auto suffix : component->compilation.installationPrefixes)
        {
          if (suffix.first == "all" || suffix.first == this->parent->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += " make install ";

        for (auto parameter : component->compilation.installationSuffixes)
        {
          if (parameter.first == "all" || parameter.first == this->parent->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        installatiobn = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        installatiobn += "sudo -u vortex -i sh -c 'cd " + component->distPath + "/" + path + "/build && ";
        installatiobn += VortexMaker::replacePlaceholders(component->compilation.exclusiveCustomInstallationProcess, replacements);
      }
      component->ExecuteActions("preinstall", component);
      std::cout << "Installation : " << installatiobn << std::endl;

      errortxt.clear();
      errortxt = "cd " + component->distPath + "/" + path + "/build && touch error_output.txt";
      installatiobn += " 2>error_output.txt";
      installatiobn += "'";{

      int result = system((char *)installatiobn.c_str());
      }

      std::ifstream errorFile3(component->distPath + "/" + path + "/build/error_output.txt");

      if (component->compilation.exclusiveCustomInstallationProcess == "not specified")
      {
        errorMessage.clear();
        if (errorFile3.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile3), std::istreambuf_iterator<char>());
          errorFile3.close();
          this->addCheckVerdict("installation", "success", errorMessage); 
          std::string rmOutputFile = "rm " + component->distPath + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }

      component->ExecuteActions("postinstall", component);
          this->addCheckVerdict("post_process", "success", errorMessage); 
    }
  
    this->finish(state, finishprops);
}

void deployPackageToDist::exec() {
    this->start();

    std::shared_ptr<VxHost> host = this->props->get<std::shared_ptr<VxHost>>("host", nullptr);
    std::shared_ptr<VxPackage> component = this->props->get<std::shared_ptr<VxPackage>>("component", nullptr);


    std::string cmd = "cp -r " + component->path + " " + host->path_datapackages;

    std::shared_ptr<hArgs> finishprops = std::make_shared<hArgs>();
    std::string state = "finished";
    this->finish(state, finishprops);
}

// Functions

//(A la fin de chaques tasks, créer un report dans le working_host.config)

// DeploySkeleton
// ExecuteTask

    // Decouper en :
    // (Main) NextTaskEvent();
    // BuildPackage();
    // ExecuteAction();
    // ExecuteScript();

void VxHost::Build()
{

  std::sort(this->packages.begin(), this->packages.end(), [](const std::shared_ptr<VxPackage> &a, const std::shared_ptr<VxPackage> &b)
            { return a->priority < b->priority; });

  for (auto packageToBuild : this->packages)
  {

    std::unordered_map<std::string, std::string> replacements = {
        {"${Target}", this->GetTriplet("target")},
        {"${Build}", this->GetTriplet("builder")},
        {"${Host}", this->GetTriplet("host")},
        {"${HostSystem}", this->host},
        {"${PackageFolder}", packageToBuild->path},
        {"${DistPackageFolder}", packageToBuild->distPath}};


    if (packageToBuild->useChroot)
    {
      std::string mkdir = "sudo mkdir " + this->host + "/tmp";
      std::string mkdirSecond = "sudo mkdir " + this->host + "/tmp/packages";
      std::string mkdirThird = "sudo mkdir " + this->host + "/tmp/packages/" + packageToBuild->label;
      system(mkdir.c_str());
      system(mkdirSecond.c_str());
      system(mkdirThird.c_str());

      std::string cmd = "cp -r " + packageToBuild->path + "/* " + this->host + "/tmp/packages/" + packageToBuild->label;



    packageToBuild->AddDiag("decompression");
    packageToBuild->AddDiag("configuration");
    packageToBuild->AddDiag("compilation");
    packageToBuild->AddDiag("installation");

    for (auto action : packageToBuild->actions)
    {
      if (action->type == "command")
      {
        std::string label = "action-" + action->type + "-" + action->executionSequence + "-";
        label += action->priority;

        packageToBuild->AddDiag(label);
      }
    }

    size_t posDernierSlash = packageToBuild->path.find_last_of('/');
    if (posDernierSlash != std::string::npos)
    {
      std::string resultat = packageToBuild->path.substr(posDernierSlash + 1);
      packageToBuild->distPath =  this->host + "/tmp/packages/" + packageToBuild->label + "/" + resultat;
    }

    int returnCode = system(cmd.c_str());
    //packageToBuild->SetDiagCode("Prepare", returnCode);


      packageToBuild->distPath = this->host + "/tmp/packages/" + packageToBuild->label;

      // If décompréssé = decompression
      std::string path;
      if (packageToBuild->compressed == ".tar.xz" || packageToBuild->compressed == ".tar.gz" || packageToBuild->compressed == ".tar.bz2")
      {
        path = VortexMaker::ExtractPackageWithTar(packageToBuild->distPath, packageToBuild->fileName);
        packageToBuild->SetDiagCode("decompression", 0);
      }

      std::string moveToBuildFolder = "cd " + packageToBuild->distPath + "/" + path + " && mkdir build && cd build";
      packageToBuild->path = path;

      packageToBuild->distPath = "/tmp/packages/" + packageToBuild->label;

      system((char *)moveToBuildFolder.c_str());

      std::string cmdf = "chown vortex -R " + packageToBuild->distPath;
      system(cmdf.c_str());

      // Configure
      std::string configuration;
      if (packageToBuild->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        std::string cmd = "";

        cmd += "cd " + packageToBuild->distPath + "/" + path + "/build && ";

        for (auto suffix : packageToBuild->compilation.configurationSuffixes)
        {
          if (suffix.first == "all" || suffix.first == this->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += "../configure ";

        for (auto parameter : packageToBuild->compilation.configurationPrefixes)
        {
          if (parameter.first == "all" || parameter.first == this->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        configuration = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        configuration += "cd " + packageToBuild->distPath + "/" + path + "/build && ";
        configuration += VortexMaker::replacePlaceholders(packageToBuild->compilation.exclusiveCustomConfigProcess, replacements);
      }

      packageToBuild->ExecuteActions("preconfig", packageToBuild);
      std::cout << "Configuration : " << configuration << std::endl;

      std::string errortxt = "cd " +  this->host + "/tmp/packages/" + packageToBuild->label + "/" + path + "/build && touch error_output.txt'";
      configuration += " 2>error_output.txt";

      packageToBuild->SetDiagCode("configuration", this->ExecuteCmdInChroot(configuration));

      std::ifstream errorFile( this->host + "/tmp/packages/" + packageToBuild->label + "/" + path + "/build/error_output.txt");
      std::string errorMessage;

      if (packageToBuild->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        if (errorFile.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile), std::istreambuf_iterator<char>());
          errorFile.close();
          packageToBuild->SetDiagOutput("configuration", errorMessage);

          std::string rmOutputFile = "rm " + this->host + "/tmp/packages/" + packageToBuild->label + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }


      std::string compilation;
      if (packageToBuild->compilation.exclusiveCustomCompilationProcess == "not specified")
      {
        std::string cmd = "";
        cmd += "cd " + packageToBuild->distPath + "/" + path + "/build && ";

        for (auto suffix : packageToBuild->compilation.compilationSuffixes)
        {
          if (suffix.first == "all" || suffix.first == this->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        if (packageToBuild->compilation.useCompilationOptimization)
        {
          cmd += " make -j6";
        }
        else
        {
          cmd += " make";
        }

        for (auto parameter : packageToBuild->compilation.compilationPrefixes)
        {
          if (parameter.first == "all" || parameter.first == this->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        compilation = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        compilation += "cd " + packageToBuild->distPath + "/" + path + "/build && ";
        compilation += VortexMaker::replacePlaceholders(packageToBuild->compilation.exclusiveCustomCompilationProcess, replacements);
      }
      packageToBuild->ExecuteActions("precompile", packageToBuild);
      errortxt.clear();
      errortxt = "cd " + this->host + "/tmp/packages/" + packageToBuild->label + "/" + path + "/build && touch error_output.txt'";
      compilation += " 2>error_output.txt";
      std::cout << "Compilation : " << compilation << std::endl;
      packageToBuild->SetDiagCode("compilation", this->ExecuteCmdInChroot(compilation));

      std::ifstream errorFile2(this->host + "/tmp/packages/" + packageToBuild->label + "/" + path + "/build/error_output.txt");
      errorMessage.clear();

      if (packageToBuild->compilation.exclusiveCustomCompilationProcess == "not specified")
      {
        if (errorFile2.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile2), std::istreambuf_iterator<char>());
          errorFile2.close();
          packageToBuild->SetDiagOutput("compilation", errorMessage);
          std::string rmOutputFile = "rm " + this->host + "/tmp/packages/" + packageToBuild->label + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }


      std::string installatiobn;
      if (packageToBuild->compilation.exclusiveCustomInstallationProcess == "not specified")
      {
        std::string cmd = "";
        cmd += "cd " + packageToBuild->distPath + "/" + path + "/build && ";

        for (auto suffix : packageToBuild->compilation.installationSuffixes)
        {
          if (suffix.first == "all" || suffix.first == this->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += " make install ";

        for (auto parameter : packageToBuild->compilation.installationPrefixes)
        {
          if (parameter.first == "all" || parameter.first == this->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        installatiobn = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        installatiobn += "cd " + packageToBuild->distPath + "/" + path + "/build && ";
        installatiobn += VortexMaker::replacePlaceholders(packageToBuild->compilation.exclusiveCustomInstallationProcess, replacements);
      }
      packageToBuild->ExecuteActions("preinstall", packageToBuild);
      std::cout << "Installation : " << installatiobn << std::endl;

      errortxt.clear();
      errortxt = "cd " + packageToBuild->distPath + "/" + path + "/build && touch error_output.txt";
      installatiobn += " 2>error_output.txt";
      packageToBuild->SetDiagCode("installation", this->ExecuteCmdInChroot(installatiobn));

      std::ifstream errorFile3(this->host + "/tmp/packages/" + packageToBuild->label + "/" + path + "/build/error_output.txt");

      if (packageToBuild->compilation.exclusiveCustomInstallationProcess == "not specified")
      {
        errorMessage.clear();
        if (errorFile3.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile3), std::istreambuf_iterator<char>());
          errorFile3.close();
          packageToBuild->SetDiagOutput("installation", errorMessage);
          std::string rmOutputFile = "rm " + this->host + "/tmp/packages/" + packageToBuild->label + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }

      packageToBuild->ExecuteActions("postinstall", packageToBuild);
    
    }
    else
    {

      // If décompréssé = decompression
      std::string path;
      if (packageToBuild->compressed == ".tar.xz" || packageToBuild->compressed == ".tar.gz" || packageToBuild->compressed == ".tar.bz2")
      {
        path = VortexMaker::ExtractPackageWithTar(packageToBuild->distPath, packageToBuild->fileName);
        packageToBuild->SetDiagCode("decompression", 0);
      }
      // else, rentrer dans le package->path

      std::string moveToBuildFolder = "cd " + packageToBuild->distPath + "/" + path + " && mkdir build && cd build";
      packageToBuild->path = path;

      system((char *)moveToBuildFolder.c_str());

      std::string cmdf = "chown vortex -R " + packageToBuild->distPath;
      system(cmdf.c_str());

      // Configure
      std::string configuration;
      if (packageToBuild->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        std::string cmd = "";

        cmd += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";

        for (auto suffix : packageToBuild->compilation.configurationPrefixes)
        {
          if (suffix.first == "all" || suffix.first == this->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += "../configure ";

        for (auto parameter : packageToBuild->compilation.configurationSuffixes)
        {
          if (parameter.first == "all" || parameter.first == this->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        configuration = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        configuration += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";
        configuration += VortexMaker::replacePlaceholders(packageToBuild->compilation.exclusiveCustomConfigProcess, replacements);
      }

      packageToBuild->ExecuteActions("preconfig", packageToBuild);
      std::cout << "Configuration : " << configuration << std::endl;

      std::string errortxt = "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && touch error_output.txt'";
      configuration += " 2>error_output.txt";

      configuration += "'";
      packageToBuild->SetDiagCode("configuration", system((char *)configuration.c_str()));

      std::ifstream errorFile(packageToBuild->distPath + "/" + path + "/build/error_output.txt");
      std::string errorMessage;

      if (packageToBuild->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        if (errorFile.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile), std::istreambuf_iterator<char>());
          errorFile.close();
          packageToBuild->SetDiagOutput("configuration", errorMessage);

          std::string rmOutputFile = "rm " + packageToBuild->distPath + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }

      std::string compilation;
      if (packageToBuild->compilation.exclusiveCustomCompilationProcess == "not specified")
      {
        std::string cmd = "";
        cmd += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";

        for (auto suffix : packageToBuild->compilation.compilationPrefixes)
        {
          if (suffix.first == "all" || suffix.first == this->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        if (packageToBuild->compilation.useCompilationOptimization)
        {
          cmd += " make -j6";
        }
        else
        {
          cmd += " make";
        }

        for (auto parameter : packageToBuild->compilation.compilationSuffixes)
        {
          if (parameter.first == "all" || parameter.first == this->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        compilation = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        compilation += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";
        compilation += VortexMaker::replacePlaceholders(packageToBuild->compilation.exclusiveCustomCompilationProcess, replacements);
      }
      packageToBuild->ExecuteActions("precompile", packageToBuild);
      errortxt.clear();
      errortxt = "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && touch error_output.txt'";
      compilation += " 2>error_output.txt";
      std::cout << "Compilation : " << compilation << std::endl;
      compilation += "'";
      packageToBuild->SetDiagCode("compilation", system((char *)compilation.c_str()));

      std::ifstream errorFile2(packageToBuild->distPath + "/" + path + "/build/error_output.txt");
      errorMessage.clear();

      if (packageToBuild->compilation.exclusiveCustomCompilationProcess == "not specified")
      {
        if (errorFile2.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile2), std::istreambuf_iterator<char>());
          errorFile2.close();
          packageToBuild->SetDiagOutput("compilation", errorMessage);
          std::string rmOutputFile = "rm " + packageToBuild->distPath + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }

      std::string installatiobn;
      if (packageToBuild->compilation.exclusiveCustomInstallationProcess == "not specified")
      {
        std::string cmd = "";
        cmd += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";

        for (auto suffix : packageToBuild->compilation.installationPrefixes)
        {
          if (suffix.first == "all" || suffix.first == this->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += " make install ";

        for (auto parameter : packageToBuild->compilation.installationSuffixes)
        {
          if (parameter.first == "all" || parameter.first == this->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        installatiobn = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        installatiobn += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";
        installatiobn += VortexMaker::replacePlaceholders(packageToBuild->compilation.exclusiveCustomInstallationProcess, replacements);
      }
      packageToBuild->ExecuteActions("preinstall", packageToBuild);
      std::cout << "Installation : " << installatiobn << std::endl;

      errortxt.clear();
      errortxt = "cd " + packageToBuild->distPath + "/" + path + "/build && touch error_output.txt";
      installatiobn += " 2>error_output.txt";
      installatiobn += "'";
      packageToBuild->SetDiagCode("installation", system((char *)installatiobn.c_str()));

      std::ifstream errorFile3(packageToBuild->distPath + "/" + path + "/build/error_output.txt");

      if (packageToBuild->compilation.exclusiveCustomInstallationProcess == "not specified")
      {
        errorMessage.clear();
        if (errorFile3.is_open())
        {
          errorMessage.assign(std::istreambuf_iterator<char>(errorFile3), std::istreambuf_iterator<char>());
          errorFile3.close();
          packageToBuild->SetDiagOutput("installation", errorMessage);
          std::string rmOutputFile = "rm " + packageToBuild->distPath + "/" + path + "/build/error_output.txt";
          system((char *)rmOutputFile.c_str());
        }
        else
        {
          std::cerr << "Impossible d'ouvrir le fichier de sortie d'erreur." << std::endl;
          return;
        }
      }

  

      packageToBuild->ExecuteActions("postinstall", packageToBuild);
    }
  }
}
