#include "renderlab/RegistNodes.h"
#include "renderlab/PinType.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/CustomExpression.h"

#include <ee0/ReflectPropTypes.h>
#include <blueprint/NodeHelper.h>

#include <node2/RenderSystem.h>
#include <painting2/RenderSystem.h>
#include <rendergraph/node/Texture.h>
#include <js/RTTR.h>

#define REGIST_NODE_RTTI(name, prop)                            \
	rttr::registration::class_<rlab::node::name>("rlab::"#name) \
		.constructor<>()                                        \
		prop                                                    \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(name) REGIST_NODE_RTTI(name, PROP)

RTTR_REGISTRATION
{

// renderlab
REGIST_NODE_RTTI_DEFAULT(Preview)

// resource
REGIST_NODE_RTTI(Shader,                                                      \
.property("vert", &rlab::node::Shader::GetVert, &rlab::node::Shader::SetVert) \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vert")),            \
    rttr::metadata(ee0::PropEditCodeTag(), true)                            \
)                                                                             \
.property("frag", &rlab::node::Shader::GetFrag, &rlab::node::Shader::SetFrag) \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Frag")),            \
    rttr::metadata(ee0::PropEditCodeTag(), true)                            \
)                                                                             \
)
REGIST_NODE_RTTI(RenderTarget,                                        \
.property("width", &rlab::node::RenderTarget::width)                  \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))    \
)                                                                     \
.property("height", &rlab::node::RenderTarget::height)                \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height"))   \
)                                                                     \
.property("depth_buf", &rlab::node::RenderTarget::depth_buf)          \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DepthBuf")) \
)                                                                     \
.property("color_buf", &rlab::node::RenderTarget::color_buf)          \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ColorBuf")) \
)                                                                     \
)
REGIST_NODE_RTTI(Texture,                                              \
.property("filepath", &rlab::node::Texture::filepath)                  \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")), \
    rttr::metadata(js::RTTR::FilePathTag(), true),                     \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))   \
)                                                                      \
.property("type", &rlab::node::Texture::type)                          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))      \
)                                                                      \
.property("width", &rlab::node::Texture::width)                        \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))     \
)                                                                      \
.property("height", &rlab::node::Texture::height)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height"))    \
)                                                                      \
.property("format", &rlab::node::Texture::format)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Format"))    \
)                                                                      \
.property("wrap", &rlab::node::Texture::wrap)                          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Wrap"))      \
)                                                                      \
.property("filter", &rlab::node::Texture::filter)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filter"))    \
)                                                                      \
)
REGIST_NODE_RTTI(VertexArray,                                             \
.property("vertices_data", &rlab::node::VertexArray::vertices_data)       \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("VerticesData")) \
)                                                                         \
.property("indices_data", &rlab::node::VertexArray::indices_data)         \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("IndicesData"))  \
)                                                                         \
.property("position", &rlab::node::VertexArray::position)                 \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Position"))     \
)                                                                         \
.property("normal", &rlab::node::VertexArray::normal)                     \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Normal"))       \
)                                                                         \
.property("texture", &rlab::node::VertexArray::texture)                   \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Texture"))      \
)                                                                         \
)
REGIST_NODE_RTTI(PrimitiveShape,                                    \
.property("type", &rlab::node::PrimitiveShape::type)                \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))   \
)                                                                   \
.property("layout", &rlab::node::PrimitiveShape::layout)            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Layout")) \
)                                                                   \
)
REGIST_NODE_RTTI(Model,                                                \
.property("filepath", &rlab::node::Model::filepath)                    \
(                                                                      \
    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")), \
    rttr::metadata(js::RTTR::FilePathTag(), true),                     \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))   \
)                                                                      \
)
REGIST_NODE_RTTI(Heightfield,                                       \
.property("width", &rlab::node::Heightfield::width)                 \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))  \
)                                                                   \
.property("height", &rlab::node::Heightfield::height)               \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height")) \
)                                                                   \
)

// op
REGIST_NODE_RTTI(Clear,                                            \
.property("type", &rlab::node::Clear::type)                        \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))  \
)                                                                  \
.property("color", &rlab::node::Clear::color)                      \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Color")) \
)                                                                  \
)
REGIST_NODE_RTTI(Bind,                                               \
.property("channel", &rlab::node::Bind::channel)                     \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Channel")) \
)                                                                    \
)
REGIST_NODE_RTTI_DEFAULT(Unbind)
REGIST_NODE_RTTI_DEFAULT(Draw)
REGIST_NODE_RTTI(SetUniform,                                          \
.property("var_name", &rlab::node::SetUniform::var_name)              \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Var Name")) \
)                                                                     \
.property("var_type", &rlab::node::SetUniform::var_type)              \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Var Type")) \
)                                                                     \
)

