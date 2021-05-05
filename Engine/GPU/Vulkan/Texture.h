#pragma once

#include "Buffer.h"
#include "../../Resources/Image.h"

namespace vn::vk
{
	struct texture_t
	{
		VkImage imgvk;
		VkImageView imgviewvk;
		VkSampler sampler;
	};

	class Texture
	{
	public:
		Texture(vn::Device* device);

		// Upload the img here
		void loadFromImage(vn::Image& img);

		texture_t getAPITextureInfo() const
		{
			return texture_t{ textureImg, textureImgView, sampler };
		}

	private:
		VkImage textureImg;
		VkImageView textureImgView;
		VmaAllocation textureAllocation;
		VkDeviceMemory deviceMem;

		VkSampler sampler;

		vn::Device* p_device;
	};
}