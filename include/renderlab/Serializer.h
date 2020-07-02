#pragma once

#include <rapidjson/document.h>

namespace pt0 { class Camera; }

namespace renderlab
{

class Serializer
{
public:
	static void StoreCamera(const std::shared_ptr<pt0::Camera>& cam,
		rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc);
	static void LoadCamera(std::shared_ptr<pt0::Camera>& cam,
		const rapidjson::Value& val);

}; // Serializer

}