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
    case renderlab::PIN_TEXTURE:
        ret += "(T)";
        break;
    case renderlab::PIN_RENDERTARGET:
        ret += "(RT)";
        break;
    case renderlab::PIN_SHADER:
        ret += "(S)";
        break;
    case renderlab::PIN_MODEL:
        ret += "(M)";
        break;

    case renderlab::PIN_INT:
        ret += "(I)";
        break;
    case renderlab::PIN_BOOL:
        ret += "(B)";
        break;
    case renderlab::PIN_VECTOR1:
        ret += "(1)";
        break;
    case renderlab::PIN_VECTOR2:
        ret += "(2)";
        break;
    case renderlab::PIN_VECTOR3:
        ret += "(3)";
        break;
    case renderlab::PIN_VECTOR4:
        ret += "(4)";
        break;
    case renderlab::PIN_MATRIX2:
        ret += "(M2)";
        break;
    case renderlab::PIN_MATRIX3:
        ret += "(M3)";
        break;
    case renderlab::PIN_MATRIX4:
        ret += "(M4)";
        break;

    case renderlab::PIN_SAMPLER2D:
        ret += "(S2)";
        break;
    case renderlab::PIN_SAMPLE_CUBE:
        ret += "(SC)";
        break;

    case renderlab::PIN_VECTOR1_ARRAY:
        ret += "(1S)";
        break;
    case renderlab::PIN_VECTOR2_ARRAY:
        ret += "(2S)";
        break;
    case renderlab::PIN_VECTOR3_ARRAY:
        ret += "(3S)";
        break;
    case renderlab::PIN_VECTOR4_ARRAY:
        ret += "(4S)";
        break;
    }
    return ret;
}

const pt0::Color& get_color_func(int type)
{
    switch (type)
    {
    case renderlab::PIN_TEXTURE:
        return COL_TEXTURE;
    case renderlab::PIN_RENDERTARGET:
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
    if (type_from == renderlab::PIN_TEXTURE && type_to == renderlab::PIN_SAMPLER2D ||
        type_from == renderlab::PIN_SAMPLER2D && type_to == renderlab::PIN_TEXTURE) {
        return true;
    }

    return false;
}

}

namespace renderlab
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