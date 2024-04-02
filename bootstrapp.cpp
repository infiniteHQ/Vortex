#include <cstdlib>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << std::endl;
        std::cout << "Usage : vortex <paramater(s)...> <information(s)...>" << std::endl
                  << std::endl;
        std::cout << "-h --help :               See all parameters" << std::endl;
        std::cout << "-g --gui :                Open the Vortex graphical interface" << std::endl;
        std::cout << "-cp --create-project  <...> :        Create a new project" << std::endl;
        std::cout << "Vortex makes it easy to create a system, application or toolchain. It also offers a wide range of options for system maintenance and supervision. " << std::endl
                  << std::endl;
    }
    else
    {
        if (std::string(argv[1]) == "-cp" || std::string(argv[1]) == "--create-project")
        {
            if(!argv[2]){
                std::cout << "Please provide a project name" << std::endl;
            }

                std::string project_name = std::string(argv[2]);
                std::string current_path = std::filesystem::current_path();
                std::string projectPath;

            // Creating main project folder
            {
                projectPath = current_path + "/" +project_name + "/";
                std::string cmd = "mkdir " + projectPath;
                system(cmd.c_str());
            }

            {
                std::string cmd = "mkdir " + projectPath + "/.vx";
                system(cmd.c_str());
            }
            {
                std::string cmd = "touch " + projectPath + "/vortex.config";
                system(cmd.c_str());
            }
            nlohmann::json j;
            j["project"]["author"] = "unknow";
            j["project"]["description"] = "This is a toolchain";
            j["project"]["name"] = argv[2];
            j["project"]["type"] = "???";
            j["project"]["version"] = "1.0.0";

            j["data"]["toolchains"] = "./.vx/data/toolchains/";
            j["data"]["hosts"] = "./.vx/data/hosts/";
            j["data"]["scripts"] = "./.vx/data/scripts/";
            j["data"]["gpos"] = "./.vx/data/gpos/";
            j["data"]["packages"] = "./.vx/data/packages/";

            j["dist"]["toolchains"] = "./.vx/dist/toolchains/";
            j["dist"]["gpos"] = "./.vx/dist/gpos/";
            j["dist"]["packages"] = "./.vx/dist/packages/";
            j["dist"]["hosts"] = "./.vx/dist/hosts/";

            // Store this into toolchain.config
            std::ofstream o(projectPath + "/vortex.config");
            o << std::setw(4) << j << std::endl;
            o.close();

            // Data
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data/hosts";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data/kernels";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data/libs";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data/gpos";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data/scripts";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data/packages";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data/skeletons";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/data/toolchains";
                system(cmd.c_str());
            }

            {
                std::string cmd = "mkdir " + projectPath + "/.vx/config";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/dist";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/dist/hosts";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/dist/gpos";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/dist/kernels";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/dist/libs";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/dist/packages";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/dist/skeletons";
                system(cmd.c_str());
            }
            {
                std::string cmd = "mkdir " + projectPath + "/.vx/dist/toolchains";
                system(cmd.c_str());
            }

            {
                std::string cmd = "mkdir " + projectPath + "/.vx/temp";
                system(cmd.c_str());
            }
        }
        else if (std::string(argv[1]) == "-g" || std::string(argv[1]) == "--gui")
        {
            if (!CheckDirectory())
            {
                return 1;
            };

#ifdef _WIN32
#endif

#ifdef __APPLE__
#endif

            // Linux
                std::string current_path = std::filesystem::current_path();
            std::string docker_command = "xhost + && sudo docker run -it --gpus=all --rm -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY -v "+current_path+":/root/vx -e NVIDIA_VISIBLE_DEVICES=all -e NVIDIA_DRIVER_CAPABILITIES=all vortex";

            int status = std::system(docker_command.c_str());

            if (status == 0)
            {
                std::cout << "La commande a été exécutée avec succès." << std::endl;
            }
            else
            {
                std::cerr << "Erreur lors de l'exécution de la commande." << std::endl;
            }
        }
    }

    return 0;
}