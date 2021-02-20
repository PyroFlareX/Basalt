#pragma once

#include "Types.h"

#include <glm/gtx/euler_angles.hpp>
#include <iostream>

namespace vn
{
	class GameObject
	{
	public:
		GameObject(Transform& trans) : transform(trans)
		{
			
			hasTransform = true;
			mass = 1.0f;

			model_id = 0;

			init();
		};


		virtual void init()
		{

		};

		virtual void update()
		{

		};

		// @Brief Returns the current Transform (internal engine) of the object so it may be used in graphics
		Transform& getCurrentTransform()
		{
			/*if (!hasTransform)
			{
				Transform trans;
				btTransform t;
				motionState->getWorldTransform(t);

				//Position
				trans.pos = vec3(t.getOrigin().getX(), t.getOrigin().getY(), t.getOrigin().getZ());

				//Rotation (quaternion to Euler Angles)
				btQuaternion quat = t.getRotation();
				glm::quat q(quat.getW(), -quat.getX(), -quat.getY(), -quat.getZ());
				trans.rot = glm::eulerAngles(q);
				trans.rot = vec3(-trans.rot.x, -trans.rot.y, -trans.rot.z);
				trans.rot = glm::degrees(trans.rot);

				//Scale
				trans.scale = vec3(collider->getLocalScaling().getX(), collider->getLocalScaling().getY(), collider->getLocalScaling().getZ());
				transform = trans;
			}*/

			hasTransform = false;
			return transform;
		}

		float				mass;

		int model_id = 0;
		int texture_id = 0;

		virtual ~GameObject() = default;
	private:

		bool hasTransform = false;

		Transform transform;
		
	};
}