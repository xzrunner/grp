#include "renderlab/WxCodePanel.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/CustomFunction.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/WxOutputWindow.h"
#include "renderlab/WxShaderPage.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxCodeCtrl.h>
#include <ee0/GameObj.h>
#include <ee0/MsgHelper.h>
#include <blueprint/CompNode.h>
#include <blueprint/Node.h>
#include <blueprint/MessageID.h>

#include <node0/SceneNode.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/notebook.h>

namespace renderlab
{

WxCodePanel::WxCodePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
	//, m_vert_c(EShLanguage::EShLangVertex)
	//, m_frag_c(EShLanguage::EShLangFragment)
{
	InitLayout();

	m_sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	m_sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);

	wxAcceleratorEntry entries[1];
	entries[0].Set(wxACCEL_CTRL, (int)'S', wxID_SAVE);
	wxAcceleratorTable acc_tab(1, entries);
	SetAcceleratorTable(acc_tab);
}

WxCodePanel::~WxCodePanel()
{
	m_sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	m_sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
}

void WxCodePanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelectionInsert(variants);
		break;
    case ee0::MSG_NODE_SELECTION_CLEAR:
		OnSelectionClear(variants);
        break;
	}
}

void WxCodePanel::InitLayout()
{
	wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	auto btn = new wxButton(this, wxID_ANY, "Save");
	Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(WxCodePanel::OnSavePress));
	sizer->Add(btn);

	m_notebook = new wxNotebook(this, wxID_ANY);

	m_vs_page = new WxShaderPage(m_notebook, "vs", EShLanguage::EShLangVertex);
	m_fs_page = new WxShaderPage(m_notebook, "fs", EShLanguage::EShLangFragment);

	Connect(m_vs_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	Connect(m_fs_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	m_notebook->AddPage(m_vs_page, m_vs_page->GetName());
	m_notebook->AddPage(m_fs_page, m_fs_page->GetName());

	m_other_page = new ee0::WxCodeCtrl(m_notebook, "default");
	Connect(m_other_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	m_notebook->AddPage(m_other_page, m_other_page->GetName());

	sizer->Add(m_notebook, 3, wxEXPAND);

	m_output_wnd = new WxOutputWindow(this);
	sizer->Add(m_output_wnd, 2, wxEXPAND);

	SetSizer(sizer);
}

void WxCodePanel::OnSelectionInsert(const ee0::VariantSet& variants)
{
	m_vs_page->Hide();
	m_fs_page->Hide();
	m_other_page->Hide();

	auto var_obj = variants.GetVariant("obj");
	assert(var_obj.m_type == ee0::VT_PVOID);
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
	assert(GAME_OBJ_VALID(obj));
	m_selected = obj;

	auto& cnode = obj->GetUniqueComp<bp::CompNode>();
	auto bp_node = cnode.GetNode();
	auto bp_type = bp_node->get_type();
	if (bp_type == rttr::type::get<node::Shader>())
	{
		m_vs_page->Show();
		m_fs_page->Show();

		auto shader_node = std::static_pointer_cast<node::Shader>(bp_node);
		m_vs_page->SetText(shader_node->GetVert());
		m_fs_page->SetText(shader_node->GetFrag());
	}
	else if (bp_type == rttr::type::get<node::CustomData>())
	{
		m_other_page->Show();

		auto data_node = std::static_pointer_cast<node::CustomData>(bp_node);
		m_other_page->SetText(data_node->m_code);
	}
	else if (bp_type == rttr::type::get<node::CustomFunction>())
	{
		m_other_page->Show();

		auto func_node = std::static_pointer_cast<node::CustomFunction>(bp_node);
		m_other_page->SetText(func_node->GetCode());
	}

	ClearAllPagesTitle();
}

void WxCodePanel::OnSelectionClear(const ee0::VariantSet& variants)
{
	m_vs_page->SetText("");
	m_fs_page->SetText("");
	m_other_page->SetText("");

	m_selected = nullptr;
	ClearAllPagesTitle();
}

void WxCodePanel::OnSavePress(wxCommandEvent& event)
{
	int idx = m_notebook->GetSelection();
	auto title = m_notebook->GetPageText(idx).ToStdString();
	if (!title.empty() && title.back() == '*')
	{
		title.pop_back();
		m_notebook->SetPageText(idx, title);
	}

	if (!m_selected) {
		return;
	}

	auto& cnode = m_selected->GetUniqueComp<bp::CompNode>();
	auto bp_node = cnode.GetNode();
	auto bp_type = bp_node->get_type();
	if (bp_type == rttr::type::get<node::Shader>())
	{
		auto shader_node = std::static_pointer_cast<node::Shader>(bp_node);
		if (idx == 0) 
		{
			std::string msg;
			if (!m_vs_page->IsShaderValid(msg)) {
				m_output_wnd->SetValue(msg);
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetVert(m_vs_page->GetText().ToStdString());
		}
		else if (idx == 1) 
		{
			std::string msg;
			if (!m_fs_page->IsShaderValid(msg)) {
				m_output_wnd->SetValue(msg);
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetFrag(m_fs_page->GetText().ToStdString());
		}
	}
	else if (bp_type == rttr::type::get<node::CustomData>())
	{
		if (idx == 0) {
			auto data_node = std::static_pointer_cast<node::CustomData>(bp_node);
			data_node->m_code = m_other_page->GetText().ToStdString();
		}
	}
	else if (bp_type == rttr::type::get<node::CustomFunction>())
	{
		if (idx == 0) {
			auto func_node = std::static_pointer_cast<node::CustomFunction>(bp_node);
			func_node->SetCode(m_other_page->GetText().ToStdString());
		}
	}

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	ee0::MsgHelper::SendObjMsg(*m_sub_mgr, m_selected, bp::MSG_BP_NODE_PROP_CHANGED);
}

void WxCodePanel::OnTextChange(wxCommandEvent& event)
{
	int idx = m_notebook->GetSelection();
	auto title = m_notebook->GetPageText(idx).ToStdString();
	if (!title.empty() && title.back() != '*') {
		title.append("*");
		m_notebook->SetPageText(idx, title);
	}
}

void WxCodePanel::ClearAllPagesTitle()
{
	for (int i = 0, n = m_notebook->GetPageCount(); i < n; ++i) 
	{
		auto title = m_notebook->GetPageText(i).ToStdString();
		if (!title.empty() && title.back() == '*') 
		{
			title.pop_back();
			m_notebook->SetPageText(i, title);
		}
	}
}

}