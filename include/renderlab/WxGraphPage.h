#pragma once

#include "renderlab/Evaluator.h"
#include "renderlab/EffectBuilder.h"

#include <blueprint/WxGraphPage.h>

#include <rendergraph/Variable.h>

namespace rendergraph { class ScriptEnv; }

namespace renderlab
{

class WxGraphPage : public bp::WxGraphPage<rendergraph::Variable>
{
public:
    WxGraphPage(const ur::Device& dev, wxWindow* parent, const ee0::GameObj& root,
        const ee0::SubjectMgrPtr& preview_sub_mgr);

    auto& GetFrontEval() const { return m_front_eval; }

    void SetFilepath(const std::string& filepath) {
        m_filepath = filepath;
    }

    void SetCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

    auto& GetScriptEnv() const { return m_script; }

    auto& GetEffectBuilder() const { return m_fx_builder; }

protected:
    virtual void OnEvalChangeed() override;

private:
    Evaluator m_front_eval;

    std::string m_filepath;

    std::shared_ptr<rendergraph::ScriptEnv> m_script = nullptr;

    EffectBuilder m_fx_builder;

}; // WxGraphPage

}