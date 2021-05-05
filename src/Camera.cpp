#include "Camera.h"

Camera::Camera(int Mode)	:	mode(Mode)
{
	lerp = 2.5f;
	if (mode == 0)
	{
	//	proj = vn::makeProjectionMatrix(80.0f, vn::vec2(1280, 720));
	}
	//proj = vn::makeProjectionMatrix(80.0f, vn::vec2(1280, 720));
	proj = glm::perspective(glm::radians(80.0f), 800.0f / 600.0f, 0.001f, 1000000.0f);
	pos = vn::vec3(0.0f, 0.0f, 1.0f);
	rot = vn::vec3(0.0f);
}

vn::mat4 Camera::getViewMatrix() const
{
/*	switch (mode)
	{
	case 0:*/
		return vn::makeViewMatrix(*this);
/*	case 1:
//		return vn::vr::viewMatrixL * vn::vr::HMDMatrix;
	case 2:
//		return vn::vr::viewMatrixR * vn::vr::HMDMatrix;
	default:
		break;
	}
	return vn::mat4();*/
}

vn::mat4 Camera::getProjMatrix() const
{
/*	switch (mode)
	{
	case 0:
		return proj;
	case 1:
//		return vn::vr::projMatrixL;
	case 2:
//		return vn::vr::projMatrixR;
	default:
		break;
	}*/
	return proj;
}

void Camera::follow(vn::Transform& entity)
{
	p_entity = &entity;
}

void Camera::update()
{
	if (mode == 0)
	{
		pos = p_entity->pos;
		rot = p_entity->rot;
		if (rot.x > 90.0f) { rot.x = 89.9f; }
		if (rot.x < -90.0f) { rot.x = -89.9f; }
	}
}


