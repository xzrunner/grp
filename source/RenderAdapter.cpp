#include "renderlab/RenderAdapter.h"
#include "renderlab/PinType.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/node/CustomFunction.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/SubGraph.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>
#include <shaderlab/ShaderAdapter.h>

#include <cpputil/StringHelper.h>
#include <unirender/Texture.h>
#include <unirender/Device.h>
#include <rendergraph/node/VertexArray.h>
#include <rendergraph/node/CustomFunction.h>
#include <rendergraph/node/CustomData.h>
#include <rendergraph/node/Shader.h>
#include <rendergraph/node/Model.h>
#include <rendergraph/node/SubGraph.h>
#include <rendergraph/node/ShaderGraph.h>
#include <model/Model.h>
#include <model/ModelInstance.h>
#include <facade/ImageLoader.h>
#include <facade/ResPool.h>

#include <boost/filesystem.hpp>

#include <iostream>

#include <assert.h>

namespace
{

std::vector<std::pair<int, rendergraph::VariableType>> PAIR_TYPES = 
{
	{ bp::PIN_ANY_VAR,              rendergraph::VariableType::Any },
	{ bp::PIN_PORT,                 rendergraph::VariableType::Port },
	{ renderlab::PIN_TEXTURE,       rendergraph::VariableType::Texture },
	{ renderlab::PIN_RENDERTARGET,  rendergraph::VariableType::RenderTarget },
	{ renderlab::PIN_SHADER,        rendergraph::VariableType::Shader },
	{ renderlab::PIN_MODEL,         rendergraph::VariableType::Model },
	{ bp::PIN_STRING,               rendergraph::VariableType::String },
	{ renderlab::PIN_INT,           rendergraph::VariableType::Int },
	{ renderlab::PIN_BOOL,          rendergraph::VariableType::Bool },
	{ renderlab::PIN_VECTOR1,       rendergraph::VariableType::Vector1 },
	{ renderlab::PIN_VECTOR2,       rendergraph::VariableType::Vector2 },
	{ renderlab::PIN_VECTOR3,       rendergraph::VariableType::Vector3 },
	{ renderlab::PIN_VECTOR4,       rendergraph::VariableType::Vector4 },
	{ renderlab::PIN_MATRIX2,       rendergraph::VariableType::Matrix2 },
	{ renderlab::PIN_MATRIX3,       rendergraph::VariableType::Matrix3 },
	{ renderlab::PIN_MATRIX4,       rendergraph::VariableType::Matrix4 },
	{ renderlab::PIN_SAMPLER2D,     rendergraph::VariableType::Sampler2D },
	{ renderlab::PIN_SAMPLE_CUBE,   rendergraph::VariableType::SamplerCube },
	{ renderlab::PIN_VECTOR1_ARRAY, rendergraph::VariableType::Vec1Array },
	{ renderlab::PIN_VECTOR2_ARRAY, rendergraph::VariableType::Vec2Array },
	{ renderlab::PIN_VECTOR3_ARRAY, rendergraph::VariableType::Vec3Array },
	{ renderlab::PIN_VECTOR4_ARRAY, rendergraph::VariableType::Vec4Array },
	{ bp::PIN_ANY_VAR,              rendergraph::VariableType::UserType },
};

}

