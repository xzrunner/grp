#pragma once

#include "renderlab/Node.h"
#include "renderlab/ReflectPropTypes.h"

#include <blueprint/Pin.h>

#include <painting0/Color.h>

namespace renderlab
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

// renderlab
class Preview : public Node
{
public:
    Preview() : Node("Preview", true) {
        InitPins({
			{ bp::PIN_ANY_VAR, "in" }
		},{
		});
    }
    RTTR_ENABLE(Node)
}; // Preview

// resource
GRP_DEFINE_NODE(RenderTarget, false, \
    size_t width = 0;                \
    size_t height = 0;               \
    bool depth_buf = false;          \
    bool color_buf = false;          \
)
GRP_DEFINE_NODE(Texture, true,                                      \
    std::string filepath;                                           \
    renderlab::TextureType type = renderlab::TextureType::Tex2D;              \
    size_t width = 0;                                               \
    size_t height = 0;                                              \
    renderlab::TextureFormat    format = renderlab::TextureFormat::RGBA8;     \
    renderlab::TextureWrapping  wrap   = renderlab::TextureWrapping::Repeat;  \
    renderlab::TextureFiltering filter = renderlab::TextureFiltering::Linear; \
)
GRP_DEFINE_NODE(VertexArray, false, \
    std::string vertices_data;      \
    std::string indices_data;       \
    VertexAttrib position;          \
    VertexAttrib normal;            \
    VertexAttrib texture;           \
)
GRP_DEFINE_NODE(PrimitiveShape, false,                     \
    PrimitiveShapeType  type   = PrimitiveShapeType::Quad; \
    PrimitiveVertLayout layout = PrimitiveVertLayout::Pos; \
)
GRP_DEFINE_NODE(Model, false, \
    std::string filepath;     \
)
GRP_DEFINE_NODE(Heightfield, false, \
    size_t width  = 128;            \
    size_t height = 128;            \
)
// op
GRP_DEFINE_NODE(Clear, true, \
    ClearType  type;         \
    pt0::Color color;        \
)
GRP_DEFINE_NODE(Bind, false, \
    int channel = 0;         \
)
GRP_DEFINE_NODE(Unbind, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(Draw, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(SetUniform, false,                           \
    std::string var_name;                                    \
    ShaderUniformType var_type = ShaderUniformType::Unknown; \
)

// state
GRP_DEFINE_NODE(Viewport, false, \
    int x = 0;                   \
    int y = 0;                   \
    int w = 0;                   \
    int h = 0;                   \
)
GRP_DEFINE_NODE(AlphaTest, false,               \
    AlphaTestFunc func = AlphaTestFunc::Always; \
)
GRP_DEFINE_NODE(BlendEq, false,              \
    BlendEqMode mode = BlendEqMode::FuncAdd; \
)
GRP_DEFINE_NODE(BlendFunc, false,                    \
    BlendFuncFactor sfactor = BlendFuncFactor::One;  \
    BlendFuncFactor dfactor = BlendFuncFactor::Zero; \
)
GRP_DEFINE_NODE(Cull, false,        \
    CullMode type = CullMode::Back; \
)
GRP_DEFINE_NODE(ZTest, false,         \
    ZTestFunc func = ZTestFunc::Less; \
)
GRP_DEFINE_NODE(ZWrite, false, \
    bool enable = false;       \
)
GRP_DEFINE_NODE(FrontFace, false, \
    bool clockwise = false;       \
)

// value
GRP_DEFINE_NODE(Bool, false,    \
    bool val = false;           \
)
GRP_DEFINE_NODE(Vector1, false, \
    float val = 0;              \
)
GRP_DEFINE_NODE(Vector2, false, \
    sm::vec2 val;               \
)
GRP_DEFINE_NODE(Vector3, false, \
    sm::vec3 val;               \
)
GRP_DEFINE_NODE(Vector4, false, \
    sm::vec4 val;               \
)
GRP_DEFINE_NODE(Matrix2, false, \
    sm::mat2 val;               \
)
GRP_DEFINE_NODE(Matrix3, false, \
    sm::mat3 val;               \
)
GRP_DEFINE_NODE(Matrix4, false, \
    sm::mat4 val;               \
)

// math
GRP_DEFINE_NODE(Add, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(Mul, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(PerspectiveMat, false,    \
    float fovy   = 45.0f * SM_DEG_TO_RAD; \
    float aspect = 1.0f;                  \
    float znear   = 0.1f;                 \
    float zfar    = 100.0f;               \
)
GRP_DEFINE_NODE(OrthoMat, false, \
    float left   = -10.0f;       \
    float right  = 10.0f;        \
    float bottom = -10.0f;       \
    float top    = 10.0f;        \
    float znear  = 1.0f;         \
    float zfar   = 7.5f;         \
)
GRP_DEFINE_NODE(LookAtMat, false,    \
    sm::vec3 eye;                    \
    sm::vec3 center;                 \
    sm::vec3 up = sm::vec3(0, 1, 0); \
)
GRP_DEFINE_NODE(Translate, false,    \
    sm::vec3 offset;                 \
)
GRP_DEFINE_NODE(Rotate, false,       \
    float angle = 0;                 \
    sm::vec3 axis;                   \
)
GRP_DEFINE_NODE(Scale, false,        \
    sm::vec3 scale;                  \
)

// input
GRP_DEFINE_NODE(ProjectMat,     false, GRP_NODE_PROP)
GRP_DEFINE_NODE(ViewMat,        false, GRP_NODE_PROP)
GRP_DEFINE_NODE(ModelMat,       false, GRP_NODE_PROP)
GRP_DEFINE_NODE(CameraPosition, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(LightPosition,  false, GRP_NODE_PROP)
GRP_DEFINE_NODE(UserScript,     false,                    \
    std::string code;                                     \
    UserScriptRetType ret_type = UserScriptRetType::Void; \
)

// control flow
GRP_DEFINE_NODE(For, false, \
    int index_begin = 0;    \
    int index_end   = 0;    \
    int index_step  = 0;    \
)
GRP_DEFINE_NODE(ForEachLoop, false, GRP_NODE_PROP)

// control
class OutputToScreen : public Node
{
public:
    OutputToScreen()
        : Node("OutputToScreen", false)
    {
        InitPins({
			{ bp::PIN_ANY_VAR, "prev" }
		},{
            { bp::PIN_ANY_VAR, "next" }
		});
    }

    RTTR_ENABLE(Node)

}; // OutputToScreen

class PassEnd : public Node
{
public:
    PassEnd()
        : Node("PassEnd", false)
    {
        InitPins({
			{ bp::PIN_PORT, "prev" }
		},{
		});

        UpdateTitle();
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

// pipeline
GRP_DEFINE_NODE(SeparableSSS, false,                   \
    int      nsamples = 7;                             \
    sm::vec3 strength = sm::vec3(0.48f, 0.41f, 0.28f); \
    sm::vec3 falloff  = sm::vec3(1.0f, 0.37f, 0.3f);   \
)
GRP_DEFINE_NODE(GlobalIllumination, false, GRP_NODE_PROP)

}
}