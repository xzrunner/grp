#include "renderlab/WxShaderPage.h"

namespace renderlab
{

WxShaderPage::WxShaderPage(wxWindow* parent, const std::string& name, 
                           shadertrans::ShaderStage stage, bool glsl)
	: ee0::WxCodeCtrl(parent, name)
	, m_validator(stage)
{
    InitializePrefs("GLSL");
    SetReadOnly(false);
}

bool WxShaderPage::IsShaderValid(bool is_glsl, std::string& msg) const
{
    std::string code = GetText().ToStdString();
    return m_validator.Validate(code, is_glsl, msg);
}

}