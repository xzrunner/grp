#pragma once

namespace n2 { class RenderParams; }
namespace pt2 { class RenderTarget; }
namespace bp { class Node; }

namespace rlab
{

class Evaluator;

class NodePreview
{
public:
    static void Draw(const Evaluator& eval, const bp::Node& node, const n2::RenderParams& rp);

private:
    static bool DrawToRT(const Evaluator& eval, const bp::Node& node);
    static void DrawFromRT(const bp::Node& node, const n2::RenderParams& rp, const pt2::RenderTarget& rt);

}; // NodePreview

}