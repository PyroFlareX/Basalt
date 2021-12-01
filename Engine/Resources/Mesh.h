#pragma once

#include <vector>
#include "../Types/Types.h"


namespace bs
{
	struct Vertex
	{
		bs::vec3 position = bs::vec3(0.0f, 0.0f, 0.0f);
		bs::vec3 normal = bs::vec3(0.0f, 0.0f, 0.0f);
		bs::vec2 uv = bs::vec2(0.0f, 0.0f);
	};

	struct Mesh
	{
		std::vector<bs::Vertex> vertices;
		std::vector<unsigned int> indicies;
	};
	//not used in vulkan
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