#pragma once

#include "renderlab/Node.h"

#include <unirender/ShaderType.h>
#include <blueprint/BackendAdapter.h>

#include <rendergraph/node/Shader.h>
#include <shadertrans/ShaderStage.h>

namespace renderlab
{
namespace node
{

class Shader : public Node
{
public:
    enum class Stage
    {
        Vertex,
        TessCtrl,
        TessEval,
        Fragment,

        Compute,

        StageMax
    };

public:
    Shader();

    auto& GetCode(Stage stage) const { return m_codes[static_cast<int>(stage)]; }
    void  SetCode(Stage stage, const std::string& code);

    auto& GetUniforms() const { return m_uniforms; }
    auto& GetUniforms(Stage stage) const { return m_uniforms[static_cast<int>(stage)]; }

    auto GetLanguage() const { return m_lang; }
    void SetLanguage(rendergraph::node::Shader::Language lang);

    // for rtti
    const std::string& GetVert() const { return GetCode(Stage::Vertex); }
    const std::string& GetTessCtrl() const { return GetCode(Stage::TessCtrl); }
    const std::string& GetTessEval() const { return GetCode(Stage::TessEval); }
    const std::string& GetFrag() const { return GetCode(Stage::Fragment); }
    const std::string& GetCompute() const { return GetCode(Stage::Compute); }
    void SetVert(const std::string& code) { SetCode(Stage::Vertex, code); }
    void SetTessCtrl(const std::string& code) { SetCode(Stage::TessCtrl, code); }
    void SetTessEval(const std::string& code) { SetCode(Stage::TessEval, code); }
    void SetFrag(const std::string& code) { SetCode(Stage::Fragment, code); }
    void SetCompute(const std::string& code) { SetCode(Stage::Compute, code); }

private:
    void InitInputsFromUniforms();

    void UpdateUniforms(Stage stage);

    static bool IsSameUniforms(const std::vector<bp::PinDesc>& v0,
        const std::vector<bp::PinDesc>& v1);

private:
    rendergraph::node::Shader::Language m_lang = rendergraph::node::Shader::Language::GLSL;

    std::string m_codes[static_cast<int>(Stage::StageMax)];
    std::vector<bp::PinDesc> m_uniforms[static_cast<int>(Stage::StageMax)];

    RTTR_ENABLE(Node)

}; // Shader

}
}