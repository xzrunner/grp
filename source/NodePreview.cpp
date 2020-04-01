#include "renderlab/NodePreview.h"
#include "renderlab/Node.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/PinType.h"
#include "renderlab/Evaluator.h"

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

namespace renderlab
{

void NodePreview::Draw(const bp::Node& front_node, const rendergraph::Node& back_node,
                       const n2::RenderParams& rp, const Evaluator& eval)
{
    auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();
    auto rt = rt_mgr.Fetch();

    rt->Bind();
    bool succ = DrawToRT(front_node, back_node, eval);
    rt->Unbind();

    if (succ) {
        DrawFromRT(front_node, rp, *rt);
    }

    rt_mgr.Return(rt);
}

bool NodePreview::DrawToRT(const bp::Node& front_node,
                           const rendergraph::Node& back_node,
                           const Evaluator& eval)
{
    auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    auto renderer = rp::RenderMgr::Instance()->GetRenderer(rp::RenderType::SPRITE);
    auto shader = std::static_pointer_cast<pt2::Shader>(renderer->GetAllShaders()[0]);
    pt2::RenderTargetCtx ctx(rc, shader, rt_mgr.WIDTH, rt_mgr.HEIGHT);

    auto type = front_node.get_type();
    if (type == rttr::type::get<node::Texture>())
    {
        assert(back_node.get_type() == rttr::type::get<rendergraph::node::Texture>());
        auto& tex = static_cast<const rendergraph::node::Texture&>(back_node);

        sm::Matrix2D mat;
        mat.Scale(
            static_cast<float>(rt_mgr.WIDTH),
            static_cast<float>(rt_mgr.HEIGHT)
        );
        pt2::RenderSystem::DrawTexture(tex.GetWidth(), tex.GetHeight(), tex.GetTexID(), sm::rect(1, 1), mat);
    }
    else if (type == rttr::type::get<node::Preview>())
    {
        auto& conns = front_node.GetAllInput()[0]->GetConnecting();
        if (conns.empty()) {
            return false;
        }
        auto f_pin = conns[0]->GetFrom();
        if (f_pin->GetOldType() != PIN_SAMPLER2D && f_pin->GetOldType() != PIN_SAMPLE_CUBE) {
            return false;
        }

        rendergraph::RenderContext rg_rc(rc);
        const int port_idx = f_pin->GetPosIdx();
        rendergraph::ShaderVariant var;
        uint32_t flags;
        back_node.Eval(rg_rc, port_idx, var, flags);
        if (var.res_id == 0) {
            return false;
        }
        if (var.type != rendergraph::VariableType::Sampler2D &&
            var.type != rendergraph::VariableType::SamplerCube) {
            return false;
        }

        sm::Matrix2D mat;
        mat.Scale(
            static_cast<float>(rt_mgr.WIDTH),
            static_cast<float>(rt_mgr.HEIGHT)
        );

        pt2::RenderSystem::DrawTexture(0, 0, var.res_id, sm::rect(1, 1), mat);
    }
    else
    {
        auto rg_rc = std::make_shared<rendergraph::RenderContext>(rc);
        eval.Draw(rg_rc, &back_node);
    }

    return true;
}

void NodePreview::DrawFromRT(const bp::Node& front_node, const n2::RenderParams& rp,
                             const pt2::RenderTarget& rt)
{
    auto mt4 = sm::mat4(bp::NodeHelper::CalcPreviewMat(front_node, rp.GetMatrix()));
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