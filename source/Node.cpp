#include "grp/Node.h"
#include "grp/PinType.h"

#include <blueprint/Pin.h>

#include <rendergraph/Node.h>

namespace grp
{

Node::Node(const std::string& title)
    : bp::Node(title)
{
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
    assert(t.is_valid());
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

	auto rg2grp = [](std::vector<PinDesc>& dst, const std::vector<rg::Node::Port>& src)
	{
		dst.reserve(dst.size() + src.size());
		for (int i = 0, n = src.size(); i < n; ++i)
		{
            PinDesc d;
			auto& s = src[i];
            switch (s.var.type)
            {
            case rg::VariableType::Any:
                d.type = bp::PIN_ANY_VAR;
                break;
            case rg::VariableType::Port:
                d.type = bp::PIN_PORT;
                break;
            case rg::VariableType::Texture:
                d.type = PIN_TEXTURE;
                break;
            case rg::VariableType::RenderTarget:
                d.type = PIN_RENDERTARGET;
                break;
            default:
                assert(0);
            }

            d.name = s.var.name;

            dst.push_back(d);
		}
	};

	std::vector<PinDesc> input, output;
    input.push_back({ bp::PIN_PORT, "In" });
    output.push_back({ bp::PIN_PORT, "Out" });
	rg2grp(input, imports);
	rg2grp(output, exports);

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