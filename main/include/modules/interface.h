#include <vector>
#include <memory>

#include "../../../lib/uikit/Platform/GUI/editor/Image.h"

#pragma once

/**
 * @brief Executed by other with custom parameters
*/
class InputEvent{
    public: 
    virtual void execute() = 0;
};


/**
 * @brief Emmiting this even with custom parameters.
*/
class OutputEvent{
    public: 
    virtual void execute() = 0;
};

class ModuleInterface {
public:
    virtual ~ModuleInterface() {}
    virtual void execute() = 0;
    virtual void render() = 0;

    //std::shared_ptr<Walnut::Image> m_module_icon;
std::string m_name;
private:
    std::vector<std::shared_ptr<InputEvent>> m_input_events;
    std::vector<std::shared_ptr<OutputEvent>> m_output_events;
};
