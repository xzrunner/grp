#include "renderlab/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#include <painting0/Color.h>

RTTR_REGISTRATION
{

rttr::registration::class_<rlab::Node>("rlab::node")
.property("enable", &rlab::Node::GetEnable, &rlab::Node::SetEnable)
(
    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Node Enable"))
)
.property("preview", &rlab::Node::GetPreview, &rlab::Node::SetPreview)
(
    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Node Preview"))
);

rttr::registration::class_<pt0::Color>("pt0_color")
	.constructor()(rttr::policy::ctor::as_object)
	.property("r", &pt0::Color::r)
    .property("g", &pt0::Color::g)
    .property("b", &pt0::Color::b)
    .property("a", &pt0::Color::a)
;

}

namespace rlab
{

void regist_rttr()
{
	prop_types_regist_rttr();
	nodes_regist_rttr();
}

}