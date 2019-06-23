#include "renderlab/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<rlab::ClearType>("rlab_clear_type")
	.property("type", &rlab::ClearType::type)
;

rttr::registration::enumeration<rlab::TextureType>("rlab_tex_type")
(
	rttr::value("2d",   rlab::TextureType::Tex2D),
	rttr::value("cube", rlab::TextureType::TexCube)
);

rttr::registration::enumeration<rlab::TextureFormat>("rlab_tex_fmt")
(
    rttr::value("rgba16", rlab::TextureFormat::RGBA16),
	rttr::value("rgba8",  rlab::TextureFormat::RGBA8),
    rttr::value("rgba4",  rlab::TextureFormat::RGBA4),
    rttr::value("rgb",    rlab::TextureFormat::RGB),
    rttr::value("rgb565", rlab::TextureFormat::RGB565),
    rttr::value("a8",     rlab::TextureFormat::A8),
    rttr::value("depth",  rlab::TextureFormat::Depth)
);

rttr::registration::enumeration<rlab::TextureWrapping>("rlab_tex_wrap")
(
	rttr::value("repeat",          rlab::TextureWrapping::Repeat),
    rttr::value("mirrored_repeat", rlab::TextureWrapping::MirroredRepeat),
    rttr::value("clamp_to_edge",   rlab::TextureWrapping::ClampToEdge),
    rttr::value("clamp_to_border", rlab::TextureWrapping::ClampToBorder)
);

rttr::registration::enumeration<rlab::TextureFiltering>("rlab_tex_filter")
(
	rttr::value("nearest", rlab::TextureFiltering::Nearest),
    rttr::value("linear",  rlab::TextureFiltering::Linear)
);

rttr::registration::enumeration<rlab::VertexDataType>("rlab_vert_data_type")
(
	rttr::value("char",  rlab::VertexDataType::Char),
    rttr::value("float", rlab::VertexDataType::Float)
);

rttr::registration::class_<rlab::VertexAttrib>("rlab_vert_attrib")
	.property("name", &rlab::VertexAttrib::name)
    .property("num",  &rlab::VertexAttrib::num)
    .property("type", &rlab::VertexAttrib::type)
;

rttr::registration::enumeration<rlab::PrimitiveShapeType>("rlab_prim_type")
(
    rttr::value("quad", rlab::PrimitiveShapeType::Quad),
	rttr::value("cube", rlab::PrimitiveShapeType::Cube)
);

rttr::registration::enumeration<rlab::PrimitiveVertLayout>("rlab_prim_vert_layout")
(
    rttr::value("pos",             rlab::PrimitiveVertLayout::Pos),
	rttr::value("pos_tex",         rlab::PrimitiveVertLayout::PosTex),
    rttr::value("pos_norm_tex",    rlab::PrimitiveVertLayout::PosNormTex),
    rttr::value("pos_norm_tex_tb", rlab::PrimitiveVertLayout::PosNormTexTB)
);

rttr::registration::enumeration<rlab::AlphaTestFunc>("rlab_alpha_test_func")
(
	rttr::value("off",       rlab::AlphaTestFunc::Off),
    rttr::value("never",     rlab::AlphaTestFunc::Never),
    rttr::value("less",      rlab::AlphaTestFunc::Less),
    rttr::value("equal",     rlab::AlphaTestFunc::Equal),
    rttr::value("lequal",    rlab::AlphaTestFunc::LEqual),
    rttr::value("greater",   rlab::AlphaTestFunc::Greater),
    rttr::value("not_equal", rlab::AlphaTestFunc::NotEqual),
    rttr::value("gequal",    rlab::AlphaTestFunc::GEqual),
    rttr::value("always",    rlab::AlphaTestFunc::Always)
);

rttr::registration::enumeration<rlab::BlendEqMode>("rlab_blend_eq_mode")
(
	rttr::value("func_add",         rlab::BlendEqMode::FuncAdd),
    rttr::value("func_sub",         rlab::BlendEqMode::FuncSubtract),
    rttr::value("func_reverse_sub", rlab::BlendEqMode::FuncReverseSubtract),
    rttr::value("min",              rlab::BlendEqMode::Min),
    rttr::value("max",              rlab::BlendEqMode::Max)
);

rttr::registration::enumeration<rlab::BlendFuncFactor>("rlab_blend_func_factor")
(
	rttr::value("off",                 rlab::BlendFuncFactor::Off),
    rttr::value("zero",                rlab::BlendFuncFactor::Zero),
    rttr::value("one",                 rlab::BlendFuncFactor::One),
    rttr::value("src_color",           rlab::BlendFuncFactor::SrcColor),
    rttr::value("one_minus_src_color", rlab::BlendFuncFactor::OneMinusSrcColor),
    rttr::value("dst_color",           rlab::BlendFuncFactor::DstColor),
    rttr::value("one_minus_dst_color", rlab::BlendFuncFactor::OneMinusDstColor),
    rttr::value("src_alpha",           rlab::BlendFuncFactor::SrcAlpha),
    rttr::value("one_minus_src_alpha", rlab::BlendFuncFactor::OneMinusSrcAlpha),
    rttr::value("dst_alpha",           rlab::BlendFuncFactor::DstAlpha),
    rttr::value("one_minus_dst_alpha", rlab::BlendFuncFactor::OneMinusDstAlpha)
);

rttr::registration::enumeration<rlab::CullMode>("rlab_cull_type")
(
	rttr::value("off",            rlab::CullMode::Off),
    rttr::value("front",          rlab::CullMode::Front),
	rttr::value("back",           rlab::CullMode::Back),
    rttr::value("front_and_back", rlab::CullMode::FrontAndBack)
);

rttr::registration::enumeration<rlab::ZTestFunc>("rlab_ztest_func")
(
	rttr::value("off",       rlab::ZTestFunc::Off),
    rttr::value("never",     rlab::ZTestFunc::Never),
    rttr::value("less",      rlab::ZTestFunc::Less),
    rttr::value("equal",     rlab::ZTestFunc::Equal),
    rttr::value("lequal",    rlab::ZTestFunc::LEqual),
    rttr::value("greater",   rlab::ZTestFunc::Greater),
    rttr::value("not_equal", rlab::ZTestFunc::NotEqual),
    rttr::value("gequal",    rlab::ZTestFunc::GEqual),
    rttr::value("always",    rlab::ZTestFunc::Always)
);

rttr::registration::enumeration<rlab::UserScriptRetType>("user_script_ret_type")
(
	rttr::value("void",       rlab::UserScriptRetType::Void),
    rttr::value("vec3_array", rlab::UserScriptRetType::Vec3Array),
	rttr::value("vec4_array", rlab::UserScriptRetType::Vec4Array)
);

}

namespace rlab
{
void prop_types_regist_rttr()
{
}
}