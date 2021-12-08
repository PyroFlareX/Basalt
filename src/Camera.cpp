#include "Camera.h"

#include <Engine.h>

Camera::Camera()	:	lerp(2.5f)
{
	proj = glm::perspective(glm::radians(80.0f), (float)bs::vk::viewportwidth / (float)bs::vk::viewportheight, 0.001f, 1000000.0f);
	pos = bs::vec3(0.0f, 0.0f, 1.0f);
	rot = bs::vec3(0.0f);

	follow(*this);
}

bs::mat4 Camera::getViewMatrix() const
{
/*	switch (mode)
	{
	case 0:*/
		return bs::makeViewMatrix(*this);
/*	case 1:
//		return bs::vr::viewMatrixL * bs::vr::HMDMatrix;
	case 2:
//		return bs::vr::viewMatrixR * bs::vr::HMDMatrix;
	default:
		break;
	}
	return bs::mat4();*/
}

bs::mat4 Camera::getProjMatrix() const
{
/*	switch (mode)
	{
	case 0:
		return proj;
	case 1:
//		return bs::vr::projMatrixL;
	case 2:
//		return bs::vr::projMatrixR;
	default:
		break;
	}*/
	return proj;
}

void Camera::follow(bs::Transform& entity)
{
	entityPos = &entity;
}

void Camera::update()
{
	pos = entityPos->pos;
	rot = entityPos->rot;

	rot.x = std::clamp(rot.x, -89.9f, 89.9f);

	if (rot.y > 180.0f) { rot.y = -180.0f; }
	if (rot.y < -180.0f) { rot.y = 180.0f; }

	entityPos->rot = rot;

	proj = glm::perspective(glm::radians(80.0f), (float)bs::vk::viewportwidth / (float)bs::vk::viewportheight, 0.001f, 1000000.0f);
}


