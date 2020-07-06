#include "renderlab/WxCodePanel.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/CustomFunction.h"
#include "renderlab/RegistNodes.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxCodeCtrl.h>
#include <ee0/GameObj.h>
#include <blueprint/CompNode.h>
#include <blueprint/Node.h>

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
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	m_notebook = new wxNotebook(this, wxID_ANY);
	for (int i = 0; i < PAGE_COUNT; ++i)
	{
		auto name = "page" + std::to_string(i);
		m_pages[i] = new ee0::WxCodeCtrl(m_notebook, name);
		m_notebook->AddPage(m_pages[i], name);
	}
	sizer->Add(m_notebook, 3, wxEXPAND);

	auto btn = new wxButton(this, wxID_ANY, "Run");
	//Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
	//	wxCommandEventHandler(WxTextPage::OnRunPress));
	sizer->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);

	SetSizer(sizer);
}

void WxCodePanel::OnSelectionInsert(const ee0::VariantSet& variants)
{
	auto var_obj = variants.GetVariant("obj");
	assert(var_obj.m_type == ee0::VT_PVOID);
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
	assert(GAME_OBJ_VALID(obj));

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
}

void WxCodePanel::OnSelectionClear(const ee0::VariantSet& variants)
{
	for (int i = 0; i < PAGE_COUNT; ++i) {
		m_pages[i]->SetText("");
	}
}

}