#include "renderlab/WxCodePanel.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/CustomFunction.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/WxOutputWindow.h"
#include "renderlab/WxShaderPage.h"
#include "renderlab/EffectBuilder.h"
#include "renderlab/WxGraphPage.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxCodeCtrl.h>
#include <ee0/GameObj.h>
#include <ee0/MsgHelper.h>
#include <blueprint/CompNode.h>
#include <blueprint/Node.h>
#include <blueprint/MessageID.h>

#include <node0/SceneNode.h>
#include <fxlang/EffectParser.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/choice.h>

namespace renderlab
{

WxCodePanel::WxCodePanel(wxWindow* parent, const WxGraphPage& stage)
	: wxPanel(parent)
	, m_sub_mgr(stage.GetSubjectMgr())
	, m_fx_builder(const_cast<EffectBuilder&>(stage.GetEffectBuilder()))
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

	// left: UI

	wxSizer* ui_sizer = new wxBoxSizer(wxVERTICAL);

	auto btn = new wxButton(this, wxID_ANY, "Save");
	Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(WxCodePanel::OnSavePress));
	ui_sizer->Add(btn);

	ui_sizer->AddSpacer(10);

	wxArrayString shader_labels;
	shader_labels.push_back("GLSL");
	shader_labels.push_back("HLSL");
	m_lang_type = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, shader_labels);
	Connect(m_lang_type->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
		wxCommandEventHandler(WxCodePanel::OnShaderTypeChanged));
	m_lang_type->SetSelection(0);
	m_lang_type->SetName("shader");
	ui_sizer->Add(m_lang_type);
	m_lang_type->Hide();

	wxArrayString fx_labels;
	fx_labels.push_back("default");
	fx_labels.push_back("cg&dx");
	fx_labels.push_back("reshade");
	m_fx_type = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, fx_labels);
	m_fx_type->SetSelection(0);
	m_fx_type->SetName("fx");
	ui_sizer->Add(m_fx_type);
	m_fx_type->Hide();

	sizer->Add(ui_sizer);

	// center: pages

	m_notebook = new wxNotebook(this, wxID_ANY);
	Connect(m_notebook->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
		wxBookCtrlEventHandler(WxCodePanel::OnPageChanged));
	Connect(m_notebook->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,
		wxBookCtrlEventHandler(WxCodePanel::OnPageChanging));

	m_vs_page = new WxShaderPage(m_notebook, "vs", EShLanguage::EShLangVertex);
	m_fs_page = new WxShaderPage(m_notebook, "fs", EShLanguage::EShLangFragment);

	Connect(m_vs_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	Connect(m_fs_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	m_notebook->AddPage(m_vs_page, m_vs_page->GetName());
	m_notebook->AddPage(m_fs_page, m_fs_page->GetName());

	m_default_page = new ee0::WxCodeCtrl(m_notebook, "default");
	Connect(m_default_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	m_notebook->AddPage(m_default_page, m_default_page->GetName());

	m_fx_page = new ee0::WxCodeCtrl(m_notebook, "fx");
	Connect(m_fx_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	m_notebook->AddPage(m_fx_page, m_fx_page->GetName());

	sizer->Add(m_notebook, 3, wxEXPAND);

	ClearAllPages();

	// right: output

	m_output_wnd = new WxOutputWindow(this);
	sizer->Add(m_output_wnd, 2, wxEXPAND);

	SetSizer(sizer);
}

void WxCodePanel::OnPageChanged(wxBookCtrlEvent& event)
{
	//if (event.GetSelection() == static_cast<int>(PageType::FX)) {
	//	m_fx_type->Show();
	//} else {
	//	m_fx_type->Hide();
	//}
}

void WxCodePanel::OnPageChanging(wxBookCtrlEvent& event)
{
}

void WxCodePanel::OnSelectionInsert(const ee0::VariantSet& variants)
{
	ClearAllPages();
	m_lang_type->Hide();
	m_fx_type->Hide();

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
		m_lang_type->Show();
		Layout();

		m_notebook->AddPage(m_vs_page, m_vs_page->GetName());
		m_notebook->AddPage(m_fs_page, m_fs_page->GetName());

		auto shader_node = std::static_pointer_cast<node::Shader>(bp_node);

		switch (shader_node->GetLanguage())
		{
		case rendergraph::node::Shader::Language::GLSL:
			m_lang_type->SetSelection(0);
			break;
		case rendergraph::node::Shader::Language::HLSL:
			m_lang_type->SetSelection(1);
			break;
		default:
			assert(0);
		}

		m_vs_page->SetText(shader_node->GetVert());
		m_fs_page->SetText(shader_node->GetFrag());
	}
	else if (bp_type == rttr::type::get<node::CustomData>())
	{
		m_notebook->AddPage(m_default_page, m_default_page->GetName());

		auto data_node = std::static_pointer_cast<node::CustomData>(bp_node);
		m_default_page->SetText(data_node->m_code);
	}
	else if (bp_type == rttr::type::get<node::CustomFunction>())
	{
		m_notebook->AddPage(m_default_page, m_default_page->GetName());

		auto func_node = std::static_pointer_cast<node::CustomFunction>(bp_node);
		m_default_page->SetText(func_node->GetCode());
	}

	ClearAllPagesTitle();
}

void WxCodePanel::OnSelectionClear(const ee0::VariantSet& variants)
{ 
	ClearAllPages();

	m_notebook->AddPage(m_fx_page, m_fx_page->GetName());

	m_lang_type->Hide();
	m_fx_type->Show();
	Layout();

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

	if (!m_selected) 
	{
		auto fx = m_fx_page->GetText().ToStdString();
		auto type = static_cast<fxlang::EffectType>(m_fx_type->GetSelection());
		m_fx_builder.Build(fx, type);

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
		auto data_node = std::static_pointer_cast<node::CustomData>(bp_node);
		data_node->m_code = m_default_page->GetText().ToStdString();
	}
	else if (bp_type == rttr::type::get<node::CustomFunction>())
	{
		auto func_node = std::static_pointer_cast<node::CustomFunction>(bp_node);
		func_node->SetCode(m_default_page->GetText().ToStdString());
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

void WxCodePanel::ClearAllPages()
{
	while (m_notebook->GetPageCount() != 0) {
		m_notebook->RemovePage(0);
	}
}

void WxCodePanel::OnShaderTypeChanged(wxCommandEvent& event)
{
	assert(m_selected);

	auto& cnode = m_selected->GetUniqueComp<bp::CompNode>();
	auto bp_node = cnode.GetNode();
	auto bp_type = bp_node->get_type();
	assert(bp_type == rttr::type::get<node::Shader>());
	auto shader_node = std::static_pointer_cast<node::Shader>(bp_node);
	shader_node->SetLanguage(static_cast<rendergraph::node::Shader::Language>(m_lang_type->GetSelection()));
}

}