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
		//@TODO: change these to weak ptrs
		Texture(bs::Device* device);
		Texture(bs::Device* device, const bs::Image& img);

		// Upload the img here
		void loadFromImage(const bs::Image& img);
		//Destroy the image
		void destroy();

		inline texture_t getAPITextureInfo() const noexcept
		{
			return texture_t{ m_textureImg, m_textureImgView, m_sampler };
		}

		//@TODO: change this to a weak ptr
		bs::Device* getDevice() const noexcept;
	protected:
		VkImage m_textureImg;
		VkImageView m_textureImgView;
		VmaAllocation m_textureAllocation;
		VkSampler m_sampler;

		//@TODO: change this to a weak ptr
		bs::Device* p_device;
	};
}