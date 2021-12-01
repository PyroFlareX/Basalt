#pragma once

#include "../Types/Types.h"

#include <optional>

namespace bs
{
	//Quick abs val
	template<typename T>
	constexpr auto abs(const T& value)
    {
        return value < 0 ? -value : value;
    }

	//Sqrt
	constexpr float sqrt(const float& value);
	
	//Inverse sqrt
	constexpr float inversesqrt(const float& value);

	//Dot product
	constexpr float dot(vec3 vec1, vec3 vec2);

	//Normalize a vec3
	constexpr vec3 normalize(vec3 vec);

	//Converts from Euler degrees to a direction vector
	vec3 fromEulerDegreesToDirection(const vec3 rotEuler);

	//Returns the distance from the center of the plane, if there is one, otherwise returns null option
	std::optional<float> intersectPlane(Ray ray, vec3 planenormal, vec3 planecenter);

	//Templated intersectPlane
	template<vec3 PlaneCenter>
	std::optional<float> intersectPlane(Ray ray, vec3 planenormal)
	{
		constexpr vec3 planecenter = glm::normalize(PlaneCenter);		//normalized

		ray.direction = glm::normalize(ray.direction);	//normalized
		ray.start = glm::normalize(ray.start);			//normalized

		float denominator = glm::dot(planenormal, ray.direction);
		
		if(denominator > 1e-6)
		{
			float distance = glm::dot(planecenter - ray.start, planenormal) / denominator;
			distance = abs(distance);

			if(distance >= 0)
			{
				return std::make_optional<float>(distance);
			}
			else
			{
				return {};
			}
		}
		else
		{
			return {};
		}
	}

	//Calculate Intersection with a preset plane center, for the Map on World
	std::optional<float> intersectGivenPlane(bs::vec3 raystart, bs::vec3 raydirection);

	//Calculates whether the intersection happens
	bool doesIntersectGivenPlane(bs::vec3 raystart, bs::vec3 raydirection);
}