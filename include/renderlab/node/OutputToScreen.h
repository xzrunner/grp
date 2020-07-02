#pragma once

#include "renderlab/Node.h"

#include <blueprint/Pin.h>

namespace renderlab
{
namespace node
{

class OutputToScreen : public Node
{
public:
    OutputToScreen()
        : Node("OutputToScreen", false)
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, bp::PIN_ANY_VAR, "prev", *this
        ));
        m_all_output.push_back(std::make_shared<bp::Pin>(
            false, 0, bp::PIN_ANY_VAR, "next", *this
        ));

        Layout();

        SetGroup("Utility");
    }

    RTTR_ENABLE(Node)

}; // OutputToScreen

}
}