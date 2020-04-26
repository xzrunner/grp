#include "renderlab/NodePreview.h"
#include "renderlab/Node.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/PinType.h"
#include "renderlab/Evaluator.h"

#include <blueprint/NodeHelper.h>

#include <SM_Matrix2D.h>
#include <unirender2/Device.h>
#include <unirender2/Context.h>
#include <unirender2/Framebuffer.h>
#include <unirender2/Texture.h>
#include <unirender2/TextureDescription.h>
#include <unirender2/ShaderProgram.h>
#include <unirender2/Factory.h>
#include <painting0/ModelMatUpdater.h>
#include <painting2/Blackboard.h>
#include <painting2/RenderContext.h>
#include <painting2/RenderTarget.h>
#include <painting2/RenderSystem.h>
#include <painting2/RenderTargetCtx.h>
#include <painting2/RenderTargetMgr.h>
#include <painting2/WindowContext.h>
#include <painting2/WindowCtxRegion.h>
#include <painting2/Shader.h>
#include <painting2/ViewMatUpdater.h>
#include <painting2/ProjectMatUpdater.h>
#include <node2/RenderSystem.h>
#include <rendergraph/Node.h>
#include <rendergraph/DrawList.h>
#include <rendergraph/RenderContext.h>
#include <rendergraph/RenderContext.h>
#include <rendergraph/node/Texture.h>
#include <renderpipeline/RenderMgr.h>
#include <renderpipeline/IRenderer.h>

namespace
{

const size_t TEX_SIZE = 1024;

}

