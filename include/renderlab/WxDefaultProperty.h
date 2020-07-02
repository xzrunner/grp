#pragma once

#include <wx/panel.h>

class wxPropertyGrid;
class wxPropertyGridEvent;
class wxBoolProperty;

namespace renderlab
{

class WxDefaultProperty : public wxPanel
{
public:
    WxDefaultProperty(wxWindow* parent);

	bool IsSaveCamEnable() const;

private:
    void InitLayout();
    void InitProps();

    void OnPropertyGridChanged(wxPropertyGridEvent& event);

private:
    wxPropertyGrid* m_pg;

	wxBoolProperty* m_save_cam_prop = nullptr;

}; // WxDefaultProperty

}