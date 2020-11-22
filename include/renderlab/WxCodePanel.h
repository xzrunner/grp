#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <node0/typedef.h>

#include <wx/panel.h>

class wxNotebook;
class wxTextCtrl;
class wxChoice;
class wxBookCtrlEvent;

namespace ee0 { class WxCodeCtrl; }

namespace renderlab
{

class WxShaderPage;
class WxGraphPage;
class EffectBuilder;

class WxCodePanel : public wxPanel, public ee0::Observer
{
public:
	WxCodePanel(wxWindow* parent, const WxGraphPage& stage);
	virtual ~WxCodePanel();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout();
	
	void OnPageChanged(wxBookCtrlEvent& event);
	void OnPageChanging(wxBookCtrlEvent& event);

	void OnSelectionInsert(const ee0::VariantSet& variants);
	void OnSelectionClear(const ee0::VariantSet& variants);

	void OnSavePress(wxCommandEvent& event);
	void OnTextChange(wxCommandEvent& event);

	void ClearAllPagesTitle();
	void ClearAllPages();

	void OnShaderTypeChanged(wxCommandEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr = nullptr;

	EffectBuilder& m_fx_builder;

	wxChoice* m_lang_type = nullptr;
	wxChoice* m_fx_type   = nullptr;

	wxNotebook* m_notebook;

	WxShaderPage*    m_vs_page      = nullptr;
	WxShaderPage*    m_fs_page      = nullptr;
	WxShaderPage*    m_tcs_page     = nullptr;
	WxShaderPage*    m_tes_page     = nullptr;
	ee0::WxCodeCtrl* m_default_page = nullptr;
	ee0::WxCodeCtrl* m_fx_page      = nullptr;

	n0::SceneNodePtr m_selected = nullptr;

	wxTextCtrl* m_output_wnd = nullptr;

}; // WxCodePanel

}