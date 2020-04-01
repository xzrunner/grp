#pragma once

namespace n2 { class RenderParams; }
namespace pt2 { class RenderTarget; }
namespace bp { class Node; }
namespace rendergraph { class Node; }

namespace renderlab
{

class Evaluator;

class NodePreview
{
public:
    static void Draw(const bp::Node& front_node, const rendergraph::Node& back_node,
        const n2::RenderParams& rp, const Evaluator& eval);

private:
    static bool DrawToRT(const bp::Node& front_node,
        const rendergraph::Node& back_node, const Evaluator& eval);
    static void DrawFromRT(const bp::Node& front_node, const n2::RenderParams& rp, const pt2::RenderTarget& rt);

}; // NodePreview

}