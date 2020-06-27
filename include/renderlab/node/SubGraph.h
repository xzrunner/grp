#pragma once

#include <blueprint/node/SubGraph.h>

#include <rendergraph/Variable.h>
#include <node0/typedef.h>

namespace renderlab
{
namespace node
{

class SubGraph : public bp::node::SubGraph<rendergraph::Variable>
{
public:
    SubGraph();

	virtual bool Update(const ur::Device& dev) override;

    virtual void LoadFromJson(const ur::Device& dev, const std::string& dir, 
		const rapidjson::Value& val) override;

	void SetFilepath(const std::string& filepath);
	auto& GetFilepath() const { return m_filepath; }

private:
	void InitFromFile(const ur::Device& dev, const std::string& filepath);

	void Build(const std::vector<n0::SceneNodePtr>& nodes);

private:
	std::string m_filepath;

	bool m_need_rebuild = false;

    RTTR_ENABLE(bp::node::SubGraph<rendergraph::Variable>)

}; // SubGraph

}
}