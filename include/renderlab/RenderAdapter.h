#pragma once

#include <dag/Node.h>
#include <rendergraph/Variable.h>

namespace bp { class Node; }
namespace ur2 { class Device; }

namespace renderlab
{

class RenderAdapter
{
public:
    static int TypeBackToFront(rendergraph::VariableType type, int count);

    static void Front2Back(const ur2::Device& dev, const bp::Node& front,
        dag::Node<rendergraph::Variable>& back, const std::string& dir);

}; // RenderAdapter

}