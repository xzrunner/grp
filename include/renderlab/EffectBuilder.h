#pragma once

#include <fxlang/EffectType.h>
#include <SM_Vector.h>

#include <ee0/typedef.h>
#include <blueprint/typedef.h>

#include <string>

namespace renderlab
{

class EffectBuilder
{
public:
	EffectBuilder(const ee0::SubjectMgrPtr& sub_mgr);

	void Build(const std::string& fx, fxlang::EffectType type);

	auto& GetCode() const { return m_fx; }
	auto GetType() const { return m_type; }

private:
	void InsertNode(const bp::NodePtr& node, const sm::vec2& pos);

private:
	const ee0::SubjectMgrPtr m_sub_mgr = nullptr;

	std::string m_fx;

	fxlang::EffectType m_type;

}; // EffectBuilder

}