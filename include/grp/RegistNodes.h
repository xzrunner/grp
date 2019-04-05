#pragma once

#include "grp/Node.h"
#include "grp/ReflectPropTypes.h"

#include <painting0/Color.h>

namespace grp
{

void nodes_regist_rttr();

namespace node
{

#define GRP_DEFINE_NODE(name, prop) \
class name : public Node            \
{                                   \
public:                             \
	name()                          \
		: Node(#name)               \
	{                               \
		InitPins(#name);            \
	}                               \
                                    \
	prop                            \
	RTTR_ENABLE(Node)               \
};

#define GRP_NODE_PROP
#define GRP_DEFINE_NODE_DEFAULT(name) GRP_DEFINE_NODE(name, GRP_NODE_PROP)

GRP_DEFINE_NODE(Clear, \
    ClearType  type;   \
    pt0::Color color;  \
)
GRP_DEFINE_NODE(Viewport, \
    int x = 0;            \
    int y = 0;            \
    int w = 0;            \
    int h = 0;            \
)
GRP_DEFINE_NODE(Texture,                                   \
    std::string filepath;                                  \
    grp::TextureType type = grp::TextureType::Tex2D;       \
    int width = 0;                                         \
    int height = 0;                                        \
    grp::TextureFormat format = grp::TextureFormat::RGBA8; \
)
GRP_DEFINE_NODE_DEFAULT(RenderTarget)
GRP_DEFINE_NODE(Shader, \
    std::string vert;   \
    std::string frag;   \
)
GRP_DEFINE_NODE(Bind, \
    int channel = 0;  \
)
GRP_DEFINE_NODE_DEFAULT(Unbind)

}

}