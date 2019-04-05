#pragma once

#include <blueprint/Node.h>

namespace grp
{

class Node : public bp::Node
{
public:
    Node(const std::string& title);

    struct PinDesc
    {
        int         type;
        std::string name;
    };

protected:
    void InitPins(const std::vector<PinDesc>& input,
        const std::vector<PinDesc>& output);
    void InitPins(const std::string& name);

private:
    void InitPinsImpl(const std::vector<PinDesc>& pins,
        bool is_input);

    RTTR_ENABLE(bp::Node)

}; // Node

}