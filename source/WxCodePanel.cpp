#include "renderlab/WxCodePanel.h"
#include "renderlab/node/Shader.h"
#include "renderlab/node/CustomFunction.h"
#include "renderlab/RegistNodes.h"
#include "renderlab/WxOutputWindow.h"

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

namespace glslang {

const TBuiltInResource DefaultTBuiltInResource = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,
    ///* .maxDualSourceDrawBuffersEXT = */ 1,

    /* .limits = */ {
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
    }
};

}

namespace renderlab
{

WxCodePanel::WxCodePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
	, m_vert_c(EShLanguage::EShLangVertex)
	, m_frag_c(EShLanguage::EShLangFragment)
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
		auto page = new ee0::WxCodeCtrl(m_notebook, name);
		page->InitializePrefs("GLSL");
		page->SetReadOnly(false);
		m_pages[i] = page;

		Connect(m_pages[i]->GetId(), wxEVT_STC_CHANGE, 
			wxCommandEventHandler(WxCodePanel::OnTextChange));

		m_notebook->AddPage(m_pages[i], name);
	}
	sizer->Add(m_notebook, 3, wxEXPAND);

	m_output_wnd = new WxOutputWindow(this);
	sizer->Add(m_output_wnd, 2, wxEXPAND);

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
		if (idx == 0) 
		{
			std::string code = m_pages[0]->GetText().ToStdString();
			std::string msg;
			if (!m_vert_c.Validate(code, msg)) {
				m_output_wnd->SetValue(msg);
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetVert(code);
		} 
		else if (idx == 1) 
		{
			std::string code = m_pages[1]->GetText().ToStdString();
			std::string msg;
			if (!m_frag_c.Validate(code, msg)) {
				m_output_wnd->SetValue(msg);
			} else {
				m_output_wnd->SetValue("");
			}
			shader_node->SetFrag(code);
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

//////////////////////////////////////////////////////////////////////////
// class WxCodePanel::ShaderCompiler
//////////////////////////////////////////////////////////////////////////

WxCodePanel::ShaderCompiler::ShaderCompiler(EShLanguage lang)
{
	ShInitialize();
	ShInitialize();  // also test reference counting of users
	ShFinalize();    // also test reference counting of users

    m_compiler = ShConstructCompiler(lang, 0);
}

WxCodePanel::ShaderCompiler::~ShaderCompiler()
{
	ShDestruct(m_compiler);

	ShFinalize();
}


bool WxCodePanel::ShaderCompiler::Validate(const std::string& code, std::string& msg)
{
	const char* shader_str = code.c_str();
	int ret = ShCompile(m_compiler, &shader_str, 1, nullptr, EShOptNone, 
		&glslang::DefaultTBuiltInResource, 0, 100, false, EShMsgDefault);

	msg = ShGetInfoLog(m_compiler);

	return ret != 0;
}

}