#pragma once

#include "Types.h"
#include "../Resources/Material.h"

#include <glm/gtx/euler_angles.hpp>
#include <iostream>

namespace bs
{
	// GameObject, simulated in the gamestate
	class GameObject
	{
	public:
		GameObject(Transform& trans, const std::string ModelName = "") : transform(trans)
		{
			hasTransform = true;
			mass = 1.0f;

			model_id = ModelName;

			init();
		};

		//Virtual Method to be overriden, called on init
		virtual void init()
		{

		};

		//Virtual Method to be overridden, called every frame
		virtual void update()
		{

		};

		// @Brief Returns the current Transform (internal engine) of the object so it may be used in graphics
		Transform& getCurrentTransform()
		{
			return transform;
		}

		float mass;

		std::string model_id;

		Material material;
		

		virtual ~GameObject() = default;
	private:

		bool hasTransform = false;

		Transform transform;
		
	};
}