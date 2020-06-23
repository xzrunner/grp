#pragma once

#include "renderlab/Node.h"

#include <rendergraph/Variable.h>

namespace renderlab
{
namespace node
{

class CustomFunction : public Node
{
public:
    CustomFunction()
        : Node("CustomFunction", false)
    {
        InitPins("custom_function");

        SetGroup("Utility");
    }

    auto& GetCode() const { return m_code; }
    void  SetCode(const std::string& code);

private:
    bool IsPinsChanged(bool is_in, const std::vector<rendergraph::Variable>& new_vars) const;

private:
    std::string m_code;

    RTTR_ENABLE(Node)

}; // CustomFunction

}
}