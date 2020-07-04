#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>
#include <wx/notebook.h>

namespace ee0 { class WxCodeCtrl; }

namespace renderlab
{

class WxCodePanel : public wxPanel, public ee0::Observer
{
public:
	WxCodePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);
	virtual ~WxCodePanel();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout();

	void OnSelectionInsert(const ee0::VariantSet& variants);
	void OnSelectionClear(const ee0::VariantSet& variants);

private:
	ee0::SubjectMgrPtr m_sub_mgr = nullptr;

	wxNotebook* m_notebook;

	static const int PAGE_COUNT = 2;
	ee0::WxCodeCtrl* m_pages[PAGE_COUNT];

}; // WxCodePanel

}