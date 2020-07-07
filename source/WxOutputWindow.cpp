#include "renderlab/WxOutputWindow.h"

namespace renderlab
{

WxOutputWindow::WxOutputWindow(wxWindow* parent)
	: wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE)
{
}

}