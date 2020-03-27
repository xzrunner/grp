#pragma once

#include <blueprint/typedef.h>

#include <rendergraph/typedef.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

namespace rendergraph { class DrawList; class RenderContext; }

namespace renderlab
{

class Evaluator : boost::noncopyable
{
public:
    Evaluator();
    virtual ~Evaluator();

    void Rebuild(const std::vector<bp::NodePtr>& nodes);

    void Draw(const std::shared_ptr<rendergraph::RenderContext>& rc,
        const bp::NodePtr& end = nullptr) const;

    bool IsEmpty() const { return m_passes.empty(); }

    rendergraph::NodePtr QueryRGNode(const bp::Node* bp_node) const;
    void AddNodeMap(const bp::Node* bp_node, const rendergraph::NodePtr& rg_node);

private:
    void Clear();

private:
    std::vector<std::unique_ptr<rendergraph::DrawList>> m_passes;

    std::unordered_map<const bp::Node*, rendergraph::NodePtr> m_nodes_map;

}; // Evaluator

}