namespace renderlab
{

int RenderAdapter::TypeBackToFront(rendergraph::VariableType type, int count)
{
    int ret = -1;

	for (auto& pair : PAIR_TYPES) {
		if (pair.second == type) {
			ret = pair.first;
			break;
		}
	}
	assert(ret >= 0);

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

rendergraph::VariableType RenderAdapter::TypeFrontToBack(int type)
{
	for (auto& pair : PAIR_TYPES) {
		if (pair.first == type) {
			return pair.second;
		}
	}
	assert(0);
	return rendergraph::VariableType::Any;
}

void RenderAdapter::Front2Back(const ur::Device& dev, const bp::Node& front,
                               dag::Node<rendergraph::Variable>& back, const std::string& dir)
{
    auto type = front.get_type();
    if (type == rttr::type::get<node::VertexArray>())
    {
        auto& src = static_cast<const node::VertexArray&>(front);
        auto& dst = static_cast<rendergraph::node::VertexArray&>(back);

        std::vector<std::string> vert_tokens;
        cpputil::StringHelper::Split(src.vertices_data, ",", vert_tokens);
        std::vector<float> vertex_buf;
        vertex_buf.reserve(vert_tokens.size());
        for (auto& t : vert_tokens) {
            vertex_buf.push_back(std::stof(t));
        }
        dst.SetVertexBuf(vertex_buf);

        std::vector<std::string> index_tokens;
        cpputil::StringHelper::Split(src.indices_data, ",", index_tokens);
        std::vector<unsigned short> index_buf;
        index_buf.reserve(index_tokens.size());
        for (auto& t : index_tokens) {
            index_buf.push_back(std::stoi(t));
        }
        dst.SetIndexBuf(index_buf);

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
        if (src.position.num > 0) {
            va_list.push_back({ src.position.name, src.position.num, get_type_size(src.position.type) });
        }
        if (src.normal.num > 0) {
            va_list.push_back({ src.normal.name, src.normal.num, get_type_size(src.normal.type) });
        }
        if (src.texture.num > 0) {
            va_list.push_back({ src.texture.name, src.texture.num, get_type_size(src.texture.type) });
        }
        dst.SetVertList(va_list);
    }
    else if (type == rttr::type::get<node::CustomFunction>())
    {
        auto src = static_cast<const node::CustomFunction&>(front);
        static_cast<rendergraph::node::CustomFunction&>(back).SetCode(src.GetCode());
    }
    else if (type == rttr::type::get<node::CustomData>())
    {
        auto src = static_cast<const node::CustomData&>(front);
        auto dst_script = static_cast<rendergraph::node::CustomData&>(back);
        dst_script.SetCode(src.m_code);
        dst_script.SetRetType(src.m_ret_type);
    }
    else if (type == rttr::type::get<node::Shader>())
    {
        auto& src = static_cast<const node::Shader&>(front);
        auto& dst = static_cast<rendergraph::node::Shader&>(back);
        dst.SetLanguage(src.GetLanguage());
        dst.SetCodes(src.GetVert(), src.GetFrag(), std::cerr);
    }
    else if (type == rttr::type::get<node::ShaderGraph>())
    {
        auto& src = static_cast<const node::ShaderGraph&>(front);
        auto& dst = static_cast<rendergraph::node::ShaderGraph&>(back);

        std::vector<std::pair<std::string, ur::TexturePtr>> textures;
        auto fs = shaderlab::ShaderAdapter::BuildShaderCode(src.m_filepath, dev, textures);
        dst.Init(dev, fs, textures);

        const_cast<node::ShaderGraph&>(src).m_vert = dst.GetVert();
        const_cast<node::ShaderGraph&>(src).m_frag = dst.GetFrag();
    }
    else if (type == rttr::type::get<node::Texture>())
    {
        auto& src = static_cast<const node::Texture&>(front);
        auto& dst = static_cast<rendergraph::node::Texture&>(back);

        // update texture
        if (src.m_filepath != dst.GetFilepath() ||
            !dst.GetTexture())
        {
            auto filepath = boost::filesystem::absolute(src.m_filepath, dir).string();
            facade::ImageLoader loader(filepath);

            if (loader.Load(dev)) {
                dst.SetFilepath(filepath);
                dst.SetTexture(loader.GetTexture());
            } else {
                dst.SetFilepath("");
                dst.SetTexture(nullptr);
            }
        }

        // update sample
        if (src.m_wrap != dst.GetWrapping() ||
            src.m_filter != dst.GetFiltering() ||
            !dst.GetSampler())
        {
            ur::TextureWrap ur_wrap;
            switch (src.m_wrap)
            {
            case rendergraph::node::Texture::Wrapping::Repeat:
                ur_wrap = ur::TextureWrap::Repeat;
                break;
            case rendergraph::node::Texture::Wrapping::MirroredRepeat:
                ur_wrap = ur::TextureWrap::MirroredRepeat;
                break;
            case rendergraph::node::Texture::Wrapping::ClampToEdge:
                ur_wrap = ur::TextureWrap::ClampToEdge;
                break;
            case rendergraph::node::Texture::Wrapping::ClampToBorder:
                ur_wrap = ur::TextureWrap::ClampToBorder;
                break;
            default:
                assert(0);
            }

            ur::TextureMinificationFilter ur_min_filter;
            ur::TextureMagnificationFilter ur_mag_filter;
            switch (src.m_filter)
            {
            case rendergraph::node::Texture::Filtering::Nearest:
                ur_min_filter = ur::TextureMinificationFilter::Nearest;
                ur_mag_filter = ur::TextureMagnificationFilter::Nearest;
                break;
            case rendergraph::node::Texture::Filtering::Linear:
                ur_min_filter = ur::TextureMinificationFilter::Linear;
                ur_mag_filter = ur::TextureMagnificationFilter::Linear;
                break;
            }

            dst.SetWrapping(src.m_wrap);
            dst.SetFiltering(src.m_filter);

            auto sampler = dev.CreateTextureSampler(ur_min_filter, ur_mag_filter, ur_wrap, ur_wrap);
            dst.SetSampler(sampler);
        }
    }
    else if (type == rttr::type::get<node::Model>())
    {
        auto& src = static_cast<const node::Model&>(front);
        auto model = facade::ResPool::Instance().Fetch<model::Model>(src.filepath, &dev);
        auto model_inst = std::make_shared<model::ModelInstance>(model);
        static_cast<rendergraph::node::Model&>(back).SetModel(model_inst);
    }
    else if (type == rttr::type::get<node::SubGraph>())
    {
        auto& src = static_cast<const node::SubGraph&>(front);
        auto& dst = static_cast<rendergraph::node::SubGraph&>(back);

		auto front2back = [](std::vector<dag::Node<rendergraph::Variable>::Port>& dst, 
			                 const std::vector<std::shared_ptr<bp::Pin>>& src) 
		{
			dst.resize(src.size());
			for (int i = 0, n = src.size(); i < n; ++i) 
			{
				auto type = TypeFrontToBack(src[i]->GetType());
				auto name = src[i]->GetName();
				dst[i] = dag::Node<rendergraph::Variable>::Port({ type, name });
			}
		};

        std::vector<dag::Node<rendergraph::Variable>::Port> dst_in, dst_out;
		front2back(dst_in, src.GetAllInput());
		front2back(dst_out, src.GetAllOutput());

        dst.Setup(src.GetBackGraph(), dst_in, dst_out);
    }
    //// bp
    //else if (type == rttr::type::get<bp::node::Function>())
    //{
    //    auto src = static_cast<const bp::node::Function*>(node);
    //    auto dst_group = std::make_shared<rendergraph::node::Group>();
    //    dst = dst_group;

    //    InitPortsBackFromFront(*dst, *node);

    //    auto& src_children = src->GetChildren();
    //    std::vector<rendergraph::NodePtr> dst_children;
    //    for (auto& src_c : src_children)
    //    {
    //        if (!src_c->HasUniqueComp<bp::CompNode>()) {
    //            continue;
    //        }

    //        auto& cnode = src_c->GetUniqueComp<bp::CompNode>();
    //        auto bp_node = cnode.GetNode();
    //        auto node_type = bp_node->get_type();
    //        if (node_type == rttr::type::get<bp::node::Input>() ||
    //            node_type == rttr::type::get<bp::node::Output>()) {
    //            continue;
    //        }

    //        auto dst_c = CreateGraphNode(eval, bp_node.get());
    //        dst_children.push_back(dst_c);
    //    }

    //    std::vector<std::pair<rendergraph::NodePtr, int>> dst_outputs;
    //    dst_outputs.resize(dst->GetExports().size());
    //    for (auto& src_c : src_children)
    //    {
    //        if (!src_c->HasUniqueComp<bp::CompNode>()) {
    //            continue;
    //        }

    //        auto& cnode = src_c->GetUniqueComp<bp::CompNode>();
    //        auto bp_node = cnode.GetNode();
    //        auto node_type = bp_node->get_type();
    //        if (node_type == rttr::type::get<bp::node::Output>())
    //        {
    //            auto output_node = std::static_pointer_cast<bp::node::Output>(bp_node);

    //            auto& inputs = bp_node->GetAllInput();
    //            assert(inputs.size() == 1);
    //            auto& conns = inputs[0]->GetConnecting();
    //            assert(conns.size() == 1);
    //            auto p_idx = conns[0]->GetFrom()->GetPosIdx();
    //            auto& bp_node = conns[0]->GetFrom()->GetParent();
    //            auto rg_node = eval.QueryRGNode(&bp_node);

    //            int idx = -1;
    //            for (int i = 0, n = dst->GetExports().size(); i < n; ++i)
    //            {
    //                if (dst->GetExports()[i].var.full_name == output_node->GetName()) {
    //                    idx = i;
    //                    break;
    //                }
    //            }
    //            assert(idx != -1);
    //            dst_outputs[idx] = { rg_node, p_idx };
    //        }
    //    }

    //    dst_group->SetChildren(dst_children, dst_outputs);
    //}
}

//void RenderGraph::InitPortsBackFromFront(rendergraph::Node& back, const bp::Node& front)
//{
//    auto& inputs  = front.GetAllInput();
//    auto& outputs = front.GetAllOutput();
//
//    std::vector<rendergraph::Node::Port> imports, exports;
//    imports.reserve(inputs.size());
//    for (auto i : inputs)
//    {
//        rendergraph::Variable var;
//        var.type = TypeFrontToBack(i->GetType());
////        var.name = std::string("in") + std::to_string(i);
//        var.name = i->GetName();
//
//        dag::Node<rendergraph::Variable>::Port port;
//        port.var.type = var;
//        port.var.full_name = i->GetName();
//        imports.push_back(port);
//    }
//    exports.reserve(outputs.size());
//    for (auto o : outputs)
//    {
//        rendergraph::Variable var;
//        var.type = TypeFrontToBack(o->GetType());
////        var.name = std::string("in") + std::to_string(i);
//        var.name = o->GetName();
//
//        dag::Node<rendergraph::Variable>::Port port;
//        port.var.type = var;
//        port.var.full_name = o->GetName();
//        exports.push_back(port);
//    }
//    back.SetImports(imports);
//    back.SetExports(exports);
//}

}