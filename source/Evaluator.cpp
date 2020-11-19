#include "renderlab/Evaluator.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/node/PassEnd.h"

#include <blueprint/Connecting.h>
#include <blueprint/node/Function.h>

#include <rendergraph/DrawList.h>
#include <rendergraph/RenderContext.h>
#include <rendergraph/Node.h>

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

void Evaluator::Rebuild(const std::vector<bp::NodePtr>& nodes,
                        const bp::BackendGraph<rendergraph::Variable>& eval)
{
    Clear();

    // prepare passes
    std::vector<std::shared_ptr<node::PassEnd>> passes;
    for (auto& n : nodes) {
        if (n->get_type() == rttr::type::get<node::PassEnd>()) {
            //RenderGraph::CreateGraphNode(*this, n.get());
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
        auto back_node = eval.QueryBackNode(bp_node);
        if (!back_node) {
            continue;
        }
        auto rg_node = std::static_pointer_cast<rendergraph::Node>(back_node);
        std::vector<rendergraph::NodePtr> nodes;
        rendergraph::DrawList::GetAntecedentNodes(rg_node, nodes, false);
        m_passes.push_back(std::make_unique<rendergraph::DrawList>(nodes));
    }

    // input vars
    for (auto& n : nodes) 
    {
        if (n->get_type() == rttr::type::get<node::Input>()) 
        {
            auto input = std::static_pointer_cast<node::Input>(n);
            m_input_vars.push_back({ input->m_var_type, input->m_var_name });
        }
    }
}

void Evaluator::Draw(const std::shared_ptr<rendergraph::RenderContext>& rc,
                     const rendergraph::Node* end) const
{
    for (auto& p : m_passes) {
        bool finished = p->Draw(rc, end);
        if (finished) {
            break;
        }
    }
}

void Evaluator::SetInputVar(const std::string& name, const ur::TexturePtr& tex)
{
    for (auto& p : m_passes) {
        p->SetInputVar(name, tex);
    }
}

void Evaluator::Clear()
{
    m_passes.clear();
}

}