#include "renderlab/RenderGraph.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/Node.h"
#include "renderlab/PinType.h"
#include "renderlab/Evaluator.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/CustomExpression.h"

#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>
#include <blueprint/CompNode.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

// resource
#include <rendergraph/node/Shader.h>
#include <rendergraph/node/RenderTarget.h>
#include <rendergraph/node/Texture.h>
#include <rendergraph/node/VertexArray.h>
#include <rendergraph/node/PrimitiveShape.h>
#include <rendergraph/node/Model.h>
#include <rendergraph/node/Heightfield.h>
// op
#include <rendergraph/node/Clear.h>
#include <rendergraph/node/Bind.h>
#include <rendergraph/node/SetUniform.h>
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
#include <rendergraph/node/input_nodes.h>
#include <rendergraph/node/value_nodes.h>
#include <rendergraph/node/math_nodes.h>
#include <rendergraph/node/UserScript.h>
#include <rendergraph/node/CustomExpression.h>
#include <rendergraph/node/Group.h>
// features
#include <renderpipeline/SeparableSSS.h>

#include <model/Model.h>
#include <model/ModelInstance.h>
#include <unirender/typedef.h>
#include <rendergraph/Variable.h>
#include <facade/ImageLoader.h>
#include <facade/ResPool.h>
#include <cpputil/StringHelper.h>
#include <sm_const.h>
#include <node0/SceneNode.h>

