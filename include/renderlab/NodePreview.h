#pragma once

#include <unirender/typedef.h>

namespace n2 { class RenderParams; }
namespace ur { class Device; class Context; }
namespace bp { class Node; }
namespace rendergraph { class Node; }

namespace renderlab
{

class Evaluator;

class NodePreview
{
public:
    static void Draw(const ur::Device& dev, ur::Context& ctx,
        const bp::Node& front_node, const rendergraph::Node& back_node,
        const n2::RenderParams& rp, const Evaluator& eval);

private:
    static bool DrawToRT(const ur::Device& dev, ur::Context& ctx, const bp::Node& front_node,
        const rendergraph::Node& back_node, const Evaluator& eval);
    static void DrawFromRT(const ur::Device& dev, ur::Context& ctx,
        const bp::Node& front_node, const n2::RenderParams& rp, const ur::TexturePtr& tex);

    static void Clear(ur::Context& ctx);

}; // NodePreview

}