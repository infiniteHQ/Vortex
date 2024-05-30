#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <array>
#include <memory>
#include <algorithm>

#include "main/include/vortex.h"
#include "main/include/vortex_internals.h"
#include "main/include/modules/load.h"
#include "main/include/modules/runtime.h"
#include "main/include/templates/load.h"


/*
    TODO : Parent process for vortex executables regitered in /usr/local/bin/Vortex/VERSION/vortex
    This command can detect the version of a project and execute the correspondant project.

    Usage :

    vortex -e --version=1.1 -> Execute a project editor 1.1 in the directory of a project
    vortex -e --version=1.1 --contained=docker -> Execute a project editor 1.1 in the directory of a project in a docker container
    vortex -e --version=1.1 --templates_path=PATH -> With a template path preset
    vortex -e --version=1.1 --projects_path=PATH -> With a project path preset

    vortex -l --version=1.1 -> Execute a launcher with vortex 1.1


    Before lauching everything, this process can detect all version registered in the system by checking /usr/local/bin and run healthy command



*/

static std::vector<std::string> available_versions;

bool TestVortexExecutable(const std::string &path)
{
    std::array<char, 128> buffer;
    std::string result;
    std::string command = path + " -test";

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        std::cerr << "popen() failed!" << std::endl;
        return false;
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    int return_code = pclose(pipe.release());

    return (result.find("ok") != std::string::npos) && (return_code == 0);
}

void RegisterAvailableVersions()
{
    const std::string base_path = "/usr/local/bin/Vortex";

    for (const auto &entry : std::filesystem::directory_iterator(base_path))
    {
        if (entry.is_directory())
        {
            std::string version = entry.path().filename().string();
            std::string vortex_executable = entry.path().string() + "/vortex";

            if (TestVortexExecutable(vortex_executable))
            {
                available_versions.push_back(version);
            }
        }
    }
}

std::string FindCompatibleVersion()
{
}


void LaunchVortex(const std::string& version) {
    std::string command = "/usr/local/bin/Vortex/" + version + "/vortex --launcher";
    std::system(command.c_str());
}

void LaunchVortexEditor(const std::string& version) {
    std::string command = "/usr/local/bin/Vortex/" + version + "/vortex --editor";
    std::system(command.c_str());
}

bool CheckDirectory()
{
    std::ifstream mainconfig("vortex.config");
    if (!mainconfig.good())
    {
        std::cout << "This directory does not contain a Vortex project. Please initialize a new project with \"vortex --create-project <project_name>\" or repair it." << std::endl;
        return false;
    }
    return true;
}


int main(int argc, char *argv[]) {
    RegisterAvailableVersions();

    std::cout << "Available versions:" << std::endl;
    for (const std::string &version : available_versions) {
        std::cout << version << std::endl;
    }

    if (argc > 1) {
        std::string arg1 = argv[1];
        if (arg1 == "-l" || arg1 == "--launcher") {
            std::string specified_version;

            // Check for --version argument
            if (argc > 2) {
                std::string arg2 = argv[2];
                if (arg2.rfind("--version=", 0) == 0) {
                    specified_version = arg2.substr(10); // Extract version number
                }
            }

            if (specified_version.empty()) {
                // Launch the last version if --version is not specified
                if (!available_versions.empty()) {
                    LaunchVortex(available_versions.back());
                } else {
                    std::cerr << "No available versions found." << std::endl;
                }
            } else {
                // Launch the specified version
                if (std::find(available_versions.begin(), available_versions.end(), specified_version) != available_versions.end()) {
                    LaunchVortex(specified_version);
                } else {
                    std::cerr << "Specified version not found." << std::endl;
                }
            }
        } else if (arg1 == "-test") {
            std::cout << "ok" << std::endl;
            return 0;
        }
    } else {
        // Default behavior: launch the last version
        if (!available_versions.empty()) {
            LaunchVortex(available_versions.back());
        } else {
            std::cerr << "No available versions found." << std::endl;
        }
    }

    return 0;
}