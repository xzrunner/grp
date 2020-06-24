#pragma once

#include <dag/Node.h>
#include <rendergraph/Variable.h>

namespace bp { class Node; }
namespace ur { class Device; }

namespace renderlab
{

class RenderAdapter
{
public:
    static int TypeBackToFront(rendergraph::VariableType type, int count);
	static rendergraph::VariableType TypeFrontToBack(int type);

    static void Front2Back(const ur::Device& dev, const bp::Node& front,
        dag::Node<rendergraph::Variable>& back, const std::string& dir);

}; // RenderAdapter

}