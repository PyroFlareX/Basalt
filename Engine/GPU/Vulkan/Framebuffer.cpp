#include "Framebuffer.h"



namespace vn::vk
{
	RenderTargetFramebuffer::RenderTargetFramebuffer(vn::Device device, VkRenderPass& renderPass, vn::vec2 extent)
	{
		m_size = extent;

		// CREATE IMAGE
		VkImageCreateInfo image{};
		image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image.imageType = VK_IMAGE_TYPE_2D;
		image.format = VK_FORMAT_B8G8R8A8_SRGB;
		image.extent.width = extent.x;
		image.extent.height = extent.y;
		image.extent.depth = 1;
		image.mipLevels = 0;
		image.arrayLayers = 1;
		image.samples = VK_SAMPLE_COUNT_1_BIT;
		image.tiling = VK_IMAGE_TILING_OPTIMAL;
		// We will sample directly from the color attachment
		image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		if (vkCreateImage(device.getDevice(), &image, nullptr, &img) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		// CREATE IMAGE VIEWS
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = img;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device.getDevice(), &createInfo, nullptr, &imgView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}

		// CREATE FRAMEBUFFER
		VkImageView attachments[] = {
			imgView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.x;
		framebufferInfo.height = extent.y;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device.getDevice(), &framebufferInfo, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	RenderTargetFramebuffer::~RenderTargetFramebuffer()
	{
		
	}
}