#pragma once

#include "renderlab/Evaluator.h"

#include <blueprint/WxGraphPage.h>

#include <rendergraph/Variable.h>

namespace renderlab
{

class WxGraphPage : public bp::WxGraphPage<rendergraph::Variable>
{
public:
    WxGraphPage(const ur2::Device& dev, wxWindow* parent, const ee0::GameObj& root,
        const ee0::SubjectMgrPtr& preview_sub_mgr);

    auto& GetFrontEval() const { return m_front_eval; }

    void SetFilepath(const std::string& filepath) {
        m_filepath = filepath;
    }

    void SetCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

protected:
    virtual void OnEvalChangeed() override;

private:
    Evaluator m_front_eval;

    std::string m_filepath;

}; // WxGraphPage

}