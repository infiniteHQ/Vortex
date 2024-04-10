// plugin_example.cpp
#include <iostream>
#include "../../include/modules/interface.h"

class ToolchainsModule : public ModuleInterface {
public:
    void execute() override {
        std::cout << "Plugin One Example executed" << std::endl;
    }
    void render() override {
        std::cout << "GUI rendering" << std::endl;
    }
};


extern "C" ModuleInterface* create_editor_module() {
    return new ToolchainsModule();
}