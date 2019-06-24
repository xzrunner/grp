#include "renderlab/Evaluator.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/RenderGraph.h"

#include <blueprint/Connecting.h>

#include <unirender/Sandbox.h>
#include <rendergraph/DrawList.h>
#include <rendergraph/RenderContext.h>

namespace
{

class PassCmp
{
public:
	bool operator () (const std::shared_ptr<rlab::node::PassEnd>& p0,
                      const std::shared_ptr<rlab::node::PassEnd>& p1) const {
        return p0->GetOrder() < p1->GetOrder();
    }
}; // PassCmp

}

namespace rlab
{

Evaluator::Evaluator()
{
}

Evaluator::~Evaluator()
{
}

void Evaluator::Rebuild(const std::vector<std::shared_ptr<Node>>& nodes)
{
    Clear();

    // create rg nodes
    for (auto& n : nodes) {
        n->SetRGNode(nullptr);
    }
    for (auto& n : nodes) {
        RenderGraph::CreateGraphNode(n.get());
    }

    // prepare passes
    std::vector<std::shared_ptr<node::PassEnd>> passes;
    for (auto& n : nodes) {
        if (n->get_type() == rttr::type::get<node::PassEnd>()) {
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
        auto& rg_node = static_cast<const rlab::Node&>(bp_node).GetRGNode();
        if (rg_node)
        {
            std::vector<rg::NodePtr> nodes;
            rg::DrawList::GetAntecedentNodes(rg_node, nodes);
            m_passes.push_back(std::make_unique<rg::DrawList>(nodes));
        }
    }
}

void Evaluator::Draw(const rg::RenderContext& rc, const Node* end) const
{
    ur::Sandbox sb(rc.rc);

    rg::NodePtr rg_end = nullptr;
    if (end) {
        rg_end = end->GetRGNode();
    }
    for (auto& p : m_passes) {
        bool finished = p->Draw(rc, rg_end);
        if (finished) {
            break;
        }
    }
}

void Evaluator::Clear()
{
    m_passes.clear();
}

}