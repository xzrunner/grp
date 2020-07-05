#include "renderlab/Serializer.h"

#include <painting3/PerspCam.h>

#define ONLY_POS_AND_ANGLE

namespace renderlab
{

void Serializer::StoreCamera(const std::shared_ptr<pt0::Camera>& cam,
	                         rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc)
{
	if (cam->TypeID() != pt0::GetCamTypeID<pt3::PerspCam>()) {
		return;
	}

	auto p_cam = std::static_pointer_cast<pt3::PerspCam>(cam);

	rapidjson::Value cam_val;
	cam_val.SetObject();

	// pos
	auto& pos = p_cam->GetPos();
	rapidjson::Value val_pos;
	val_pos.SetArray();
	val_pos.PushBack(pos.x, alloc);
	val_pos.PushBack(pos.y, alloc);
	val_pos.PushBack(pos.z, alloc);
	cam_val.AddMember("pos", val_pos, alloc);

	// target
	auto& target = p_cam->GetTarget();
	rapidjson::Value val_tar;
	val_tar.SetArray();
	val_tar.PushBack(target.x, alloc);
	val_tar.PushBack(target.y, alloc);
	val_tar.PushBack(target.z, alloc);
	cam_val.AddMember("target", val_tar, alloc);

	// up
	auto& up = p_cam->GetUpDir();
	rapidjson::Value val_up;
	val_up.SetArray();
	val_up.PushBack(up.x, alloc);
	val_up.PushBack(up.y, alloc);
	val_up.PushBack(up.z, alloc);
	cam_val.AddMember("up", val_up, alloc);

#ifndef ONLY_POS_AND_ANGLE
	// projection
	cam_val.AddMember("near",   p_cam->GetNear(), alloc);
	cam_val.AddMember("far",    p_cam->GetFar(), alloc);
	cam_val.AddMember("aspect", p_cam->GetAspect(), alloc);
	cam_val.AddMember("fov",    p_cam->GetAngleOfView(), alloc);
#endif // ONLY_POS_AND_ANGLE

	val.AddMember("camera", cam_val, alloc);
}

void Serializer::LoadCamera(const std::shared_ptr<pt0::Camera>& cam, const rapidjson::Value& val)
{
	if (cam->TypeID() != pt0::GetCamTypeID<pt3::PerspCam>()) {
		return;
	}

	if (!val.HasMember("camera")) {
		return;
	}

	auto p_cam = std::static_pointer_cast<pt3::PerspCam>(cam);
	auto& cam_val = val["camera"];

	if (cam_val.HasMember("pos") && cam_val.HasMember("target") && cam_val.HasMember("up"))
	{
		auto& val_pos    = cam_val["pos"];
		auto& val_target = cam_val["target"];
		auto& val_up     = cam_val["up"];
		sm::vec3 pos(val_pos[0].GetFloat(), val_pos[1].GetFloat(), val_pos[2].GetFloat());
		sm::vec3 target(val_target[0].GetFloat(), val_target[1].GetFloat(), val_target[2].GetFloat());
		sm::vec3 up(val_up[0].GetFloat(), val_up[1].GetFloat(), val_up[2].GetFloat());
		p_cam->SetPosAndAngle(pos, target, up);
	}

#ifndef ONLY_POS_AND_ANGLE
	if (cam_val.HasMember("near")) {
		p_cam->SetNear(cam_val["near"].GetFloat());
	}
	if (cam_val.HasMember("far")) {
		p_cam->SetFar(cam_val["far"].GetFloat());
	}
	if (cam_val.HasMember("aspect")) {
		p_cam->SetAspect(cam_val["aspect"].GetFloat());
	}
	if (cam_val.HasMember("fov")) {
		p_cam->SetAngleOfView(cam_val["fov"].GetFloat());
	}
#endif // ONLY_POS_AND_ANGLE
}

}