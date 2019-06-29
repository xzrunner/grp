#include "renderlab/WxNodeProperty.h"
#include "renderlab/ReflectPropTypes.h"
#include "renderlab/PinType.h"

#include <ee0/SubjectMgr.h>
#include <ee0/ReflectPropTypes.h>
#include <ee0/MessageID.h>
#include <ee0/WxPropHelper.h>
#include <blueprint/Node.h>
#include <blueprint/MessageID.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

#include <cpputil/StringHelper.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace
{

const wxChar* PIN_IDX_TITLE[] = { wxT("Unknown"), wxT("Port"), wxT("Tex"), wxT("RT"), wxT("Shader"), wxT("Model"), wxT("Int"), wxT("Bool"),
                                  wxT("Vec1"), wxT("Vec2"), wxT("Vec3"), wxT("Vec4"), wxT("Mat2"), wxT("Mat3"), wxT("Mat4"),
                                  wxT("Vec1Array"), wxT("Vec2Array"), wxT("Vec3Array"), wxT("Vec4Array"), NULL };

const int PIN_IDX_TO_TYPE[] = {
    bp::PIN_ANY_VAR,
    bp::PIN_PORT,

    rlab::PIN_TEXTURE,
    rlab::PIN_RENDERTARGET,
    rlab::PIN_SHADER,
    rlab::PIN_MODEL,

    rlab::PIN_INT,
    rlab::PIN_BOOL,
    rlab::PIN_VECTOR1,
    rlab::PIN_VECTOR2,
    rlab::PIN_VECTOR3,
    rlab::PIN_VECTOR4,
    rlab::PIN_MATRIX2,
    rlab::PIN_MATRIX3,
    rlab::PIN_MATRIX4,

    rlab::PIN_VECTOR1_ARRAY,
    rlab::PIN_VECTOR2_ARRAY,
    rlab::PIN_VECTOR3_ARRAY,
    rlab::PIN_VECTOR4_ARRAY,
};

int PinTypeToIdx(int type)
{
    const int num = sizeof(PIN_IDX_TO_TYPE) / sizeof(PIN_IDX_TO_TYPE[0]);
    for (int i = 0; i < num; ++i)
    {
        if (PIN_IDX_TO_TYPE[i] == type) {
            return i;
        }
    }
    assert(0);
    return -1;
}

}

