#define SDL_MAIN_HANDLED

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

// UI instances
#include "./ui/crash_handler/crash_handler.hpp"
#include "./ui/editor/app/src/editor.hpp"

#include "./lib/cherry/cherry.hpp"
#include "./main/include/templates/load.h"
#include "./main/include/vortex.h"

static std::string session_id = "unknow";

/*
    TODO : Parent process for vortex executables regitered in
   /usr/local/bin/Vortex/VERSION/vortex This command can detect the version of a
   project and execute the correspondant project.

    Usage :

    vortex -e --version=1.1 -> Execute a project editor 1.1 in the directory of
   a project vortex -e --version=1.1 --contained=docker -> Execute a project
   editor 1.1 in the directory of a project in a docker container vortex -e
   --version=1.1 --templates_path=PATH -> With a template path preset vortex -e
   --version=1.1 --projects_path=PATH -> With a project path preset

    vortex -l --version=1.1 -> Execute a launcher with vortex 1.1


    Before lauching everything, this process can detect all version registered
   in the system by checking /usr/local/bin and run healthy command



*/
void PrintInfinite() {
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

void PrintHeader(const std::string &additions = "") {
  // Print this every time
  std::cout << std::endl;
  std::cout << "\033[38;2;177;255;49m";
  PrintInfinite();
  std::cout << "\033[0m";

  std::cout << "\033[38;2;177;255;49m"
            << " Vortex " << VORTEX_VERSION << " " << additions << "\033[0m"
            << std::endl;

  std::cout << "┌──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┐"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Description : "
            << "\033[0m"
            << "                                                               "
               "                  │ "
            << std::endl;

  std::cout << "│ ❓ Vortex is a complete open creation platform that contain "
               "a bunch of tools for creators and  │ "
            << std::endl;
  std::cout << "│  makers.                                                     "
               "                                  │ "
            << std::endl;
  std::cout << "├──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┤"
            << std::endl;

  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Usage : "
            << "\033[0m"
            << "vortex <paramater(s)...> <information(s)...>                   "
               "                        │ "
            << std::endl;

  std::cout << "├──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┤"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Helpfull commands : "
            << "\033[0m"
            << "                                                               "
               "            │ "
            << std::endl;
  std::cout << "│ -h   --help :                          See all parameters    "
               "                                  │ "
            << std::endl;
  std::cout << "│ -l   --launcher :                      Open the Vortex "
               "launcher (for projects, components...)  │ "
            << std::endl;
  std::cout << "│ -e   --editor :                        Open the Vortex "
               "graphical interface                     │ "
            << std::endl;
  std::cout << "│ -cp  --create-project  <...> :         Create a new project  "
               "                                  │ "
            << std::endl;
  std::cout << "│ -i   --install  <...> :                Install a ressource "
               "into the user environment           │ "
            << std::endl;
  std::cout << "│ -bi  --build-install  <...> :          Build & Install a "
               "ressource into the user environment   │ "
            << std::endl;
  std::cout << "├──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┤"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Links : "
            << "\033[0m"
            << "                                                               "
               "                        │ "
            << std::endl;
  std::cout << "│ 🌍 Vortex website : https://infinite.si/                     "
               "                                  │ "
            << std::endl;
  std::cout << "│ 📝 News : https://infinite.si/blog                           "
               "                                  │ "
            << std::endl;
  std::cout << "│ 💬 Discord : https://discord.gg/H2wptkecUg                   "
               "                                  │ "
            << std::endl;
  std::cout << "│ 🧑‍💻 GitHub : https://github.com/infiniteHQ/Vortex        "
               "                                       │ "
            << std::endl;
  std::cout << "└──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┘ "
            << std::endl;
  std::cout << std::endl;
}

bool CheckDirectory() {
  std::ifstream mainconfig("vortex.config");
  if (!mainconfig.good()) {
    std::cout << "This directory does not contain a Vortex project. Please "
                 "initialize a new project with \"vortex --create-project "
                 "<project_name>\" or repair it."
              << std::endl;
    return false;
  }
  return true;
}

VxContext *InitRuntime(bool logger) {
  VxContext *ctx = VortexMaker::CreateContext();
  VortexMaker::InitializePlatformVendor();
  std::cout << "Initializing runtime..." << std::endl;

  ctx->state.session_id = session_id;

  VortexMaker::CreateGlobalLogger();
  VortexMaker::CreateConsoleLogger();
  ctx->logger = logger;

  VortexMaker::CreateSessionTopic(ctx->state.session_id);

  // Initialize environment
  VortexMaker::InitEnvironment();
  VortexMaker::DetectPlatform();
  VortexMaker::DetectArch();

  // Refresh environment registered projects
  VortexMaker::RefreshEnvironmentProjects();

  VortexMaker::LoadSystemTemplates(ctx->IO.sys_templates);

  std::ifstream file("vortex.config");

  if (file) {
    nlohmann::json jsonContent;
    file >> jsonContent;
    VortexMaker::InitProject(jsonContent);
  }

  return ctx;
}
// Project sys size== 0

// Project creator,
// Template deployment overrides (project, modules_content, etc...)

VxContext *InitBlankRuntime(bool logger) {
  VxContext *ctx = VortexMaker::CreateContext();

  ctx->state.session_id = session_id;

  VortexMaker::InitializePlatformVendor();
  VortexMaker::CreateGlobalLogger();
  VortexMaker::CreateConsoleLogger();
  VortexMaker::LogInfo("Bootstrapp", "Initializing runtime...");

  VortexMaker::CreateSessionTopic(ctx->state.session_id);

  // Initialize environment
  VortexMaker::InitEnvironment();
  VortexMaker::DetectPlatform();
  VortexMaker::DetectArch();

  // Refresh environment registered projects
  VortexMaker::RefreshEnvironmentProjects();

  VortexMaker::LoadSystemTemplates(ctx->IO.sys_templates);

  ctx->logger = logger;
  return ctx;
}

/**
 * @brief : Entry point of main Vortex runtime command.
 */
int main(int argc, char *argv[]) {
  if (argc < 2) {
    PrintHeader();
  } else {
    if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
      PrintHeader();
    } else if (std::string(argv[1]) == "-test") {
      std::cout << "ok" << std::endl;
      return 0;
    } else if (std::string(argv[1]) == "-crash" ||
               std::string(argv[1]) == "--get-last-crash") {
      PrintHeader("(Crash handler)");

      if (argc > 2) {
        std::string arg2 = argv[2];
        if (arg2.rfind("--session_id=\"", 0) == 0 && arg2.back() == '\"') {
          session_id = arg2.substr(13, arg2.length() - 14);
        }
      }

      InitBlankRuntime(true);
      VortexMaker::LogInfo("Bootstrapp", "Opening the graphical interface...");

      VxContext *ctx = VortexMaker::GetCurrentContext();

      std::thread receiveThread;
      std::thread Thread([&]() { VortexMaker::VortexCrash(argc, argv); });
      receiveThread.swap(Thread);

      receiveThread.join();
    } else if (std::string(argv[1]) == "-i" ||
               std::string(argv[1]) == "--install") {
      InitBlankRuntime(true);
      PrintHeader();
      VortexMaker::LogInfo("Bootstrapp", "Find and install component...");
      std::string current_path = std::filesystem::current_path().string();
      InitBlankRuntime(true);
      VortexMaker::InstallContentOnSystem(current_path);
    } else if (std::string(argv[1]) == "-bi" ||
               std::string(argv[1]) == "--build-install") {
      InitBlankRuntime(true);
      PrintHeader();
      VortexMaker::LogInfo("Bootstrapp", "Find and build/install component...");
      std::string current_path = std::filesystem::current_path().string();
      InitBlankRuntime(true);
      VortexMaker::InstallContentOnSystem(current_path);
    } else if (std::string(argv[1]) == "-cp" ||
               std::string(argv[1]) == "--create-project") {
      if (argc < 3) {
        std::cout << "Usage : vortex -cp <project_name>" << std::endl;
        std::cout << "Or : vortex --create-project <project_name>" << std::endl;
      } else {
        PrintHeader();
        VxContext *ctx = VortexMaker::CreateContext();
        VortexMaker::LogInfo("Bootstrapp", "Creating a new project...");
        std::string project_name = std::string(argv[2]);
        std::string current_path = std::filesystem::current_path().string();
        VortexMaker::CreateProject(project_name, current_path);
      }
    } else if (std::string(argv[1]) == "-e" ||
               std::string(argv[1]) == "--editor") {
      PrintHeader("(Editor)");
      if (argc > 2) {
        std::string arg2 = argv[2];
        if (arg2.rfind("--session_id=\"", 0) == 0 && arg2.back() == '\"') {
          session_id = arg2.substr(13, arg2.length() - 14);
        }
      }

      InitRuntime(true);
      VortexMaker::LogInfo("Bootstrapp", "Opening the graphical interface...");

      std::thread receiveThread;
      try {
        if (std::string(argv[2]) == "-v" || std::string(argv[3]) == "-v") {
          VortexMaker::LogWarn("Bootstrapp",
                               "Opening the graphical interface...");
          std::thread Thread([&]() { VortexMaker::VortexEditor(argc, argv); });
          receiveThread.swap(Thread);
        }
      } catch (std::exception e) {
        std::thread Thread([&]() { VortexMaker::VortexEditor(argc, argv); });
        receiveThread.swap(Thread);
      }

      receiveThread.join();
    }
  }

  return 0;
}