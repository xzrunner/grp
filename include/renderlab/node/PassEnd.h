#pragma once

#include "renderlab/Node.h"

#include <blueprint/Pin.h>

namespace renderlab
{
namespace node
{

class PassEnd : public Node
{
public:
    PassEnd()
        : Node("PassEnd", false)
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, bp::PIN_PORT, "prev", *this
        ));

        Layout();

        UpdateTitle();

        SetGroup("ControlFlow");
    }

    int  GetOrder() const { return m_order; }
    void SetOrder(int order) {
        m_order = order;
        UpdateTitle();
    }

private:
    void UpdateTitle() {
        m_title = "PassEnd" + std::to_string(m_order);
    }

private:
    int m_order = 0;

    RTTR_ENABLE(Node)

}; // PassEnd

}
}