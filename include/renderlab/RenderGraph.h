#pragma once

#include <rendergraph/typedef.h>
#include <rendergraph/Node.h>

namespace bp { class Node; class Pin; }

namespace renderlab
{

class Evaluator;

class RenderGraph
{
public:
    static rendergraph::NodePtr CreateGraphNode(Evaluator& eval,
        const bp::Node* node);

    static int TypeBackToFront(rendergraph::VariableType type, int count);
    static rendergraph::VariableType TypeFrontToBack(int pin_type);

private:
    static bool CreateFromNode(Evaluator& eval,
        const std::shared_ptr<bp::Pin>& bp_from_port, rendergraph::Node::PortAddr& from_port);

    static void InitPortsBackFromFront(rendergraph::Node& back, const bp::Node& front);;

}; // RenderGraph

}