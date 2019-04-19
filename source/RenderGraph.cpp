#include "renderlab/RenderGraph.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/Node.h"
#include "renderlab/PinType.h"

#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>

// resource
#include <rendergraph/node/Shader.h>
#include <rendergraph/node/Texture.h>
#include <rendergraph/node/VertexArray.h>
#include <rendergraph/node/PrimitiveShape.h>
#include <rendergraph/node/Model.h>
// op
#include <rendergraph/node/Clear.h>
#include <rendergraph/node/Bind.h>
// state
#include <rendergraph/node/Viewport.h>
#include <rendergraph/node/AlphaTest.h>
#include <rendergraph/node/BlendEq.h>
#include <rendergraph/node/BlendFunc.h>
#include <rendergraph/node/Cull.h>
#include <rendergraph/node/ZTest.h>
#include <rendergraph/node/ZWrite.h>
#include <rendergraph/node/FrontFace.h>
// others
#include <rendergraph/node/value_nodes.h>
#include <rendergraph/node/math_nodes.h>
// features
#include <renderpipeline/SeparableSSS.h>

#include <model/Model.h>
#include <model/ModelInstance.h>
#include <unirender/typedef.h>
#include <facade/ImageLoader.h>
#include <facade/ResPool.h>
#include <cpputil/StringHelper.h>

