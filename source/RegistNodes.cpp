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
	rttr::registration::class_<renderlab::node::name>("renderlab::"#name) \
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
.property("vert", &renderlab::node::Shader::GetVert, &renderlab::node::Shader::SetVert) \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vert")),            \
    rttr::metadata(ee0::PropEditCodeTag(), true)                            \
)                                                                             \
.property("frag", &renderlab::node::Shader::GetFrag, &renderlab::node::Shader::SetFrag) \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Frag")),            \
    rttr::metadata(ee0::PropEditCodeTag(), true)                            \
)                                                                             \
)
REGIST_NODE_RTTI(RenderTarget,                                        \
.property("width", &renderlab::node::RenderTarget::width)                  \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))    \
)                                                                     \
.property("height", &renderlab::node::RenderTarget::height)                \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height"))   \
)                                                                     \
.property("depth_buf", &renderlab::node::RenderTarget::depth_buf)          \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DepthBuf")) \
)                                                                     \
.property("color_buf", &renderlab::node::RenderTarget::color_buf)          \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ColorBuf")) \
)                                                                     \
)
REGIST_NODE_RTTI(Texture,                                              \
.property("filepath", &renderlab::node::Texture::filepath)                  \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")), \
    rttr::metadata(js::RTTR::FilePathTag(), true),                     \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))   \
)                                                                      \
.property("type", &renderlab::node::Texture::type)                          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))      \
)                                                                      \
.property("width", &renderlab::node::Texture::width)                        \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))     \
)                                                                      \
.property("height", &renderlab::node::Texture::height)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height"))    \
)                                                                      \
.property("format", &renderlab::node::Texture::format)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Format"))    \
)                                                                      \
.property("wrap", &renderlab::node::Texture::wrap)                          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Wrap"))      \
)                                                                      \
.property("filter", &renderlab::node::Texture::filter)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filter"))    \
)                                                                      \
)
REGIST_NODE_RTTI(VertexArray,                                             \
.property("vertices_data", &renderlab::node::VertexArray::vertices_data)       \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("VerticesData")) \
)                                                                         \
.property("indices_data", &renderlab::node::VertexArray::indices_data)         \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("IndicesData"))  \
)                                                                         \
.property("position", &renderlab::node::VertexArray::position)                 \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Position"))     \
)                                                                         \
.property("normal", &renderlab::node::VertexArray::normal)                     \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Normal"))       \
)                                                                         \
.property("texture", &renderlab::node::VertexArray::texture)                   \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Texture"))      \
)                                                                         \
)
REGIST_NODE_RTTI(PrimitiveShape,                                    \
.property("type", &renderlab::node::PrimitiveShape::type)                \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))   \
)                                                                   \
.property("layout", &renderlab::node::PrimitiveShape::layout)            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Layout")) \
)                                                                   \
)
REGIST_NODE_RTTI(Model,                                                \
.property("filepath", &renderlab::node::Model::filepath)                    \
(                                                                      \
    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")), \
    rttr::metadata(js::RTTR::FilePathTag(), true),                     \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))   \
)                                                                      \
)
REGIST_NODE_RTTI(Heightfield,                                       \
.property("width", &renderlab::node::Heightfield::width)                 \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))  \
)                                                                   \
.property("height", &renderlab::node::Heightfield::height)               \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height")) \
)                                                                   \
)

// op
REGIST_NODE_RTTI(Clear,                                            \
.property("type", &renderlab::node::Clear::type)                        \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))  \
)                                                                  \
.property("color", &renderlab::node::Clear::color)                      \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Color")) \
)                                                                  \
)
REGIST_NODE_RTTI(Bind,                                               \
.property("channel", &renderlab::node::Bind::channel)                     \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Channel")) \
)                                                                    \
)
REGIST_NODE_RTTI_DEFAULT(Unbind)
REGIST_NODE_RTTI_DEFAULT(Draw)
REGIST_NODE_RTTI(SetUniform,                                          \
.property("var_name", &renderlab::node::SetUniform::var_name)              \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Var Name")) \
)                                                                     \
.property("var_type", &renderlab::node::SetUniform::var_type)              \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Var Type")) \
)                                                                     \
)

