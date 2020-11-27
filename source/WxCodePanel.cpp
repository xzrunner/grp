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
#include <rendergraph/node/Shader.h>
#include <unirender/Device.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/choice.h>

namespace renderlab
{

WxCodePanel::WxCodePanel(wxWindow* parent, const ur::Device& dev, WxGraphPage* stage)
	: wxPanel(parent)
	, m_dev(dev)
	, m_stage(stage)
{
	InitLayout();

	auto sub_mgr = m_stage->GetSubjectMgr();
	sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
	sub_mgr->RegisterObserver(bp::MSG_BP_CONN_REBUILD, this);

	wxAcceleratorEntry entries[1];
	entries[0].Set(wxACCEL_CTRL, (int)'S', wxID_SAVE);
	wxAcceleratorTable acc_tab(1, entries);
	SetAcceleratorTable(acc_tab);
}

WxCodePanel::~WxCodePanel()
{
	auto sub_mgr = m_stage->GetSubjectMgr();
	sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
	sub_mgr->UnregisterObserver(bp::MSG_BP_CONN_REBUILD, this);
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

	case bp::MSG_BP_CONN_REBUILD:
		OutputLogger();
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

	m_vs_page  = new WxShaderPage(m_notebook, "vs",  shadertrans::ShaderStage::VertexShader,   true);
	m_tcs_page = new WxShaderPage(m_notebook, "tcs", shadertrans::ShaderStage::TessCtrlShader, true);
	m_tes_page = new WxShaderPage(m_notebook, "tes", shadertrans::ShaderStage::TessEvalShader, true);
	m_fs_page  = new WxShaderPage(m_notebook, "fs",  shadertrans::ShaderStage::PixelShader,    true);
	m_cs_page  = new WxShaderPage(m_notebook, "cs",  shadertrans::ShaderStage::ComputeShader,  true);

	Connect(m_vs_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	Connect(m_tcs_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	Connect(m_tes_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	Connect(m_fs_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	Connect(m_cs_page->GetId(), wxEVT_STC_CHANGE,
		wxCommandEventHandler(WxCodePanel::OnTextChange));
	m_notebook->AddPage(m_vs_page, m_vs_page->GetName());
	m_notebook->AddPage(m_tcs_page, m_tcs_page->GetName());
	m_notebook->AddPage(m_tes_page, m_tes_page->GetName());
	m_notebook->AddPage(m_fs_page, m_fs_page->GetName());
	m_notebook->AddPage(m_cs_page, m_cs_page->GetName());

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

		m_notebook->AddPage(m_vs_page,  m_vs_page->GetName());
		m_notebook->AddPage(m_tcs_page, m_tcs_page->GetName());
		m_notebook->AddPage(m_tes_page, m_tes_page->GetName());
		m_notebook->AddPage(m_fs_page,  m_fs_page->GetName());
		m_notebook->AddPage(m_cs_page,  m_cs_page->GetName());

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

		m_vs_page->SetText(shader_node->GetCode(node::Shader::Stage::Vertex));
		m_tcs_page->SetText(shader_node->GetCode(node::Shader::Stage::TessCtrl));
		m_tes_page->SetText(shader_node->GetCode(node::Shader::Stage::TessEval));
		m_fs_page->SetText(shader_node->GetCode(node::Shader::Stage::Fragment));
		m_cs_page->SetText(shader_node->GetCode(node::Shader::Stage::Compute));
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
	else if (bp_type == rttr::type::get<node::ShaderGraph>())
	{
		m_notebook->AddPage(m_vs_page, m_vs_page->GetName());
		m_notebook->AddPage(m_fs_page, m_fs_page->GetName());

		auto sg_node = std::static_pointer_cast<node::ShaderGraph>(bp_node);
		m_vs_page->SetText(sg_node->m_vert);
		m_fs_page->SetText(sg_node->m_frag);
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
		if (!fx.empty())
		{
			auto type = static_cast<fxlang::EffectType>(m_fx_type->GetSelection());

			std::stringstream ss;
			const_cast<EffectBuilder&>(m_stage->GetEffectBuilder()).Build(fx, type, ss);
			m_output_wnd->SetValue(ss.str());

			m_stage->GetSubjectMgr()->NotifyObservers(bp::MSG_BP_CONN_REBUILD);
		}

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
			std::stringstream ss;
			if (!m_vs_page->IsShaderValid(m_lang_type->GetSelection() == 0, ss)) {
				m_output_wnd->SetValue(ss.str());
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetCode(node::Shader::Stage::Vertex, m_vs_page->GetText().ToStdString());
		}
		else if (idx == 1)
		{
			std::stringstream ss;
			if (!m_tcs_page->IsShaderValid(m_lang_type->GetSelection() == 0, ss)) {
				m_output_wnd->SetValue(ss.str());
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetCode(node::Shader::Stage::TessCtrl, m_tcs_page->GetText().ToStdString());
		}
		else if (idx == 2)
		{
			std::stringstream ss;
			if (!m_tes_page->IsShaderValid(m_lang_type->GetSelection() == 0, ss)) {
				m_output_wnd->SetValue(ss.str());
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetCode(node::Shader::Stage::TessEval, m_tes_page->GetText().ToStdString());
		}
		else if (idx == 3)
		{
			std::stringstream ss;
			if (!m_fs_page->IsShaderValid(m_lang_type->GetSelection() == 0, ss)) {
				m_output_wnd->SetValue(ss.str());
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetCode(node::Shader::Stage::Fragment, m_fs_page->GetText().ToStdString());
		}
		else if (idx == 4)
		{
			std::stringstream ss;
			if (!m_cs_page->IsShaderValid(m_lang_type->GetSelection() == 0, ss)) {
				m_output_wnd->SetValue(ss.str());
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetCode(node::Shader::Stage::Compute, m_cs_page->GetText().ToStdString());
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

	auto sub_mgr = m_stage->GetSubjectMgr();
	sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	ee0::MsgHelper::SendObjMsg(*sub_mgr, m_selected, bp::MSG_BP_NODE_PROP_CHANGED);

	OutputLogger();
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

void WxCodePanel::OutputLogger()
{
	auto& ss = static_cast<std::ostringstream&>(m_dev.GetLogger());

	//std::stringstream ss;
	//ss << static_cast<std::ostringstream&>(m_dev.GetLogger()).rdbuf();

	std::string err_log = ss.str();

	if (err_log.empty())
	{
		m_output_wnd->SetValue("");
	}
	else
	{
		std::cerr << err_log;

		m_output_wnd->SetValue(err_log);

		// clear ss
		ss.str("");
		ss.clear();
	}
}

}