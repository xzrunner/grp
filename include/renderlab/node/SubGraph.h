#pragma once

#include <blueprint/node/SubGraph.h>

#include <rendergraph/Variable.h>

namespace renderlab
{
namespace node
{

class SubGraph : public bp::node::SubGraph<rendergraph::Variable>
{
public:
    SubGraph();

	virtual void Draw(const ur::Device& dev, ur::Context& ctx,
		const n2::RenderParams& rp) const override;

    virtual void LoadFromJson(const ur::Device& dev, const std::string& dir, 
		const rapidjson::Value& val) override;

	void SetFilepath(const std::string& filepath);
	auto& GetFilepath() const { return m_filepath; }

private:
	void InitFromFile(const ur::Device& dev, const std::string& filepath);

private:
	std::string m_filepath;

	bool m_need_rebuild = false;

    RTTR_ENABLE(bp::node::SubGraph<rendergraph::Variable>)

}; // SubGraph

}
}