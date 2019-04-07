#pragma once

#include <rendergraph/typedef.h>
#include <rendergraph/Node.h>

namespace bp { class Node; }

namespace grp
{

class RenderGraph
{
public:
    rg::NodePtr CreateGraphNode(const bp::Node& node);

    void ClearCache() { m_cached_nodes.clear(); }

private:
    bool CreateFromNode(const bp::Node& node, int input_idx, rg::Node::PortAddr& from_port);

private:
    std::vector<rg::NodePtr> m_cached_nodes;

}; // RenderGraph

}