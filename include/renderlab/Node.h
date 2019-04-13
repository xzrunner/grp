#pragma once

#include <blueprint/Node.h>

#include <rendergraph/typedef.h>

namespace rlab
{

class NodePreview;

class Node : public bp::Node
{
public:
    Node(const std::string& title, bool preview);
    virtual ~Node();

    virtual void Draw(const n2::RenderParams& rp) const override;
    virtual bool Update(const bp::UpdateParams& params) override;

    virtual void PreviewDraw(const rg::NodePtr& node, const sm::Matrix2D& mat) const {}

    auto& GetRGNode() const { return m_rg_node; }
    void SetRGNode(const rg::NodePtr& node) { m_rg_node = node; }

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
    std::unique_ptr<NodePreview> m_preview = nullptr;

    rg::NodePtr m_rg_node = nullptr;

    RTTR_ENABLE(bp::Node)

}; // Node

}