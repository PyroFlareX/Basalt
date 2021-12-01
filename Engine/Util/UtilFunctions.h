#pragma once

#include "MathUtils.h"
#include <Types/Types.h>

namespace bs
{
	void* cast(u64 i);

	float toPercent(float input);

	constexpr inline float lerp(float pos1, float pos2, float gradient)
	{
		return (((pos2 - pos1) * gradient) + pos1);
	}

	constexpr inline vec2 lerp(vec2 pos1, vec2 pos2, float gradient)
	{
		return vec2(((pos2 - pos1) * gradient) + pos1);
	}

	constexpr inline vec3 lerp(vec3 pos1, vec3 pos2, float gradient)
	{
		return vec3(((pos2 - pos1) * gradient) + pos1);
	}

	constexpr inline vec4 lerp(vec4 pos1, vec4 pos2, float gradient)
	{
		return vec4(((pos2 - pos1) * gradient) + pos1);
	}

	inline mat4	makeProjectionMatrix(const float fov, const vec2 contextSize) noexcept;
	const mat4 makeViewMatrix(const Transform& camera) noexcept;
	const mat4 makeModelMatrix(const Transform& entity) noexcept;

	bool isNumber(const char c);
	
	const void printVec(const vec2& vec, const std::string prefix);
	const void printVec(const vec3& vec, const std::string prefix);
	const void printVec(const vec4& vec, const std::string prefix);

}