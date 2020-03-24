#pragma once

#include <blueprint/Node.h>

namespace renderlab
{

class Node : public bp::Node
{
public:
    Node(const std::string& title, bool preview);
    virtual ~Node();

    virtual void Draw(const n2::RenderParams& rp) const override;

    bool GetEnable() const { return m_enable; }
    void SetEnable(bool enable) { m_enable = enable; }

    bool GetPreview() const { return m_preview; }
    void SetPreview(bool preview) { m_preview = preview; }

    struct PinDesc
    {
        bool operator == (const PinDesc& desc) const {
            return type == desc.type && name == desc.name;
        }

        int         type;
        std::string name;
    };

protected:
    void InitPins(const std::vector<PinDesc>& input,
        const std::vector<PinDesc>& output);
    void InitPins(const std::string& name);

private:
    void InitPinsImpl(const std::vector<PinDesc>& pins,
        bool is_input);

private:
    bool m_enable = true;
    bool m_preview = false;

    RTTR_ENABLE(bp::Node)

}; // Node

}