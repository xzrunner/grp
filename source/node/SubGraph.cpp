#include "renderlab/node/SubGraph.h"
#include "renderlab/RenderAdapter.h"
#include "renderlab/RegistNodes.h"

#include <blueprint/NodeLayout.h>
#include <blueprint/CompNode.h>

#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <rendergraph/Variable.h>

#include <boost/filesystem.hpp>

namespace renderlab
{
namespace node
{

SubGraph::SubGraph()
    : bp::node::SubGraph<rendergraph::Variable>("SubGraph", "rendergraph", "renderlab", nullptr)
{
	SetGroup("Utility");

	bp::NodeLayout::UpdateNodeStyle(*this);
}

void SubGraph::Draw(const ur::Device& dev, ur::Context& ctx, const n2::RenderParams& rp) const
{
	Node::Draw(dev, ctx, rp);

	// fixme: move to update
	if (m_need_rebuild) {
		const_cast<SubGraph*>(this)->InitFromFile(dev, m_filepath);
	}
}

void SubGraph::LoadFromJson(const ur::Device& dev, const std::string& dir, 
	                        const rapidjson::Value& val)
{
    Node::LoadFromJson(dev, dir, val);

	InitFromFile(dev, m_filepath);
}

void SubGraph::SetFilepath(const std::string& filepath) 
{ 
	m_filepath = filepath; 
	m_need_rebuild = true;
}

void SubGraph::InitFromFile(const ur::Device& dev, const std::string& filepath)
{
	if (filepath.empty()) {
		return;
	}

    auto root = ns::NodeFactory::Create(dev, filepath);
	if (!root) {
		return;
	}

	m_graph->SetFrontToBackCB([&](const bp::Node& front, dag::Node<rendergraph::Variable>& back) {
		auto dir = boost::filesystem::path(filepath).parent_path().string();
		RenderAdapter::Front2Back(dev, front, back, dir);
	});
	// todo
	//m_graph->SetContext();

    auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
    auto& children = ccomplex.GetAllChildren();

	m_all_input.clear();
	m_all_output.clear();

	std::vector<bp::NodePtr> bp_nodes;
    for (auto& c : children)
    {
        if (!c->HasUniqueComp<bp::CompNode>()) {
            continue;
        }
        auto bp_node = c->GetUniqueComp<bp::CompNode>().GetNode();
		assert(bp_node);
		bp_nodes.push_back(bp_node);

		auto bp_type = bp_node->get_type();
		if (bp_type == rttr::type::get<node::Input>()) 
		{
			auto input = std::static_pointer_cast<node::Input>(bp_node);
			auto type = RenderAdapter::TypeBackToFront(input->m_var_type, 1);
			m_all_input.push_back(std::make_shared<bp::Pin>(
				true, m_all_input.size(), type, input->m_var_name, *this
			));
		} 
		else if (bp_type == rttr::type::get<node::Output>()) 
		{
			auto output = std::static_pointer_cast<node::Output>(bp_node);
			auto type = RenderAdapter::TypeBackToFront(output->m_var_type, 1);
			m_all_output.push_back(std::make_shared<bp::Pin>(
				true, m_all_output.size(), type, "_out", *this
			));
		}
    }

	SetChildren(bp_nodes);
	bp::NodeLayout::UpdateNodeStyle(*this);

	m_need_rebuild = false;
}

}
}