namespace rlab
{

WxNodeProperty::WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: bp::WxNodeProperty(parent, sub_mgr)
{
}

bool WxNodeProperty::InitView(const rttr::property& prop, const bp::NodePtr& node)
{
    bool ret = true;

    auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
    if (!ui_info_obj.is_valid()) {
        return false;
    }

    auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
    auto node_type = node->get_type();
    if ((node_type == rttr::type::get<bp::node::Input>() && ui_info.desc == bp::node::Input::STR_TYPE) ||
        (node_type == rttr::type::get<bp::node::Output>() && ui_info.desc == bp::node::Output::STR_TYPE))
    {
		auto mode_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, PIN_IDX_TITLE);
		auto mode = prop.get_value(node).get_value<int>();
        if (mode < 0) {
            mode = bp::PIN_ANY_VAR;
        }
		mode_prop->SetValue(PinTypeToIdx(mode));
		m_pg->Append(mode_prop);
        return true;
    }

    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<ClearType>())
    {
        const wxChar* CLEAR_TYPES[] = { wxT("Color"), wxT("Depth"), wxT("Stencil"), NULL };
		const long    CLEAR_VALUES[] = {
            ClearType::CLEAR_COLOR,
            ClearType::CLEAR_DEPTH,
            ClearType::CLEAR_STENCIL
		};
		auto channels = prop.get_value(node).get_value<ClearType>().type;
		m_pg->Append(new wxFlagsProperty(ui_info.desc, wxPG_LABEL, CLEAR_TYPES, CLEAR_VALUES, channels));
    }
    else if (prop_type == rttr::type::get<TextureType>())
    {
		const wxChar* TYPES[] = { wxT("2D"), wxT("Cube"), NULL };
		auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
		auto type = prop.get_value(node).get_value<TextureType>();
        type_prop->SetValue(static_cast<int>(type));
		m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<TextureWrapping>())
    {
        const wxChar* WRAPS[] = { wxT("Repeat"), wxT("MirroredRepeat"), wxT("ClampToEdge"), wxT("ClampToBorder"), NULL };
        auto wrap_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, WRAPS);
        auto wrap = prop.get_value(node).get_value<TextureWrapping>();
        wrap_prop->SetValue(static_cast<int>(wrap));
        m_pg->Append(wrap_prop);
    }
    else if (prop_type == rttr::type::get<TextureFiltering>())
    {
        const wxChar* FILTER[] = { wxT("Nearest"), wxT("Linear"), NULL };
        auto filter_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, FILTER);
        auto filter = prop.get_value(node).get_value<TextureFiltering>();
        filter_prop->SetValue(static_cast<int>(filter));
        m_pg->Append(filter_prop);
    }
    else if (prop_type == rttr::type::get<TextureFormat>())
    {
        const wxChar* FORMATS[] = { wxT("rgba16"), wxT("rgba8"), wxT("rgba4"), wxT("rgb"), wxT("rgb565"), wxT("a8"), wxT("depth"), NULL };
        auto format_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, FORMATS);
        auto format = prop.get_value(node).get_value<TextureType>();
        format_prop->SetValue(static_cast<int>(format));
        m_pg->Append(format_prop);
    }
    //else if (prop_type == rttr::type::get<VertexDataType>())
    //{
    //    const wxChar* TYPES[] = { wxT("Char"), wxT("Float"), NULL };
    //    auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
    //    auto type = prop.get_value(node).get_value<VertexDataType>();
    //    type_prop->SetValue(static_cast<int>(type));
    //    m_pg->Append(type_prop);
    //}
    else if (prop_type == rttr::type::get<VertexAttrib>())
    {
        auto va = prop.get_value(node).get_value<VertexAttrib>();
        wxPGProperty* va_prop = m_pg->Append(new wxStringProperty(ui_info.desc, wxPG_LABEL, wxT("<composed>")));
        va_prop->SetExpanded(false);
        m_pg->AppendIn(va_prop, new wxStringProperty(wxT("Name"), wxPG_LABEL, va.name));
        m_pg->AppendIn(va_prop, new wxUIntProperty(wxT("Num"), wxPG_LABEL, va.num));

        const wxChar* TYPES[] = { wxT("Char"), wxT("Float"), NULL };
        auto type_prop = new wxEnumProperty(wxT("Type"), wxPG_LABEL, TYPES);
        type_prop->SetValue(static_cast<int>(va.type));
        m_pg->AppendIn(va_prop, type_prop);
    }
    else if (prop_type == rttr::type::get<PrimitiveShapeType>())
    {
        const wxChar* TYPES[] = { wxT("Quad"), wxT("Cube"), NULL };
        auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
        auto type = prop.get_value(node).get_value<PrimitiveShapeType>();
        type_prop->SetValue(static_cast<int>(type));
        m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<PrimitiveVertLayout>())
    {
        const wxChar* TYPES[] = { wxT("Pos"), wxT("PosTex"), wxT("PosNormTex"), wxT("PosNormTexTB"), NULL };
        auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
        auto type = prop.get_value(node).get_value<PrimitiveVertLayout>();
        type_prop->SetValue(static_cast<int>(type));
        m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<AlphaTestFunc>())
    {
        const wxChar* FUNCS[] = { wxT("Off"), wxT("Never"), wxT("Less"), wxT("Equal"), wxT("LEqual"),
            wxT("Greater"), wxT("NotEqual"), wxT("GEqual"), wxT("Always"),NULL };
        auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, FUNCS);
        auto type = prop.get_value(node).get_value<AlphaTestFunc>();
        type_prop->SetValue(static_cast<int>(type));
        m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<BlendEqMode>())
    {
        const wxChar* MODES[] = { wxT("FuncAdd"), wxT("FuncSubtract"), wxT("FuncReverseSubtract"), wxT("Min"), wxT("Max"), NULL };
        auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, MODES);
        auto type = prop.get_value(node).get_value<BlendEqMode>();
        type_prop->SetValue(static_cast<int>(type));
        m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<BlendFuncFactor>())
    {
        const wxChar* FACTORS[] = { wxT("Off"), wxT("Zero"), wxT("One"), wxT("SrcColor"), wxT("OneMinusSrcColor"),
            wxT("DstColor"), wxT("OneMinusDstColor"), wxT("SrcAlpha"), wxT("OneMinusSrcAlpha"), wxT("DstAlpha"), wxT("OneMinusDstAlpha"), NULL };
        auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, FACTORS);
        auto type = prop.get_value(node).get_value<BlendFuncFactor>();
        type_prop->SetValue(static_cast<int>(type));
        m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<CullMode>())
    {
		const wxChar* TYPES[] = { wxT("Off"), wxT("Front"), wxT("Back"), wxT("FrontAndBack"), NULL };
		auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
		auto type = prop.get_value(node).get_value<CullMode>();
        type_prop->SetValue(static_cast<int>(type));
		m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<ZTestFunc>())
    {
		const wxChar* FUNCS[] = { wxT("Off"), wxT("Never"), wxT("Less"), wxT("Equal"), wxT("LEqual"),
            wxT("Greater"), wxT("NotEqual"), wxT("GEqual"), wxT("Always"), NULL };
		auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, FUNCS);
		auto type = prop.get_value(node).get_value<ZTestFunc>();
        type_prop->SetValue(static_cast<int>(type));
		m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<UserScriptRetType>())
    {
		const wxChar* TYPES[] = { wxT("Void"), wxT("Vec1Array"), wxT("Vec2Array"), wxT("Vec3Array"), wxT("Vec4Array"),  NULL };
		auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
		auto type = prop.get_value(node).get_value<UserScriptRetType>();
        type_prop->SetValue(static_cast<int>(type));
		m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<ShaderUniformType>())
    {
		const wxChar* TYPES[] = { wxT("Unknown"), wxT("Int"), wxT("Bool"), wxT("Vector1"), wxT("Vector2"), wxT("Vector3"), wxT("Vector4"),
            wxT("Matrix2"), wxT("Matrix3"), wxT("Matrix4"), wxT("Sampler2D"), wxT("SamplerCube"), NULL };
		auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
		auto type = prop.get_value(node).get_value<ShaderUniformType>();
        type_prop->SetValue(static_cast<int>(type));
		m_pg->Append(type_prop);
    }
    else
    {
        ret = false;
    }

    return ret;
}

