#pragma once

#include <blueprint/WxNodeProperty.h>

namespace rlab
{

class WxNodeProperty : public bp::WxNodeProperty
{
public:
	WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);

protected:
    virtual bool InitView(const rttr::property& prop, const bp::NodePtr& node) override;
    virtual bool UpdateView(const rttr::property& prop, const wxPGProperty& wx_prop) override;

}; // WxNodeProperty

}