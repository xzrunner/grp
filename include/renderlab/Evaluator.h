#pragma once

#include <boost/noncopyable.hpp>

#include <vector>
#include <memory>

namespace rg { class DrawList; class RenderContext; }

namespace rlab
{

class Node;

class Evaluator : boost::noncopyable
{
public:
    Evaluator() {}

    void Rebuild(const std::vector<std::shared_ptr<Node>>& nodes);

    void Draw(const rg::RenderContext& rc, const Node* end = nullptr) const;

    bool IsEmpty() const { return m_passes.empty(); }

private:
    void Clear();

private:
    std::vector<std::shared_ptr<rg::DrawList>> m_passes;

}; // Evaluator

}