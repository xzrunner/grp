#include "renderlab/WxGraphPage.h"
#include "renderlab/MessageID.h"
#include "renderlab/RenderAdapter.h"

#include <unirender/Blackboard.h>
#include <rendergraph/DrawList.h>
#include <rendergraph/RenderContext.h>

#include <boost/filesystem.hpp>

namespace renderlab
{

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::GameObj& root,
                         const ee0::SubjectMgrPtr& preview_sub_mgr)
    : bp::WxGraphPage<rendergraph::Variable>(parent, root, preview_sub_mgr, MSG_RENDERER_CHANGED, "rendergraph", "renderlab")
{
    auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
    auto ctx = std::make_shared<rendergraph::RenderContext>(ur_rc);
    m_eval->SetContext(ctx);

    m_eval->SetFront2BackCB([&](const bp::Node& front, dag::Node<rendergraph::Variable>& back) {
        auto dir = boost::filesystem::path(m_filepath).parent_path().string();
        RenderAdapter::Front2Back(front, back, dir);
    });
}

void WxGraphPage::OnEvalChangeed()
{
    std::vector<bp::NodePtr> nodes;
    Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (obj->HasUniqueComp<bp::CompNode>())
        {
            auto bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
            if (bp_node) {
                nodes.push_back(bp_node);
            }
        }
        return true;
    });

    m_front_eval.Rebuild(nodes, *m_eval);
}

}