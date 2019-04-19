#pragma once

#include <cstdint>
#include <string>

namespace rlab
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
    RGBA8,
    RGBA4,
    RGB,
    RGB565,
    A8,
    Depth,
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

void prop_types_regist_rttr();

}