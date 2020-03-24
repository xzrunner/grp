#include "renderlab/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<renderlab::ClearType>("rlab_clear_type")
	.property("type", &renderlab::ClearType::type)
;

rttr::registration::enumeration<renderlab::TextureType>("rlab_tex_type")
(
	rttr::value("2d",   renderlab::TextureType::Tex2D),
	rttr::value("cube", renderlab::TextureType::TexCube)
);

rttr::registration::enumeration<renderlab::TextureFormat>("rlab_tex_fmt")
(
    rttr::value("rgba16", renderlab::TextureFormat::RGBA16),
	rttr::value("rgba8",  renderlab::TextureFormat::RGBA8),
    rttr::value("rgba4",  renderlab::TextureFormat::RGBA4),
    rttr::value("rgb",    renderlab::TextureFormat::RGB),
    rttr::value("rgb565", renderlab::TextureFormat::RGB565),
    rttr::value("a8",     renderlab::TextureFormat::A8),
    rttr::value("depth",  renderlab::TextureFormat::Depth)
);

rttr::registration::enumeration<renderlab::TextureWrapping>("rlab_tex_wrap")
(
	rttr::value("repeat",          renderlab::TextureWrapping::Repeat),
    rttr::value("mirrored_repeat", renderlab::TextureWrapping::MirroredRepeat),
    rttr::value("clamp_to_edge",   renderlab::TextureWrapping::ClampToEdge),
    rttr::value("clamp_to_border", renderlab::TextureWrapping::ClampToBorder)
);

rttr::registration::enumeration<renderlab::TextureFiltering>("rlab_tex_filter")
(
	rttr::value("nearest", renderlab::TextureFiltering::Nearest),
    rttr::value("linear",  renderlab::TextureFiltering::Linear)
);

rttr::registration::enumeration<renderlab::VertexDataType>("rlab_vert_data_type")
(
	rttr::value("char",  renderlab::VertexDataType::Char),
    rttr::value("float", renderlab::VertexDataType::Float)
);

rttr::registration::class_<renderlab::VertexAttrib>("rlab_vert_attrib")
	.property("name", &renderlab::VertexAttrib::name)
    .property("num",  &renderlab::VertexAttrib::num)
    .property("type", &renderlab::VertexAttrib::type)
;

rttr::registration::enumeration<renderlab::PrimitiveShapeType>("rlab_prim_type")
(
    rttr::value("quad", renderlab::PrimitiveShapeType::Quad),
	rttr::value("cube", renderlab::PrimitiveShapeType::Cube)
);

rttr::registration::enumeration<renderlab::PrimitiveVertLayout>("rlab_prim_vert_layout")
(
    rttr::value("pos",             renderlab::PrimitiveVertLayout::Pos),
	rttr::value("pos_tex",         renderlab::PrimitiveVertLayout::PosTex),
    rttr::value("pos_norm_tex",    renderlab::PrimitiveVertLayout::PosNormTex),
    rttr::value("pos_norm_tex_tb", renderlab::PrimitiveVertLayout::PosNormTexTB)
);

rttr::registration::enumeration<renderlab::AlphaTestFunc>("rlab_alpha_test_func")
(
	rttr::value("off",       renderlab::AlphaTestFunc::Off),
    rttr::value("never",     renderlab::AlphaTestFunc::Never),
    rttr::value("less",      renderlab::AlphaTestFunc::Less),
    rttr::value("equal",     renderlab::AlphaTestFunc::Equal),
    rttr::value("lequal",    renderlab::AlphaTestFunc::LEqual),
    rttr::value("greater",   renderlab::AlphaTestFunc::Greater),
    rttr::value("not_equal", renderlab::AlphaTestFunc::NotEqual),
    rttr::value("gequal",    renderlab::AlphaTestFunc::GEqual),
    rttr::value("always",    renderlab::AlphaTestFunc::Always)
);

rttr::registration::enumeration<renderlab::BlendEqMode>("rlab_blend_eq_mode")
(
	rttr::value("func_add",         renderlab::BlendEqMode::FuncAdd),
    rttr::value("func_sub",         renderlab::BlendEqMode::FuncSubtract),
    rttr::value("func_reverse_sub", renderlab::BlendEqMode::FuncReverseSubtract),
    rttr::value("min",              renderlab::BlendEqMode::Min),
    rttr::value("max",              renderlab::BlendEqMode::Max)
);

