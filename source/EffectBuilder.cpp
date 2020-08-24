#include "renderlab/EffectBuilder.h"
#include "renderlab/node/Shader.h"

#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/CompNode.h>

#include <fxlang/EffectParser.h>
#include <fxlang/ShaderGenerator.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompTransform.h>

namespace renderlab
{

EffectBuilder::EffectBuilder(const ee0::SubjectMgrPtr& sub_mgr)
    : m_sub_mgr(sub_mgr)
{
}

void EffectBuilder::Build(const std::string& fx, fxlang::EffectType type)
{
    m_sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_CLEAR);

    m_fx = fx;
    m_type = type;

	fxlang::EffectParser parser(fx, type);
	parser.Parse();

    auto shader = std::make_shared<node::Shader>();

    switch (type)
    {
    case fxlang::EffectType::Reshade:
        shader->SetLanguage(rendergraph::node::Shader::Language::HLSL);
        break;
    default:
        shader->SetLanguage(rendergraph::node::Shader::Language::GLSL);
    }

    std::string vs, fs;
    fxlang::ShaderGenerator gen(parser.GetEffect());
    gen.Gen(vs, fs);
    shader->SetVert(vs);
    shader->SetFrag(fs);

    InsertNode(shader, { -200, -50 });
}

void EffectBuilder::InsertNode(const bp::NodePtr& bp_node, const sm::vec2& pos)
{
    auto node = ns::NodeFactory::Create2D();

    auto& cnode = node->AddUniqueComp<bp::CompNode>();
    cnode.SetNode(bp_node);

    auto& caabb = node->GetUniqueComp<n2::CompBoundingBox>();
    auto& style = bp_node->GetStyle();
    caabb.SetSize(*node, sm::rect(style.width, style.height));

    auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
    const float x = -200;
    const float y = -50;
    ctrans.SetPosition(*node, sm::vec2(x, y));

    ee0::MsgHelper::InsertNode(*m_sub_mgr, node);
}

}