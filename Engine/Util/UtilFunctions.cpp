#include "UtilFunctions.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace vn {
	constexpr inline float lerp(float pos1, float pos2, float gradient) {
		return (((pos2 - pos1) * gradient) + pos1);
	}

	constexpr inline vec2 lerp(vec2 pos1, vec2 pos2, float gradient) {
		return vec2(((pos2 - pos1) * gradient) + pos1);
	}

	constexpr inline vec3 lerp(vec3 pos1, vec3 pos2, float gradient) {
		return vec3(((pos2 - pos1) * gradient) + pos1);
	}

	constexpr inline vec4 lerp(vec4 pos1, vec4 pos2, float gradient) {
		return vec4(((pos2 - pos1) * gradient) + pos1);
	}

	inline mat4 makeProjectionMatrix(const float fov, const vec2 contextSize) {
		return glm::perspective(glm::radians(fov), contextSize.x / contextSize.y, 0.001f, 1000.0f);
	}

	const mat4 makeViewMatrix(const Transform &camera) {
		mat4 matrix = mat4(1.0f);

		matrix = glm::rotate(matrix, glm::radians(camera.rot.x), {1, 0, 0});
		matrix = glm::rotate(matrix, glm::radians(camera.rot.y), {0, 1, 0});
		matrix = glm::rotate(matrix, glm::radians(camera.rot.z), {0, 0, 1});

		matrix = glm::translate(matrix, -camera.pos);

		return matrix;
	}

	const mat4 makeModelMatrix(const Transform &entity) {
		mat4 matrix = mat4(1.0f);

		matrix = glm::rotate(matrix, glm::radians(entity.rot.x), entity.pos/*{ 1, 0, 0 }*/);
		matrix = glm::rotate(matrix, glm::radians(entity.rot.y), entity.pos/*{ 0, 1, 0 }*/);
		matrix = glm::rotate(matrix, glm::radians(entity.rot.z), entity.pos/*{ 0, 0, 1 }*/);


		matrix = glm::translate(matrix, entity.pos);

		return matrix;
	}
}