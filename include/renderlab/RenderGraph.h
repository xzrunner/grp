#pragma once

#include <rendergraph/typedef.h>
#include <rendergraph/Node.h>

namespace bp { class Node; }

namespace rlab
{

class Evaluator;

class RenderGraph
{
public:
    static rg::NodePtr CreateGraphNode(Evaluator& eval,
        const bp::Node* node);

    static int TypeBackToFront(rg::VariableType type, int count);
    static rg::VariableType TypeFrontToBack(int pin_type);

private:
    static bool CreateFromNode(Evaluator& eval, const bp::Node* node,
        int input_idx, rg::Node::PortAddr& from_port);

}; // RenderGraph

}