#include <vector>
#include <memory>
#include <cstddef>
#include <cstdint>

#include "../../../lib/uikit/Platform/GUI/editor/Image.h"

#pragma once

/**
 * @brief Executed by other with custom parameters
 */
class InputEvent
{
public:
    virtual void execute() = 0;
};

/**
 * @brief Emmiting this even with custom parameters.
 */
class OutputEvent
{
public:
    virtual void execute() = 0;
};

class ModuleInterface
{
public:
    virtual ~ModuleInterface() {}
    virtual void execute() = 0;
    virtual void render() = 0;

    void AddLogo(uint8_t* hexa, size_t size);

    // std::shared_ptr<Walnut::Image> m_module_icon;
    std::string m_type;
    std::string m_proper_name;
    std::string m_name;
    std::string m_version;
    std::string m_author;
    std::string m_picture;
    std::string m_description;
    std::vector<std::string> m_contributors;
    
    const uint8_t* m_logo;
    size_t m_logo_size;

private:
    std::vector<std::shared_ptr<InputEvent>> m_input_events;
    std::vector<std::shared_ptr<OutputEvent>> m_output_events;
};
