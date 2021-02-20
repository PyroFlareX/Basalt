#pragma once

#include <vector>
#include "../Types/Types.h"


namespace vn
{
	struct Vertex
	{
		vn::vec3 position = vn::vec3(0.0f, 0.0f, 0.0f);
		vn::vec3 normal = vn::vec3(0.0f, 0.0f, 0.0f);
		vn::vec2 uv = vn::vec2(0.0f, 0.0f);
	};

	struct Mesh
	{
		std::vector<vn::Vertex> vertices;
		std::vector<unsigned int> indicies;
	};

	struct renderInfo
	{
		unsigned int VAO = 0;
		int indiciesCount = 0;
		inline void reset()
		{
			VAO = 0;
			indiciesCount = 0;
		}
	};
}