#include "Framebuffer.h"



namespace bs::vk
{
	RenderTargetFramebuffer::RenderTargetFramebuffer(bs::Device device, VkRenderPass& renderPass, bs::vec2i extent)
	{
		m_size = extent;

		//Allocation Info
		VmaAllocationCreateInfo imgAllocInfo = {};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// CREATE IMAGE
		VkImageCreateInfo image{};
		image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image.imageType = VK_IMAGE_TYPE_2D;
		image.format = VK_FORMAT_B8G8R8A8_SRGB;
		image.extent.width = (int)extent.x;
		image.extent.height = (int)extent.y;
		image.extent.depth = 1;
		image.mipLevels = 1;
		image.arrayLayers = 1;
		image.samples = VK_SAMPLE_COUNT_1_BIT;
		image.tiling = VK_IMAGE_TILING_OPTIMAL;
		// We will sample directly from the color attachment
		image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		vmaCreateImage(device.getAllocator(), &image, &imgAllocInfo, &img, &imgAllocation, nullptr);

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

		// DO IT ALL AGAIN BUT FOR DEPTH
		// CREATE IMAGE
		VkImageCreateInfo Dimage{};
		Dimage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		Dimage.imageType = VK_IMAGE_TYPE_2D;
		Dimage.format = VK_FORMAT_D32_SFLOAT;
		Dimage.extent.width = (int)extent.x;
		Dimage.extent.height = (int)extent.y;
		Dimage.extent.depth = 1;
		Dimage.mipLevels = 1;
		Dimage.arrayLayers = 1;
		Dimage.samples = VK_SAMPLE_COUNT_1_BIT;
		Dimage.tiling = VK_IMAGE_TILING_OPTIMAL;
		// We will sample directly from the color attachment
		Dimage.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		vmaCreateImage(device.getAllocator(), &Dimage, &imgAllocInfo, &depthImg, &depthImageAllocation, nullptr);

		// CREATE IMAGE VIEWS
		VkImageViewCreateInfo DcreateInfo{};
		DcreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		DcreateInfo.image = depthImg;
		DcreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		DcreateInfo.format = VK_FORMAT_D32_SFLOAT;
		DcreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		DcreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		DcreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		DcreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		DcreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		DcreateInfo.subresourceRange.baseMipLevel = 0;
		DcreateInfo.subresourceRange.levelCount = 1;
		DcreateInfo.subresourceRange.baseArrayLayer = 0;
		DcreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device.getDevice(), &DcreateInfo, nullptr, &depthImgView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}

		// CREATE FRAMEBUFFER
		VkImageView attachments[] = {
			imgView,
			depthImgView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = (int)extent.x;
		framebufferInfo.height = (int)extent.y;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device.getDevice(), &framebufferInfo, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	FramebufferData RenderTargetFramebuffer::getFramebufferData()
	{
		return FramebufferData{{handle}, imgView, m_size};
	}

	RenderTargetFramebuffer::~RenderTargetFramebuffer()
	{

	}
}