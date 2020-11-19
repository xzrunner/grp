#pragma once

#include <blueprint/typedef.h>
#include <blueprint/BackendGraph.h>

#include <unirender/typedef.h>
#include <rendergraph/typedef.h>
#include <rendergraph/Variable.h>
#include <rendergraph/DrawList.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

namespace rendergraph { class RenderContext; }

namespace renderlab
{

class Evaluator : boost::noncopyable
{
public:
    Evaluator();

    void Rebuild(const std::vector<bp::NodePtr>& nodes,
        const bp::BackendGraph<rendergraph::Variable>& eval);

    void Draw(const std::shared_ptr<rendergraph::RenderContext>& rc,
        const rendergraph::Node* end = nullptr) const;

    bool IsEmpty() const { return m_passes.empty(); }

    void SetInputVar(const std::string& name, const ur::TexturePtr& tex);

private:
    void Clear();

private:
    std::vector<std::unique_ptr<rendergraph::DrawList>> m_passes;

    std::vector<std::pair<rendergraph::VariableType, std::string>> m_input_vars;

}; // Evaluator

}