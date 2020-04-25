#pragma once

#include <unirender2/typedef.h>

namespace n2 { class RenderParams; }
namespace ur2 { class Device; class Context; }
namespace bp { class Node; }
namespace rendergraph { class Node; }

namespace renderlab
{

class Evaluator;

class NodePreview
{
public:
    static void Draw(const ur2::Device& dev, ur2::Context& ctx,
        const bp::Node& front_node, const rendergraph::Node& back_node,
        const n2::RenderParams& rp, const Evaluator& eval);

private:
    static bool DrawToRT(const ur2::Device& dev, ur2::Context& ctx, const bp::Node& front_node,
        const rendergraph::Node& back_node, const Evaluator& eval);
    static void DrawFromRT(const ur2::Device& dev, ur2::Context& ctx,
        const bp::Node& front_node, const n2::RenderParams& rp, const ur2::TexturePtr& tex);

}; // NodePreview

}