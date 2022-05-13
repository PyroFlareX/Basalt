#pragma once

#include "../Types/Types.h"

#include <optional>

/**
 * @brief 
 * 
 * SEVERAL OF THESE FUNCTIONS ARE EITHER TAKEN FROM
 * OR USE THE SAME KIND OF IMPL AS HACKYSTL
 * 
 * The corresponding ones are MARKED!
 */

namespace bs
{
	//Quick abs val
	template<typename T>
	constexpr auto abs(const T& value)
    {
        return value < 0 ? -value : value;
    }

	//This is borrowed from HackySTL
	constexpr float midpoint(const float a, const float b)
	{
		constexpr auto _low_limit = std::numeric_limits<float>::min() * 2;
        constexpr auto _high_limit = std::numeric_limits<float>::max() / 2;
        const auto _abs_a = abs(a);
        const auto _abs_b = abs(b);

        if (_abs_a <= _high_limit && _abs_b <= _high_limit)
        {
            return (a + b) / 2;
        }
        else if (_abs_a < _low_limit)
        {
            return a + b / 2;
        }
    	else if (_abs_b < _low_limit)
        {
            return a / 2 + b;
        }
        else
        {
            return a / 2 + b / 2;
        }
	}

	//Parts of impl borrowed from HackySTL
	constexpr float sqrt(const float value)
	{
		constexpr auto numIterationsForPrecision = 10;
		auto mid_val = static_cast<float>(1.0);

        if (value <= static_cast<float>(-0.0))
        {
            return 0.0f; //std::numeric_limits<float>::quiet_NaN();
        }
    	for (auto count = 0; count < numIterationsForPrecision; ++count)
    	{
        	const auto euler = value / mid_val;
        	mid_val = midpoint(mid_val, euler);
		}

		return mid_val;
	}

	//Inverse sqrt
	constexpr float inversesqrt(const float value)
	{
		//could also be:
		//return sqrt(value) / value;
		return 1.0f / sqrt(value);
	}

	//Dot product
	constexpr inline float dot(const vec3 v1, const vec3 v2)
	{
		const vec3 temp = v1 * v2;
		return temp.x + temp.y + temp.z;
	}

	//Cross product
	constexpr inline vec3 cross(const vec3 v1, const vec3 v2)
	{
		return vec3(v1.y * v2.z - v2.y * v1.z,
				v1.z * v2.x - v2.z * v1.x,
				v1.x * v2.y - v2.x * v1.y);
	}

	//Normalize a vector
	constexpr vec3 normalize(const vec3 v)
	{
		return v * inversesqrt(dot(v, v));
	}

	//Converts from Euler degrees to a direction vector
	vec3 fromEulerDegreesToDirection(const vec3 eulerAngle);

	//Converts from Euler Angles (degrees) to an axis-angle rotation
	vec3 eulerAnglesToAxisAngleRot(const vec3 eulerAngle, float& angle);

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
	std::optional<float> intersectGivenPlane(vec3 raystart, vec3 raydirection);

	//Calculates whether the intersection happens
	bool doesIntersectGivenPlane(vec3 raystart, vec3 raydirection);
}