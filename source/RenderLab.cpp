#include "renderlab/RenderLab.h"
#include "renderlab/PinCallback.h"
#include "renderlab/Node.h"

#include <blueprint/NodeBuilder.h>
#include <blueprint/node/Commentary.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

#include <rendergraph/RenderGraph.h>
#include <renderpipeline/RenderPipeline.h>

namespace rlab
{

CU_SINGLETON_DEFINITION(RenderLab);

extern void regist_rttr();

RenderLab::RenderLab()
{
	rg::RenderGraph::Instance();
    rp::RenderPipeline::Instance();

	regist_rttr();

	Init();
	InitNodes();

    InitPinCallback();
}

void RenderLab::Init()
{
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