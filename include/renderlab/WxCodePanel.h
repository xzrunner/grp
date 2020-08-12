#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <node0/typedef.h>

#include <wx/panel.h>

class wxNotebook;
class wxTextCtrl;

namespace ee0 { class WxCodeCtrl; }

namespace renderlab
{

class WxShaderPage;

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

	void OnSavePress(wxCommandEvent& event);
	void OnTextChange(wxCommandEvent& event);

	void ClearAllPagesTitle();

private:
	ee0::SubjectMgrPtr m_sub_mgr = nullptr;

	wxNotebook* m_notebook;

	WxShaderPage* m_vs_page = nullptr;
	WxShaderPage* m_fs_page = nullptr;
	ee0::WxCodeCtrl* m_default_page = nullptr;
	ee0::WxCodeCtrl* m_fx_page = nullptr;

	n0::SceneNodePtr m_selected = nullptr;

	wxTextCtrl* m_output_wnd = nullptr;

}; // WxCodePanel

}