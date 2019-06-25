#pragma once

#include "renderlab/Node.h"

namespace rlab
{
namespace node
{

class Shader : public Node
{
public:
    Shader()
        : Node("Shader", false)
    {
        InitPins("Shader");
    }

    auto& GetVert() const { return m_vert; }              
    void  SetVert(const std::string& vert);               
    auto& GetFrag() const { return m_frag; }              
    void  SetFrag(const std::string& frag);               

private:                                                  
    void InitInputsFromUniforms();                        

    static void GetCodeUniforms(const std::string& code,  
        std::vector<Node::PinDesc>& uniforms,             
        std::set<std::string>& names);                    

private:
    std::string m_vert;                                   
    std::string m_frag;                                   
    std::vector<Node::PinDesc> m_vert_uniforms;           
    std::vector<Node::PinDesc> m_frag_uniforms;           

    RTTR_ENABLE(Node)

}; // Shader

}
}