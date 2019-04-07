#include "renderlab/NodePreview.h"
#include "renderlab/Node.h"

#include <blueprint/NodeHelper.h>

#include <SM_Matrix2D.h>
#include <unirender/Blackboard.h>
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
#include <renderpipeline/RenderMgr.h>
#include <renderpipeline/IRenderer.h>

namespace rlab
{

NodePreview::NodePreview(const Node& node)
    : m_node(node)
{
}

void NodePreview::Draw(const n2::RenderParams& rp) const
{
    if (!m_draw_list) {
        return;
    }

    // draw to rt

    auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();

    auto rt = rt_mgr.Fetch();
    rt->Bind();
    //{
    //    pt2::WindowCtxRegion wcr(static_cast<float>(rt_mgr.WIDTH), static_cast<float>(rt_mgr.HEIGHT));

    //    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    //    m_draw_list->Draw(rc);
    //    m_node.PreviewDraw(m_rg_node, rp);
    //}
//    {
//        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
////        pt2::RenderTargetCtx ctx(rc, nullptr, rt_mgr.WIDTH, rt_mgr.HEIGHT);
//
//        auto wc = std::make_shared<pt2::WindowContext>(rt_mgr.WIDTH, rt_mgr.HEIGHT, rt_mgr.WIDTH, rt_mgr.HEIGHT);
//        rp::RenderMgr::Instance()->BindWndCtx2D(wc);
//        wc->Bind();
//
//        m_draw_list->Draw(rc);
//        m_node.PreviewDraw(m_rg_node, rp);
//
//        rp::RenderMgr::Instance()->UnbindWndCtx2D(wc);
//        auto& wc_ori = pt2::Blackboard::Instance()->GetWindowContext();
//        wc_ori->Bind();
//    }
    {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        auto renderer = rp::RenderMgr::Instance()->GetRenderer(rp::RenderType::SPRITE);
        auto shader = std::static_pointer_cast<pt2::Shader>(renderer->GetAllShaders()[0]);
        pt2::RenderTargetCtx ctx(rc, shader, rt_mgr.WIDTH, rt_mgr.HEIGHT);

        m_draw_list->Draw(rc);

        sm::Matrix2D mat;
        mat.Scale(
            static_cast<float>(rt_mgr.WIDTH),
            static_cast<float>(rt_mgr.HEIGHT)
        );
        m_node.PreviewDraw(m_rg_node, mat);
    }
    rt->Unbind();

    // draw rt to screen

    auto mt4 = sm::mat4(bp::NodeHelper::CalcPreviewMat(m_node, rp.GetMatrix()));
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
    pt2::RenderSystem::DrawTexQuad(vertices, texcoords, rt->GetTexID(), 0xffffffff);

    rt_mgr.Return(rt);
}

void NodePreview::Update()
{
    m_rg.ClearCache();
    m_rg_node = m_rg.CreateGraphNode(m_node);
    m_draw_list = std::make_shared<rg::DrawList>(m_rg_node);
}

}