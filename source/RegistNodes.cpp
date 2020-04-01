#include "renderlab/RegistNodes.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/CustomExpression.h"

#include <ee0/ReflectPropTypes.h>

#include <rendergraph/VariableType.h>
#include <js/RTTR.h>

#define REGIST_NODE_RTTI(type, name, prop)                                \
	rttr::registration::class_<renderlab::node::type>("renderlab::"#name) \
		.constructor<>()                                                  \
		prop                                                              \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(type, name) REGIST_NODE_RTTI(type, name, PROP)

RTTR_REGISTRATION
{

// base

rttr::registration::class_<renderlab::Node>("renderlab::node")
.property("name", &renderlab::Node::GetName, &renderlab::Node::SetName)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))
)
;

#define EXE_FILEPATH "renderlab/node_rttr_gen.h"
#define SKIP_FILE_NODE
#include "renderlab/node_regist_cfg.h"
#undef SKIP_FILE_NODE
#undef EXE_FILEPATH

// resource
REGIST_NODE_RTTI(VertexArray, vertex_array,                                            \
.property("vertices_data", &renderlab::node::VertexArray::vertices_data)  \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("VerticesData")) \
)                                                                         \
.property("indices_data", &renderlab::node::VertexArray::indices_data)    \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("IndicesData"))  \
)                                                                         \
.property("position", &renderlab::node::VertexArray::position)            \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Position"))     \
)                                                                         \
.property("normal", &renderlab::node::VertexArray::normal)                \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Normal"))       \
)                                                                         \
.property("texture", &renderlab::node::VertexArray::texture)              \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Texture"))      \
)                                                                         \
)
REGIST_NODE_RTTI(Model, model,                                         \
.property("filepath", &renderlab::node::Model::filepath)               \
(                                                                      \
    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")), \
    rttr::metadata(js::RTTR::FilePathTag(), true),                     \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))   \
)                                                                      \
)

// renderlab
REGIST_NODE_RTTI_DEFAULT(Preview, preview)
REGIST_NODE_RTTI(Shader, shader,                                                     \
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
REGIST_NODE_RTTI(CustomExpression, custom_expression,                                                         \
.property("code", &renderlab::node::CustomExpression::GetCode, &renderlab::node::CustomExpression::SetCode) \
(                                                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Code")),                                \
    rttr::metadata(ee0::PropLongStringTag(), true)                                                \
)
)

// control
REGIST_NODE_RTTI_DEFAULT(OutputToScreen, output_to_screen)
REGIST_NODE_RTTI(PassEnd, pass_end,                                                             \
.property("pass_end", &renderlab::node::PassEnd::GetOrder, &renderlab::node::PassEnd::SetOrder) \
(                                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Order"))                              \
)
)

// pipeline
REGIST_NODE_RTTI(SeparableSSS, separable_sss,                         \
.property("nsamples", &renderlab::node::SeparableSSS::nsamples)       \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("nSamples")) \
)                                                                     \
.property("strength", &renderlab::node::SeparableSSS::strength)       \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Strength")) \
)                                                                     \
.property("falloff", &renderlab::node::SeparableSSS::falloff)         \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Falloff"))  \
)                                                                     \
)
REGIST_NODE_RTTI_DEFAULT(GlobalIllumination, gi)

}

namespace renderlab
{

void nodes_regist_rttr()
{
}

}