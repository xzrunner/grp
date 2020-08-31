#include "renderlab/node/Shader.h"
#include "renderlab/RenderAdapter.h"

#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>
#include <blueprint/BackendAdapter.h>

#include <rendergraph/node/ShaderInfo.h>

#include <iostream>

namespace renderlab
{
namespace node
{

Shader::Shader()
    : Node("Shader", false)
{
    InitPins("shader");

    SetGroup("Resource");
}

void Shader::SetVert(const std::string& vert)
{
    if (m_vert == vert) {
        return;
    }

    m_vert = vert;

    UpdateVertUniforms();
}

void Shader::SetFrag(const std::string& frag)
{
    if (m_frag == frag) {
        return;
    }

    m_frag = frag;

    UpdateFragUniforms();
}

void Shader::SetLanguage(rendergraph::node::Shader::Language lang) 
{ 
    if (m_lang == lang) {
        return;
    }

    m_lang = lang; 

    UpdateVertUniforms();
    UpdateFragUniforms();
}

void Shader::InitInputsFromUniforms()
{
    std::vector<bp::PinDesc> uniforms = m_vert_uniforms;
    std::copy(m_frag_uniforms.begin(), m_frag_uniforms.end(), std::back_inserter(uniforms));

    bool same = true;
    if (uniforms.size() == m_all_input.size() - 1) {
        for (int i = 0, n = uniforms.size(); i < n; ++i) {
            if (uniforms[i].type != m_all_input[1 + i]->GetOldType() ||
                uniforms[i].name != m_all_input[1 + i]->GetName()) {
                same = false;
                break;
            }
        }
    } else {
        same = false;
    }

    if (same) {
        return;
    }

    for (int i = 1, n = m_all_input.size(); i < n; ++i) {
        for (auto& c : m_all_input[i]->GetConnecting()) {
            bp::disconnect(c);
        }
    }
    m_all_input.clear();

    for (auto& desc : uniforms) {
        auto pin = std::make_shared<bp::Pin>(true, m_all_input.size(), desc.type, desc.name, *this);
        m_all_input.push_back(pin);
    }
    Layout();

    SetSizeChanged(true);
}

void Shader::UpdateVertUniforms()
{
    std::vector<bp::PinDesc> uniforms;

    std::set<std::string> names;
    for (auto& p : m_frag_uniforms) {
        names.insert(p.name);
    }

    try {
        GetCodeUniforms(shadertrans::ShaderStage::VertexShader, m_vert, m_lang, uniforms, names);
        if (!IsSameUniforms(uniforms, m_vert_uniforms)) {
            m_vert_uniforms = uniforms;
            InitInputsFromUniforms();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void Shader::UpdateFragUniforms()
{
    std::vector<bp::PinDesc> uniforms;

    std::set<std::string> names;
    for (auto& p : m_vert_uniforms) {
        names.insert(p.name);
    }

    try {
        GetCodeUniforms(shadertrans::ShaderStage::PixelShader, m_frag, m_lang, uniforms, names);
        if (!IsSameUniforms(uniforms, m_frag_uniforms)) {
            m_frag_uniforms = uniforms;
            InitInputsFromUniforms();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

bool Shader::IsSameUniforms(const std::vector<bp::PinDesc>& v0,
                            const std::vector<bp::PinDesc>& v1)
{
    if (v0.size() != v1.size()) {
        return false;
    }

    for (size_t i = 0, n = v0.size(); i < n; ++i)
    {
        if (v0[i].type != v1[i].type ||
            v0[i].name != v1[i].name) {
            return false;
        }
    }

    return true;
}

void Shader::GetCodeUniforms(shadertrans::ShaderStage stage, const std::string& code, rendergraph::node::Shader::Language lang,
                             std::vector<bp::PinDesc>& uniforms, std::set<std::string>& names)
{
    std::vector<rendergraph::Variable> rg_unifs;
    rendergraph::node::ShaderInfo::GetCodeUniforms(stage, code, lang, rg_unifs, names, std::cerr);
    for (auto& u : rg_unifs)
    {
        bp::PinDesc desc;

        desc.name = u.GetDisplayName();
        desc.type = RenderAdapter::TypeBackToFront(u.type, u.count);

        uniforms.push_back(desc);
    }
}

}
}