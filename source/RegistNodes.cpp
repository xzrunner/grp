#include "grp/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#define REGIST_NODE_RTTI(name, prop)                          \
	rttr::registration::class_<grp::node::name>("grp::"#name) \
		.constructor<>()                                      \
		prop                                                  \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(name) REGIST_NODE_RTTI(name, PROP)

RTTR_REGISTRATION
{

REGIST_NODE_RTTI(Clear,                                            \
.property("type", &grp::node::Clear::type)                         \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))  \
)                                                                  \
.property("color", &grp::node::Clear::color)                       \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Color")) \
)                                                                  \
)

REGIST_NODE_RTTI(Viewport,                                          \
.property("x", &grp::node::Viewport::x)                             \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("X"))      \
)                                                                   \
.property("y", &grp::node::Viewport::y)                             \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Y"))      \
)                                                                   \
.property("w", &grp::node::Viewport::w)                             \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))  \
)                                                                   \
.property("h", &grp::node::Viewport::h)                             \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height")) \
)                                                                   \
)

REGIST_NODE_RTTI(Texture,                                             \
.property("filepath", &grp::node::Texture::filepath)                  \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")) \
)                                                                     \
.property("type", &grp::node::Texture::type)                          \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))     \
)                                                                     \
.property("width", &grp::node::Texture::width)                        \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))    \
)                                                                     \
.property("height", &grp::node::Texture::height)                      \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height"))   \
)                                                                     \
.property("format", &grp::node::Texture::format)                      \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Format"))   \
)                                                                     \
)

REGIST_NODE_RTTI_DEFAULT(RenderTarget)

REGIST_NODE_RTTI(Shader,                                           \
.property("vert", &grp::node::Shader::vert)                        \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vert")), \
    rttr::metadata(ee0::PropLongStringTag(), true)                 \
)                                                                  \
.property("frag", &grp::node::Shader::frag)                        \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Frag")), \
    rttr::metadata(ee0::PropLongStringTag(), true)                 \
)                                                                  \
)

REGIST_NODE_RTTI(Bind,                                               \
.property("channel", &grp::node::Bind::channel)                      \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Channel")) \
)                                                                    \
)

REGIST_NODE_RTTI_DEFAULT(Unbind)

}

namespace grp
{
void nodes_regist_rttr()
{
}
}