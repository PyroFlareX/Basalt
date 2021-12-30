#pragma once

#include "../Types/Types.h"
#include <vector>

namespace bs
{
	struct Vertex
	{
		vec3 position = vec3(0.0f);
		vec3 normal = vec3(0.0f);
		vec2 uv = vec2(0.0f);

		vec3 vertexColor = vec3(0.0f);
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<u32> indicies;
	};
}