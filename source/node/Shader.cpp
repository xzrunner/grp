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

void Shader::SetCode(Stage stage, const std::string& code)
{
    int stage_idx = static_cast<int>(stage);

    if (m_codes[stage_idx] == code) {
        return;
    }

    m_codes[stage_idx] = code;

    UpdateUniforms(stage);
}

void Shader::SetLanguage(rendergraph::node::Shader::Language lang) 
{ 
    if (m_lang == lang) {
        return;
    }

    m_lang = lang; 

    for (int i = 0, n = static_cast<int>(Stage::StageMax); i < n; ++i) {
        UpdateUniforms(static_cast<Stage>(i));
    }
}

void Shader::InitInputsFromUniforms()
{    
    std::vector<bp::PinDesc> uniforms;
    for (auto& us : m_uniforms) {
        std::copy(us.begin(), us.end(), std::back_inserter(uniforms));
    }

    bool same = true;
    if (uniforms.size() == m_all_input.size() - rendergraph::node::Shader::I_MAX_NUM) {
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

    std::vector<std::shared_ptr<bp::Pin>> cache_old_pins;
    for (auto& p : m_all_input) {
        cache_old_pins.push_back(p);
    }

    m_all_input.erase(m_all_input.begin() + rendergraph::node::Shader::I_MAX_NUM, m_all_input.end());
    for (auto& desc : uniforms) 
    {
        std::shared_ptr<bp::Pin> pin = nullptr;

        for (auto& old : cache_old_pins)
        {
            if (old->GetName() == desc.name &&
                old->GetOldType() == desc.type) {
                pin = old;
                break;
            }
        }
        if (pin) {
            pin->SetPosIdx(m_all_input.size());
        } else {
            pin = std::make_shared<bp::Pin>(true, m_all_input.size(), desc.type, desc.name, *this);
        }

        m_all_input.push_back(pin);
    }

    // disconnect old pins
    for (auto& old : cache_old_pins) 
    {
        bool find = false;
        for (auto& _new : m_all_input) {
            if (_new == old) {
                find = true;
                break;
            }
        }
        if (!find) {
            for (auto& conn : old->GetConnecting()) {
                bp::disconnect(conn);
            }
        }
    }

    Layout();

    SetSizeChanged(true);
}

void Shader::UpdateUniforms(Stage stage)
{
    const shadertrans::ShaderStage st_stages[] = {
        shadertrans::ShaderStage::VertexShader,
        shadertrans::ShaderStage::TessCtrlShader,
        shadertrans::ShaderStage::TessEvalShader,
        shadertrans::ShaderStage::PixelShader,
        shadertrans::ShaderStage::ComputeShader
    };
    int stage_idx = static_cast<int>(stage);

    std::vector<bp::PinDesc> uniforms;
    std::set<std::string> names;

    try {
        GetCodeUniforms(st_stages[stage_idx], m_codes[stage_idx], m_lang, uniforms, names);
        if (!IsSameUniforms(uniforms, m_uniforms[stage_idx])) {
            m_uniforms[stage_idx] = uniforms;
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