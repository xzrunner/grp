#pragma once

#include <rendergraph/typedef.h>
#include <rendergraph/Node.h>

namespace bp { class Node; class Pin; }

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
    static bool CreateFromNode(Evaluator& eval,
        const std::shared_ptr<bp::Pin>& bp_from_port, rg::Node::PortAddr& from_port);

    static void InitPortsBackFromFront(rg::Node& back, const bp::Node& front);;

}; // RenderGraph

}