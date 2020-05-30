#pragma once

#include "renderlab/Node.h"

#include <blueprint/BackendAdapter.h>

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

private:
    void InitInputsFromUniforms();

    static bool IsSameUniforms(const std::vector<bp::PinDesc>& v0,
        const std::vector<bp::PinDesc>& v1);

    static void GetCodeUniforms(const std::string& code,
        std::vector<bp::PinDesc>& uniforms,
        std::set<std::string>& names);

private:
    std::string m_vert;
    std::string m_frag;
    std::vector<bp::PinDesc> m_vert_uniforms;
    std::vector<bp::PinDesc> m_frag_uniforms;

    RTTR_ENABLE(Node)

}; // Shader

}
}