// state
REGIST_NODE_RTTI(Viewport,                                          \
.property("x", &renderlab::node::Viewport::x)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("X"))      \
)                                                                   \
.property("y", &renderlab::node::Viewport::y)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Y"))      \
)                                                                   \
.property("w", &renderlab::node::Viewport::w)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))  \
)                                                                   \
.property("h", &renderlab::node::Viewport::h)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height")) \
)                                                                   \
)
REGIST_NODE_RTTI(AlphaTest,                                       \
.property("func", &renderlab::node::AlphaTest::func)                   \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Func")) \
)                                                                 \
)
REGIST_NODE_RTTI(BlendEq,                                         \
.property("mode", &renderlab::node::BlendEq::mode)                     \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Mode")) \
)                                                                 \
)
REGIST_NODE_RTTI(BlendFunc,                                          \
.property("src_factor", &renderlab::node::BlendFunc::sfactor)             \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SFactor")) \
)                                                                    \
.property("dst_factor", &renderlab::node::BlendFunc::dfactor)             \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DFactor")) \
)                                                                    \
)
REGIST_NODE_RTTI(Cull,                                            \
.property("type", &renderlab::node::Cull::type)                        \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Cull")) \
)                                                                 \
)
REGIST_NODE_RTTI(ZTest,                                           \
.property("func", &renderlab::node::ZTest::func)                       \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Func")) \
)                                                                 \
)
REGIST_NODE_RTTI(ZWrite,                                            \
.property("enable", &renderlab::node::ZWrite::enable)                    \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Enable")) \
)                                                                   \
)
REGIST_NODE_RTTI(FrontFace,                                            \
.property("clockwise", &renderlab::node::FrontFace::clockwise)              \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Clockwise")) \
)                                                                      \
)

// value
REGIST_NODE_RTTI(Bool,                                           \
.property("val", &renderlab::node::Bool::val)                         \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Vector1,                                        \
.property("val", &renderlab::node::Vector1::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Vector2,                                        \
.property("val", &renderlab::node::Vector2::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Vector3,                                        \
.property("val", &renderlab::node::Vector3::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Vector4,                                        \
.property("val", &renderlab::node::Vector4::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Matrix2,                                        \
.property("val", &renderlab::node::Matrix2::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Matrix3,                                        \
.property("val", &renderlab::node::Matrix3::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)
REGIST_NODE_RTTI(Matrix4,                                        \
.property("val", &renderlab::node::Matrix4::val)                      \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Val")) \
)                                                                \
)

// math
REGIST_NODE_RTTI_DEFAULT(Add)
REGIST_NODE_RTTI_DEFAULT(Mul)
REGIST_NODE_RTTI(PerspectiveMat,                                    \
.property("fovy", &renderlab::node::PerspectiveMat::fovy)                \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Fovy"))   \
)                                                                   \
.property("aspect", &renderlab::node::PerspectiveMat::aspect)            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Aspect")) \
)                                                                   \
.property("znear", &renderlab::node::PerspectiveMat::znear)              \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ZNear"))  \
)                                                                   \
.property("zfar", &renderlab::node::PerspectiveMat::zfar)                \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ZFar"))   \
)                                                                   \
)
REGIST_NODE_RTTI(OrthoMat,                                          \
.property("left", &renderlab::node::OrthoMat::left)                      \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Left"))   \
)                                                                   \
.property("right", &renderlab::node::OrthoMat::right)                    \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Right"))  \
)                                                                   \
.property("bottom", &renderlab::node::OrthoMat::bottom)                  \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Bottom")) \
)                                                                   \
.property("top", &renderlab::node::OrthoMat::top)                        \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Top"))    \
)                                                                   \
.property("znear", &renderlab::node::OrthoMat::znear)                    \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ZNear"))  \
)                                                                   \
.property("zfar", &renderlab::node::OrthoMat::zfar)                      \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ZFar"))   \
)                                                                   \
)
REGIST_NODE_RTTI(LookAtMat,                                         \
.property("eye", &renderlab::node::LookAtMat::eye)                       \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Eye"))    \
)                                                                   \
.property("center", &renderlab::node::LookAtMat::center)                 \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Center")) \
)                                                                   \
.property("up", &renderlab::node::LookAtMat::up)                         \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Up"))     \
)                                                                   \
)
REGIST_NODE_RTTI(Translate,                                         \
.property("offset", &renderlab::node::Translate::offset)                 \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Offset")) \
)                                                                   \
)
REGIST_NODE_RTTI(Rotate,                                            \
.property("angle", &renderlab::node::Rotate::angle)                      \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Angle"))  \
)                                                                   \
.property("axis", &renderlab::node::Rotate::axis)                        \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Axis"))   \
)                                                                   \
)
REGIST_NODE_RTTI(Scale,                                             \
.property("scale", &renderlab::node::Scale::scale)                       \
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
.property("code", &renderlab::node::UserScript::code)                          \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Code")),        \
    rttr::metadata(ee0::PropLongStringTag(), true)                        \
)                                                                         \
.property("ret_type", &renderlab::node::UserScript::ret_type)                  \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Return Type"))  \
)
)

// control flow
REGIST_NODE_RTTI_DEFAULT(ForEachLoop)

// script
REGIST_NODE_RTTI(CustomExpression,                                                                \
.property("code", &renderlab::node::CustomExpression::GetCode, &renderlab::node::CustomExpression::SetCode) \
(                                                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Code")),                                \
    rttr::metadata(ee0::PropLongStringTag(), true)                                                \
)
)

// control
REGIST_NODE_RTTI_DEFAULT(OutputToScreen)
REGIST_NODE_RTTI(PassEnd,                                                             \
.property("pass_end", &renderlab::node::PassEnd::GetOrder, &renderlab::node::PassEnd::SetOrder) \
(                                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Order"))                    \
)
)

// pipeline
REGIST_NODE_RTTI(SeparableSSS,                                        \
.property("nsamples", &renderlab::node::SeparableSSS::nsamples)            \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("nSamples")) \
)                                                                     \
.property("strength", &renderlab::node::SeparableSSS::strength)            \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Strength")) \
)                                                                     \
.property("falloff", &renderlab::node::SeparableSSS::falloff)              \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Falloff"))  \
)                                                                     \
)
REGIST_NODE_RTTI_DEFAULT(GlobalIllumination)

}

namespace renderlab
{

void nodes_regist_rttr()
{
}

}