#include "renderlab/Evaluator.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/RenderGraph.h"

#include <blueprint/Connecting.h>
#include <blueprint/node/Function.h>

#include <unirender/Sandbox.h>
#include <rendergraph/DrawList.h>
#include <rendergraph/RenderContext.h>

namespace
{

class PassCmp
{
public:
	bool operator () (const std::shared_ptr<renderlab::node::PassEnd>& p0,
                      const std::shared_ptr<renderlab::node::PassEnd>& p1) const {
        return p0->GetOrder() < p1->GetOrder();
    }
}; // PassCmp

}

namespace renderlab
{

Evaluator::Evaluator()
{
}

Evaluator::~Evaluator()
{
}

void Evaluator::Rebuild(const std::vector<bp::NodePtr>& nodes)
{
    Clear();

    // prepare passes
    std::vector<std::shared_ptr<node::PassEnd>> passes;
    for (auto& n : nodes) {
        if (n->get_type() == rttr::type::get<node::PassEnd>()) {
            RenderGraph::CreateGraphNode(*this, n.get());
            passes.push_back(std::static_pointer_cast<node::PassEnd>(n));
        }
    }
    std::sort(passes.begin(), passes.end(), PassCmp());

    // build drawlist for each pass
    for (auto& p : passes)
    {
        auto& conns = p->GetAllInput()[0]->GetConnecting();
        if (conns.empty()) {
            continue;
        }
        auto& bp_node = conns[0]->GetFrom()->GetParent();
        auto rg_node = QueryRGNode(&bp_node);
        assert(rg_node);
        std::vector<rendergraph::NodePtr> nodes;
        rendergraph::DrawList::GetAntecedentNodes(rg_node, nodes);
        m_passes.push_back(std::make_unique<rendergraph::DrawList>(nodes));
    }
}

void Evaluator::Draw(const std::shared_ptr<rendergraph::RenderContext>& rc,
                     const bp::NodePtr& end) const
{
    ur::Sandbox sb(rc->rc);

    rendergraph::NodePtr rg_end = nullptr;
    if (end) {
        rg_end = QueryRGNode(end.get());
        assert(rg_end);
    }
    for (auto& p : m_passes) {
        bool finished = p->Draw(rc, rg_end);
        if (finished) {
            break;
        }
    }
}

rendergraph::NodePtr Evaluator::QueryRGNode(const bp::Node* bp_node) const
{
    auto itr = m_nodes_map.find(bp_node);
    return itr == m_nodes_map.end() ? nullptr : itr->second;
}

void Evaluator::AddNodeMap(const bp::Node* bp_node, const rendergraph::NodePtr& rg_node)
{
    m_nodes_map.insert({ bp_node, rg_node });
}

void Evaluator::Clear()
{
    m_passes.clear();
    m_nodes_map.clear();
}

}