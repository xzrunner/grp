#pragma once

#include <cstdint>
#include <string>

namespace renderlab
{

// Clear
struct ClearType
{
    ClearType(uint32_t type = 0)
		: type(type) {}

    static const uint32_t CLEAR_COLOR   = 0x1;
    static const uint32_t CLEAR_DEPTH   = 0x2;
    static const uint32_t CLEAR_STENCIL = 0x4;

	uint32_t type = 0;
};

// Texture
enum class TextureType
{
    Tex2D,
    TexCube,
};

enum class TextureFormat
{
    RGBA16,
    RGBA8,
    RGBA4,
    RGB,
    RGB565,
    A8,
    RED,
    Depth
};

enum class TextureWrapping
{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
};

enum class TextureFiltering
{
    Nearest,
    Linear,
};

// VertexArray

enum class VertexDataType
{
    Char,
    Float,
};

struct VertexAttrib
{
    std::string    name;
    size_t         num = 0;
    VertexDataType type = VertexDataType::Float;
};

enum class PrimitiveShapeType
{
    Quad,
    Cube,
};

enum class PrimitiveVertLayout
{
    Pos,
    PosTex,
    PosNormTex,
    PosNormTexTB,
};

// state

enum class AlphaTestFunc
{
    Off,
    Never,
    Less,
    Equal,
    LEqual,
    Greater,
    NotEqual,
    GEqual,
    Always
};

enum class BlendEqMode
{
    FuncAdd,
    FuncSubtract,
    FuncReverseSubtract,
    Min,
    Max
};

enum class BlendFuncFactor
{
    Off = 0,
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha
};

enum class CullMode
{
    Off,
    Front,
    Back,
    FrontAndBack,
};

enum class ZTestFunc
{
    Off = 0,
    Never,
    Less,
    Equal,
    LEqual,
    Greater,
    NotEqual,
    GEqual,
    Always
};

// others

enum class UserScriptRetType
{
    Void = 0,
    Vec1Array,
    Vec2Array,
    Vec3Array,
    Vec4Array,
};

enum class ShaderUniformType
{
    Unknown = 0,
    Int,
    Bool,
    Vector1,
    Vector2,
    Vector3,
    Vector4,
    Matrix2,
    Matrix3,
    Matrix4,
    Sampler2D,
    SamplerCube,
};

void prop_types_regist_rttr();

}