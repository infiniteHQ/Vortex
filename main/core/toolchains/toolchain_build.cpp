#include "../../../main/include/vortex.h"
#include "../../../main/include/vortex_internals.h"

// TODO: DELETE

void VxToolchain::Build()
{

  std::sort(this->packages.begin(), this->packages.end(), [](const std::shared_ptr<VxPackage> &a, const std::shared_ptr<VxPackage> &b)
            { return a->priority < b->priority; });

  for (auto packageToBuild : this->packages)
  {
    std::unordered_map<std::string, std::string> replacements = {
        {"${Target}", this->targetTriplet},
        {"${Build}", this->builderTriplet},
        {"${Host}", this->hostTriplet},
        {"${Crosstools}", this->crosstoolsPath},
        {"${Sysroot}", this->sysrootPath},
        {"${PackageFolder}", packageToBuild->path},
        {"${DistPackageFolder}", packageToBuild->distPath}};

    // If décompréssé = decompression
    std::string path;
    if (packageToBuild->compressed == ".tar.xz" || packageToBuild->compressed == ".tar.gz"|| packageToBuild->compressed == ".tar.bz2")
    {
      path = VortexMaker::ExtractPackageWithTar(packageToBuild->distPath, packageToBuild->fileName);
      packageToBuild->SetDiagCode("decompression", 0);
    }
    // else, rentrer dans le package->path

    std::string moveToBuildFolder = "cd " + packageToBuild->distPath + "/" + path + " && mkdir build && cd build";
    packageToBuild->path = path;

    system((char *)moveToBuildFolder.c_str());

    // Configure
    std::string configuration;
    if (packageToBuild->compilation.exclusiveCustomConfigProcess == "not specified")
    {
      std::string cmd = "";

      std::string cmdf = "chown vortex -R " + packageToBuild->distPath;
      system(cmdf.c_str());
      cmd += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";

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

    if (packageToBuild->compilation.exclusiveCustomConfigProcess == "not specified"){
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
    }}

    std::string compilation;
    if (packageToBuild->compilation.exclusiveCustomCompilationProcess == "not specified")
    {
      std::string cmd = "";
      cmd += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";

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

    if (packageToBuild->compilation.exclusiveCustomCompilationProcess == "not specified"){
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
    }}

    std::string installatiobn;
    if (packageToBuild->compilation.exclusiveCustomInstallationProcess == "not specified")
    {
      std::string cmd = "";
      cmd += "sudo -u vortex -i sh -c 'cd " + packageToBuild->distPath + "/" + path + "/build && ";

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

    if (packageToBuild->compilation.exclusiveCustomInstallationProcess == "not specified"){
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
