#include "renderlab/PinCallback.h"
#include "renderlab/PinType.h"

#include <painting0/Color.h>
#include <blueprint/Pin.h>

#include <string>

namespace
{

const pt0::Color COL_DEFAULT   = pt0::Color(255, 255, 255);

const pt0::Color COL_TEXTURE   = pt0::Color(132, 228, 231);
const pt0::Color COL_RT        = pt0::Color(154, 239, 146);

std::string get_desc_func(const std::string& name, int type)
{
    std::string ret = name;
    switch (type)
    {
    case rlab::PIN_TEXTURE:
        ret += "(T)";
        break;
    case rlab::PIN_RENDERTARGET:
        ret += "(RT)";
        break;
    }
    return ret;
}

const pt0::Color& get_color_func(int type)
{
    switch (type)
    {
    case rlab::PIN_TEXTURE:
        return COL_TEXTURE;
    case rlab::PIN_RENDERTARGET:
        return COL_RT;
    default:
        return COL_DEFAULT;
    }
}

bool can_type_cast_func(int type_from, int type_to)
{
    if (type_from == type_to) {
        return true;
    }
    if (type_from == bp::PIN_ANY_VAR ||
        type_to == bp::PIN_ANY_VAR) {
        return true;
    }
    if (type_from == rlab::PIN_TEXTURE && type_to == rlab::PIN_SAMPLER2D ||
        type_from == rlab::PIN_SAMPLER2D && type_to == rlab::PIN_TEXTURE) {
        return true;
    }

    return false;
}

}

namespace rlab
{

void InitPinCallback()
{
    bp::Pin::SetExtendFuncs({
        get_desc_func,
        get_color_func,
        can_type_cast_func
    });
}

}