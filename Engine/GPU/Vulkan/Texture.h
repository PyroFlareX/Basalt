#pragma once

#include "Buffer.h"
#include "../../Resources/Image.h"
#include <vulkan/vulkan_core.h>

namespace vn::vk
{
	struct texture_t
	{
		VkImage imgvk;
		VkImageView imgviewvk;
	};

	class Texture
	{
	public:
		Texture(vn::Device* device);

		// Upload the img here
		void loadFromImage(vn::Image& img);

		texture_t getAPITextureInfo()
		{
			return texture_t{ textureImg, textureImgView };
		}

	private:
		VkImage textureImg;
		VkImageView textureImgView;
		VmaAllocation textureAllocation;
		VkDeviceMemory deviceMem;

		vn::Device* p_device;
	};
}