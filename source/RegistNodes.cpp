#include "renderlab/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>
#include <blueprint/NodeHelper.h>

#include <node2/RenderSystem.h>
#include <painting2/RenderSystem.h>
#include <rendergraph/node/Texture.h>
#include <js/RTTR.h>

#define REGIST_NODE_RTTI(name, prop)                          \
	rttr::registration::class_<rlab::node::name>("rlab::"#name) \
		.constructor<>()                                      \
		prop                                                  \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(name) REGIST_NODE_RTTI(name, PROP)

RTTR_REGISTRATION
{

REGIST_NODE_RTTI(Clear,                                            \
.property("type", &rlab::node::Clear::type)                         \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))  \
)                                                                  \
.property("color", &rlab::node::Clear::color)                       \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Color")) \
)                                                                  \
)

REGIST_NODE_RTTI(Viewport,                                          \
.property("x", &rlab::node::Viewport::x)                             \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("X"))      \
)                                                                   \
.property("y", &rlab::node::Viewport::y)                             \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Y"))      \
)                                                                   \
.property("w", &rlab::node::Viewport::w)                             \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))  \
)                                                                   \
.property("h", &rlab::node::Viewport::h)                             \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height")) \
)                                                                   \
)

REGIST_NODE_RTTI(Texture,                                              \
.property("filepath", &rlab::node::Texture::filepath)                   \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")), \
    rttr::metadata(js::RTTR::FilePathTag(), true),                     \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.png")) \
)                                                                      \
.property("type", &rlab::node::Texture::type)                           \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))      \
)                                                                      \
.property("width", &rlab::node::Texture::width)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))     \
)                                                                      \
.property("height", &rlab::node::Texture::height)                       \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height"))    \
)                                                                      \
.property("format", &rlab::node::Texture::format)                       \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Format"))    \
)                                                                      \
)

REGIST_NODE_RTTI_DEFAULT(RenderTarget)

REGIST_NODE_RTTI(Shader,                                           \
.property("vert", &rlab::node::Shader::vert)                        \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vert")), \
    rttr::metadata(ee0::PropLongStringTag(), true)                 \
)                                                                  \
.property("frag", &rlab::node::Shader::frag)                        \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Frag")), \
    rttr::metadata(ee0::PropLongStringTag(), true)                 \
)                                                                  \
)

REGIST_NODE_RTTI(Bind,                                               \
.property("channel", &rlab::node::Bind::channel)                      \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Channel")) \
)                                                                    \
)

REGIST_NODE_RTTI_DEFAULT(Unbind)

}

namespace rlab
{

void nodes_regist_rttr()
{
}

namespace node
{

void Texture::PreviewDraw(const rg::NodePtr& node, const sm::Matrix2D& mat) const
{
    if (!node) {
        return;
    }

    assert(node->get_type() == rttr::type::get<rg::node::Texture>());
    auto tex = std::static_pointer_cast<rg::node::Texture>(node);
    pt2::RenderSystem::DrawTexture(width, height, tex->GetTexID(), sm::rect(1, 1), mat);
}

}
}