#include "MathUtils.h"

#include <cmath>
#include <limits>

namespace bs
{
	//This is borrowed from HackySTL
	constexpr float midpoint(float a, float b)
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
	constexpr float sqrt(const float& value)
	{
		auto mid_val = static_cast<float>(1.0);

        if (value <= static_cast<float>(-0.0))
        {
            return 0.0f; //std::numeric_limits<float>::quiet_NaN();
        }
    	for (size_t count = 0; count < 10; ++count)
    	{
        	auto euler = value / mid_val;
        	mid_val = midpoint(mid_val, euler);
		}

		return mid_val;
	}

	constexpr float inversesqrt(const float& value)
	{

		return 1.0f / sqrt(value);
	}

	constexpr float dot(vec3 vec1, vec3 vec2)
	{
		vec3 temp = vec1 * vec2;

		return temp.x + temp.y + temp.z;
	}

	constexpr vec3 normalize(vec3 vec)
	{
		return vec * inversesqrt(dot(vec, vec));
	}

	vec3 fromEulerDegreesToDirection(const vec3 rotEuler)
	{
		mat4 matrix = mat4(1.0f);

		matrix = glm::rotate(matrix, glm::radians(rotEuler.x), { 1, 0, 0 });
		matrix = glm::rotate(matrix, glm::radians(rotEuler.y), { 0, 1, 0 });
		matrix = glm::rotate(matrix, glm::radians(rotEuler.z), { 0, 0, 1 });

		const vec4 dir = matrix * vec4(0.0f, 0.0f, 1.0f, 0.0f);

		return vec3(dir);
	}

	std::optional<float> intersectplane(Ray ray, vec3 planenormal, vec3 planecenter)
	{
		planecenter = glm::normalize(planecenter);		//normalized

		ray.direction = glm::normalize(ray.direction);	//normalized
		ray.start = glm::normalize(ray.start);			//normalized

		const float denominator = glm::dot(planenormal, ray.direction);
		
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

	std::optional<float> intersectGivenPlane(bs::vec3 raystart, bs::vec3 raydirection)
	{
		constexpr bs::vec3 planenormal(0.0f, -1.0f, 0.0f);		//normalized
		constexpr bs::vec3 _planecenter(0.0f, 0.0078125f, 0.0f);

		bs::vec3 planecenter = glm::normalize(_planecenter);		//normalized, input val is just the plane center

		raydirection = glm::normalize(raydirection);	//normalized
		raystart = glm::normalize(raystart);			//normalized

		const float denominator = glm::dot(planenormal, raydirection);
		
		if(denominator > 1e-6)
		{
			float distance = glm::dot(planecenter - raystart, planenormal) / denominator;
			
			distance = glm::abs(distance);
			if(distance < 0)
			{
				return {};
			}
			else
			{
				return std::make_optional<float>(distance);
			}
		}

		return {};
	}

	bool doesIntersectGivenPlane(bs::vec3 raystart, bs::vec3 raydirection)
	{
		constexpr bs::vec3 planenormal(0.0f, -1.0f, 0.0f);		//normalized
		constexpr bs::vec3 _planecenter(0.0f, 0.0078125f, 0.0f);

		const bs::vec3 planecenter(glm::normalize(_planecenter));		//normalized

		raydirection = glm::normalize(raydirection);	//normalized
		raystart = glm::normalize(raystart);			//normalized

		float denominator = glm::dot(planenormal, raydirection);
		
		if(denominator > 1e-6)
		{
			float distance = glm::dot(planecenter - raystart, planenormal) / denominator;
			
			return (distance >= 0);
		}

		return false;
	}
}