#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"


#ifndef __PACKAGE__MODULE_H__
#define __PACKAGE__MODULE_H__

class Package;
struct PackagesModuleCTX
{
  std::vector<std::shared_ptr<Package>>  m_packages;    
  std::shared_ptr<ModuleInterface>       m_interface;
};

#ifndef PACKAGE_MODULE_API
#define PACKAGE_MODULE_API
#endif

#ifndef CPackagesModule
extern PACKAGE_MODULE_API PackagesModuleCTX *CPackagesModule; // Current implicit context pointer
#endif


namespace PackageModule{
    PACKAGE_MODULE_API bool RegisterPackage(std::string filepath, std::shared_ptr<Package> newPackage, nlohmann::json filecontent);
}



struct PackageAction{
    int         priority;
    std::string type;
    std::string emplacement;
    std::string command;
    std::string executionSequence;
};


struct PackageCompilation{
    bool useCompilationOptimization = false; // For a fully custom behavior

    std::string customScript = "unknow"; // For a fully custom behavior
    std::string configurationCommand = "unknow"; // Final cooked configuration command (Step 1 of vortex basic  compilation)
    std::string compilationCommand = "unknow"; // Final cooked compilation command (Step 2 of vortex basic  compilation)
    std::string installationCommand = "unknow"; // Final cooked installation command (Step 3 of vortex basic compilation)
    std::vector<std::pair<std::string, std::string>> configurationParams; // previous command' parameters
    std::vector<std::pair<std::string, std::string>> compilationParams; // previous command' parameters
    std::vector<std::pair<std::string, std::string>> installationParams; // previous command' parameters

    std::vector<std::pair<std::string, std::string>> configurationPrefixes;
    std::vector<std::pair<std::string, std::string>> configurationSuffixes;

    std::vector<std::pair<std::string, std::string>> compilationPrefixes;
    std::vector<std::pair<std::string, std::string>> compilationSuffixes;

    std::vector<std::pair<std::string, std::string>> installationPrefixes;
    std::vector<std::pair<std::string, std::string>> installationSuffixes;

    // Configuration Commands
    std::string customConfigurationProcess = "not specified"; // Customized configuration processus
    std::string exclusiveCustomConfigProcess = "not specified"; // Absolute customized configuration processus

    // Compilation Commands
    std::string customCompilationProcess = "not specified";
    std::string exclusiveCustomCompilationProcess = "not specified";

    // Installation Commands
    std::string customInstallationProcess = "not specified";
    std::string exclusiveCustomInstallationProcess = "not specified";
};


struct Diag{
    std::string properName;
    std::string statut;
    std::string description;
    std::string output;
    int code;
};


struct PackageActionSave{
    char type[128] = "unknow";
    int  priority = -1;
    char sequence[128] = "unknow";
    char command[128] = "unknow";
};

struct PackageSave{ 
    char name[128] = "unknow";
    char label[128] = "unknow";
    char author[128] = "unknow";
    char version[128] = "unknow";
    char description[128] = "unknow";

    int priority = -1;
    char compressed[128] = "unknow";
    
    // Change to a list of files 
    char filename[128] = "unknow";

    // Configuration
    bool useCompilationOptimization = true;
    char useOnlyCustomConfigurationProcess[128] = "unknow";
    char useOnlyCustomCompilationProcess[128] = "unknow";
    char useOnlyCustomInstallationProcess[128] = "unknow";

    // Les actions devront être des composants a part, non présents dans 
    std::vector<char[128]> compatibleArchitecture;
    
    std::vector<std::pair<char[128], char[128]>> configurationPrefixes;
    std::vector<std::pair<char[128], char[128]>> configurationSuffixes;
    char configurationExclusiveCommand[128];

    std::vector<std::pair<char[128], char[128]>> compilationPrefixes;
    std::vector<std::pair<char[128], char[128]>> compilationSuffixes;
    char compilationExclusiveCommand[128];

