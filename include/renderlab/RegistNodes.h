#pragma once

#include "renderlab/Node.h"
#include "renderlab/ReflectPropTypes.h"

#include <blueprint/Pin.h>

#include <painting0/Color.h>

namespace rlab
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

// resource
GRP_DEFINE_NODE(Shader, false,                            \
    auto& GetVert() const { return m_vert; }              \
    void  SetVert(const std::string& vert);               \
    auto& GetFrag() const { return m_frag; }              \
    void  SetFrag(const std::string& frag);               \
private:                                                  \
    void InitInputsFromUniforms();                        \
    static void GetCodeUniforms(const std::string& code,  \
        std::vector<Node::PinDesc>& uniforms);            \
private:                                                  \
    std::string m_vert;                                   \
    std::string m_frag;                                   \
    std::vector<Node::PinDesc> m_vert_uniforms;           \
    std::vector<Node::PinDesc> m_frag_uniforms;           \
)
GRP_DEFINE_NODE(RenderTarget, false, \
    size_t width = 0;                \
    size_t height = 0;               \
    bool depth_buf = false;          \
    bool color_buf = false;          \
)
GRP_DEFINE_NODE(Texture, true,                                      \
    std::string filepath;                                           \
    rlab::TextureType type = rlab::TextureType::Tex2D;              \
    size_t width = 0;                                               \
    size_t height = 0;                                              \
    rlab::TextureFormat    format = rlab::TextureFormat::RGBA8;     \
    rlab::TextureWrapping  wrap   = rlab::TextureWrapping::Repeat;  \
    rlab::TextureFiltering filter = rlab::TextureFiltering::Linear; \
)
GRP_DEFINE_NODE(VertexArray, false, \
    std::string vertices_data;      \
    std::string indices_data;       \
    VertexAttrib position;          \
    VertexAttrib normal;            \
    VertexAttrib texture;           \
)
GRP_DEFINE_NODE(PrimitiveShape, false,                  \
    PrimitiveShapeType type = PrimitiveShapeType::Quad; \
)
GRP_DEFINE_NODE(Model, false, \
    std::string filepath;     \
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

// input
GRP_DEFINE_NODE(ProjectMat,     false, GRP_NODE_PROP)
GRP_DEFINE_NODE(ViewMat,        false, GRP_NODE_PROP)
GRP_DEFINE_NODE(ModelMat,       false, GRP_NODE_PROP)
GRP_DEFINE_NODE(CameraPosition, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(LightPosition,  false, GRP_NODE_PROP)

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
			{ bp::PIN_ANY_VAR, "prev" }
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

}
}