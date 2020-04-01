#pragma once

#include <cstdint>
#include <string>

namespace renderlab
{

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

// others

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