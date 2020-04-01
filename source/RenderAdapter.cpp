#include "renderlab/RenderAdapter.h"
#include "renderlab/PinType.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/node/Shader.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>

#include <cpputil/StringHelper.h>
#include <rendergraph/node/VertexArray.h>
#include <rendergraph/node/Shader.h>
#include <rendergraph/node/Model.h>
#include <model/Model.h>
#include <model/ModelInstance.h>
#include <facade/ImageLoader.h>
#include <facade/ResPool.h>

#include <boost/filesystem.hpp>

#include <assert.h>

namespace renderlab
{

int RenderAdapter::TypeBackToFront(rendergraph::VariableType type, int count)
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

void RenderAdapter::Front2Back(const bp::Node& front, dag::Node<rendergraph::Variable>& back,
                               const std::string& dir)
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
    else if (type == rttr::type::get<node::Shader>())
    {
        auto& src = static_cast<const node::Shader&>(front);
        static_cast<rendergraph::node::Shader&>(back).SetCodes(src.GetVert(), src.GetFrag());
    }
    else if (type == rttr::type::get<node::Texture>())
    {
        auto& src = static_cast<const node::Texture&>(front);
        auto& dst = static_cast<rendergraph::node::Texture&>(back);

        auto filepath = boost::filesystem::absolute(dst.GetFilepath(), dir).string();
        facade::ImageLoader loader(filepath);
        auto ur_wrap = static_cast<ur::TEXTURE_WRAP>(src.m_wrap);
        auto ur_filter = static_cast<ur::TEXTURE_FILTER>(src.m_filter);
        if (loader.Load(ur_wrap, ur_filter)) {
            dst.SetFilepath(filepath);
            dst.SetTexID(loader.GetID());
        } else {
            dst.SetFilepath("");
        }
    }
    else if (type == rttr::type::get<node::Model>())
    {
        auto& src = static_cast<const node::Model&>(front);
        auto model = facade::ResPool::Instance().Fetch<model::Model>(src.filepath);
        auto model_inst = std::make_shared<model::ModelInstance>(model);
        static_cast<rendergraph::node::Model&>(back).SetModel(model_inst);
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