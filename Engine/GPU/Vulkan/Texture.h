#pragma once

#include "Buffer.h"
#include "../../Resources/Image.h"

namespace vn::vk
{
	class Texture
	{
	public:
		Texture(vn::Device* device);

		// Upload the img here
		void loadFromImage(vn::Image& img);

	private:
		VkImage textureImg;
		VmaAllocation textureAllocation;
	};
}