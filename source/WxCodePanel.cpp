#include "renderlab/WxCodePanel.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/CustomFunction.h"
#include "renderlab/RegistNodes.h"

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

namespace renderlab
{

WxCodePanel::WxCodePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
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
	for (int i = 0; i < PAGE_COUNT; ++i)
	{
		auto name = "page" + std::to_string(i);
		m_pages[i] = new ee0::WxCodeCtrl(m_notebook, name);

		Connect(m_pages[i]->GetId(), wxEVT_STC_CHANGE, 
			wxCommandEventHandler(WxCodePanel::OnTextChange));

		m_notebook->AddPage(m_pages[i], name);
	}
	sizer->Add(m_notebook, 3, wxEXPAND);

	SetSizer(sizer);
}

void WxCodePanel::OnSelectionInsert(const ee0::VariantSet& variants)
{
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
		auto shader_node = std::static_pointer_cast<node::Shader>(bp_node);
		m_pages[0]->SetText(shader_node->GetVert());
		m_pages[1]->SetText(shader_node->GetFrag());
	}
	else if (bp_type == rttr::type::get<node::CustomData>())
	{
		auto data_node = std::static_pointer_cast<node::CustomData>(bp_node);
		m_pages[0]->SetText(data_node->m_code);
	}
	else if (bp_type == rttr::type::get<node::CustomFunction>())
	{
		auto func_node = std::static_pointer_cast<node::CustomFunction>(bp_node);
		m_pages[0]->SetText(func_node->GetCode());
	}

	ClearAllPagesTitle();
}

void WxCodePanel::OnSelectionClear(const ee0::VariantSet& variants)
{
	for (int i = 0; i < PAGE_COUNT; ++i) {
		m_pages[i]->SetText("");
	}

	m_selected = nullptr;
	ClearAllPagesTitle();
}

void WxCodePanel::OnSavePress(wxCommandEvent& event)
{
	int idx = m_notebook->GetSelection();
	m_notebook->SetPageText(idx, "page" + std::to_string(idx));

	if (!m_selected) {
		return;
	}

	auto& cnode = m_selected->GetUniqueComp<bp::CompNode>();
	auto bp_node = cnode.GetNode();
	auto bp_type = bp_node->get_type();
	if (bp_type == rttr::type::get<node::Shader>())
	{
		auto shader_node = std::static_pointer_cast<node::Shader>(bp_node);
		if (idx == 0) {
			shader_node->SetVert(m_pages[0]->GetText().ToStdString());
		} else if (idx == 1) {
			shader_node->SetFrag(m_pages[1]->GetText().ToStdString());
		}
	}
	else if (bp_type == rttr::type::get<node::CustomData>())
	{
		if (idx == 0) {
			auto data_node = std::static_pointer_cast<node::CustomData>(bp_node);
			data_node->m_code = m_pages[0]->GetText().ToStdString();
		}
	}
	else if (bp_type == rttr::type::get<node::CustomFunction>())
	{
		if (idx == 0) {
			auto func_node = std::static_pointer_cast<node::CustomFunction>(bp_node);
			func_node->SetCode(m_pages[0]->GetText().ToStdString());
		}
	}

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	ee0::MsgHelper::SendObjMsg(*m_sub_mgr, m_selected, bp::MSG_BP_NODE_PROP_CHANGED);
}

void WxCodePanel::OnTextChange(wxCommandEvent& event)
{
	int idx = m_notebook->GetSelection();
	m_notebook->SetPageText(idx, "page" + std::to_string(idx) + "*");
}

void WxCodePanel::ClearAllPagesTitle()
{
	for (int i = 0; i < PAGE_COUNT; ++i) {
		m_notebook->SetPageText(i, "page" + std::to_string(i));
	}
}

}