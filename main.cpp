#include <iostream>
#include <string>
#include <thread>
#include <fstream>

// UI instances
#include "./tools/editor/app/include/editor.h"
#include "./tools/launcher/app/include/launcher.h"

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
                                                                                                              )"
              << std::endl;
}

void PrintHeader()
{
    // Print this every time
    std::cout << std::endl;
    std::cout << "\033[38;2;177;255;49m";
    PrintInfinite();
    std::cout << "\033[0m";

    std::cout << "\033[38;2;177;255;49m"
              << " Vortex " << VORTEX_VERSION
              << "\033[0m" << std::endl;

    std::cout << "┌────────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│"
              << "\033[38;2;177;255;49m"
              << " Description : "
              << "\033[0m"
              << "                                                                                 │ " << std::endl;

    std::cout << "│ ❓ Vortex is a complete open creation platform that contain a bunch of tools for creators and  │ " << std::endl;
    std::cout << "│  makers.                                                                                       │ " << std::endl;
    std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;

    std::cout << "│"
              << "\033[38;2;177;255;49m"
              << " Usage : "
              << "\033[0m"
              << "vortex <paramater(s)...> <information(s)...>                                           │ " << std::endl;

    std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│"
              << "\033[38;2;177;255;49m"
              << " Helpfull commands : "
              << "\033[0m"
              << "                                                                           │ " << std::endl;
    std::cout << "│ -h   --help :                          See all parameters                                      │ " << std::endl;
    std::cout << "│ -l   --launcher :                      Open the Vortex launcher (for projects, components...)  │ " << std::endl;
    std::cout << "│ -e   --editor :                        Open the Vortex graphical interface                     │ " << std::endl;
    std::cout << "│ -cp  --create-project  <...> :         Create a new project                                    │ " << std::endl;
    std::cout << "│ -i   --install  <...> :                Install a ressource into the user environment           │ " << std::endl;
    std::cout << "│ -bi  --build-install  <...> :          Build & Install a ressource into the user environment   │ " << std::endl;
    std::cout << "├────────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│"
              << "\033[38;2;177;255;49m"
              << " Links : "
              << "\033[0m"
              << "                                                                                       │ " << std::endl;
    std::cout << "│ 🌍 Vortex website : https://infinite.si/                                                       │ " << std::endl;
    std::cout << "│ 📝 News : https://infinite.si/blog                                                             │ " << std::endl;
    std::cout << "│ 💬 Discord : https://discord.gg/H2wptkecUg                                                     │ " << std::endl;
    std::cout << "│ 🧑‍💻 GitHub : https://github.com/infiniteHQ/Vortex                                               │ " << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────────────────────────────────────┘ " << std::endl;
    std::cout << std::endl;
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

// Project creator,
// Template deployment overrides (project, modules_content, etc...)

VxContext *InitBlankRuntime(bool logger)
{
    VxContext *ctx = VortexMaker::CreateContext();
    VortexMaker::LogInfo("Bootstrapp", "Initializing runtime...");

    // Initialize environment
    VortexMaker::InitEnvironment();

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
        PrintHeader();
    }
    else
    {
        if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
        {
            PrintHeader();
        }
        else if (std::string(argv[1]) == "-i" || std::string(argv[1]) == "--install")
        {
            InitBlankRuntime(true);
            PrintHeader();
            VortexMaker::LogInfo("Bootstrapp", "Find and install component...");
            std::string current_path = std::filesystem::current_path();
            InitBlankRuntime(true);
            VortexMaker::InstallContentOnSystem(current_path);
        }
        else if (std::string(argv[1]) == "-bi" || std::string(argv[1]) == "--build-install")
        {
            InitBlankRuntime(true);
            PrintHeader();
            VortexMaker::LogInfo("Bootstrapp", "Find and build/install component...");
            std::string current_path = std::filesystem::current_path();
            InitBlankRuntime(true);
            VortexMaker::InstallContentOnSystem(current_path);
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
                PrintHeader();
                VxContext *ctx = VortexMaker::CreateContext();
                VortexMaker::LogInfo("Bootstrapp", "Creating a new project...");
                std::string project_name = std::string(argv[2]);
                std::string current_path = std::filesystem::current_path();
                VortexMaker::CreateProject(project_name, current_path);
            }
        }
        else if (std::string(argv[1]) == "-e" || std::string(argv[1]) == "--editor")
        {
            PrintHeader();
            InitBlankRuntime(true);
            if (!CheckDirectory())
            {
                return 1;
            };
            VortexMaker::LogInfo("Bootstrapp", "Opening the graphical interface...");

            std::thread receiveThread;
            try
            {
                if (std::string(argv[2]) == "-v")
                {
                    VortexMaker::LogWarn("Bootstrapp", "Opening the graphical interface...");
                    InitRuntime(true);
                    std::thread Thread([&]()
                                       { VortexMaker::VortexEditor(argc, argv); });
                    receiveThread.swap(Thread);
                }
            }
            catch (std::exception e)
            {
                InitRuntime(false);
                std::thread Thread([&]()
                                   { VortexMaker::VortexEditor(argc, argv); });
                receiveThread.swap(Thread);
            }

            receiveThread.join();
        }
        else if (std::string(argv[1]) == "-l" || std::string(argv[1]) == "--launcher")
        {
            PrintHeader();
            InitBlankRuntime(true);
            VortexMaker::LogInfo("Bootstrapp", "Opening the graphical interface...");

            std::thread receiveThread;
            try
            {
                if (std::string(argv[2]) == "-v")
                {
                    VortexMaker::LogWarn("Bootstrapp", "Opening the graphical interface...");
                    std::thread Thread([&]()
                                       { VortexMaker::VortexLauncher(argc, argv); });
                    receiveThread.swap(Thread);
                }
            }
            catch (std::exception e)
            {
                std::thread Thread([&]()
                                   { VortexMaker::VortexLauncher(argc, argv); });
                receiveThread.swap(Thread);
            }

            receiveThread.join();
        }
    }

    return 0;
}