bool WxNodeProperty::UpdateView(const rttr::property& prop, const wxPGProperty& wx_prop)
{
    bool ret = true;

    auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
    if (!ui_info_obj.is_valid()) {
        return false;
    }

    auto key  = wx_prop.GetName();
    wxAny val = wx_prop.GetValue();

    auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
    auto node_type = m_node->get_type();
    if (key == ui_info.desc &&
        ((node_type == rttr::type::get<bp::node::Input>() && ui_info.desc == bp::node::Input::STR_TYPE) ||
            (node_type == rttr::type::get<bp::node::Output>() && ui_info.desc == bp::node::Output::STR_TYPE)))
    {
        prop.set_value(m_node, PIN_IDX_TO_TYPE[wxANY_AS(val, int)]);
        return true;
    }

    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<ClearType>() && key == ui_info.desc)
    {
        prop.set_value(m_node, static_cast<ClearType>(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<TextureType>() && key == ui_info.desc)
    {
        prop.set_value(m_node, TextureType(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<TextureFormat>() && key == ui_info.desc)
    {
        prop.set_value(m_node, TextureFormat(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<TextureWrapping>() && key == ui_info.desc)
    {
        prop.set_value(m_node, TextureWrapping(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<TextureFiltering>() && key == ui_info.desc)
    {
        prop.set_value(m_node, TextureFiltering(wxANY_AS(val, int)));
    }
    //else if (prop_type == rttr::type::get<VertexDataType>() && key == ui_info.desc)
    //{
    //    prop.set_value(m_node, VertexDataType(wxANY_AS(val, int)));
    //}
    else if (prop_type == rttr::type::get<VertexAttrib>() && key == ui_info.desc)
    {
        std::vector<std::string> tokens;
        auto str = wxANY_AS(val, wxString).ToStdString();
        boost::split(tokens, str, boost::is_any_of(";"));
        assert(tokens.size() == 3);

        auto va = prop.get_value(m_node).get_value<VertexAttrib>();
        va.name = tokens[0];
        va.num  = std::stoi(tokens[1]);

        auto type_str = tokens[2];
        cpputil::StringHelper::ReplaceAll(type_str, " ", "");
        std::transform(type_str.begin(), type_str.end(), type_str.begin(), tolower);
        va.type = rttr::type::get<VertexDataType>().get_enumeration()
            .name_to_value(type_str).get_value<VertexDataType>();

        prop.set_value(m_node, va);
    }
    else if (prop_type == rttr::type::get<PrimitiveShapeType>() && key == ui_info.desc)
    {
        prop.set_value(m_node, PrimitiveShapeType(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<PrimitiveVertLayout>() && key == ui_info.desc)
    {
        prop.set_value(m_node, PrimitiveVertLayout(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<AlphaTestFunc>() && key == ui_info.desc)
    {
        prop.set_value(m_node, AlphaTestFunc(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<BlendEqMode>() && key == ui_info.desc)
    {
        prop.set_value(m_node, BlendEqMode(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<BlendFuncFactor>() && key == ui_info.desc)
    {
        prop.set_value(m_node, BlendFuncFactor(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<CullMode>() && key == ui_info.desc)
    {
        prop.set_value(m_node, CullMode(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<ZTestFunc>() && key == ui_info.desc)
    {
        prop.set_value(m_node, ZTestFunc(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<UserScriptRetType>() && key == ui_info.desc)
    {
        prop.set_value(m_node, UserScriptRetType(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<ShaderUniformType>() && key == ui_info.desc)
    {
        prop.set_value(m_node, ShaderUniformType(wxANY_AS(val, int)));
    }
    else
    {
        ret = false;
    }

    return ret;
}

}