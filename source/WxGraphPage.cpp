#include "renderlab/WxGraphPage.h"
#include "renderlab/MessageID.h"
#include "renderlab/RenderAdapter.h"

#include <ee0/WxStageCanvas.h>

#include <rendergraph/DrawList.h>
#include <rendergraph/RenderContext.h>
#include <rendergraph/ScriptEnv.h>

#include <boost/filesystem.hpp>

namespace renderlab
{

WxGraphPage::WxGraphPage(const ur::Device& dev, wxWindow* parent, const ee0::GameObj& root,
                         const ee0::SubjectMgrPtr& preview_sub_mgr)
    : bp::WxGraphPage<rendergraph::Variable>(parent, root, preview_sub_mgr, MSG_RENDERER_CHANGED, "rendergraph", "renderlab",
        [&](const bp::Node& front, dag::Node<rendergraph::Variable>& back) {
        auto dir = boost::filesystem::path(m_filepath).parent_path().string();
        RenderAdapter::Front2Back(dev, front, back, dir);
    })
    , m_fx_builder(m_sub_mgr)
{
    m_script = std::make_shared<rendergraph::ScriptEnv>();
}

void WxGraphPage::SetCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas)
{
    GetImpl().SetCanvas(canvas);

    auto ctx = std::make_shared<rendergraph::RenderContext>(m_script);
    m_ctx = ctx;

    ctx->ur_dev = &canvas->GetRenderDevice();
    ctx->ur_ctx = canvas->GetRenderContext().ur_ctx.get();

    GetSceneTree()->GetCurrEval()->SetContext(ctx);

    m_script->SetRenderContext(ctx);
}

void WxGraphPage::OnEvalChangeed()
{
    auto root = GetSceneTree()->GetRootNode();

    std::vector<bp::NodePtr> nodes;

    assert(root->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
    for (auto& child : ccomplex.GetAllChildren())
    {
        if (!child->HasUniqueComp<bp::CompNode>()) {
            continue;
        }
        auto bp_node = child->GetUniqueComp<bp::CompNode>().GetNode();
        if (bp_node) {
            nodes.push_back(bp_node);
        }
    }

    m_front_eval.Rebuild(nodes, *GetSceneTree()->GetRootGraph());
}

}