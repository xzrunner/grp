#include "grp/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<grp::ClearType>("grp_clear_type")
	.property("type", &grp::ClearType::type)
;

rttr::registration::enumeration<grp::TextureType>("grp_tex_type")
(
	rttr::value("2d",   grp::TextureType::Tex2D),
	rttr::value("cube", grp::TextureType::TexCube)
);

rttr::registration::enumeration<grp::TextureFormat>("grp_tex_fmt")
(
	rttr::value("rgba8",  grp::TextureFormat::RGBA8),
    rttr::value("rgba4",  grp::TextureFormat::RGBA4),
    rttr::value("rgb",    grp::TextureFormat::RGB),
    rttr::value("rgb565", grp::TextureFormat::RGB565),
    rttr::value("a8",     grp::TextureFormat::A8)
);

}

namespace grp
{
void prop_types_regist_rttr()
{
}
}