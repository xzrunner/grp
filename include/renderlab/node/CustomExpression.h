#pragma once

#include "renderlab/Node.h"

#include <rendergraph/Variable.h>

namespace renderlab
{
namespace node
{

class CustomExpression : public Node
{
public:
    CustomExpression()
        : Node("CustomExpression", false)
    {
        InitPins("custom_expression");

        SetGroup("Utility");
    }

    auto& GetCode() const { return m_code; }
    void  SetCode(const std::string& code);

private:
    bool IsPinsChanged(bool is_in, const std::vector<rendergraph::Variable>& new_vars) const;

private:
    std::string m_code;

    RTTR_ENABLE(Node)

}; // CustomExpression

}
}