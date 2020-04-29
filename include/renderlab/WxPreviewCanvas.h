#pragma once

#include <ee3/WxStageCanvas.h>

#include <rendergraph/Variable.h>

namespace ee0 { class WxStagePage; }

namespace renderlab
{

class WxGraphPage;

class WxPreviewCanvas : public ee3::WxStageCanvas
{
public:
    WxPreviewCanvas(const ur::Device& dev, ee0::WxStagePage* stage,
        ECS_WORLD_PARAM const ee0::RenderContext& rc);
    virtual ~WxPreviewCanvas();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    void SetGraphPage(const WxGraphPage* graph_page) {
        m_graph_page = graph_page;
    }

protected:
    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;
    virtual void DrawForeground2D() const override;

private:
    const WxGraphPage* m_graph_page = nullptr;

}; // WxPreviewCanvas


}