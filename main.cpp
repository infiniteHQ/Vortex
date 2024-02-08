#include <iostream>
#include <string>
#include <thread>
#include <fstream>


#include "./tools/editor/app/src/editor.h"
#include "./vortex.h"

bool CheckDirectory(){
    std::ifstream mainconfig("vortex.config");
    if (!mainconfig.good()) {
        std::cout << "This directory does not contain a Vortex project. Please initialize a new project with \"vortex -I <project_name>\" or repair it." << std::endl;
        return false;
    }
    return true;
}

VxContext* InitRuntime(){ 
    VxContext* ctx = VortexMaker::CreateContext();
    std::ifstream file("vortex.config");

    // Vérification si le fichier existe
    if (file) {
        nlohmann::json jsonContenu;
        file >> jsonContenu;

        std::cout << "Contenu JSON du fichier :" << std::endl;
        std::cout << jsonContenu.dump(4) << std::endl;
        VortexMaker::InitProject(jsonContenu);
    } else {
    }


    // ajouter les fichiers de config
    // et init le projet, charger tout.

    return ctx;
}

int main(int argc, char *argv[])
{




    if (argc < 2)
    {
        std::cout << std::endl;
        std::cout << "Usage : vortex <paramater(s)...> <information(s)...>" << std::endl << std::endl;
        std::cout << "-h --help :               See all parameters" << std::endl;
        std::cout << "-g --gui :                Open the Vortex graphical interface" << std::endl;
        std::cout << "-I --init  <...> :        Initialize a new project" << std::endl;
        std::cout << "-i --install  <...> :     Install a new content" << std::endl;
        std::cout << "-b --build <...> :        Build the project" << std::endl;
        std::cout << "-a --add <...> :          Add a content (packages, tools, libs, toolchains...)" << std::endl;
        std::cout << "-p --preview <...> :      Preview final component(s) of the project" << std::endl << std::endl;
        std::cout << "Vortex makes it easy to create a system, application or toolchain. It also offers a wide range of options for system maintenance and supervision. " << std::endl << std::endl;
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
        if (std::string(argv[1]) == "-g" || std::string(argv[1]) == "--gui")
        {
            if(!CheckDirectory()){return 1;};
            std::cout << "Opening the graphical interface..." << std::endl;

            std::thread receiveThread;
            std::thread Thread([&]() { VortexMaker::DebugTools(argc, argv, InitRuntime()); });
            receiveThread.swap(Thread);
            receiveThread.join();
        }
        if (std::string(argv[1]) == "-test" || std::string(argv[1]) == "--test")
        {
            if(!CheckDirectory()){return 1;};
            std::cout << "Opening the graphical interface..." << std::endl;

            std::thread receiveThread;
            std::thread Thread([&]() { system("vortex -g"); });
            std::thread Thread2([&]() { system("vortex -g"); });
            receiveThread.swap(Thread);
            receiveThread.swap(Thread2);
            receiveThread.join();
        }
        else if (std::string(argv[1]) == "-b" || std::string(argv[1]) == "--build")
        {
            if(!CheckDirectory()){return 1;};
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
            if(!CheckDirectory()){return 1;};
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
        else if (std::string(argv[1]) == "-p" || std::string(argv[1]) == "--preview")
        {
            if(!CheckDirectory()){return 1;};
            if (argc < 3)
            {
            }
        }
    }

    return 0;
}