#include "renderlab/node/Shader.h"
#include "renderlab/RenderGraph.h"

#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>

#include <rendergraph/node/Shader.h>

namespace rlab
{
namespace node
{

void Shader::SetVert(const std::string& vert)
{
    if (m_vert == vert) {
        return;
    }

    m_vert = vert;

    std::vector<Node::PinDesc> uniforms;

    std::set<std::string> names;
    for (auto& p : m_frag_uniforms) {
        names.insert(p.name);
    }

    GetCodeUniforms(m_vert, uniforms, names);
    if (uniforms != m_vert_uniforms) {
        m_vert_uniforms = uniforms;
        InitInputsFromUniforms();
    }
}

void Shader::SetFrag(const std::string& frag)
{
    if (m_frag == frag) {
        return;
    }

    m_frag = frag;

    std::vector<Node::PinDesc> uniforms;

    std::set<std::string> names;
    for (auto& p : m_vert_uniforms) {
        names.insert(p.name);
    }

    GetCodeUniforms(m_frag, uniforms, names);
    if (uniforms != m_frag_uniforms) {
        m_frag_uniforms = uniforms;
        InitInputsFromUniforms();
    }
}

void Shader::InitInputsFromUniforms()
{
    std::vector<Node::PinDesc> uniforms = m_vert_uniforms;
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

void Shader::GetCodeUniforms(const std::string& code, std::vector<Node::PinDesc>& uniforms,
                             std::set<std::string>& names)
{
    std::vector<rg::Variable> rg_unifs;
    rg::node::Shader::GetCodeUniforms(code, rg_unifs, names);
    for (auto& u : rg_unifs)
    {
        Node::PinDesc desc;

        desc.name = u.GetDisplayName();
        desc.type = RenderGraph::TypeBackToFront(u.type, u.count);

        uniforms.push_back(desc);
    }
}

}
}