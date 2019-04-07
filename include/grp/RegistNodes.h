#pragma once

#include "grp/Node.h"
#include "grp/ReflectPropTypes.h"

#include <painting0/Color.h>

namespace grp
{

void nodes_regist_rttr();

namespace node
{

#define GRP_DEFINE_NODE(name, preview, prop) \
class name : public Node                     \
{                                            \
public:                                      \
	name()                                   \
		: Node(#name, preview)               \
	{                                        \
		InitPins(#name);                     \
	}                                        \
                                             \
	prop                                     \
	RTTR_ENABLE(Node)                        \
};

#define GRP_NODE_PROP

GRP_DEFINE_NODE(Clear, true, \
    ClearType  type;         \
    pt0::Color color;        \
)
GRP_DEFINE_NODE(Viewport, true, \
    int x = 0;                  \
    int y = 0;                  \
    int w = 0;                  \
    int h = 0;                  \
)
GRP_DEFINE_NODE(Texture, true,                                                                 \
    virtual void PreviewDraw(const rg::NodePtr& node, const sm::Matrix2D& mat) const override; \
    std::string filepath;                                                                      \
    grp::TextureType type = grp::TextureType::Tex2D;                                           \
    int width = 0;                                                                             \
    int height = 0;                                                                            \
    grp::TextureFormat format = grp::TextureFormat::RGBA8;                                     \
)
GRP_DEFINE_NODE(RenderTarget, true, GRP_NODE_PROP)
GRP_DEFINE_NODE(Shader, true, \
    std::string vert;         \
    std::string frag;         \
)
GRP_DEFINE_NODE(Bind, false, \
    int channel = 0;         \
)
GRP_DEFINE_NODE(Unbind, false, GRP_NODE_PROP)

}

}