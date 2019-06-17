#include "renderlab/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

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

}

namespace bp
{
extern void regist_sm_rttr();
extern void regist_pt0_rttr();
}

namespace rlab
{

void regist_rttr()
{
    bp::regist_sm_rttr();
    bp::regist_pt0_rttr();

	prop_types_regist_rttr();
	nodes_regist_rttr();
}

}