    std::vector<std::pair<char[128], char[128]>> installationPrefixes;
    std::vector<std::pair<char[128], char[128]>> installationSuffixes;
    char installationExclusiveCommand[128];

    std::vector<PackageActionSave> actions;


};


struct Package{
    std::string name = "unknow"; // Proper name of the package
    std::string author = "unknow"; // Proper name of the package
    std::string description = "unknow";  // Short description of the package
    std::string configFilePath = "unknow";  // Short description of the package


    std::string path = "unknow"; // Path to package
    std::string origin = "unknow"; // Path to package
    std::string distPath = "unknow"; // Path to package
    std::string fileName = "unknow"; // Path to package
    std::string compressed = "unknow"; // Path to package
    std::string label = "unknow"; // Dedicated package name (unique)
    std::string format = "unknow"; // Format of the package
    int         priority = -1;  // Priority of process when the package is in a queue
    std::vector<std::string> archs; // CPU archs supported by package
    std::vector<std::string> scopes; // Where the package can be used. (in a Toolchain, in a Host, Final system, etc...)
    std::string locality = "unknow"; // local (in the component), global (in the project), public (in a repository)
    std::string installationMethod = "unknow"; // compilation, installation
    bool        enabled = false; 
    bool        useChroot = false; 
    std::string strapper = "unknow";
    std::string type = "unknow";

    PackageCompilation compilation;
    // Package manager profile

    std::shared_ptr<Task> latestTask;
    std::vector<std::shared_ptr<Task>> allTasks;

    std::unordered_map<std::string, std::shared_ptr<Diag>> diags;

    hVector<std::shared_ptr<PackageAction>> actions;

    void PushSave(std::shared_ptr<PackageSave> save);
    void Refresh();

    void ExecuteActions(std::string sequence, std::shared_ptr<Package> package);

    void AddDiag(std::string name) {
        std::shared_ptr<Diag> diag = std::make_shared<Diag>();
        diag->code = 999;
        diag->output = "No output.";
        this->diags[name] = diag;
    };

    void SetDiagCode(std::string name, int code) {
          auto it = diags.find(name);
    if (it != diags.end()) {
        it->second->code = code;
    } else {
        std::cerr << "Key not finded" << std::endl;
    }
    };


    void SetDiagOutput(std::string name, std::string output) {
          auto it = diags.find(name);
    if (it != diags.end()) {
        it->second->output = output;
    } else {
        std::cerr << "Key not finded" << std::endl;
    }
    };


    
    std::string GetDiagStatus(std::string name);

    std::string GetDiagOutput(std::string name) {
        auto it = diags.find(name);
    if (it != diags.end()) {
        std::shared_ptr<Diag> foundDiag = it->second;
        return foundDiag->output;
    } else {
        return "No found.";
        std::cerr << "Key not finded" << std::endl;
    }

    };

    int GetDiagCode(std::string name) {
        auto it = diags.find(name);
    if (it != diags.end()) {
        std::shared_ptr<Diag> foundDiag = it->second;
        return foundDiag->code;
    } else {
        return -999;
        std::cerr << "Key not finded" << std::endl;
    }
    };

    

};

struct PackageReport{
    std::string label;
    std::string source;
    std::string taskID;
    std::string sizeImpact;

    std::string result;
    std::string state; 
    std::string report;
};

struct PackageInterface{
    std::string emplacement;
    std::string label;
    bool resolved;
};

static void Register()
{
  VxContext &ctx = *CVortexMaker;

  // Hosts
  for (const auto &file : VortexMaker::SearchFiles(CPackagesModule->m_interface->m_datapath, "package.config"))
  {
    try
    {
      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      std::shared_ptr<Package> newPackage = std::make_shared<Package>();

      newPackage->path = file;
      newPackage->configFilePath = file;

      bool alreadyExist = false;

      for (auto alreadyRegistered : CPackagesModule->m_packages)
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

      PackageModule::RegisterPackage(file, newPackage, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}



#endif // __PACKAGE__MODULE_H__