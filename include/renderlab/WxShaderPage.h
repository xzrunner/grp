#pragma once

#include "ee0/WxCodeCtrl.h"

#include <shadertrans/ShaderValidator.h>

namespace renderlab
{

class WxShaderPage : public ee0::WxCodeCtrl
{
public:
	WxShaderPage(wxWindow* parent, const std::string& name,
		shadertrans::ShaderStage stage, bool glsl);

	bool IsShaderValid(bool is_glsl, std::string& msg) const;

private:
	shadertrans::ShaderValidator m_validator;

}; // WxShaderPage

}