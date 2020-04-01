#pragma once

#include <blueprint/Node.h>

namespace renderlab
{

class Node : public bp::Node
{
public:
    Node(const std::string& title, bool preview);
    virtual ~Node();

    bool GetEnable() const { return m_enable; }
    void SetEnable(bool enable) { m_enable = enable; }

    bool GetPreview() const { return m_preview; }
    void SetPreview(bool preview) { m_preview = preview; }

protected:
    void InitPins(const std::string& name);

private:
    bool m_enable = true;
    bool m_preview = false;

    RTTR_ENABLE(bp::Node)

}; // Node

}