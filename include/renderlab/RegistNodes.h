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
#include <rendergraph/node/CustomData.h>

namespace renderlab
{

void nodes_regist_rttr();

namespace node
{

#define EXE_FILEPATH "renderlab/node_def_gen.h"
#include "renderlab/node_regist_cfg.h"
#undef EXE_FILEPATH

class Preview : public Node
{
public:
    Preview()
        : Node("Preview", true)
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, bp::PIN_ANY_VAR, "in", *this
        ));

        Layout();

        SetGroup("Utility");
    }
    RTTR_ENABLE(Node)
}; // Preview

class OutputToScreen : public Node
{
public:
    OutputToScreen()
        : Node("OutputToScreen", false)
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, bp::PIN_ANY_VAR, "prev", *this
        ));
        m_all_output.push_back(std::make_shared<bp::Pin>(
            false, 0, bp::PIN_ANY_VAR, "next", *this
        ));

        Layout();

        SetGroup("Utility");
    }

    RTTR_ENABLE(Node)

}; // OutputToScreen

class PassEnd : public Node
{
public:
    PassEnd()
        : Node("PassEnd", false)
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, bp::PIN_PORT, "prev", *this
        ));

        Layout();

        UpdateTitle();

        SetGroup("ControlFlow");
    }

    int  GetOrder() const { return m_order; }
    void SetOrder(int order) {
        m_order = order;
        UpdateTitle();
    }

private:
    void UpdateTitle() {
        m_title = "PassEnd" + std::to_string(m_order);
    }

private:
    int m_order = 0;

    RTTR_ENABLE(Node)

}; // PassEnd

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

GRP_DEFINE_NODE(SeparableSSS, separable_sss, false, Pipeline, \
    int      nsamples = 7;                                    \
    sm::vec3 strength = sm::vec3(0.48f, 0.41f, 0.28f);        \
    sm::vec3 falloff  = sm::vec3(1.0f, 0.37f, 0.3f);          \
)
GRP_DEFINE_NODE(GlobalIllumination, gi, false, Pipeline, GRP_NODE_PROP)

}
}