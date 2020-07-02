#include "renderlab/WxDefaultProperty.h"

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>

namespace
{

const char* STR_SAVE_CAMERA = "Save Camera";

}

namespace renderlab
{

WxDefaultProperty::WxDefaultProperty(wxWindow* parent)
	: wxPanel(parent)
{
	InitLayout();
	InitProps();
}

bool WxDefaultProperty::IsSaveCamEnable() const
{
	return m_save_cam_prop->GetValue();
}

void WxDefaultProperty::InitLayout()
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

	m_pg = new wxPropertyGrid(this, -1, wxDefaultPosition, wxSize(500, -1),
		wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED
	);
    Connect(m_pg->GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler(
        WxDefaultProperty::OnPropertyGridChanged));
	sizer->Add(m_pg, wxEXPAND);

	SetSizer(sizer);
}

void WxDefaultProperty::InitProps()
{
	m_save_cam_prop = new wxBoolProperty(STR_SAVE_CAMERA, wxPG_LABEL);
	m_save_cam_prop->SetValue(false);
	m_pg->Append(m_save_cam_prop);
	m_pg->SetPropertyAttribute(STR_SAVE_CAMERA, wxPG_BOOL_USE_CHECKBOX, true, wxPG_RECURSE);
}

void WxDefaultProperty::OnPropertyGridChanged(wxPropertyGridEvent& event)
{
    wxPGProperty* property = event.GetProperty();
    auto key = property->GetName();
    wxAny val = property->GetValue();

    //if (key == STR_SAVE_CAMERA)
    //{
    //    auto save = wxANY_AS(val, bool);
    //}
}

}