#pragma once

#include "renderlab/Node.h"

#include <blueprint/Pin.h>

namespace renderlab
{
namespace node
{

class Preview : public Node
{
public:
    Preview()
        : Node("Preview", true)
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, bp::PIN_ANY_VAR, "in", *this
        ));

        Layout();

        SetGroup("Utility");
    }

    RTTR_ENABLE(Node)

}; // Preview

}
}