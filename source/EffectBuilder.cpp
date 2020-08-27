#include "renderlab/EffectBuilder.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/PassEnd.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/PinType.h"

#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/CompNode.h>
#include <blueprint/Connecting.h>

#include <fxlang/EffectParser.h>
#include <fxlang/ShaderGenerator.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompTransform.h>
#include <shadertrans/ShaderValidator.h>

#include <iostream>

namespace renderlab
{

EffectBuilder::EffectBuilder(const ee0::SubjectMgrPtr& sub_mgr)
    : m_sub_mgr(sub_mgr)
{
}

void EffectBuilder::Build(const std::string& fx, fxlang::EffectType type,
                          std::ostream& out)
{
    m_sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_CLEAR);

    m_fx = fx;
    m_type = type;

    auto clear = std::make_shared<node::Clear>();
    clear->m_clear_type = { rendergraph::node::Clear::Type::Color };
    clear->m_col.FromFloat(0.2f, 0.3f, 0.3f);
    InsertNode(clear, { -450, 50 }); 

    auto cull = std::make_shared<node::Cull>();
    cull->m_mode = rendergraph::node::Cull::Mode::Off;
    InsertNode(cull, { -300, 50 });

    bp::make_connecting(clear->GetAllOutput()[0], cull->GetAllInput()[0]);

    auto ztest = std::make_shared<node::ZTest>();
    ztest->m_func = rendergraph::node::ZTest::Func::Off;
    InsertNode(ztest, { -150, 50 });

    bp::make_connecting(cull->GetAllOutput()[0], ztest->GetAllInput()[0]);

    auto shader = BuildShaderNode(out);
    InsertNode(shader, { -200, -150 });

    auto bind = std::make_shared<node::Bind>();
    InsertNode(bind, { 0, 0 });

    bp::make_connecting(ztest->GetAllOutput()[0], bind->GetAllInput()[0]);
    bp::make_connecting(shader->GetAllOutput()[0], bind->GetAllInput()[1]);

    auto draw = std::make_shared<node::Draw>();
    InsertNode(draw, { 130, 0 });

    auto prim = std::make_shared<node::PrimitiveShape>();
    prim->m_type = rendergraph::node::PrimitiveShape::Type::Quad;
    prim->m_vert_layout = rendergraph::node::PrimitiveShape::VertLayout::PosTex;
    InsertNode(prim, { 0, -80 });

    bp::make_connecting(bind->GetAllOutput()[0], draw->GetAllInput()[0]);
    bp::make_connecting(prim->GetAllOutput()[0], draw->GetAllInput()[1]);

    auto pass_end = std::make_shared<node::PassEnd>();
    pass_end->SetOrder(0);
    InsertNode(pass_end, { 250, 0 });

    bp::make_connecting(draw->GetAllOutput()[0], pass_end->GetAllInput()[0]);
}

void EffectBuilder::InsertNode(const bp::NodePtr& bp_node, const sm::vec2& pos) const
{
    auto node = ns::NodeFactory::Create2D();

    auto& cnode = node->AddUniqueComp<bp::CompNode>();
    cnode.SetNode(bp_node);

    auto& caabb = node->GetUniqueComp<n2::CompBoundingBox>();
    auto& style = bp_node->GetStyle();
    caabb.SetSize(*node, sm::rect(style.width, style.height));

    auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
    ctrans.SetPosition(*node, pos);

    ee0::MsgHelper::InsertNode(*m_sub_mgr, node);
}

std::shared_ptr<Node> 
EffectBuilder::BuildShaderNode(std::ostream& out) const
{
	fxlang::EffectParser parser(m_fx, m_type);
    try {
        parser.Parse();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    auto shader = std::make_shared<node::Shader>();

    switch (m_type)
    {
    case fxlang::EffectType::Reshade:
        shader->SetLanguage(rendergraph::node::Shader::Language::HLSL);
        break;
    default:
        shader->SetLanguage(rendergraph::node::Shader::Language::GLSL);
    }

    std::string vs, fs;
    auto& effect = parser.GetEffect();
    fxlang::ShaderGenerator gen(effect);
    gen.Gen(vs, fs);

    shadertrans::ShaderValidator valid_vs(shadertrans::ShaderStage::VertexShader);
    valid_vs.Validate(vs, shader->GetLanguage() == rendergraph::node::Shader::Language::GLSL, out);

    shadertrans::ShaderValidator valid_fs(shadertrans::ShaderStage::PixelShader);
    valid_fs.Validate(fs, shader->GetLanguage() == rendergraph::node::Shader::Language::GLSL, out);

    shader->SetVert(vs);
    shader->SetFrag(fs);

    std::vector<bp::PinDesc> uniforms = shader->GetVertUnifs();
    std::copy(shader->GetFragUnifs().begin(), shader->GetFragUnifs().end(), std::back_inserter(uniforms));
    for (size_t i = 0, n = uniforms.size(); i < n; ++i)
    {
        std::shared_ptr<bp::Node> input = nullptr;

        auto& unif = uniforms[i];
        switch (unif.type)
        {
        case PIN_INT:
            input = std::make_shared<node::Int>();
            break;
        case PIN_BOOL:
            input = std::make_shared<node::Bool>();
            break;
        case PIN_VECTOR1:
            input = std::make_shared<node::Vector1>();
            break;
        case PIN_VECTOR2:
            input = std::make_shared<node::Vector2>();
            break;
        case PIN_VECTOR3:
            input = std::make_shared<node::Vector3>();
            break;
        case PIN_VECTOR4:
            input = std::make_shared<node::Vector4>();
            break;
        case PIN_SAMPLER2D:
            input = std::make_shared<node::Texture>();
            break;
        }

        if (input) 
        {
            const fxlang::Uniform* desc = nullptr;
            for (auto& u : effect.uniforms) {
                if (u.var.name == unif.name) {
                    desc = &u;
                    break;
                }
            }
            if (!desc) {
                for (auto& u : effect.uniforms) {
                    if ("_Globals." + u.var.name == unif.name) {
                        desc = &u;
                        break;
                    }
                }
            }

            if (desc) 
            {
                switch (desc->var.type)
                {
                case fxlang::VariableType::Bool:
                    std::static_pointer_cast<node::Bool>(input)->m_val = desc->var.b;
                    break;
                case fxlang::VariableType::Int:
                    std::static_pointer_cast<node::Int>(input)->m_val = desc->var.i;
                    break;
                case fxlang::VariableType::Float:
                    std::static_pointer_cast<node::Vector1>(input)->m_val = desc->var.f;
                    break;
                case fxlang::VariableType::Float2:
                    std::static_pointer_cast<node::Vector2>(input)->m_val
                        = sm::vec2(desc->var.f2[0], desc->var.f2[1]);
                    break;
                case fxlang::VariableType::Float3:
                    std::static_pointer_cast<node::Vector3>(input)->m_val 
                        = sm::vec3(desc->var.f3[0], desc->var.f3[1], desc->var.f3[2]);
                    break;
                case fxlang::VariableType::Float4:
                    std::static_pointer_cast<node::Vector4>(input)->m_val 
                        = sm::vec4(desc->var.f4[0], desc->var.f4[1], desc->var.f4[2], desc->var.f4[3]);
                    break;
                }
            }

            InsertNode(input, sm::vec2(-380.0f, -50.0f * i));
            bp::make_connecting(input->GetAllOutput()[0], shader->GetAllInput()[i]);
        }
    }

    return shader;
}

}