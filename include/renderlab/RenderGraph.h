#pragma once

#include <rendergraph/typedef.h>
#include <rendergraph/Node.h>

namespace rlab
{

class Node;

class RenderGraph
{
public:
    static rg::NodePtr CreateGraphNode(const Node* node);

    static int TypeBackToFront(rg::VariableType type, int count);
    static rg::VariableType TypeFrontToBack(int pin_type);

private:
    static bool CreateFromNode(const Node* node, int input_idx, rg::Node::PortAddr& from_port);

}; // RenderGraph

}