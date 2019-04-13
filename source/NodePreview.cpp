#include "renderlab/NodePreview.h"
#include "renderlab/Node.h"

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
    if (!m_draw_list || !m_node.GetRGNode()) {
        return;
    }

    auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();

    // draw to rt
    auto rt = rt_mgr.Fetch();
    rt->Bind();
    {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        auto renderer = rp::RenderMgr::Instance()->GetRenderer(rp::RenderType::SPRITE);
        auto shader = std::static_pointer_cast<pt2::Shader>(renderer->GetAllShaders()[0]);
        pt2::RenderTargetCtx ctx(rc, shader, rt_mgr.WIDTH, rt_mgr.HEIGHT);

        rp::RenderMgr::Instance()->SetRenderer(rp::RenderType::EXTERN);

        int shader_id = rc.GetBindedShader();
        size_t rt_depth = rc.GetRenderTargetDepth();
        m_draw_list->Draw(rg::RenderContext(rc));
        size_t rt_depth2 = rc.GetRenderTargetDepth();
        for (int i = 0; i < rt_depth2 - rt_depth; ++i) {
            rc.UnbindRenderTarget();
        }
        rc.BindShader(shader_id);

        sm::Matrix2D mat;
        mat.Scale(
            static_cast<float>(rt_mgr.WIDTH),
            static_cast<float>(rt_mgr.HEIGHT)
        );
        m_node.PreviewDraw(m_node.GetRGNode(), mat);
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
    if (!m_draw_list) {
        auto& rg_node = m_node.GetRGNode();
        if (rg_node) {
            m_draw_list = std::make_shared<rg::DrawList>(rg_node);
        }
    }
}

}