namespace renderlab
{

void NodePreview::Draw(const ur2::Device& dev, ur2::Context& ctx,
                       const bp::Node& front_node, const rendergraph::Node& back_node,
                       const n2::RenderParams& rp, const Evaluator& eval)
{
    //ur2::TextureDescription desc;
    //desc.width  = TEX_SIZE;
    //desc.height = TEX_SIZE;
    //desc.target = ur2::TextureTarget::Texture2D;
    //desc.format = ur2::TextureFormat::RGBA8;
    //auto tex = dev.CreateTexture(desc);

    //auto fbo = dev.CreateFramebuffer();

    //const auto type = ur2::AttachmentType::Color0;
    //fbo->SetAttachment(type, ur2::TextureTarget::Texture2D, tex, nullptr);

    auto fbo = pt2::RenderTargetMgr::Instance()->Fetch(dev);

    rp::RenderMgr::Instance()->Flush(dev, ctx);

    ctx.SetFramebuffer(fbo);
    int x, y, w, h;
    ctx.GetViewport(x, y, w, h);
    ctx.SetViewport(0, 0, TEX_SIZE, TEX_SIZE);
    bool succ = DrawToRT(dev, ctx, front_node, back_node, eval);
    ctx.SetViewport(x, y, w, h);
    ctx.SetFramebuffer(nullptr);

    if (succ) {
        auto tex = pt2::RenderTargetMgr::Instance()->GetBindedTex(fbo);
        assert(tex);
        DrawFromRT(dev, ctx, front_node, rp, tex);
    }

    pt2::RenderTargetMgr::Instance()->Return(fbo);
}

bool NodePreview::DrawToRT(const ur2::Device& dev, ur2::Context& ctx, const bp::Node& front_node,
                           const rendergraph::Node& back_node, const Evaluator& eval)
{
    auto renderer = rp::RenderMgr::Instance()->GetRenderer(rp::RenderType::SPRITE);
    auto shader = renderer->GetAllShaders()[0];

    std::shared_ptr<pt0::ModelMatUpdater::Snapshot>   model_snap = nullptr;
    std::shared_ptr<pt2::ViewMatUpdater::Snapshot>    view_snap  = nullptr;
    std::shared_ptr<pt2::ProjectMatUpdater::Snapshot> proj_snap  = nullptr;

    auto model_updater = shader->QueryUniformUpdater(ur2::GetUpdaterTypeID<pt0::ModelMatUpdater>());
    if (model_updater)
    {
        auto updater = std::static_pointer_cast<pt0::ModelMatUpdater>(model_updater);
        model_snap = std::make_shared<pt0::ModelMatUpdater::Snapshot>(*updater);
        updater->Update(sm::mat4());
    }
    auto view_updater = shader->QueryUniformUpdater(ur2::GetUpdaterTypeID<pt2::ViewMatUpdater>());
    if (view_updater)
    {
        auto updater = std::static_pointer_cast<pt2::ViewMatUpdater>(view_updater);
        view_snap = std::make_shared<pt2::ViewMatUpdater::Snapshot>(*updater);
        updater->Update(sm::vec2(0, 0), 1.0f);
    }
    auto proj_updater = shader->QueryUniformUpdater(ur2::GetUpdaterTypeID<pt2::ProjectMatUpdater>());
    if (proj_updater)
    {
        auto updater = std::static_pointer_cast<pt2::ProjectMatUpdater>(proj_updater);
        proj_snap = std::make_shared<pt2::ProjectMatUpdater::Snapshot>(*updater);
        updater->Update(TEX_SIZE, TEX_SIZE);
    }

    auto type = front_node.get_type();
    if (type == rttr::type::get<node::Texture>())
    {
        assert(back_node.get_type() == rttr::type::get<rendergraph::node::Texture>());
        auto tex = static_cast<const rendergraph::node::Texture&>(back_node).GetTexture();
        if (tex)
        {
            sm::Matrix2D mat;
            mat.Scale(static_cast<float>(TEX_SIZE), static_cast<float>(TEX_SIZE));
            auto rs = ur2::DefaultRenderState2D();
            pt2::RenderSystem::DrawTexture(dev, ctx, rs, tex->GetWidth(), tex->GetHeight(), tex, sm::rect(1, 1), mat);
        }
    }
    else if (type == rttr::type::get<node::Preview>())
    {
        //auto& conns = front_node.GetAllInput()[0]->GetConnecting();
        //if (conns.empty()) {
        //    return false;
        //}
        //auto f_pin = conns[0]->GetFrom();
        //if (f_pin->GetOldType() != PIN_SAMPLER2D && f_pin->GetOldType() != PIN_SAMPLE_CUBE) {
        //    return false;
        //}

        //rendergraph::RenderContext rg_rc(rc);
        //const int port_idx = f_pin->GetPosIdx();
        //rendergraph::ShaderVariant var;
        //uint32_t flags;
        //back_node.Eval(rg_rc, port_idx, var, flags);
        //if (var.res_id == 0) {
        //    return false;
        //}
        //if (var.type != rendergraph::VariableType::Sampler2D &&
        //    var.type != rendergraph::VariableType::SamplerCube) {
        //    return false;
        //}

        //sm::Matrix2D mat;
        //mat.Scale(static_cast<float>(TEX_SIZE), static_cast<float>(TEX_SIZE));

        //pt2::RenderSystem::DrawTexture(0, 0, var.res_id, sm::rect(1, 1), mat);
    }
    else
    {
        auto rc = std::make_shared<rendergraph::RenderContext>();
        rc->ur_dev = &dev;
        rc->ur_ctx = &ctx;
        eval.Draw(rc, &back_node);
    }

    rp::RenderMgr::Instance()->Flush(dev, ctx);

    if (model_snap) {
        model_snap->Restore();
    }
    if (view_snap) {
        view_snap->Restore();
    }
    if (proj_snap) {
        proj_snap->Restore();
    }

    return true;
}

void NodePreview::DrawFromRT(const ur2::Device& dev, ur2::Context& ctx, const bp::Node& front_node,
                             const n2::RenderParams& rp, const ur2::TexturePtr& tex)
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

    auto rs = ur2::DefaultRenderState2D();
    pt2::RenderSystem::DrawTexQuad(dev, ctx, rs, vertices, texcoords, tex, 0xffffffff);
}

}