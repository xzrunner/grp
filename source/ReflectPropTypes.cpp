#include "renderlab/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<rlab::ClearType>("grp_clear_type")
	.property("type", &rlab::ClearType::type)
;

rttr::registration::enumeration<rlab::TextureType>("grp_tex_type")
(
	rttr::value("2d",   rlab::TextureType::Tex2D),
	rttr::value("cube", rlab::TextureType::TexCube)
);

rttr::registration::enumeration<rlab::TextureFormat>("grp_tex_fmt")
(
	rttr::value("rgba8",  rlab::TextureFormat::RGBA8),
    rttr::value("rgba4",  rlab::TextureFormat::RGBA4),
    rttr::value("rgb",    rlab::TextureFormat::RGB),
    rttr::value("rgb565", rlab::TextureFormat::RGB565),
    rttr::value("a8",     rlab::TextureFormat::A8)
);

}

namespace rlab
{
void prop_types_regist_rttr()
{
}
}