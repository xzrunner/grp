#include "renderlab/Node.h"
#include "renderlab/PinType.h"
#include "renderlab/NodePreview.h"
#include "renderlab/Blackboard.h"
#include "renderlab/RenderGraph.h"

#include <blueprint/Pin.h>
#include <blueprint/BackendAdapter.h>

#include <unirender/Blackboard.h>
#include <rendergraph/Node.h>
#include <node2/RenderSystem.h>

namespace renderlab
{

Node::Node(const std::string& title, bool preview)
    : bp::Node(title)
    , m_preview(preview)
{
}

Node::~Node() = default;

void Node::Draw(const n2::RenderParams& rp) const
{
    bp::Node::Draw(rp);

    if (m_preview) {
        auto eval = Blackboard::Instance()->GetEval();
        if (eval) {
            NodePreview::Draw(*eval, *this, rp);
        }
    }
}

void Node::InitPins(const std::string& name)
{
	auto back2front = [](const dag::Node<rendergraph::Variable>::Port& back) -> bp::PinDesc
	{
        bp::PinDesc front;

        front.type = RenderGraph::TypeBackToFront(back.var.type.type, 1);
        const_cast<rendergraph::Node::Port&>(back).var.full_name = back.var.type.name;
        front.name = back.var.full_name;

        return front;
	};

    bp::BackendAdapter<rendergraph::Variable>
        trans("rendergraph", back2front);
    trans.InitNodePins(*this, name);
}

}