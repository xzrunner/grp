#include "renderlab/ReflectPropTypes.h"

#include <rttr/registration.h>

#define REGIST_ENUM_ITEM(type, name, label) \
    rttr::value(name, type),                \
    rttr::metadata(type, label)             \

RTTR_REGISTRATION
{

rttr::registration::enumeration<renderlab::VertexDataType>("rlab_vert_data_type")
(
    REGIST_ENUM_ITEM(renderlab::VertexDataType::Char,  "char",  "Char"),
    REGIST_ENUM_ITEM(renderlab::VertexDataType::Float, "float", "Float")
);

rttr::registration::class_<renderlab::VertexAttrib>("rlab_vert_attrib")
	.property("name", &renderlab::VertexAttrib::name)
    .property("num",  &renderlab::VertexAttrib::num)
    .property("type", &renderlab::VertexAttrib::type)
;

rttr::registration::enumeration<renderlab::ShaderUniformType>("shader_uniform_type")
(
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Unknown,     "unknown",  "Unknown"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Int,         "int",      "Int"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Bool,        "bool",     "Bool"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Vector1,     "vec1",     "Vector1"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Vector2,     "vec2",     "Vector2"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Vector3,     "vec3",     "Vector3"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Vector4,     "vec4",     "Vector4"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Matrix2,     "mat2",     "Matrix2"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Matrix3,     "mat3",     "Matrix3"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Matrix4,     "mat4",     "Matrix4"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::Sampler2D,   "tex_2d",   "Sampler2D"),
    REGIST_ENUM_ITEM(renderlab::ShaderUniformType::SamplerCube, "tex_cubd", "SamplerCube")
);

}

namespace renderlab
{
void prop_types_regist_rttr()
{
}
}