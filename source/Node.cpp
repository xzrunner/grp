#include "renderlab/Node.h"
#include "renderlab/PinType.h"
#include "renderlab/NodePreview.h"
#include "renderlab/Blackboard.h"
#include "renderlab/RenderGraph.h"

#include <blueprint/Pin.h>

#include <unirender/Blackboard.h>
#include <rendergraph/Node.h>
#include <node2/RenderSystem.h>

namespace rlab
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

void Node::InitPins(const std::vector<PinDesc>& input,
                    const std::vector<PinDesc>& output)
{
    InitPinsImpl(input, true);
    InitPinsImpl(output, false);
    Layout();
}

void Node::InitPins(const std::string& name)
{
	rttr::type t = rttr::type::get_by_name("rg::" + name);
    if (!t.is_valid()) {
        t = rttr::type::get_by_name("rp::" + name);
    }
    if (!t.is_valid()) {
        return;
    }

	rttr::variant var = t.create();
	assert(var.is_valid());

	auto method_imports = t.get_method("GetImports");
	assert(method_imports.is_valid());
	auto var_imports = method_imports.invoke(var);
	assert(var_imports.is_valid()
		&& var_imports.is_type<std::vector<rg::Node::Port>>());
	auto& imports = var_imports.get_value<std::vector<rg::Node::Port>>();

	auto method_exports = t.get_method("GetExports");
	assert(method_exports.is_valid());
	auto var_exports = method_exports.invoke(var);
	assert(var_exports.is_valid()
		&& var_exports.is_type<std::vector<rg::Node::Port>>());
	auto& exports = var_exports.get_value<std::vector<rg::Node::Port>>();

	auto port_back2front = [](std::vector<PinDesc>& dst, const std::vector<rg::Node::Port>& src)
	{
		dst.reserve(dst.size() + src.size());
		for (int i = 0, n = src.size(); i < n; ++i)
		{
            PinDesc d;

			auto& s = src[i];
            d.type = RenderGraph::TypeBackToFront(s.var.type, 1);
            d.name = s.var.name;

            dst.push_back(d);
		}
	};

	std::vector<PinDesc> input, output;
	port_back2front(input, imports);
	port_back2front(output, exports);

	InitPins(input, output);
}

void Node::InitPinsImpl(const std::vector<PinDesc>& pins, bool is_input)
{
    auto& dst = is_input ? m_all_input : m_all_output;
	dst.clear();
	dst.reserve(pins.size());
	int idx = 0;
	for (auto& d : pins)
	{
		auto p = std::make_shared<bp::Pin>(is_input, idx++, d.type, d.name, *this);
		if (!CheckPinName(*p, dst)) {
			assert(0);
			return;
		}
		dst.push_back(p);
	}
}

}