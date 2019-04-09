#pragma once

#include <cstdint>

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
};

// cull
enum class CullType
{
    Off,
    Back,
    Front
};

void prop_types_regist_rttr();

}