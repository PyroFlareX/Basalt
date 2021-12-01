#pragma once

#include "Buffer.h"
#include "../../Resources/Image.h"

namespace bs::vk
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
		Texture(bs::Device* device);

		// Upload the img here
		void loadFromImage(bs::Image& img);

		texture_t getAPITextureInfo() const
		{
			return texture_t{ textureImg, textureImgView, sampler };
		}

		bs::Device* getDevice() { return p_device; }
	protected:
		VkImage textureImg;
		VkImageView textureImgView;
		VmaAllocation textureAllocation;
		VkSampler sampler;

		bs::Device* p_device;
	};
}