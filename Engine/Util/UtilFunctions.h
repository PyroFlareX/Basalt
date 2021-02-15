#pragma once

#include "../Types/Types.h"

namespace vn
{
	constexpr inline float	lerp(float pos1, float pos2, float gradient);
	constexpr inline vec2	lerp(vec2 pos1, vec2 pos2, float gradient);
	constexpr inline vec3	lerp(vec3 pos1, vec3 pos2, float gradient);
	constexpr inline vec4	lerp(vec4 pos1, vec4 pos2, float gradient);

	inline mat4	makeProjectionMatrix(const float fov, const vec2 contextSize);
	const mat4 makeViewMatrix(const Transform& camera);
	const mat4 makeModelMatrix(const Transform& entity);
}