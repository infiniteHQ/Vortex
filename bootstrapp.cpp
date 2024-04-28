#include <cstdlib>
#include <iostream>
#include <fstream>
#include <filesystem>

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
            if (!argv[2])
            {
                std::cout << "Please provide a project name" << std::endl;
                std::cout << "Usage : vortex -cp \"name\" <template>" << std::endl;
            }
#ifdef _WIN32
            std::string project_name = argv[2];
            fs::path current_path = fs::current_path();
            fs::path projectPath = current_path / project_name;

            // Creating main project folder
            fs::create_directory(projectPath);

            // Creating .vx folder
            fs::path vxPath = projectPath / ".vx";
            fs::create_directory(vxPath);

            // Creating vortex.config file
            std::ofstream config_file(projectPath / "vortex.config");
            if (config_file.is_open())
            {
                std::string j = "{\"project\":{\"author\":\"unknown\",\"description\":\"This is a toolchain\",\"name\":\"" + project_name + "\",\"type\":\"???\",\"version\":\"1.0.0\"},\"data\":{\"toolchains\":\"./.vx/data/toolchains/\",\"hosts\":\"./.vx/data/hosts/\",\"scripts\":\"./.vx/data/scripts/\",\"gpos\":\"./.vx/data/gpos/\",\"packages\":\"./.vx/data/packages/\"},\"dist\":{\"toolchains\":\"./.vx/dist/toolchains/\",\"gpos\":\"./.vx/dist/gpos/\",\"packages\":\"./.vx/dist/packages/\",\"hosts\":\"./.vx/dist/hosts/\"}}";
                config_file << j << std::endl;
                config_file.close();
            }

            // Creating data directories
            fs::create_directories(vxPath / "data/hosts");
            fs::create_directories(vxPath / "data/kernels");
            fs::create_directories(vxPath / "data/libs");
            fs::create_directories(vxPath / "data/gpos");
            fs::create_directories(vxPath / "data/scripts");
            fs::create_directories(vxPath / "data/packages");
            fs::create_directories(vxPath / "data/skeletons");
            fs::create_directories(vxPath / "data/toolchains");

            // Creating dist directories
            fs::create_directories(vxPath / "dist/hosts");
            fs::create_directories(vxPath / "dist/gpos");
            fs::create_directories(vxPath / "dist/kernels");
            fs::create_directories(vxPath / "dist/libs");
            fs::create_directories(vxPath / "dist/packages");
            fs::create_directories(vxPath / "dist/skeletons");
            fs::create_directories(vxPath / "dist/toolchains");

            // Creating temp directory
            fs::create_directories(vxPath / "temp");
#endif

#ifdef __linux__
            std::string project_name = std::string(argv[2]);
            std::string current_path = std::filesystem::current_path();
            std::string projectPath;

            // Creating main project folder
            {
                projectPath = current_path + "/" + project_name + "/";
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
            std::string j = "{\"project\":{\"author\":\"unknow\",\"description\":\"This is a toolchain\",\"name\":\"" + project_name + "\",\"type\":\"???\",\"version\":\"1.0.0\"},\"data\":{\"toolchains\":\"./.vx/data/toolchains/\",\"hosts\":\"./.vx/data/hosts/\",\"scripts\":\"./.vx/data/scripts/\",\"gpos\":\"./.vx/data/gpos/\",\"packages\":\"./.vx/data/packages/\"},\"dist\":{\"toolchains\":\"./.vx/dist/toolchains/\",\"gpos\":\"./.vx/dist/gpos/\",\"packages\":\"./.vx/dist/packages/\",\"hosts\":\"./.vx/dist/hosts/\"}}";

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
#endif
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

#ifdef __linux__
            // Linux
            std::string current_path = std::filesystem::current_path();
            std::string docker_command = "xhost + && sudo docker run -it --gpus=all --rm -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY -v " + current_path + ":/root/vx -e NVIDIA_VISIBLE_DEVICES=all -e NVIDIA_DRIVER_CAPABILITIES=all vortex";

            int status = system(docker_command.c_str());

            if (status == 0)
            {
                std::cout << "La commande a été exécutée avec succès." << std::endl;
            }
            else
            {
                std::cerr << "Erreur lors de l'exécution de la commande." << std::endl;
            }

#endif
        }
    }

    return 0;
}
