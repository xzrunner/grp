#include "renderlab/NodePreview.h"
#include "renderlab/Node.h"
#include "renderlab/Evaluator.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/PinType.h"

#include <blueprint/NodeHelper.h>

#include <SM_Matrix2D.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <painting2/Blackboard.h>
#include <painting2/RenderContext.h>
#include <painting2/RenderTarget.h>
#include <painting2/RenderSystem.h>
#include <painting2/RenderTargetCtx.h>
#include <painting2/WindowContext.h>
#include <painting2/WindowCtxRegion.h>
#include <painting2/Shader.h>
#include <node2/RenderSystem.h>
#include <rendergraph/Node.h>
#include <rendergraph/DrawList.h>
#include <rendergraph/RenderContext.h>
#include <rendergraph/RenderContext.h>
#include <rendergraph/node/Texture.h>
#include <renderpipeline/RenderMgr.h>
#include <renderpipeline/IRenderer.h>

namespace rlab
{

void NodePreview::Draw(const Evaluator& eval, const Node& node, const n2::RenderParams& rp)
{
    auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();
    auto rt = rt_mgr.Fetch();

    rt->Bind();
    bool succ = DrawToRT(eval, node);
    rt->Unbind();

    if (succ) {
        DrawFromRT(node, rp, *rt);
    }

    rt_mgr.Return(rt);
}

bool NodePreview::DrawToRT(const Evaluator& eval, const Node& node)
{
    auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    auto renderer = rp::RenderMgr::Instance()->GetRenderer(rp::RenderType::SPRITE);
    auto shader = std::static_pointer_cast<pt2::Shader>(renderer->GetAllShaders()[0]);
    pt2::RenderTargetCtx ctx(rc, shader, rt_mgr.WIDTH, rt_mgr.HEIGHT);

    auto type = node.get_type();
    if (type == rttr::type::get<node::Texture>())
    {
        auto rg_node = node.GetRGNode();
        if (!rg_node) {
            return false;
        }
        assert(rg_node && rg_node->get_type() == rttr::type::get<rg::node::Texture>());
        auto tex = std::static_pointer_cast<rg::node::Texture>(rg_node);

        sm::Matrix2D mat;
        mat.Scale(
            static_cast<float>(rt_mgr.WIDTH),
            static_cast<float>(rt_mgr.HEIGHT)
        );
        pt2::RenderSystem::DrawTexture(tex->GetWidth(), tex->GetHeight(), tex->GetTexID(), sm::rect(1, 1), mat);

        return true;
    }
    else if (type == rttr::type::get<node::Preview>())
    {
        auto& conns = node.GetAllInput()[0]->GetConnecting();
        if (conns.empty()) {
            return false;
        }
        auto f_pin = conns[0]->GetFrom();
        if (f_pin->GetOldType() != PIN_SAMPLER2D && f_pin->GetOldType() != PIN_SAMPLE_CUBE) {
            return false;
        }
        auto rg_node = static_cast<const Node&>(f_pin->GetParent()).GetRGNode();

        rg::RenderContext rg_rc(rc);
        const int port_idx = f_pin->GetPosIdx();
        rg::ShaderVariant var;
        uint32_t flags;
        rg_node->Eval(rg_rc, port_idx, var, flags);
        if (var.res_id == 0) {
            return false;
        }
        if (var.type != rg::VariableType::Sampler2D &&
            var.type != rg::VariableType::SamplerCube) {
            return false;
        }

        sm::Matrix2D mat;
        mat.Scale(
            static_cast<float>(rt_mgr.WIDTH),
            static_cast<float>(rt_mgr.HEIGHT)
        );

        pt2::RenderSystem::DrawTexture(0, 0, var.res_id, sm::rect(1, 1), mat);

        return true;
    }
    else
    {
//        eval.Draw(rg::RenderContext(rc), &node);
    }

    return false;
}

void NodePreview::DrawFromRT(const Node& node, const n2::RenderParams& rp, const pt2::RenderTarget& rt)
{
    auto mt4 = sm::mat4(bp::NodeHelper::CalcPreviewMat(node, rp.GetMatrix()));
    const auto scale = mt4.GetScale();
    const float hw = scale.x * 0.5f;
    const float hh = scale.y * 0.5f;
    const auto pos = mt4.GetTranslate();
    const float vertices[] = {
        pos.x - hw, pos.y - hh,
        pos.x + hw, pos.y - hh,
        pos.x + hw, pos.y + hh,
        pos.x - hw, pos.y + hh,
    };
    const float texcoords[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1
    };
    pt2::RenderSystem::DrawTexQuad(vertices, texcoords, rt.GetTexID(), 0xffffffff);
}

}