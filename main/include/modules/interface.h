#include <vector>
#include <memory>


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

private:
    std::vector<std::shared_ptr<InputEvent>> m_input_events;
    std::vector<std::shared_ptr<OutputEvent>> m_output_events;
};
