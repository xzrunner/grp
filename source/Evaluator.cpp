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

void Evaluator::Rebuild(const std::vector<std::shared_ptr<Node>>& nodes)
{
    m_passes.clear();

    std::vector<std::shared_ptr<node::PassEnd>> passes;
    for (auto& n : nodes)
    {
        n->SetRGNode(nullptr);
        if (n->get_type() == rttr::type::get<node::PassEnd>()) {
            RenderGraph::CreateGraphNode(n.get());
            passes.push_back(std::static_pointer_cast<node::PassEnd>(n));
        }
    }
    std::sort(passes.begin(), passes.end(), PassCmp());

    for (auto& p : passes)
    {
        auto& conns = p->GetAllInput()[0]->GetConnecting();
        if (conns.empty()) {
            continue;
        }
        auto& bp_node = conns[0]->GetFrom()->GetParent();
        auto& rg_node = static_cast<const rlab::Node&>(bp_node).GetRGNode();
        if (rg_node) {
            m_passes.push_back(std::make_shared<rg::DrawList>(rg_node));
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

}