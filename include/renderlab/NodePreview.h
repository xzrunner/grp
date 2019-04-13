#pragma once

#include "renderlab/RenderGraph.h"

#include <SM_Matrix2D.h>
#include <rendergraph/typedef.h>

#include <vector>

namespace n2 { class RenderParams; }
namespace rg { class DrawList; }

namespace rlab
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

    std::shared_ptr<rg::DrawList> m_draw_list;

}; // NodePreview

}