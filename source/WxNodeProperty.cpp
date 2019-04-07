#include "renderlab/WxNodeProperty.h"
#include "renderlab/ReflectPropTypes.h"

#include <ee0/SubjectMgr.h>
#include <ee0/ReflectPropTypes.h>
#include <ee0/MessageID.h>
#include <ee0/WxPropHelper.h>
#include <blueprint/Node.h>
#include <blueprint/MessageID.h>

#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>

namespace rlab
{

WxNodeProperty::WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
{
	InitLayout();
}

void WxNodeProperty::LoadFromNode(const n0::SceneNodePtr& obj, const bp::NodePtr& node)
{
    m_obj = obj;
	m_node = node;

	m_pg->Clear();

	auto node_type = node->get_type();
	for (auto& prop : node_type.get_properties())
	{
        auto name = prop.get_name().to_string();

		auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
		if (!ui_info_obj.is_valid()) {
			continue;
		}

		auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
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
        else if (prop_type == rttr::type::get<TextureFormat>())
        {
            const wxChar* FORMATS[] = { wxT("rgba8"), wxT("rgba4"), wxT("rgb"), wxT("rgb565"), wxT("a8"), NULL };
            auto format_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, FORMATS);
            auto format = prop.get_value(node).get_value<TextureType>();
            format_prop->SetValue(static_cast<int>(format));
            m_pg->Append(format_prop);
        }
        else
        {
            ee0::WxPropHelper::CreateProp(m_pg, ui_info, node, prop, [&](const std::string& filepath)
            {
                m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
                m_sub_mgr->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
            });
        }
	}
}

void WxNodeProperty::InitLayout()
{
	auto sizer = new wxBoxSizer(wxVERTICAL);

	m_pg = new wxPropertyGrid(this, -1, wxDefaultPosition, wxSize(300, 600),
		wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED
	);
    Connect(m_pg->GetId(), wxEVT_PG_CHANGING, wxPropertyGridEventHandler(
        WxNodeProperty::OnPropertyGridChanging));
	Connect(m_pg->GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler(
		WxNodeProperty::OnPropertyGridChanged));
	sizer->Add(m_pg, 1, wxEXPAND);

	SetSizer(sizer);
}

void WxNodeProperty::OnPropertyGridChanging(wxPropertyGridEvent& event)
{
    if (!m_node) {
        return;
    }

    wxPGProperty* property = event.GetProperty();
    auto key = property->GetName();
    wxAny val = property->GetValue();

    auto node_type = m_node->get_type();

    for (auto& prop : node_type.get_properties())
    {
        auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
        if (!ui_info_obj.is_valid()) {
            continue;
        }
        auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
        if (prop.get_metadata(ee0::PropOpenFileTag()).is_valid() && key == ui_info.desc) {
            event.Veto();
            continue;
        }
    }
}

void WxNodeProperty::OnPropertyGridChanged(wxPropertyGridEvent& event)
{
	if (!m_node) {
		return;
	}

	wxPGProperty* property = event.GetProperty();
	auto key = property->GetName();
	wxAny val = property->GetValue();

	auto node_type = m_node->get_type();
	for (auto& prop : node_type.get_properties())
	{
        if (prop.get_metadata(ee0::PropOpenFileTag()).is_valid()) {
            continue;
        }
		auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
		if (!ui_info_obj.is_valid()) {
			continue;
		}
		auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
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
        else
        {
            ee0::WxPropHelper::UpdateProp(key, val, ui_info, m_node, prop);
        }
	}

	m_node->Refresh();

    // update aabb
    auto& st = m_node->GetStyle();
    m_obj->GetUniqueComp<n2::CompBoundingBox>().SetSize(
        *m_obj, sm::rect(st.width, st.height)
    );

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	m_sub_mgr->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
}

}