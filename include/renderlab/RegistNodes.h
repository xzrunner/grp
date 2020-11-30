#pragma once

#include "renderlab/Node.h"
#include "renderlab/ReflectPropTypes.h"

#include <blueprint/Pin.h>

#include <painting0/Color.h>
#include <rendergraph/node/input_nodes.h>
#include <rendergraph/node/math_nodes.h>
#include <rendergraph/node/value_nodes.h>
#include <rendergraph/node/Texture.h>
#include <rendergraph/node/PrimitiveShape.h>
#include <rendergraph/node/AlphaTest.h>
#include <rendergraph/node/BlendEq.h>
#include <rendergraph/node/BlendFunc.h>
#include <rendergraph/node/Cull.h>
#include <rendergraph/node/ZTest.h>
#include <rendergraph/node/Clear.h>
#include <rendergraph/node/Barrier.h>
#include <rendergraph/node/CustomData.h>
#include <rendergraph/node/AttachRT.h>
#include <rendergraph/node/String.h>
#include <rendergraph/node/ShaderGraph.h>
#include <rendergraph/node/Rasterization.h>

namespace renderlab
{

void nodes_regist_rttr();

namespace node
{

#define EXE_FILEPATH "renderlab/node_def_gen.h"
#include "renderlab/node_regist_cfg.h"
#undef EXE_FILEPATH

#define GRP_DEFINE_NODE(type, name, preview, group, prop) \
class type : public Node                                  \
{                                                         \
public:                                                   \
	type()                                                \
		: Node(#name, preview)                            \
	{                                                     \
		InitPins(#name);                                  \
        SetGroup(XSTR(group));                            \
	}                                                     \
                                                          \
	prop                                                  \
	RTTR_ENABLE(Node)                                     \
};

#define GRP_NODE_PROP

GRP_DEFINE_NODE(VertexArray, vertex_array, false, Resource, \
    std::string vertices_data;                              \
    std::string indices_data;                               \
    VertexAttrib position;                                  \
    VertexAttrib normal;                                    \
    VertexAttrib texture;                                   \
)

GRP_DEFINE_NODE(Model, model, false, Resource, \
    std::string filepath;                      \
)

GRP_DEFINE_NODE(String, string, false, Value, \
    std::string val;                          \
    rendergraph::node::String::Type type;     \
)

GRP_DEFINE_NODE(SeparableSSS, separable_sss, false, Pipeline, \
    int      nsamples = 7;                                    \
    sm::vec3 strength = sm::vec3(0.48f, 0.41f, 0.28f);        \
    sm::vec3 falloff  = sm::vec3(1.0f, 0.37f, 0.3f);          \
)
GRP_DEFINE_NODE(GlobalIllumination, gi, false, Pipeline, GRP_NODE_PROP)

}
}