#pragma once

#include "renderlab/Node.h"

#include <unirender/ShaderType.h>
#include <blueprint/BackendAdapter.h>

#include <rendergraph/node/Shader.h>

namespace renderlab
{
namespace node
{

class Shader : public Node
{
public:
    Shader()
        : Node("Shader", false)
    {
        InitPins("shader");

        SetGroup("Resource");
    }

    auto& GetVert() const { return m_vert; }
    void  SetVert(const std::string& vert);
    auto& GetFrag() const { return m_frag; }
    void  SetFrag(const std::string& frag);

    auto GetLanguage() const { return m_lang; }
    void SetLanguage(rendergraph::node::Shader::Language lang);

private:
    void InitInputsFromUniforms();

    void UpdateVertUniforms();
    void UpdateFragUniforms();

    static bool IsSameUniforms(const std::vector<bp::PinDesc>& v0,
        const std::vector<bp::PinDesc>& v1);

    static void GetCodeUniforms(ur::ShaderType stage, const std::string& code, 
        rendergraph::node::Shader::Language lang, std::vector<bp::PinDesc>& uniforms, std::set<std::string>& names);

private:
    rendergraph::node::Shader::Language m_lang = rendergraph::node::Shader::Language::GLSL;

    std::string m_vert;
    std::string m_frag;
    std::vector<bp::PinDesc> m_vert_uniforms;
    std::vector<bp::PinDesc> m_frag_uniforms;

    RTTR_ENABLE(Node)

}; // Shader

}
}