// state
REGIST_NODE_RTTI(Viewport,                                          \
.property("x", &rlab::node::Viewport::x)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("X"))      \
)                                                                   \
.property("y", &rlab::node::Viewport::y)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Y"))      \
)                                                                   \
.property("w", &rlab::node::Viewport::w)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))  \
)                                                                   \
.property("h", &rlab::node::Viewport::h)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height")) \
)                                                                   \
)
REGIST_NODE_RTTI(AlphaTest,                                       \
.property("func", &rlab::node::AlphaTest::func)                   \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Func")) \
)                                                                 \
)
REGIST_NODE_RTTI(BlendEq,                                         \
.property("mode", &rlab::node::BlendEq::mode)                     \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Mode")) \
)                                                                 \
)
REGIST_NODE_RTTI(BlendFunc,                                          \
.property("src_factor", &rlab::node::BlendFunc::sfactor)             \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SFactor")) \
)                                                                    \
.property("dst_factor", &rlab::node::BlendFunc::dfactor)             \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DFactor")) \
)                                                                    \
)
REGIST_NODE_RTTI(Cull,                                            \
.property("type", &rlab::node::Cull::type)                        \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Cull")) \
)                                                                 \
)
REGIST_NODE_RTTI(ZTest,                                           \
.property("func", &rlab::node::ZTest::func)                       \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Func")) \
)                                                                 \
)
REGIST_NODE_RTTI(ZWrite,                                            \
.property("enable", &rlab::node::ZWrite::enable)                    \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Enable")) \
)                                                                   \
)
REGIST_NODE_RTTI(FrontFace,                                            \
.property("clockwise", &rlab::node::FrontFace::clockwise)              \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Clockwise")) \
)                                                                      \
)

// value
REGIST_NODE_RTTI(Bool,                                           \
.property("val", &rlab::node::Bool::val)                         \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Vector1,                                        \
.property("val", &rlab::node::Vector1::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Vector2,                                        \
.property("val", &rlab::node::Vector2::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Vector3,                                        \
.property("val", &rlab::node::Vector3::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Vector4,                                        \
.property("val", &rlab::node::Vector4::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Matrix2,                                        \
.property("val", &rlab::node::Matrix2::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Matrix3,                                        \
.property("val", &rlab::node::Matrix3::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Matrix4,                                        \
.property("val", &rlab::node::Matrix4::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)

// math
REGIST_NODE_RTTI_DEFAULT(Add)
REGIST_NODE_RTTI_DEFAULT(Mul)
REGIST_NODE_RTTI(PerspectiveMat,                                    \
.property("fovy", &rlab::node::PerspectiveMat::fovy)                \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Fovy"))   \
)                                                                   \
.property("aspect", &rlab::node::PerspectiveMat::aspect)            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Aspect")) \
)                                                                   \
.property("znear", &rlab::node::PerspectiveMat::znear)              \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ZNear"))  \
)                                                                   \
.property("zfar", &rlab::node::PerspectiveMat::zfar)                \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ZFar"))   \
)                                                                   \
)
REGIST_NODE_RTTI(OrthoMat,                                          \
.property("left", &rlab::node::OrthoMat::left)                      \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Left"))   \
)                                                                   \
.property("right", &rlab::node::OrthoMat::right)                    \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Right"))  \
)                                                                   \
.property("bottom", &rlab::node::OrthoMat::bottom)                  \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Bottom")) \
)                                                                   \
.property("top", &rlab::node::OrthoMat::top)                        \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Top"))    \
)                                                                   \
.property("znear", &rlab::node::OrthoMat::znear)                    \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ZNear"))  \
)                                                                   \
.property("zfar", &rlab::node::OrthoMat::zfar)                      \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ZFar"))   \
)                                                                   \
)
REGIST_NODE_RTTI(LookAtMat,                                         \
.property("eye", &rlab::node::LookAtMat::eye)                       \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Eye"))    \
)                                                                   \
.property("center", &rlab::node::LookAtMat::center)                 \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Center")) \
)                                                                   \
.property("up", &rlab::node::LookAtMat::up)                         \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Up"))     \
)                                                                   \
)
REGIST_NODE_RTTI(Translate,                                         \
.property("offset", &rlab::node::Translate::offset)                 \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Offset")) \
)                                                                   \
)
REGIST_NODE_RTTI(Rotate,                                            \
.property("angle", &rlab::node::Rotate::angle)                      \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Angle"))  \
)                                                                   \
.property("axis", &rlab::node::Rotate::axis)                        \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Axis"))   \
)                                                                   \
)
REGIST_NODE_RTTI(Scale,                                             \
.property("scale", &rlab::node::Scale::scale)                       \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))  \
)                                                                   \
)

// input
REGIST_NODE_RTTI_DEFAULT(ProjectMat)
REGIST_NODE_RTTI_DEFAULT(ViewMat)
REGIST_NODE_RTTI_DEFAULT(ModelMat)
REGIST_NODE_RTTI_DEFAULT(CameraPosition)
REGIST_NODE_RTTI_DEFAULT(LightPosition)
REGIST_NODE_RTTI(UserScript,                                              \
.property("code", &rlab::node::UserScript::code)                          \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Code")),        \
    rttr::metadata(ee0::PropLongStringTag(), true)                        \
)                                                                         \
.property("ret_type", &rlab::node::UserScript::ret_type)                  \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Return Type"))  \
)
)

// control flow
REGIST_NODE_RTTI_DEFAULT(ForEachLoop)

// script
REGIST_NODE_RTTI(CustomExpression,                                                                \
.property("code", &rlab::node::CustomExpression::GetCode, &rlab::node::CustomExpression::SetCode) \
(                                                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Code")),                                \
    rttr::metadata(ee0::PropLongStringTag(), true)                                                \
)
)

// control
REGIST_NODE_RTTI_DEFAULT(OutputToScreen)
REGIST_NODE_RTTI(PassEnd,                                                             \
.property("pass_end", &rlab::node::PassEnd::GetOrder, &rlab::node::PassEnd::SetOrder) \
(                                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Order"))                    \
)
)

// pipeline
REGIST_NODE_RTTI(SeparableSSS,                                        \
.property("nsamples", &rlab::node::SeparableSSS::nsamples)            \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("nSamples")) \
)                                                                     \
.property("strength", &rlab::node::SeparableSSS::strength)            \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Strength")) \
)                                                                     \
.property("falloff", &rlab::node::SeparableSSS::falloff)              \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Falloff"))  \
)                                                                     \
)
REGIST_NODE_RTTI_DEFAULT(GlobalIllumination)

}

namespace rlab
{

void nodes_regist_rttr()
{
}

}