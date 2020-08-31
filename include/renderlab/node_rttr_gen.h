#define XSTR(s) STR(s)
#define STR(s) #s

#ifndef PARM_NODE_TYPE
#error "You must define PARM_NODE_TYPE macro before include this file"
#endif

#ifndef PARM_NODE_NAME
#error "You must define PARM_NODE_NAME macro before include this file"
#endif

#ifndef PARM_FILEPATH
#define PARM_FILEPATH rendergraph/node/##PARM_NODE_TYPE##.parm.h
#endif

#define RTTR_NAME renderlab::##PARM_NODE_NAME

rttr::registration::class_<renderlab::node::PARM_NODE_TYPE>(XSTR(RTTR_NAME))
.constructor<>()

#define PARAM_INFO(id, type, name, member, default_val)        \
.property(#name, &renderlab::node::##PARM_NODE_TYPE::member)   \
(                                                              \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(#id)) \
)
#ifndef NO_PARM_FILEPATH
#include XSTR(PARM_FILEPATH)
#endif // NO_PARM_FILEPATH
#undef  PARAM_INFO

;

#undef PARM_NODE_NAME
#undef PARM_NODE_TYPE
#undef PARM_FILEPATH

#undef NO_PARM_FILEPATH