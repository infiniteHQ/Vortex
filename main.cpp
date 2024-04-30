#include <iostream>
#include <string>
#include <thread>
#include <fstream>

#include "./tools/editor/app/include/editor.h"
#include "./main/include/vortex.h"
#include "./lib/uikit/src/EntryPoint.h"

void PrintInfinite()
{
        std::cout << R"(
                  ███  ███   ███  ███████▓▒███ ███░  ░███ ███▓█████████ ████████                   
                  ███  ███▒  ███  ███████▓▒███ █████ ░███ ███▓█████████ ████████                   
                  ███  ████  ███  ███▓▓▓▓ ▒███ ██████▒███ ███▓   ███░   ███                        
                  ███  █████ ███  ███████ ▒███ ███▒██████ ███▓   ███░   ███                        
                  ███  █████████  ███     ▒███ ███▒ █████ ███▓   ███░   ███                        
                  ███  ███▓█████  ███     ▒███ ███▒  ████ ███▓   ███░   ███                        
                  ███  ███ █████  ███                     ███▓   ███░   ███████                    
                  ███  ███ ▒████  ██░                        ░   ███░   ███████                    
                  ███  ███  ████                                 ███░   ███                        
                  ███  ███  ░█▓                                    █░   ███                        
                  ███  ███                                              ███                        
                  ███  ███                                              ███▒                       
                  ███  ██                                               ░████                      
                  ███                                                     ████▒                    
                  ███                                                       ███░                   
                  ██▒                                                        ███                   
                  █░                                                          ██                   
                  █                                                            █                   
                                                                                                              )" << std::endl;
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

VxContext *InitRuntime(bool logger)
{
    std::cout << "Initializing runtime..." << std::endl;
    VxContext *ctx = VortexMaker::CreateContext();
    ctx->logger = logger;
    std::ifstream file("vortex.config");

    if (file)
    {
        nlohmann::json jsonContenu;
        file >> jsonContenu;
        VortexMaker::InitProject(jsonContenu);
    }
    else
    {
    }

    return ctx;
}