rttr::registration::enumeration<renderlab::BlendFuncFactor>("rlab_blend_func_factor")
(
	rttr::value("off",                 renderlab::BlendFuncFactor::Off),
    rttr::value("zero",                renderlab::BlendFuncFactor::Zero),
    rttr::value("one",                 renderlab::BlendFuncFactor::One),
    rttr::value("src_color",           renderlab::BlendFuncFactor::SrcColor),
    rttr::value("one_minus_src_color", renderlab::BlendFuncFactor::OneMinusSrcColor),
    rttr::value("dst_color",           renderlab::BlendFuncFactor::DstColor),
    rttr::value("one_minus_dst_color", renderlab::BlendFuncFactor::OneMinusDstColor),
    rttr::value("src_alpha",           renderlab::BlendFuncFactor::SrcAlpha),
    rttr::value("one_minus_src_alpha", renderlab::BlendFuncFactor::OneMinusSrcAlpha),
    rttr::value("dst_alpha",           renderlab::BlendFuncFactor::DstAlpha),
    rttr::value("one_minus_dst_alpha", renderlab::BlendFuncFactor::OneMinusDstAlpha)
);

rttr::registration::enumeration<renderlab::CullMode>("rlab_cull_type")
(
	rttr::value("off",            renderlab::CullMode::Off),
    rttr::value("front",          renderlab::CullMode::Front),
	rttr::value("back",           renderlab::CullMode::Back),
    rttr::value("front_and_back", renderlab::CullMode::FrontAndBack)
);

rttr::registration::enumeration<renderlab::ZTestFunc>("rlab_ztest_func")
(
	rttr::value("off",       renderlab::ZTestFunc::Off),
    rttr::value("never",     renderlab::ZTestFunc::Never),
    rttr::value("less",      renderlab::ZTestFunc::Less),
    rttr::value("equal",     renderlab::ZTestFunc::Equal),
    rttr::value("lequal",    renderlab::ZTestFunc::LEqual),
    rttr::value("greater",   renderlab::ZTestFunc::Greater),
    rttr::value("not_equal", renderlab::ZTestFunc::NotEqual),
    rttr::value("gequal",    renderlab::ZTestFunc::GEqual),
    rttr::value("always",    renderlab::ZTestFunc::Always)
);

rttr::registration::enumeration<renderlab::UserScriptRetType>("user_script_ret_type")
(
	rttr::value("void",       renderlab::UserScriptRetType::Void),
    rttr::value("vec1_array", renderlab::UserScriptRetType::Vec1Array),
    rttr::value("vec2_array", renderlab::UserScriptRetType::Vec2Array),
    rttr::value("vec3_array", renderlab::UserScriptRetType::Vec3Array),
	rttr::value("vec4_array", renderlab::UserScriptRetType::Vec4Array)
);

rttr::registration::enumeration<renderlab::ShaderUniformType>("shader_uniform_type")
(
	rttr::value("unknown",  renderlab::ShaderUniformType::Unknown),
    rttr::value("int",      renderlab::ShaderUniformType::Int),
    rttr::value("bool",     renderlab::ShaderUniformType::Bool),
    rttr::value("vec1",     renderlab::ShaderUniformType::Vector1),
    rttr::value("vec2",     renderlab::ShaderUniformType::Vector2),
    rttr::value("vec3",     renderlab::ShaderUniformType::Vector3),
    rttr::value("vec4",     renderlab::ShaderUniformType::Vector4),
    rttr::value("mat2",     renderlab::ShaderUniformType::Matrix2),
    rttr::value("mat3",     renderlab::ShaderUniformType::Matrix3),
    rttr::value("mat4",     renderlab::ShaderUniformType::Matrix4),
    rttr::value("tex_2d",   renderlab::ShaderUniformType::Sampler2D),
    rttr::value("tex_cubd", renderlab::ShaderUniformType::SamplerCube)
);

}

namespace renderlab
{
void prop_types_regist_rttr()
{
}
}