#pragma once

#include "grp/RenderGraph.h"

#include <SM_Matrix2D.h>
#include <rendergraph/typedef.h>

#include <vector>

namespace n2 { class RenderParams; }
namespace rg { class DrawList; }

namespace grp
{

class Node;

class NodePreview
{
public:
    NodePreview(const Node& node);

    void Draw(const n2::RenderParams& rp) const;
    void Update();

private:
    const Node& m_node;

    RenderGraph m_rg;

    rg::NodePtr m_rg_node = nullptr;
    std::shared_ptr<rg::DrawList> m_draw_list;

}; // NodePreview

}