namespace rlab
{

rendergraph::NodePtr RenderGraph::CreateGraphNode(Evaluator& eval, const bp::Node* node)
{
    auto cached = eval.QueryRGNode(node);
    if (cached) {
        return cached;
    }

    auto type = node->get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    std::string lib_str = "rendergraph";
    if (type == rttr::type::get<rlab::node::GlobalIllumination>() ||
        type == rttr::type::get<rlab::node::SeparableSSS>()) {
        lib_str = "rp";
    }
    auto find_lib = src_type.find("rlab::");
    if (find_lib != std::string::npos) {
        dst_type = lib_str + "::" + src_type.substr(find_lib + strlen("rlab::"));
    }

    rendergraph::NodePtr dst = nullptr;
    bool enable = true;

    // create rendergraph node
    if (!dst_type.empty())
    {
	    rttr::type t = rttr::type::get_by_name(dst_type);
        // fixme: specify node type
	    if (!t.is_valid())
        {
            dst = std::make_shared<rendergraph::Node>();
            InitPortsBackFromFront(*dst, *node);
	    }
        else
        {
            rttr::variant var = t.create();
            assert(var.is_valid());

            dst = var.get_value<std::shared_ptr<rendergraph::Node>>();
            assert(dst);
        }


        if (type.is_derived_from<Node>()) {
            enable = static_cast<const Node*>(node)->GetEnable();
        }
    }

    // bp
    if (type == rttr::type::get<bp::node::Function>())
    {
        auto src = static_cast<const bp::node::Function*>(node);
        auto dst_group = std::make_shared<rendergraph::node::Group>();
        dst = dst_group;

        InitPortsBackFromFront(*dst, *node);

        auto& src_children = src->GetChildren();
        std::vector<rendergraph::NodePtr> dst_children;
        for (auto& src_c : src_children)
        {
            if (!src_c->HasUniqueComp<bp::CompNode>()) {
                continue;
            }

            auto& cnode = src_c->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            auto node_type = bp_node->get_type();
            if (node_type == rttr::type::get<bp::node::Input>() ||
                node_type == rttr::type::get<bp::node::Output>()) {
                continue;
            }

            auto dst_c = CreateGraphNode(eval, bp_node.get());
            dst_children.push_back(dst_c);
        }

        std::vector<std::pair<rendergraph::NodePtr, int>> dst_outputs;
        dst_outputs.resize(dst->GetExports().size());
        for (auto& src_c : src_children)
        {
            if (!src_c->HasUniqueComp<bp::CompNode>()) {
                continue;
            }

            auto& cnode = src_c->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            auto node_type = bp_node->get_type();
            if (node_type == rttr::type::get<bp::node::Output>())
            {
                auto output_node = std::static_pointer_cast<bp::node::Output>(bp_node);

                auto& inputs = bp_node->GetAllInput();
                assert(inputs.size() == 1);
                auto& conns = inputs[0]->GetConnecting();
                assert(conns.size() == 1);
                auto p_idx = conns[0]->GetFrom()->GetPosIdx();
                auto& bp_node = conns[0]->GetFrom()->GetParent();
                auto rg_node = eval.QueryRGNode(&bp_node);

                int idx = -1;
                for (int i = 0, n = dst->GetExports().size(); i < n; ++i)
                {
                    if (dst->GetExports()[i].var.name == output_node->GetName()) {
                        idx = i;
                        break;
                    }
                }
                assert(idx != -1);
                dst_outputs[idx] = { rg_node, p_idx };
            }
        }

        dst_group->SetChildren(dst_children, dst_outputs);
    }
    // resource
    else if (type == rttr::type::get<node::Shader>())
    {
        auto src = static_cast<const node::Shader*>(node);
        std::static_pointer_cast<rendergraph::node::Shader>(dst)->SetCodes(src->GetVert(), src->GetFrag());
    }
    else if (type == rttr::type::get<node::RenderTarget>())
    {
        auto src = static_cast<const node::RenderTarget*>(node);
        auto rt = std::static_pointer_cast<rendergraph::node::RenderTarget>(dst);

        auto& conns = src->GetAllInput()[1]->GetConnecting();
        if (!conns.empty()) {
            auto& tex_node = conns[0]->GetFrom()->GetParent();
            if (tex_node.get_type() == rttr::type::get<node::Texture>()) {
                auto tex = static_cast<const node::Texture&>(tex_node);
                const_cast<node::RenderTarget*>(src)->width  = tex.width;
                const_cast<node::RenderTarget*>(src)->height = tex.height;
            }
        }

        rt->SetSize(src->width, src->height);
        if (src->depth_buf) {
            rt->EnableDepthRBO();
        }
        if (src->color_buf) {
            rt->EnableColorRBO();
        }
    }
    else if (type == rttr::type::get<node::Texture>())
    {
        auto src = static_cast<const node::Texture*>(node);
        auto tex = std::static_pointer_cast<rendergraph::node::Texture>(dst);

        rendergraph::node::Texture::Wrapping wrap;
        switch (src->wrap)
        {
        case TextureWrapping::Repeat:
            wrap = rendergraph::node::Texture::Wrapping::Repeat;
            break;
        case TextureWrapping::MirroredRepeat:
            wrap = rendergraph::node::Texture::Wrapping::MirroredRepeat;
            break;
        case TextureWrapping::ClampToEdge:
            wrap = rendergraph::node::Texture::Wrapping::ClampToEdge;
            break;
        case TextureWrapping::ClampToBorder:
            wrap = rendergraph::node::Texture::Wrapping::ClampToBorder;
            break;
        }
        tex->SetWrapping(wrap);

        rendergraph::node::Texture::Filtering filter;
        switch (src->filter)
        {
        case TextureFiltering::Nearest:
            filter = rendergraph::node::Texture::Filtering::Nearest;
            break;
        case TextureFiltering::Linear:
            filter = rendergraph::node::Texture::Filtering::Linear;
            break;
        }
        tex->SetFiltering(filter);

        facade::ImageLoader loader(src->filepath);
        auto ur_wrap = static_cast<ur::TEXTURE_WRAP>(wrap);
        auto ur_filter = static_cast<ur::TEXTURE_FILTER>(filter);
        if (loader.Load(ur_wrap, ur_filter))
        {
            tex->SetTexID(loader.GetID());
            tex->SetFilepath(src->filepath);

            auto src_tex = const_cast<node::Texture*>(src);
            src_tex->width  = loader.GetWidth();
            src_tex->height = loader.GetHeight();

            switch (loader.GetType())
            {
            case ur::TEXTURE_2D:
                src_tex->type = TextureType::Tex2D;
                break;
            case ur::TEXTURE_CUBE:
                src_tex->type = TextureType::TexCube;
                break;
            }

            switch (loader.GetFormat())
            {
            case ur::TEXTURE_RGBA16F:
                src_tex->format = TextureFormat::RGBA16;
                break;
            case ur::TEXTURE_RGBA8:
                src_tex->format = TextureFormat::RGBA8;
                break;
            case ur::TEXTURE_RGBA4:
                src_tex->format = TextureFormat::RGBA4;
                break;
            case ur::TEXTURE_RGB:
                src_tex->format = TextureFormat::RGB;
                break;
            case ur::TEXTURE_RGB565:
                src_tex->format = TextureFormat::RGB565;
                break;
            case ur::TEXTURE_A8:
                src_tex->format = TextureFormat::A8;
                break;
            case ur::TEXTURE_RED:
                src_tex->format = TextureFormat::RED;
                break;
            case ur::TEXTURE_DEPTH:
                src_tex->format = TextureFormat::Depth;
                break;
            }
        }

        tex->SetSize(src->width, src->height);

        rendergraph::node::Texture::Type type;
        switch (src->type)
        {
        case TextureType::Tex2D:
            type = rendergraph::node::Texture::Type::Tex2D;
            break;
        case TextureType::TexCube:
            type = rendergraph::node::Texture::Type::TexCube;
            break;
        }
        tex->SetType(type);

        rendergraph::node::Texture::Format format;
        switch (src->format)
        {
        case TextureFormat::RGBA16:
            format = rendergraph::node::Texture::Format::RGBA16;
            break;
        case TextureFormat::RGBA8:
            format = rendergraph::node::Texture::Format::RGBA8;
            break;
        case TextureFormat::RGBA4:
            format = rendergraph::node::Texture::Format::RGBA4;
            break;
        case TextureFormat::RGB:
            format = rendergraph::node::Texture::Format::RGB;
            break;
        case TextureFormat::RGB565:
            format = rendergraph::node::Texture::Format::RGB565;
            break;
        case TextureFormat::A8:
            format = rendergraph::node::Texture::Format::A8;
            break;
        case TextureFormat::Depth:
            format = rendergraph::node::Texture::Format::Depth;
            break;
        }
        tex->SetFormat(format);
    }
    else if (type == rttr::type::get<node::VertexArray>())
    {
        auto src = static_cast<const node::VertexArray*>(node);
        auto va = std::static_pointer_cast<rendergraph::node::VertexArray>(dst);

        std::vector<std::string> vert_tokens;
        cpputil::StringHelper::Split(src->vertices_data, ",", vert_tokens);
        std::vector<float> vertex_buf;
        vertex_buf.reserve(vert_tokens.size());
        for (auto& t : vert_tokens) {
            vertex_buf.push_back(std::stof(t));
        }
        va->SetVertexBuf(vertex_buf);

        std::vector<std::string> index_tokens;
        cpputil::StringHelper::Split(src->indices_data, ",", index_tokens);
        std::vector<unsigned short> index_buf;
        index_buf.reserve(index_tokens.size());
        for (auto& t : index_tokens) {
            index_buf.push_back(std::stoi(t));
        }
        va->SetIndexBuf(index_buf);

        std::vector<rendergraph::node::VertexArray::VertexAttrib> va_list;
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
        auto dst_shape = std::static_pointer_cast<rendergraph::node::PrimitiveShape>(dst);

        rendergraph::node::PrimitiveShape::Type type;
        switch (src->type)
        {
        case PrimitiveShapeType::Quad:
            type = rendergraph::node::PrimitiveShape::Type::Quad;
            break;
        case PrimitiveShapeType::Cube:
            type = rendergraph::node::PrimitiveShape::Type::Cube;
            break;
        default:
            assert(0);
        }
        dst_shape->SetType(type);

        rendergraph::node::PrimitiveShape::VertLayout layout;
        switch (src->layout)
        {
        case PrimitiveVertLayout::Pos:
            layout = rendergraph::node::PrimitiveShape::VertLayout::Pos;
            break;
        case PrimitiveVertLayout::PosTex:
            layout = rendergraph::node::PrimitiveShape::VertLayout::PosTex;
            break;
        case PrimitiveVertLayout::PosNormTex:
            layout = rendergraph::node::PrimitiveShape::VertLayout::PosNormTex;
            break;
        case PrimitiveVertLayout::PosNormTexTB:
            layout = rendergraph::node::PrimitiveShape::VertLayout::PosNormTexTB;
            break;
        default:
            assert(0);
        }
        dst_shape->SetVertLayout(layout);
    }
    else if (type == rttr::type::get<node::Model>())
    {
        auto src = static_cast<const node::Model*>(node);
        auto model = facade::ResPool::Instance().Fetch<model::Model>(src->filepath);
        auto model_inst = std::make_shared<model::ModelInstance>(model);
        std::static_pointer_cast<rendergraph::node::Model>(dst)->SetModel(model_inst);
    }
    else if (type == rttr::type::get<node::Heightfield>())
    {
        auto src = static_cast<const node::Heightfield*>(node);
        auto dst_hf = std::static_pointer_cast<rendergraph::node::Heightfield>(dst);
        dst_hf->SetSize(src->width, src->height);
    }
    // op
    else if (type == rttr::type::get<node::Clear>())
    {
        auto src = static_cast<const node::Clear*>(node);
        auto clear = std::static_pointer_cast<rendergraph::node::Clear>(dst);

        uint32_t type = 0;
        if (src->type.type & ClearType::CLEAR_COLOR) {
            type |= rendergraph::node::Clear::CLEAR_COLOR;
        }
        if (src->type.type & ClearType::CLEAR_DEPTH) {
            type |= rendergraph::node::Clear::CLEAR_DEPTH;
        }
        if (src->type.type & ClearType::CLEAR_STENCIL) {
            type |= rendergraph::node::Clear::CLEAR_STENCIL;
        }
        clear->SetClearType(type);

        clear->SetColor(src->color);
    }
    else if (type == rttr::type::get<node::Bind>())
    {
        auto src = static_cast<const node::Bind*>(node);
        std::static_pointer_cast<rendergraph::node::Bind>(dst)->SetChannel(src->channel);
    }
    else if (type == rttr::type::get<node::SetUniform>())
    {
        auto src = static_cast<const node::SetUniform*>(node);
        auto dst_set = std::static_pointer_cast<rendergraph::node::SetUniform>(dst);

        dst_set->SetVarName(src->var_name);

        rendergraph::VariableType type;
        switch (src->var_type)
        {
        case ShaderUniformType::Unknown:
        {
            auto& conns = node->GetAllInput()[rendergraph::node::SetUniform::I_VALUE]->GetConnecting();
            if (!conns.empty()) {
                assert(conns.size() == 1);
                type = TypeFrontToBack(conns[0]->GetFrom()->GetType());
            } else {
                type = rendergraph::VariableType::Any;
            }
        }
            break;
        case ShaderUniformType::Int:
            type = rendergraph::VariableType::Int;
            break;
        case ShaderUniformType::Bool:
            type = rendergraph::VariableType::Bool;
            break;
        case ShaderUniformType::Vector1:
            type = rendergraph::VariableType::Vector1;
            break;
        case ShaderUniformType::Vector2:
            type = rendergraph::VariableType::Vector2;
            break;
        case ShaderUniformType::Vector3:
            type = rendergraph::VariableType::Vector3;
            break;
        case ShaderUniformType::Vector4:
            type = rendergraph::VariableType::Vector4;
            break;
        case ShaderUniformType::Matrix2:
            type = rendergraph::VariableType::Matrix2;
            break;
        case ShaderUniformType::Matrix3:
            type = rendergraph::VariableType::Matrix3;
            break;
        case ShaderUniformType::Matrix4:
            type = rendergraph::VariableType::Matrix4;
            break;
        case ShaderUniformType::Sampler2D:
            type = rendergraph::VariableType::Sampler2D;
            break;
        case ShaderUniformType::SamplerCube:
            type = rendergraph::VariableType::SamplerCube;
            break;
        default:
            assert(0);
        }
        dst_set->SetVarType(type);
    }
    // state
    else if (type == rttr::type::get<node::Viewport>())
    {
        auto src = static_cast<const node::Viewport*>(node);
        auto vp = std::static_pointer_cast<rendergraph::node::Viewport>(dst);
        vp->SetParams(src->x, src->y, src->w, src->h);
    }
    else if (type == rttr::type::get<node::AlphaTest>())
    {
        auto src = static_cast<const node::AlphaTest*>(node);
        rendergraph::node::AlphaTest::Func func;
        switch (src->func)
        {
        case AlphaTestFunc::Off:
            func = rendergraph::node::AlphaTest::Func::Off;
            break;
        case AlphaTestFunc::Never:
            func = rendergraph::node::AlphaTest::Func::Never;
            break;
        case AlphaTestFunc::Less:
            func = rendergraph::node::AlphaTest::Func::Less;
            break;
        case AlphaTestFunc::Equal:
            func = rendergraph::node::AlphaTest::Func::Equal;
            break;
        case AlphaTestFunc::LEqual:
            func = rendergraph::node::AlphaTest::Func::LEqual;
            break;
        case AlphaTestFunc::Greater:
            func = rendergraph::node::AlphaTest::Func::Greater;
            break;
        case AlphaTestFunc::NotEqual:
            func = rendergraph::node::AlphaTest::Func::NotEqual;
            break;
        case AlphaTestFunc::GEqual:
            func = rendergraph::node::AlphaTest::Func::GEqual;
            break;
        case AlphaTestFunc::Always:
            func = rendergraph::node::AlphaTest::Func::Always;
            break;
        }
        std::static_pointer_cast<rendergraph::node::AlphaTest>(dst)->SetFunc(func);
    }
    else if (type == rttr::type::get<node::BlendEq>())
    {
        auto src = static_cast<const node::BlendEq*>(node);
        rendergraph::node::BlendEq::Mode mode;
        switch (src->mode)
        {
        case BlendEqMode::FuncAdd:
            mode = rendergraph::node::BlendEq::Mode::FuncAdd;
            break;
        case BlendEqMode::FuncSubtract:
            mode = rendergraph::node::BlendEq::Mode::FuncSubtract;
            break;
        case BlendEqMode::FuncReverseSubtract:
            mode = rendergraph::node::BlendEq::Mode::FuncReverseSubtract;
            break;
        case BlendEqMode::Min:
            mode = rendergraph::node::BlendEq::Mode::Min;
            break;
        case BlendEqMode::Max:
            mode = rendergraph::node::BlendEq::Mode::Max;
            break;
        }
    }
    else if (type == rttr::type::get<node::BlendFunc>())
    {
        auto src = static_cast<const node::BlendFunc*>(node);
        auto trans = [](const BlendFuncFactor& factor)->rendergraph::node::BlendFunc::Factor
        {
            rendergraph::node::BlendFunc::Factor ret;
            switch (factor)
            {
            case BlendFuncFactor::Off:
                ret = rendergraph::node::BlendFunc::Factor::Off;
                break;
            case BlendFuncFactor::Zero:
                ret = rendergraph::node::BlendFunc::Factor::Zero;
                break;
            case BlendFuncFactor::One:
                ret = rendergraph::node::BlendFunc::Factor::One;
                break;
            case BlendFuncFactor::SrcColor:
                ret = rendergraph::node::BlendFunc::Factor::SrcColor;
                break;
            case BlendFuncFactor::OneMinusSrcColor:
                ret = rendergraph::node::BlendFunc::Factor::OneMinusSrcColor;
                break;
            case BlendFuncFactor::DstColor:
                ret = rendergraph::node::BlendFunc::Factor::DstColor;
                break;
            case BlendFuncFactor::OneMinusDstColor:
                ret = rendergraph::node::BlendFunc::Factor::OneMinusDstColor;
                break;
            case BlendFuncFactor::SrcAlpha:
                ret = rendergraph::node::BlendFunc::Factor::SrcAlpha;
                break;
            case BlendFuncFactor::OneMinusSrcAlpha:
                ret = rendergraph::node::BlendFunc::Factor::OneMinusSrcAlpha;
                break;
            case BlendFuncFactor::DstAlpha:
                ret = rendergraph::node::BlendFunc::Factor::DstAlpha;
                break;
            case BlendFuncFactor::OneMinusDstAlpha:
                ret = rendergraph::node::BlendFunc::Factor::OneMinusDstAlpha;
                break;
            }
            return ret;
        };
        auto func = std::static_pointer_cast<rendergraph::node::BlendFunc>(dst);
        func->SetSrcFactor(trans(src->sfactor));
        func->SetDstFactor(trans(src->dfactor));
    }
    else if (type == rttr::type::get<node::Cull>())
    {
        auto src = static_cast<const node::Cull*>(node);
        rendergraph::node::Cull::Mode mode;
        switch (src->type)
        {
        case CullMode::Off:
            mode = rendergraph::node::Cull::Mode::Off;
            break;
        case CullMode::Front:
            mode = rendergraph::node::Cull::Mode::Front;
            break;
        case CullMode::Back:
            mode = rendergraph::node::Cull::Mode::Back;
            break;
        case CullMode::FrontAndBack:
            mode = rendergraph::node::Cull::Mode::FrontAndBack;
            break;
        }
        std::static_pointer_cast<rendergraph::node::Cull>(dst)->SetMode(mode);
    }
    else if (type == rttr::type::get<node::ZTest>())
    {
        auto src = static_cast<const node::ZTest*>(node);
        rendergraph::node::ZTest::Func func;
        switch (src->func)
        {
        case ZTestFunc::Off:
            func = rendergraph::node::ZTest::Func::Off;
            break;
        case ZTestFunc::Never:
            func = rendergraph::node::ZTest::Func::Never;
            break;
        case ZTestFunc::Less:
            func = rendergraph::node::ZTest::Func::Less;
            break;
        case ZTestFunc::Equal:
            func = rendergraph::node::ZTest::Func::Equal;
            break;
        case ZTestFunc::LEqual:
            func = rendergraph::node::ZTest::Func::LEqual;
            break;
        case ZTestFunc::Greater:
            func = rendergraph::node::ZTest::Func::Greater;
            break;
        case ZTestFunc::NotEqual:
            func = rendergraph::node::ZTest::Func::NotEqual;
            break;
        case ZTestFunc::GEqual:
            func = rendergraph::node::ZTest::Func::GEqual;
            break;
        case ZTestFunc::Always:
            func = rendergraph::node::ZTest::Func::Always;
            break;
        }
        std::static_pointer_cast<rendergraph::node::ZTest>(dst)->SetFunc(func);
    }
    else if (type == rttr::type::get<node::ZWrite>())
    {
        auto src = static_cast<const node::ZWrite*>(node);
        std::static_pointer_cast<rendergraph::node::ZWrite>(dst)->SetDepthWrite(src->enable);
    }
    else if (type == rttr::type::get<node::FrontFace>())
    {
        auto src = static_cast<const node::FrontFace*>(node);
        std::static_pointer_cast<rendergraph::node::FrontFace>(dst)->SetClockwise(src->clockwise);
    }
    // input
    else if (type == rttr::type::get<node::UserScript>())
    {
        auto src = static_cast<const node::UserScript*>(node);
        auto dst_script = std::static_pointer_cast<rendergraph::node::UserScript>(dst);

        dst_script->SetValue(src->code);

        rendergraph::node::UserScript::ReturnType type;
        switch (src->ret_type)
        {
        case UserScriptRetType::Void:
            type = rendergraph::node::UserScript::ReturnType::Void;
            break;
        case UserScriptRetType::Vec1Array:
            type = rendergraph::node::UserScript::ReturnType::Vec1Array;
            break;
        case UserScriptRetType::Vec2Array:
            type = rendergraph::node::UserScript::ReturnType::Vec2Array;
            break;
        case UserScriptRetType::Vec3Array:
            type = rendergraph::node::UserScript::ReturnType::Vec3Array;
            break;
        case UserScriptRetType::Vec4Array:
            type = rendergraph::node::UserScript::ReturnType::Vec4Array;
            break;
        default:
            assert(0);
        }
        dst_script->SetRetType(type);
    }
    // script
    else if (type == rttr::type::get<node::CustomExpression>())
    {
        auto src = static_cast<const node::CustomExpression*>(node);
        std::static_pointer_cast<rendergraph::node::CustomExpression>(dst)->SetCode(src->GetCode());
    }
    // value
    else if (type == rttr::type::get<node::Bool>())
    {
        auto src = static_cast<const node::Bool*>(node);
        std::static_pointer_cast<rendergraph::node::Bool>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Vector1>())
    {
        auto src = static_cast<const node::Vector1*>(node);
        std::static_pointer_cast<rendergraph::node::Vector1>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Vector2>())
    {
        auto src = static_cast<const node::Vector2*>(node);
        std::static_pointer_cast<rendergraph::node::Vector2>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Vector3>())
    {
        auto src = static_cast<const node::Vector3*>(node);
        std::static_pointer_cast<rendergraph::node::Vector3>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Vector4>())
    {
        auto src = static_cast<const node::Vector4*>(node);
        std::static_pointer_cast<rendergraph::node::Vector4>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Matrix2>())
    {
        auto src = static_cast<const node::Matrix2*>(node);
        std::static_pointer_cast<rendergraph::node::Matrix2>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Matrix3>())
    {
        auto src = static_cast<const node::Matrix3*>(node);
        std::static_pointer_cast<rendergraph::node::Matrix3>(dst)->SetValue(src->val);
    }
    else if (type == rttr::type::get<node::Matrix4>())
    {
        auto src = static_cast<const node::Matrix4*>(node);
        std::static_pointer_cast<rendergraph::node::Matrix4>(dst)->SetValue(src->val);
    }
    // math
    else if (type == rttr::type::get<node::PerspectiveMat>())
    {
        auto src = static_cast<const node::PerspectiveMat*>(node);
        std::static_pointer_cast<rendergraph::node::PerspectiveMat>(dst)->SetProps(
            src->fovy, src->aspect, src->znear, src->zfar
        );
    }
    else if (type == rttr::type::get<node::OrthoMat>())
    {
        auto src = static_cast<const node::OrthoMat*>(node);
        std::static_pointer_cast<rendergraph::node::OrthoMat>(dst)->SetProps(
            src->left, src->right, src->bottom, src->top, src->znear, src->zfar
        );
    }
    else if (type == rttr::type::get<node::LookAtMat>())
    {
        auto src = static_cast<const node::LookAtMat*>(node);
        std::static_pointer_cast<rendergraph::node::LookAtMat>(dst)->SetProps(
            src->eye, src->center, src->up
        );
    }
    else if (type == rttr::type::get<node::Translate>())
    {
        auto src = static_cast<const node::Translate*>(node);
        std::static_pointer_cast<rendergraph::node::Translate>(dst)->SetOffset(src->offset);
    }
    else if (type == rttr::type::get<node::Rotate>())
    {
        auto src = static_cast<const node::Rotate*>(node);
        auto dst_rot = std::static_pointer_cast<rendergraph::node::Rotate>(dst);
        dst_rot->SetAngle(src->angle * SM_DEG_TO_RAD);
        dst_rot->SetAxis(src->axis.Normalized());
    }
    else if (type == rttr::type::get<node::Scale>())
    {
        auto src = static_cast<const node::Scale*>(node);
        std::static_pointer_cast<rendergraph::node::Scale>(dst)->SetScale(src->scale);
    }
    // features
    else if (type == rttr::type::get<node::SeparableSSS>())
    {
        auto src = static_cast<const node::SeparableSSS*>(node);
        std::static_pointer_cast<rp::node::SeparableSSS>(dst)->SetProps(
            src->nsamples, src->strength, src->falloff
        );
    }

    // insert to cache
    if (dst) {
        dst->SetEnable(enable);
        eval.AddNodeMap(node, dst);
    }

    // connect input
    if (dst)
    {
        for (int i = 0, n = node->GetAllInput().size(); i < n; ++i)
        {
            auto& imports = dst->GetImports();
            if (node->IsExtensibleInputPorts() && i >= static_cast<int>(imports.size())) {
                continue;
            }
            rendergraph::Node::PortAddr from_port;
            auto& conns = node->GetAllInput()[i]->GetConnecting();
            if (conns.empty()) {
                continue;
            }
            assert(conns.size() == 1);
            auto& bp_from_port = conns[0]->GetFrom();
            assert(bp_from_port);
            if (CreateFromNode(eval, bp_from_port, from_port)) {
                rendergraph::make_connecting(from_port, { dst, i });
            }
        }
    }

    return dst;
}

int RenderGraph::TypeBackToFront(rendergraph::VariableType type, int count)
{
    int ret = -1;

    switch (type)
    {
    case rendergraph::VariableType::Any:
        ret = bp::PIN_ANY_VAR;
        break;
    case rendergraph::VariableType::Port:
        ret = bp::PIN_PORT;
        break;
    case rendergraph::VariableType::Texture:
        ret = PIN_TEXTURE;
        break;
    case rendergraph::VariableType::RenderTarget:
        ret = PIN_RENDERTARGET;
        break;
    case rendergraph::VariableType::Shader:
        ret = PIN_SHADER;
        break;
    case rendergraph::VariableType::Model:
        ret = PIN_MODEL;
        break;
    case rendergraph::VariableType::Int:
        ret = PIN_INT;
        break;
    case rendergraph::VariableType::Bool:
        ret = PIN_BOOL;
        break;
    case rendergraph::VariableType::Vector1:
        ret = PIN_VECTOR1;
        break;
    case rendergraph::VariableType::Vector2:
        ret = PIN_VECTOR2;
        break;
    case rendergraph::VariableType::Vector3:
        ret = PIN_VECTOR3;
        break;
    case rendergraph::VariableType::Vector4:
        ret = PIN_VECTOR4;
        break;
    case rendergraph::VariableType::Matrix2:
        ret = PIN_MATRIX2;
        break;
    case rendergraph::VariableType::Matrix3:
        ret = PIN_MATRIX3;
        break;
    case rendergraph::VariableType::Matrix4:
        ret = PIN_MATRIX4;
        break;
    case rendergraph::VariableType::Sampler2D:
        ret = PIN_SAMPLER2D;
        break;
    case rendergraph::VariableType::SamplerCube:
        ret = PIN_SAMPLE_CUBE;
        break;
    case rendergraph::VariableType::Vec1Array:
        ret = PIN_VECTOR1_ARRAY;
        break;
    case rendergraph::VariableType::Vec2Array:
        ret = PIN_VECTOR2_ARRAY;
        break;
    case rendergraph::VariableType::Vec3Array:
        ret = PIN_VECTOR3_ARRAY;
        break;
    case rendergraph::VariableType::Vec4Array:
        ret = PIN_VECTOR4_ARRAY;
        break;
    case rendergraph::VariableType::UserType:
        ret = bp::PIN_ANY_VAR;
        break;
    default:
        assert(0);
    }

    if (count > 1)
    {
        switch (type)
        {
        case rendergraph::VariableType::Vector1:
            ret = PIN_VECTOR1_ARRAY;
            break;
        case rendergraph::VariableType::Vector2:
            ret = PIN_VECTOR2_ARRAY;
            break;
        case rendergraph::VariableType::Vector3:
            ret = PIN_VECTOR3_ARRAY;
            break;
        case rendergraph::VariableType::Vector4:
            ret = PIN_VECTOR4_ARRAY;
            break;
        default:
            assert(0);
        }
    }

    return ret;
}

rendergraph::VariableType RenderGraph::TypeFrontToBack(int pin_type)
{
    rendergraph::VariableType ret = rendergraph::VariableType::Any;
    switch (pin_type)
    {
    case bp::PIN_ANY_VAR:
        ret = rendergraph::VariableType::Any;
        break;
    case bp::PIN_PORT:
        ret = rendergraph::VariableType::Port;
        break;
    case PIN_TEXTURE:
        ret = rendergraph::VariableType::Texture;
        break;
    case PIN_RENDERTARGET:
        ret = rendergraph::VariableType::RenderTarget;
        break;
    case PIN_SHADER:
        ret = rendergraph::VariableType::Shader;
        break;
    case PIN_MODEL:
        ret = rendergraph::VariableType::Model;
        break;
    case PIN_INT:
        ret = rendergraph::VariableType::Int;
        break;
    case PIN_BOOL:
        ret = rendergraph::VariableType::Bool;
        break;
    case PIN_VECTOR1:
        ret = rendergraph::VariableType::Vector1;
        break;
    case PIN_VECTOR2:
        ret = rendergraph::VariableType::Vector2;
        break;
    case PIN_VECTOR3:
        ret = rendergraph::VariableType::Vector3;
        break;
    case PIN_VECTOR4:
        ret = rendergraph::VariableType::Vector4;
        break;
    case PIN_MATRIX2:
        ret = rendergraph::VariableType::Matrix2;
        break;
    case PIN_MATRIX3:
        ret = rendergraph::VariableType::Matrix3;
        break;
    case PIN_MATRIX4:
        ret = rendergraph::VariableType::Matrix4;
        break;
    case PIN_SAMPLER2D:
        ret = rendergraph::VariableType::Sampler2D;
        break;
    case PIN_SAMPLE_CUBE:
        ret = rendergraph::VariableType::SamplerCube;
        break;
    case PIN_VECTOR1_ARRAY:
        ret = rendergraph::VariableType::Vec1Array;
        break;
    case PIN_VECTOR2_ARRAY:
        ret = rendergraph::VariableType::Vec2Array;
        break;
    case PIN_VECTOR3_ARRAY:
        ret = rendergraph::VariableType::Vec3Array;
        break;
    case PIN_VECTOR4_ARRAY:
        ret = rendergraph::VariableType::Vec4Array;
        break;
    }
    return ret;
}

bool RenderGraph::CreateFromNode(Evaluator& eval, const std::shared_ptr<bp::Pin>& bp_from_port,
                                 rendergraph::Node::PortAddr& from_port)
{
    auto& parent = bp_from_port->GetParent();
    auto p_type = parent.get_type();
    if (p_type == rttr::type::get<bp::node::Input>())
    {
        auto& input = static_cast<const bp::node::Input&>(parent);
        auto func_node = input.GetParent();
        if (!func_node) {
            return false;
        }
        auto& func_inputs = func_node->GetAllInput();
        for (int i = 0, n = func_inputs.size(); i < n; ++i)
        {
            if (func_inputs[i]->GetName() != input.GetName()) {
                continue;
            }
            auto& conns = func_node->GetAllInput()[i]->GetConnecting();
            if (conns.empty()) {
                return false;
            }
            assert(conns.size() == 1);
            auto bp_from_port = conns[0]->GetFrom();
            assert(bp_from_port);

            return CreateFromNode(eval, bp_from_port, from_port);
        }
        return false;
    }
    from_port.node = CreateGraphNode(eval, &bp_from_port->GetParent());
    from_port.idx  = bp_from_port->GetPosIdx();

    return true;
}

void RenderGraph::InitPortsBackFromFront(rendergraph::Node& back, const bp::Node& front)
{
    auto& inputs  = front.GetAllInput();
    auto& outputs = front.GetAllOutput();

    std::vector<rendergraph::Node::Port> imports, exports;
    imports.reserve(inputs.size());
    for (auto i : inputs)
    {
        rendergraph::Variable var;
        var.type = TypeFrontToBack(i->GetType());
//        var.name = std::string("in") + std::to_string(i);
        var.name = i->GetName();
        imports.push_back(var);
    }
    exports.reserve(outputs.size());
    for (auto o : outputs)
    {
        rendergraph::Variable var;
        var.type = TypeFrontToBack(o->GetType());
//        var.name = std::string("in") + std::to_string(i);
        var.name = o->GetName();
        exports.push_back(var);
    }
    back.SetImports(imports);
    back.SetExports(exports);
}

}