#pragma once

#include "ee0/WxCodeCtrl.h"

#include <glslang/Public/ShaderLang.h>

namespace renderlab
{

class WxShaderPage : public ee0::WxCodeCtrl
{
public:
	WxShaderPage(wxWindow* parent, const std::string& name,
		EShLanguage lang);

	bool IsShaderValid(std::string& msg) const;

private:
	class ShaderCompiler
	{
	public:
		ShaderCompiler(EShLanguage lang);
		~ShaderCompiler();

		bool Validate(const std::string& code, std::string& msg) const;

	private:
		ShHandle m_compiler = nullptr;

	}; // ShaderCompiler

private:
	ShaderCompiler m_compiler;

}; // WxShaderPage

}