#include "MathUtils.h"

#include <cmath>
#include <limits>

#include <glm/gtx/euler_angles.hpp>	//Read this again, builds matrices from euler angles
constexpr bool USE_MATRIX_ANGLE_EULER = false; //for old way, put 'true'

namespace bs
{
	vec3 fromEulerDegreesToDirection(const vec3 eulerAngle)
	{
		mat4 matrix = mat4(1.0f);	//Identity Matrix
		constexpr vec4 dir(0.0f, 0.0f, 1.0f, 0.0f); //unit vector pointing forward (z+)
		
		//No rotation is just forward
		const vec3 angles(glm::radians(eulerAngle));

		if constexpr(USE_MATRIX_ANGLE_EULER)
		{
			matrix = glm::rotate(matrix, angles.x, { 1, 0, 0 }); //rot x
			matrix = glm::rotate(matrix, angles.y, { 0, 1, 0 }); //rot y
			matrix = glm::rotate(matrix, angles.z, { 0, 0, 1 }); //rot z
		}
		else
		{
			matrix = glm::eulerAngleXYZ(angles.x, angles.y, angles.z);
		}

		return vec3(matrix * dir);
	}

	vec3 eulerAnglesToAxisAngleRot(const vec3 eulerAngle, float& angle)
	{	//Currently UNTESTED
		constexpr vec3 dir(0.0f, 0.0f, 1.0f); //unit vector pointing forward (z+)

		const vec3 angles = glm::radians(eulerAngle);
		const vec3 rotDirection = fromEulerDegreesToDirection(eulerAngle);

		const vec3 normalAxis = glm::cross(dir, rotDirection);
		angle = glm::acos(dot(angles, rotDirection)); //gets the cos of the angle between norm and result, gets arccos

		return normalAxis;
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