VxContext *InitBlankRuntime(bool logger)
{
    std::cout << "Initializing runtime..." << std::endl;
    VxContext *ctx = VortexMaker::CreateContext();
    ctx->logger = logger;
    return ctx;
}
/**
 * @brief : Entry point of main Vortex runtime command.
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << std::endl;
        std::cout << "\033[38;2;177;255;49m"; 
        PrintInfinite();
        std::cout << "\033[0m";

        std::cout << "┌────────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
        std::cout << "|" << "\033[38;2;177;255;49m" <<  " Description : " << "\033[0m" << "                                                                                 | " << std::endl;

        std::cout << "| ❓ Vortex makes it easy to create a system, application or toolchain. It also offers a wide    | " << std::endl;
        std::cout << "| range of options for system maintenance and supervision.                                       | " << std::endl;
        std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;

        std::cout << "|" << "\033[38;2;177;255;49m" <<  " Usage : " << "\033[0m" << "vortex <paramater(s)...> <information(s)...>                                           | " << std::endl;


        std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
        std::cout << "|" << "\033[38;2;177;255;49m" <<  " Helpfull commands : " << "\033[0m" << "                                                                           | " << std::endl;
        std::cout << "| -h   --help :                          See all parameters                                      | " << std::endl;
        std::cout << "| -g   --gui :                           Open the Vortex graphical interface                     | " << std::endl;
        std::cout << "| -cp  --create-project  <...> :         Create a new project                                    | " << std::endl;
        std::cout << "| -i   --install  <...> :                Install a ressource into the user environment           | " << std::endl;
        std::cout << "| -bi  --build-install  <...> :          Build & Install a ressource into the user environment   | " << std::endl;
        std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
        std::cout << "|" << "\033[38;2;177;255;49m" <<  " Links : " << "\033[0m" << "                                                                                       | " << std::endl;
        std::cout << "| 🌍 Vortex website :                                                                            | " << std::endl;
        std::cout << "| 📝 News :                                                                                      | " << std::endl;
        std::cout << "| 💬 Discord :                                                                                   | " << std::endl;
        std::cout << "| 💬 Source code :                                                                               | " << std::endl;
        std::cout << "└────────────────────────────────────────────────────────────────────────────────────────────────┘ " << std::endl;
        std::cout << std::endl;
    }
    else
    {
        if (std::string(argv[1]) == "-breakpoint")
        {
            std::cout << "||| BREAKPOINT : Asked by command \"vortex -breakpoint\" |||" << std::endl;
            std::cout << "Press a key and hit ENTER to continue ..." << std::endl;
            std::string ui;
            std::cin >> ui;
        }
        else if (std::string(argv[1]) == "-i" || std::string(argv[1]) == "--install")
        {
            std::string current_path = std::filesystem::current_path();
            InitBlankRuntime(true);
            VortexMaker::InstallModuleToSystem(current_path);
        }
        else if (std::string(argv[1]) == "-bi" || std::string(argv[1]) == "--build-install")
        {
            std::string current_path = std::filesystem::current_path();
            InitBlankRuntime(true);
            VortexMaker::InstallModuleToSystem(current_path);
        }
        else if (std::string(argv[1]) == "-cp" || std::string(argv[1]) == "--create-project")
        {
            if (argc < 3)
            {
                std::cout << "Usage : vortex -cp <project_name>" << std::endl;
                std::cout << "Or : vortex --create-project <project_name>" << std::endl;
            }
            else
            {
                VxContext *ctx = VortexMaker::CreateContext();
                std::cout << "Creating a new project..." << std::endl;
                std::string project_name = std::string(argv[2]);
                std::string current_path = std::filesystem::current_path();

                std::cout << "Creating the project..." << std::endl;
                VortexMaker::CreateProject(project_name, current_path);
            }
        }
        else if (std::string(argv[1]) == "-g" || std::string(argv[1]) == "--gui")
        {
            if (!CheckDirectory())
            {
                return 1;
            };
            std::cout << "Opening the graphical interface..." << std::endl;

            std::thread receiveThread;
            try
            {
                if (std::string(argv[2]) == "-v")
                {
                    InitRuntime(true);
                    std::thread Thread([&]()
                                       { VortexMaker::VortexInterface(argc, argv); });
                    receiveThread.swap(Thread);
                }
            }
            catch (std::exception e)
            {
                InitRuntime(false);
                std::thread Thread([&]()
                                   { VortexMaker::VortexInterface(argc, argv); });
                receiveThread.swap(Thread);
            }

            receiveThread.join();
        }
        else if (std::string(argv[1]) == "-test" || std::string(argv[1]) == "--test")
        {
            if (!CheckDirectory())
            {
                return 1;
            };
            std::cout << "Opening the graphical interface..." << std::endl;

            std::thread receiveThread;
            std::thread Thread([&]()
                               { system("vortex -g"); });
            std::thread Thread2([&]()
                                { system("vortex -g"); });
            receiveThread.swap(Thread);
            receiveThread.swap(Thread2);
            receiveThread.join();
        }
        else if (std::string(argv[1]) == "-b" || std::string(argv[1]) == "--build")
        {
            if (!CheckDirectory())
            {
                return 1;
            };
            if (argc < 3)
            {
                std::cout << "Usage : vortex -b <build_type>" << std::endl;
                std::cout << "Or : vortex --build <build_type>" << std::endl;
                std::cout << "<build_type> : " << std::endl;
                std::cout << "all :             Build the entire project" << std::endl;
                std::cout << "toolchains :      Build all toochains" << std::endl;
                std::cout << "toolchain <name>: Build specific toolchain" << std::endl;
                std::cout << "packages :        Build all packages" << std::endl;
                std::cout << "package <name> :  Build specific package" << std::endl;
            }
            else
            {
                if (std::string(argv[2]) == "all")
                {
                }
                else if (std::string(argv[2]) == "toolchains")
                {
                }
                else if (std::string(argv[2]) == "toolchain")
                {
                    if (argc < 4)
                    {
                        std::cout << "Please enter the toolchain path" << std::endl;
                        std::cout << "Usage : vortex -b toolchain <path>" << std::endl;
                    }
                }
                else if (std::string(argv[2]) == "packages")
                {
                }
                else if (std::string(argv[2]) == "package")
                {
                    if (argc < 4)
                    {
                        std::cout << "Please enter the package path" << std::endl;
                        std::cout << "Usage : vortex -b package <path>" << std::endl;
                    }
                }
            }
        }
        else if (std::string(argv[1]) == "-a" || std::string(argv[1]) == "--add")
        {
            if (!CheckDirectory())
            {
                return 1;
            };
            if (argc < 3)
            {
                std::cout << "Usage : vortex -a <content_type>" << std::endl;
                std::cout << "Or : vortex --add <content_type>" << std::endl;
                std::cout << "<content_type> : " << std::endl;
                std::cout << "package <path>:   Build the entire project" << std::endl;
                std::cout << "lib <path>:       Build all toochains" << std::endl;
                std::cout << "toolchain <path>: Build specific toolchain" << std::endl;
            }
        }
    }

    return 0;
}