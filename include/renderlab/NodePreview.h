#pragma once

namespace n2 { class RenderParams; }

namespace rlab
{

class Evaluator;
class Node;

class NodePreview
{
public:
    static void Draw(const Evaluator& eval, const Node& node, const n2::RenderParams& rp);

}; // NodePreview

}