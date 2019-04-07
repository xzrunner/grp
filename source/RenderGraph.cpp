#include "grp/RenderGraph.h"
#include "grp/RegistNodes.h"
#include "grp/Node.h"

#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>

#include <unirender/typedef.h>
#include <rendergraph/node/Clear.h>
#include <rendergraph/node/Viewport.h>
#include <rendergraph/node/Texture.h>
#include <rendergraph/node/Shader.h>
#include <rendergraph/node/Bind.h>
#include <facade/ImageLoader.h>

namespace grp
{

rg::NodePtr RenderGraph::CreateGraphNode(const bp::Node& node)
{
    auto type = node.get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    auto find_sg = src_type.find("grp::");
    if (find_sg != std::string::npos) {
        dst_type = "rg::" + src_type.substr(find_sg + strlen("grp::"));
    }
    if (dst_type.empty()) {
        return nullptr;
    }

	rttr::type t = rttr::type::get_by_name(dst_type);
	if (!t.is_valid()) {
		return nullptr;
	}
	assert(t.is_valid());
	rttr::variant var = t.create();
	assert(var.is_valid());

    rg::NodePtr dst = var.get_value<std::shared_ptr<rg::Node>>();
	assert(dst);

    if (type == rttr::type::get<node::Clear>())
    {
        auto& src = static_cast<const node::Clear&>(node);
        auto clear = std::static_pointer_cast<rg::node::Clear>(dst);

        uint32_t type = 0;
        if (src.type.type & ClearType::CLEAR_COLOR) {
            type |= rg::node::Clear::CLEAR_COLOR;
        }
        if (src.type.type & ClearType::CLEAR_DEPTH) {
            type |= rg::node::Clear::CLEAR_DEPTH;
        }
        if (src.type.type & ClearType::CLEAR_STENCIL) {
            type |= rg::node::Clear::CLEAR_STENCIL;
        }
        clear->SetClearType(type);

        clear->SetColor(src.color);
    }
    else if (type == rttr::type::get<node::Viewport>())
    {
        auto& src = static_cast<const node::Viewport&>(node);
        auto vp = std::static_pointer_cast<rg::node::Viewport>(dst);
        vp->SetParams(src.x, src.y, src.w, src.h);
    }
    else if (type == rttr::type::get<node::Texture>())
    {
        auto& src = static_cast<const node::Texture&>(node);
        auto tex = std::static_pointer_cast<rg::node::Texture>(dst);

        tex->SetFilepath(src.filepath);

        int type = 0;
        switch (src.type)
        {
        case TextureType::Tex2D:
            type = ur::TEXTURE_2D;
            break;
        case TextureType::TexCube:
            type = ur::TEXTURE_CUBE;
            break;
        }

        int format = 0;
        switch (src.format)
        {
        case TextureFormat::RGBA8:
            format = ur::TEXTURE_RGBA8;
            break;
        case TextureFormat::RGBA4:
            format = ur::TEXTURE_RGBA4;
            break;
        case TextureFormat::RGB:
            format = ur::TEXTURE_RGB;
            break;
        case TextureFormat::RGB565:
            format = ur::TEXTURE_RGB565;
            break;
        case TextureFormat::A8:
            format = ur::TEXTURE_A8;
            break;
        }

        tex->SetParams(type, src.width, src.height, format);

        // todo
        facade::ImageLoader loader(src.filepath);
        loader.Load();
        tex->SetTexID(loader.GetID());
    }
    else if (type == rttr::type::get<node::Shader>())
    {
        auto& src = static_cast<const node::Shader&>(node);
        std::static_pointer_cast<rg::node::Shader>(dst)->SetCodes(src.vert, src.frag);
    }
    else if (type == rttr::type::get<node::Bind>())
    {
        auto& src = static_cast<const node::Bind&>(node);
        std::static_pointer_cast<rg::node::Bind>(dst)->SetChannel(src.channel);
    }

    // connect
    for (int i = 0, n = node.GetAllInput().size(); i < n; ++i)
    {
        auto& imports = dst->GetImports();
        if (node.IsExtensibleInputPorts() && i >= static_cast<int>(imports.size())) {
            continue;
        }
        rg::Node::PortAddr from_port;
        if (!CreateFromNode(node, i, from_port) ||
            from_port.node.expired()) {
            continue;
        }
        rg::make_connecting(from_port, { dst, i });
    }

    m_cached_nodes.push_back(dst);

    return dst;
}

bool RenderGraph::CreateFromNode(const bp::Node& node, int input_idx, rg::Node::PortAddr& from_port)
{
    auto& to_port = node.GetAllInput()[input_idx];
    auto& conns = to_port->GetConnecting();
    if (conns.empty()) {
        return false;
    }
    assert(conns.size() == 1);
    auto& bp_from_port = conns[0]->GetFrom();
    assert(bp_from_port);

    from_port.node = CreateGraphNode(bp_from_port->GetParent());
    from_port.idx  = bp_from_port->GetPosIdx();
    return true;
}

}