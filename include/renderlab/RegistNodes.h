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

GRP_DEFINE_NODE(Clear, true, \
    ClearType  type;         \
    pt0::Color color;        \
)
GRP_DEFINE_NODE(Viewport, true, \
    int x = 0;                  \
    int y = 0;                  \
    int w = 0;                  \
    int h = 0;                  \
)
GRP_DEFINE_NODE(Texture, true,                                                                 \
    virtual void PreviewDraw(const rg::NodePtr& node, const sm::Matrix2D& mat) const override; \
    std::string filepath;                                                                      \
    rlab::TextureType type = rlab::TextureType::Tex2D;                                         \
    int width = 0;                                                                             \
    int height = 0;                                                                            \
    rlab::TextureFormat format = rlab::TextureFormat::RGBA8;                                   \
)
GRP_DEFINE_NODE(RenderTarget, true, GRP_NODE_PROP)
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
GRP_DEFINE_NODE(Bind, false, \
    int channel = 0;         \
)
GRP_DEFINE_NODE(Unbind, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(DrawCube, false, GRP_NODE_PROP)

class OutputToScreen : public Node
{
public:
    OutputToScreen()
        : Node("OutputToScreen", false)
    {
        InitPins({
			{ bp::PIN_ANY_VAR, "prev" }
		},{
		});
    }

    RTTR_ENABLE(Node)

}; // OutputToScreen

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
GRP_DEFINE_NODE(CamProjMat,     false, GRP_NODE_PROP)
GRP_DEFINE_NODE(CamViewMat,     false, GRP_NODE_PROP)
GRP_DEFINE_NODE(CameraPosition, false, GRP_NODE_PROP)
GRP_DEFINE_NODE(LightPosition,  false, GRP_NODE_PROP)

}
}