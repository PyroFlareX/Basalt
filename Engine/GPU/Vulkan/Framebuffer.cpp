#include "Framebuffer.h"



namespace bs::vk
{
	RenderTargetFramebuffer::RenderTargetFramebuffer(bs::Device& device, VkRenderPass renderpass, bs::vec2i extent)
		: m_size(extent), m_device(device), m_renderpass(renderpass)
	{	
		createRenderImage();
		createDepthAttachment();

		createFramebuffer();
	}

	RenderTargetFramebuffer::~RenderTargetFramebuffer()
	{
		destroyHandles();
	}

	FramebufferData RenderTargetFramebuffer::getFramebufferData() const
	{
		return FramebufferData{{m_handle}, m_imgView, m_size};
	}

	VkImageView RenderTargetFramebuffer::getColorImgView() const
	{
		return m_imgView;
	}

	VkImageView RenderTargetFramebuffer::getDepthImgView() const
	{
		return m_depthImgView;
	}

	void RenderTargetFramebuffer::recreateFramebuffer(bs::vec2i size)
	{
		m_size = size;
		destroyHandles();

		createRenderImage();
		createDepthAttachment();

		createFramebuffer();
	}

	void RenderTargetFramebuffer::createRenderImage()
	{
		VmaAllocationCreateInfo imgAllocInfo = {};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// CREATE IMAGE
		VkImageCreateInfo image{};
		image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image.imageType = VK_IMAGE_TYPE_2D;
		image.format = VK_FORMAT_B8G8R8A8_SRGB;
		image.extent.width = m_size.x;
		image.extent.height = m_size.y;
		image.extent.depth = 1;
		image.mipLevels = 1;
		image.arrayLayers = 1;
		image.samples = VK_SAMPLE_COUNT_1_BIT;
		image.tiling = VK_IMAGE_TILING_OPTIMAL;
		// We will sample directly from the color attachment
		image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		vmaCreateImage(m_device.getAllocator(), &image, &imgAllocInfo, &m_img, &m_imgAllocation, nullptr);

		// CREATE IMAGE VIEWS
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_img;
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

		if (vkCreateImageView(m_device.getDevice(), &createInfo, nullptr, &m_imgView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image views!");
		}
	}

	void RenderTargetFramebuffer::createDepthAttachment()
	{
		VmaAllocationCreateInfo imgAllocInfo = {};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkImageCreateInfo Dimage{};
		Dimage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		Dimage.imageType = VK_IMAGE_TYPE_2D;
		Dimage.format = VK_FORMAT_D32_SFLOAT;
		Dimage.extent.width = m_size.x;
		Dimage.extent.height = m_size.y;
		Dimage.extent.depth = 1;
		Dimage.mipLevels = 1;
		Dimage.arrayLayers = 1;
		Dimage.samples = VK_SAMPLE_COUNT_1_BIT;
		Dimage.tiling = VK_IMAGE_TILING_OPTIMAL;
		// We will sample directly from the color attachment
		Dimage.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		vmaCreateImage(m_device.getAllocator(), &Dimage, &imgAllocInfo, &m_depthImg, &m_depthImageAllocation, nullptr);

		// CREATE IMAGE VIEWS
		VkImageViewCreateInfo DcreateInfo{};
		DcreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		DcreateInfo.image = m_depthImg;
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

		if (vkCreateImageView(m_device.getDevice(), &DcreateInfo, nullptr, &m_depthImgView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create depth image views!");
		}
	}

	void RenderTargetFramebuffer::createFramebuffer()
	{
		// CREATE FRAMEBUFFER
		const VkImageView p_attachments[2] = 
		{
			m_imgView,
			m_depthImgView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderpass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = p_attachments;
		framebufferInfo.width = m_size.x;
		framebufferInfo.height = m_size.y;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_device.getDevice(), &framebufferInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	void RenderTargetFramebuffer::destroyHandles()
	{
		vkDestroyFramebuffer(m_device.getDevice(), m_handle, nullptr);

		vkDestroyImageView(m_device.getDevice(), m_imgView, nullptr);
		vkDestroyImageView(m_device.getDevice(), m_depthImgView, nullptr);

		vmaDestroyImage(m_device.getAllocator(), m_img, m_imgAllocation);
		vmaDestroyImage(m_device.getAllocator(), m_depthImg, m_depthImageAllocation);
	}
}