namespace rlab
{

rg::NodePtr RenderGraph::CreateGraphNode(const Node* node)
{
    if (auto& rg_node = node->GetRGNode()) {
        return rg_node;
    }

    auto type = node->get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    auto find_sg = src_type.find("rlab::");
    if (find_sg != std::string::npos) {
        dst_type = "rg::" + src_type.substr(find_sg + strlen("rlab::"));
    }
    if (dst_type.empty()) {
        node->SetRGNode(nullptr);
        return nullptr;
    }

	rttr::type t = rttr::type::get_by_name(dst_type);
    // fixme: specify node type
	if (!t.is_valid())
    {
        rg::NodePtr ret = nullptr;
        auto& inputs = node->GetAllInput();
        if (!inputs.empty()) {
            auto& conns = inputs[0]->GetConnecting();
            if (!conns.empty()) {
                ret = CreateGraphNode(&static_cast<const Node&>(conns[0]->GetFrom()->GetParent()));
            }
        }
        node->SetRGNode(ret);
        return ret;
	}
	assert(t.is_valid());
	rttr::variant var = t.create();
	assert(var.is_valid());

    rg::NodePtr dst = var.get_value<std::shared_ptr<rg::Node>>();
	assert(dst);

    dst->SetEnable(node->GetEnable());

    // resource
    if (type == rttr::type::get<node::Shader>())
    {
        auto src = static_cast<const node::Shader*>(node);
        std::static_pointer_cast<rg::node::Shader>(dst)->SetCodes(src->GetVert(), src->GetFrag());
    }
    else if (type == rttr::type::get<node::Texture>())
    {
        auto src = static_cast<const node::Texture*>(node);
        auto tex = std::static_pointer_cast<rg::node::Texture>(dst);

        tex->SetSize(src->width, src->height);

        rg::node::Texture::Type type;
        switch (src->type)
        {
        case TextureType::Tex2D:
            type = rg::node::Texture::Type::Tex2D;
            break;
        case TextureType::TexCube:
            type = rg::node::Texture::Type::TexCube;
            break;
        }
        tex->SetType(type);

        rg::node::Texture::Format format;
        switch (src->format)
        {
        case TextureFormat::RGBA8:
            format = rg::node::Texture::Format::RGBA8;
            break;
        case TextureFormat::RGBA4:
            format = rg::node::Texture::Format::RGBA4;
            break;
        case TextureFormat::RGB:
            format = rg::node::Texture::Format::RGB;
            break;
        case TextureFormat::RGB565:
            format = rg::node::Texture::Format::RGB565;
            break;
        case TextureFormat::A8:
            format = rg::node::Texture::Format::A8;
            break;
        case TextureFormat::Depth:
            format = rg::node::Texture::Format::Depth;
            break;
        }
        tex->SetFormat(format);

        rg::node::Texture::Wrapping wrap;
        switch (src->wrap)
        {
        case TextureWrapping::Repeat:
            wrap = rg::node::Texture::Wrapping::Repeat;
            break;
        case TextureWrapping::MirroredRepeat:
            wrap = rg::node::Texture::Wrapping::MirroredRepeat;
            break;
        case TextureWrapping::ClampToEdge:
            wrap = rg::node::Texture::Wrapping::ClampToEdge;
            break;
        case TextureWrapping::ClampToBorder:
            wrap = rg::node::Texture::Wrapping::ClampToBorder;
            break;
        }
        tex->SetWrapping(wrap);

        rg::node::Texture::Filtering filter;
        switch (src->filter)
        {
        case TextureFiltering::Nearest:
            filter = rg::node::Texture::Filtering::Nearest;
            break;
        case TextureFiltering::Linear:
            filter = rg::node::Texture::Filtering::Linear;
            break;
        }
        tex->SetFiltering(filter);

        // todo
        facade::ImageLoader loader(src->filepath);
        auto ur_wrap = static_cast<ur::TEXTURE_WRAP>(wrap);
        auto ur_filter = static_cast<ur::TEXTURE_FILTER>(filter);
        if (loader.Load(ur_wrap, ur_filter)) {
            tex->SetTexID(loader.GetID());
            tex->SetFilepath(src->filepath);
        }
    }
    else if (type == rttr::type::get<node::VertexArray>())
    {
        auto src = static_cast<const node::VertexArray*>(node);
        auto va = std::static_pointer_cast<rg::node::VertexArray>(dst);

        std::vector<std::string> tokens;
        cpputil::StringHelper::Split(src->vertices_data, ",", tokens);
        std::vector<float> vertex_buf;
        vertex_buf.reserve(tokens.size());
        for (auto& t : tokens) {
            vertex_buf.push_back(std::stof(t));
        }
        va->SetVertexBuf(vertex_buf);

        std::vector<rg::node::VertexArray::VertexAttrib> va_list;
        auto get_type_size = [](VertexDataType type)->size_t
        {
            size_t ret = 0;
            switch (type)
            {
            case VertexDataType::Char:
                ret = 1;
                break;
            case VertexDataType::Float:
                ret = 4;
                break;
            }
            return ret;
        };
        if (src->position.num > 0) {
            va_list.push_back({ src->position.name, src->position.num, get_type_size(src->position.type) });
        }
        if (src->normal.num > 0) {
            va_list.push_back({ src->normal.name, src->normal.num, get_type_size(src->normal.type) });
        }
        if (src->texture.num > 0) {
            va_list.push_back({ src->texture.name, src->texture.num, get_type_size(src->texture.type) });
        }
        va->SetVertList(va_list);
    }
    else if (type == rttr::type::get<node::PrimitiveShape>())
    {
        auto src = static_cast<const node::PrimitiveShape*>(node);

        rg::node::PrimitiveShape::Type type;
        switch (src->type)
        {
        case PrimitiveShapeType::Quad:
            type = rg::node::PrimitiveShape::Type::Quad;
        case PrimitiveShapeType::Cube:
            type = rg::node::PrimitiveShape::Type::Cube;
            break;
        }
        std::static_pointer_cast<rg::node::PrimitiveShape>(dst)->SetType(type);
    }
    else if (type == rttr::type::get<node::Model>())
    {
        auto src = static_cast<const node::Model*>(node);
        auto model = facade::ResPool::Instance().Fetch<model::Model>(src->filepath);
        auto model_inst = std::make_shared<model::ModelInstance>(model);
        std::static_pointer_cast<rg::node::Model>(dst)->SetModel(model_inst);
    }
    // op
    else if (type == rttr::type::get<node::Clear>())
    {
        auto src = static_cast<const node::Clear*>(node);
        auto clear = std::static_pointer_cast<rg::node::Clear>(dst);

        uint32_t type = 0;
        if (src->type.type & ClearType::CLEAR_COLOR) {
            type |= rg::node::Clear::CLEAR_COLOR;
        }
        if (src->type.type & ClearType::CLEAR_DEPTH) {
            type |= rg::node::Clear::CLEAR_DEPTH;
        }
        if (src->type.type & ClearType::CLEAR_STENCIL) {
            type |= rg::node::Clear::CLEAR_STENCIL;
        }
        clear->SetClearType(type);

        clear->SetColor(src->color);
    }
    else if (type == rttr::type::get<node::Bind>())
    {
        auto src = static_cast<const node::Bind*>(node);
        std::static_pointer_cast<rg::node::Bind>(dst)->SetChannel(src->channel);
    }
    // state
    else if (type == rttr::type::get<node::Viewport>())
    {
        auto src = static_cast<const node::Viewport*>(node);
        auto vp = std::static_pointer_cast<rg::node::Viewport>(dst);
        vp->SetParams(src->x, src->y, src->w, src->h);
    }
    else if (type == rttr::type::get<node::AlphaTest>())
    {
        auto src = static_cast<const node::AlphaTest*>(node);
        rg::node::AlphaTest::Func func;
        switch (src->func)
        {
        case AlphaTestFunc::Off:
            func = rg::node::AlphaTest::Func::Off;
            break;
        case AlphaTestFunc::Never:
            func = rg::node::AlphaTest::Func::Never;
            break;
        case AlphaTestFunc::Less:
            func = rg::node::AlphaTest::Func::Less;
            break;
        case AlphaTestFunc::Equal:
            func = rg::node::AlphaTest::Func::Equal;
            break;
        case AlphaTestFunc::LEqual:
            func = rg::node::AlphaTest::Func::LEqual;
            break;
        case AlphaTestFunc::Greater:
            func = rg::node::AlphaTest::Func::Greater;
            break;
        case AlphaTestFunc::NotEqual:
            func = rg::node::AlphaTest::Func::NotEqual;
            break;
        case AlphaTestFunc::GEqual:
            func = rg::node::AlphaTest::Func::GEqual;
            break;
        case AlphaTestFunc::Always:
            func = rg::node::AlphaTest::Func::Always;
            break;
        }
        std::static_pointer_cast<rg::node::AlphaTest>(dst)->SetFunc(func);
    }
    else if (type == rttr::type::get<node::BlendEq>())
    {
        auto src = static_cast<const node::BlendEq*>(node);
        rg::node::BlendEq::Mode mode;
        switch (src->mode)
        {
        case BlendEqMode::FuncAdd:
            mode = rg::node::BlendEq::Mode::FuncAdd;
            break;
        case BlendEqMode::FuncSubtract:
            mode = rg::node::BlendEq::Mode::FuncSubtract;
            break;
        case BlendEqMode::FuncReverseSubtract:
            mode = rg::node::BlendEq::Mode::FuncReverseSubtract;
            break;
        case BlendEqMode::Min:
            mode = rg::node::BlendEq::Mode::Min;
            break;
        case BlendEqMode::Max:
            mode = rg::node::BlendEq::Mode::Max;
            break;
        }
    }
    else if (type == rttr::type::get<node::BlendFunc>())
    {
        auto src = static_cast<const node::BlendFunc*>(node);
        auto trans = [](const BlendFuncFactor& factor)->rg::node::BlendFunc::Factor
        {
            rg::node::BlendFunc::Factor ret;
            switch (factor)
            {
            case BlendFuncFactor::Off:
                ret = rg::node::BlendFunc::Factor::Off;
                break;
            case BlendFuncFactor::Zero:
                ret = rg::node::BlendFunc::Factor::Zero;
                break;
            case BlendFuncFactor::One:
                ret = rg::node::BlendFunc::Factor::One;
                break;
            case BlendFuncFactor::SrcColor:
                ret = rg::node::BlendFunc::Factor::SrcColor;
                break;
            case BlendFuncFactor::OneMinusSrcColor:
                ret = rg::node::BlendFunc::Factor::OneMinusSrcColor;
                break;
            case BlendFuncFactor::DstColor:
                ret = rg::node::BlendFunc::Factor::DstColor;
                break;
            case BlendFuncFactor::OneMinusDstColor:
                ret = rg::node::BlendFunc::Factor::OneMinusDstColor;
                break;
            case BlendFuncFactor::SrcAlpha:
                ret = rg::node::BlendFunc::Factor::SrcAlpha;
                break;
            case BlendFuncFactor::OneMinusSrcAlpha:
                ret = rg::node::BlendFunc::Factor::OneMinusSrcAlpha;
                break;
            case BlendFuncFactor::DstAlpha:
                ret = rg::node::BlendFunc::Factor::DstAlpha;
                break;
            case BlendFuncFactor::OneMinusDstAlpha:
                ret = rg::node::BlendFunc::Factor::OneMinusDstAlpha;
                break;
            }
            return ret;
        };
        auto func = std::static_pointer_cast<rg::node::BlendFunc>(dst);
        func->SetSrcFactor(trans(src->sfactor));
        func->SetDstFactor(trans(src->dfactor));
    }
    else if (type == rttr::type::get<node::Cull>())
    {
        auto src = static_cast<const node::Cull*>(node);
        rg::node::Cull::Mode mode;
        switch (src->type)
        {
        case CullMode::Off:
            mode = rg::node::Cull::Mode::Off;
            break;
        case CullMode::Front:
            mode = rg::node::Cull::Mode::Front;
            break;
        case CullMode::Back:
            mode = rg::node::Cull::Mode::Back;
            break;
        case CullMode::FrontAndBack:
            mode = rg::node::Cull::Mode::FrontAndBack;
            break;
        }
        std::static_pointer_cast<rg::node::Cull>(dst)->SetMode(mode);
    }
    else if (type == rttr::type::get<node::ZTest>())
    {
        auto src = static_cast<const node::ZTest*>(node);
        rg::node::ZTest::Func func;
        switch (src->func)
        {
        case ZTestFunc::Off:
            func = rg::node::ZTest::Func::Off;
            break;
        case ZTestFunc::Never:
            func = rg::node::ZTest::Func::Never;
            break;
        case ZTestFunc::Less:
            func = rg::node::ZTest::Func::Less;
            break;
        case ZTestFunc::Equal:
            func = rg::node::ZTest::Func::Equal;
            break;
        case ZTestFunc::LEqual:
            func = rg::node::ZTest::Func::LEqual;
            break;
        case ZTestFunc::Greater:
            func = rg::node::ZTest::Func::Greater;
            break;
        case ZTestFunc::NotEqual:
            func = rg::node::ZTest::Func::NotEqual;
            break;
        case ZTestFunc::GEqual:
            func = rg::node::ZTest::Func::GEqual;
            break;
        case ZTestFunc::Always:
            func = rg::node::ZTest::Func::Always;
            break;
        }
        std::static_pointer_cast<rg::node::ZTest>(dst)->SetFunc(func);
    }
    else if (type == rttr::type::get<node::ZWrite>())
    {
        auto src = static_cast<const node::ZWrite*>(node);
        std::static_pointer_cast<rg::node::ZWrite>(dst)->SetDepthWrite(src->enable);
    }
    else if (type == rttr::type::get<node::FrontFace>())
    {
        auto src = static_cast<const node::FrontFace*>(node);
        std::static_pointer_cast<rg::node::FrontFace>(dst)->SetClockwise(src->clockwise);
    }
    // value
    else if (type == rttr::type::get<node::Vector1>())
    {
        auto src = static_cast<const node::Vector1*>(node);
        std::static_pointer_cast<rg::node::Vector1>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Vector2>())
    {
        auto src = static_cast<const node::Vector2*>(node);
        std::static_pointer_cast<rg::node::Vector2>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Vector3>())
    {
        auto src = static_cast<const node::Vector3*>(node);
        std::static_pointer_cast<rg::node::Vector3>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Vector4>())
    {
        auto src = static_cast<const node::Vector4*>(node);
        std::static_pointer_cast<rg::node::Vector4>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Matrix2>())
    {
        auto src = static_cast<const node::Matrix2*>(node);
        std::static_pointer_cast<rg::node::Matrix2>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Matrix3>())
    {
        auto src = static_cast<const node::Matrix3*>(node);
        std::static_pointer_cast<rg::node::Matrix3>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Matrix4>())
    {
        auto src = static_cast<const node::Matrix4*>(node);
        std::static_pointer_cast<rg::node::Matrix4>(dst)->SetValue(src->val);
    }
    // math
    else if (type == rttr::type::get<node::PerspectiveMat>())
    {
        auto src = static_cast<const node::PerspectiveMat*>(node);
        std::static_pointer_cast<rg::node::PerspectiveMat>(dst)->SetProps(
            src->fovy, src->aspect, src->znear, src->zfar
        );
    }
    else if (type == rttr::type::get<node::OrthoMat>())
    {
        auto src = static_cast<const node::OrthoMat*>(node);
        std::static_pointer_cast<rg::node::OrthoMat>(dst)->SetProps(
            src->left, src->right, src->bottom, src->top, src->znear, src->zfar
        );
    }
    else if (type == rttr::type::get<node::LookAtMat>())
    {
        auto src = static_cast<const node::LookAtMat*>(node);
        std::static_pointer_cast<rg::node::LookAtMat>(dst)->SetProps(
            src->eye, src->center, src->up
        );
    }
    // features
    else if (type == rttr::type::get<node::SeparableSSS>())
    {
        auto src = static_cast<const node::SeparableSSS*>(node);
        std::static_pointer_cast<rp::node::SeparableSSS>(dst)->SetProps(
            src->nsamples, src->strength, src->falloff
        );
    }

    // connect
    for (int i = 0, n = node->GetAllInput().size(); i < n; ++i)
    {
        auto& imports = dst->GetImports();
        if (node->IsExtensibleInputPorts() && i >= static_cast<int>(imports.size())) {
            continue;
        }
        rg::Node::PortAddr from_port;
        if (!CreateFromNode(node, i, from_port) ||
            from_port.node.expired()) {
            continue;
        }
        rg::make_connecting(from_port, { dst, i });
    }

    node->SetRGNode(dst);

    return dst;
}

bool RenderGraph::CreateFromNode(const Node* node, int input_idx, rg::Node::PortAddr& from_port)
{
    auto& to_port = node->GetAllInput()[input_idx];
    auto& conns = to_port->GetConnecting();
    if (conns.empty()) {
        return false;
    }
    assert(conns.size() == 1);
    auto& bp_from_port = conns[0]->GetFrom();
    assert(bp_from_port);

    from_port.node = CreateGraphNode(&static_cast<const Node&>(bp_from_port->GetParent()));
    from_port.idx  = bp_from_port->GetPosIdx();
    return true;
}

}