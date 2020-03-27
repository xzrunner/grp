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

#define GRP_DEFINE_NODE(type, name, preview, prop) \
class type : public Node                     \
{                                            \
public:                                      \
	type()                                   \
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
    Preview()
        : Node("Preview", true)
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, bp::PIN_ANY_VAR, "in", *this
        ));

        Layout();
    }
    RTTR_ENABLE(Node)
}; // Preview

// resource
GRP_DEFINE_NODE(RenderTarget, render_target, false, \
    size_t width = 0;                \
    size_t height = 0;               \
    bool depth_buf = false;          \
    bool color_buf = false;          \
)
GRP_DEFINE_NODE(Texture, texture, true,                                      \
    std::string filepath;                                           \
    renderlab::TextureType type = renderlab::TextureType::Tex2D;              \
    size_t width = 0;                                               \
    size_t height = 0;                                              \
    renderlab::TextureFormat    format = renderlab::TextureFormat::RGBA8;     \
    renderlab::TextureWrapping  wrap   = renderlab::TextureWrapping::Repeat;  \
    renderlab::TextureFiltering filter = renderlab::TextureFiltering::Linear; \
)
GRP_DEFINE_NODE(VertexArray, vertex_array, false, \
    std::string vertices_data;      \
    std::string indices_data;       \
    VertexAttrib position;          \
    VertexAttrib normal;            \
    VertexAttrib texture;           \
)
GRP_DEFINE_NODE(PrimitiveShape, primitive_shape, false,                     \
    PrimitiveShapeType  type   = PrimitiveShapeType::Quad; \
    PrimitiveVertLayout layout = PrimitiveVertLayout::Pos; \
)
GRP_DEFINE_NODE(Model, model, false, \
    std::string filepath;     \
)
GRP_DEFINE_NODE(Heightfield, heightfield, false, \
    size_t width  = 128;            \
    size_t height = 128;            \
)
// op
GRP_DEFINE_NODE(Clear, clear, true, \
    ClearType  type;         \
    pt0::Color color;        \
)
GRP_DEFINE_NODE(Bind, bind, false, \
    int channel = 0;         \
)
GRP_DEFINE_NODE(Unbind, unbind, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(Draw, draw, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(SetUniform, set_uniform, false,                           \
    std::string var_name;                                    \
    ShaderUniformType var_type = ShaderUniformType::Unknown; \
)

// state
GRP_DEFINE_NODE(Viewport, viewport, false, \
    int x = 0;                   \
    int y = 0;                   \
    int w = 0;                   \
    int h = 0;                   \
)
GRP_DEFINE_NODE(AlphaTest, alpha_test, false,               \
    AlphaTestFunc func = AlphaTestFunc::Always; \
)
GRP_DEFINE_NODE(BlendEq, blend_eq, false,              \
    BlendEqMode mode = BlendEqMode::FuncAdd; \
)
GRP_DEFINE_NODE(BlendFunc, blend_func, false,                    \
    BlendFuncFactor sfactor = BlendFuncFactor::One;  \
    BlendFuncFactor dfactor = BlendFuncFactor::Zero; \
)
GRP_DEFINE_NODE(Cull, cull, false,        \
    CullMode type = CullMode::Back; \
)
GRP_DEFINE_NODE(ZTest, ztest, false,         \
    ZTestFunc func = ZTestFunc::Less; \
)
GRP_DEFINE_NODE(ZWrite, zwrite, false, \
    bool enable = false;       \
)
GRP_DEFINE_NODE(FrontFace, front_face, false, \
    bool clockwise = false;       \
)

// value
GRP_DEFINE_NODE(Bool, bool, false,    \
    bool val = false;           \
)
GRP_DEFINE_NODE(Vector1, vector1, false, \
    float val = 0;              \
)
GRP_DEFINE_NODE(Vector2, vector2, false, \
    sm::vec2 val;               \
)
GRP_DEFINE_NODE(Vector3, vector3, false, \
    sm::vec3 val;               \
)
GRP_DEFINE_NODE(Vector4, vector4, false, \
    sm::vec4 val;               \
)
GRP_DEFINE_NODE(Matrix2, matrix2, false, \
    sm::mat2 val;               \
)
GRP_DEFINE_NODE(Matrix3, matrix3, false, \
    sm::mat3 val;               \
)
GRP_DEFINE_NODE(Matrix4, matrix4, false, \
    sm::mat4 val;               \
)

// math
GRP_DEFINE_NODE(Add, add, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(Mul, mul, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(PerspectiveMat, perspective_mat, false,    \
    float fovy   = 45.0f * SM_DEG_TO_RAD; \
    float aspect = 1.0f;                  \
    float znear   = 0.1f;                 \
    float zfar    = 100.0f;               \
)
GRP_DEFINE_NODE(OrthoMat, ortho_mat, false, \
    float left   = -10.0f;       \
    float right  = 10.0f;        \
    float bottom = -10.0f;       \
    float top    = 10.0f;        \
    float znear  = 1.0f;         \
    float zfar   = 7.5f;         \
)
GRP_DEFINE_NODE(LookAtMat, look_at_mat, false,    \
    sm::vec3 eye;                    \
    sm::vec3 center;                 \
    sm::vec3 up = sm::vec3(0, 1, 0); \
)
GRP_DEFINE_NODE(Translate, translate, false,    \
    sm::vec3 offset;                 \
)
GRP_DEFINE_NODE(Rotate, rotate, false,       \
    float angle = 0;                 \
    sm::vec3 axis;                   \
)
GRP_DEFINE_NODE(Scale, scale, false,        \
    sm::vec3 scale;                  \
)

// input
GRP_DEFINE_NODE(ProjectMat, project_mat,     false, GRP_NODE_PROP)
GRP_DEFINE_NODE(ViewMat, view_mat,        false, GRP_NODE_PROP)
GRP_DEFINE_NODE(ModelMat, model_mat,       false, GRP_NODE_PROP)
GRP_DEFINE_NODE(CameraPosition, camera_position, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(LightPosition, light_position,  false, GRP_NODE_PROP)
GRP_DEFINE_NODE(UserScript, user_script,     false,                    \
    std::string code;                                     \
    UserScriptRetType ret_type = UserScriptRetType::Void; \
)

// control flow
GRP_DEFINE_NODE(For, for, false, \
    int index_begin = 0;    \
    int index_end   = 0;    \
    int index_step  = 0;    \
)
GRP_DEFINE_NODE(ForEachLoop, for_each_loop, false, GRP_NODE_PROP)

// control
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
            true, 0, bp::PIN_ANY_VAR, "next", *this
        ));

        Layout();
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
GRP_DEFINE_NODE(SeparableSSS, separable_sss, false,                   \
    int      nsamples = 7;                             \
    sm::vec3 strength = sm::vec3(0.48f, 0.41f, 0.28f); \
    sm::vec3 falloff  = sm::vec3(1.0f, 0.37f, 0.3f);   \
)
GRP_DEFINE_NODE(GlobalIllumination, gi, false, GRP_NODE_PROP)

}
}