#pragma once

#include <wx/textctrl.h>

namespace renderlab
{

class WxOutputWindow : public wxTextCtrl
{
public:
	WxOutputWindow(wxWindow* parent);

}; // WxOutputWindow

}