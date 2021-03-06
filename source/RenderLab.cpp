#include "renderlab/RenderLab.h"
#include "renderlab/PinCallback.h"
#include "renderlab/Node.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/node/SubGraph.h"

#include <blueprint/CompNode.h>
#include <blueprint/NodeBuilder.h>
#include <blueprint/node/Commentary.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

#include <node0/CompIdentity.h>
#include <node0/SceneNode.h>
#include <node2/UpdateSystem.h>
#include <renderpipeline/RenderPipeline.h>
#include <rendergraph/RenderGraph.h>
#include <shaderlab/ReflectPropTypes.h>

namespace bp
{
extern void regist_sm_rttr();
extern void regist_pt0_rttr();
}

namespace shaderlab
{
extern void nodes_regist_rttr();
}

namespace renderlab
{

CU_SINGLETON_DEFINITION(RenderLab);

RenderLab::RenderLab()
{
    rp::RenderPipeline::Instance();
    rendergraph::RenderGraph::Instance();

    RegistRTTR();

	Init();
	InitNodes();

    InitPinCallback();
}

void RenderLab::RegistRTTR()
{
    bp::regist_sm_rttr();
    bp::regist_pt0_rttr();

    prop_types_regist_rttr();
    nodes_regist_rttr();

	shaderlab::prop_types_regist_rttr();
	shaderlab::nodes_regist_rttr();
}

void RenderLab::Init()
{
	n2::UpdateSystem::Instance()->AddUpdateCompFunc([](const n0::SceneNode& node, const ur::Device* dev)->bool
	{
		if (!node.HasUniqueComp<bp::CompNode>()) {
			return false;
		}

		bool dirty = false;

		auto& cnode = node.GetUniqueComp<bp::CompNode>();

		auto bp_node = cnode.GetNode();

		if (bp_node->get_type().is_derived_from<node::SubGraph>()) 
		{
			assert(node.HasUniqueComp<n0::CompIdentity>());
			auto filepath = std::static_pointer_cast<node::SubGraph>(bp_node)->GetFilepath();
			node.GetUniqueComp<n0::CompIdentity>().SetFilepath(filepath);
		}

		return dirty;
	});

	//bp::NodeBuilder::Callback cb;
	//cb.on_created = [](bp::Node& node, std::vector<n0::SceneNodePtr>& nodes) {
	//	NodeBuilder::CreateDefaultInputs(nodes, node);
	//};
	//cb.on_connecting = [](bp::Pin& from, bp::Pin& to) {
	//	NodeHelper::RemoveDefaultNode(to);
	//};
	//cb.on_connected = [](bp::Pin& from, bp::Pin& to) {
 //       TypeDeduction::DeduceConn(from, to);
	//};
 //   cb.on_disconnected = [](bp::Pin& from, bp::Pin& to) {
 //       TypeDeduction::DeduceNode(from.GetParent());
 //       TypeDeduction::DeduceNode(to.GetParent());
 //   };
	//bp::NodeBuilder::Instance()->RegistCB(cb);
}

void RenderLab::InitNodes()
{
    const int bp_count = 1;

	auto list = rttr::type::get<Node>().get_derived_classes();
	m_nodes.reserve(bp_count + list.size());

    m_nodes.push_back(std::make_shared<bp::node::Commentary>());
    m_nodes.push_back(std::make_shared<bp::node::Function>());
    m_nodes.push_back(std::make_shared<bp::node::Input>());
    m_nodes.push_back(std::make_shared<bp::node::Output>());

	m_nodes.push_back(std::make_shared<node::SubGraph>());

	for (auto& t : list)
	{
		auto obj = t.create();
		assert(obj.is_valid());
		auto node = obj.get_value<bp::NodePtr>();
		assert(node);
		m_nodes.push_back(node);
	}
}

}