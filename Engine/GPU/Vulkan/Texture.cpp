#include "Texture.h"

#include "Device.h"

namespace bs::vk
{
	Texture::Texture(bs::Device* device)	:	p_device(device)
	{
		// Create the img
		m_textureImg = VK_NULL_HANDLE;
		m_textureImgView = VK_NULL_HANDLE;
		m_sampler = VK_NULL_HANDLE;
		m_textureAllocation = VK_NULL_HANDLE;
	}

	Texture::Texture(bs::Device* device, const bs::Image& img)	:	p_device(device)
	{
		// Create the img
		m_textureImg = VK_NULL_HANDLE;
		m_textureImgView = VK_NULL_HANDLE;
		m_sampler = VK_NULL_HANDLE;
		m_textureAllocation = VK_NULL_HANDLE;

		loadFromImage(img);
	}

	Texture::Texture(Texture&& rval) :	p_device(rval.p_device), m_textureImg(rval.m_textureImg),
		m_textureImgView(rval.m_textureImgView), m_textureAllocation(rval.m_textureAllocation),
		m_sampler(rval.m_sampler)
	{
		rval.p_device = nullptr;
		
		rval.m_textureImg = VK_NULL_HANDLE;
		rval.m_textureImgView = VK_NULL_HANDLE;
		rval.m_sampler = VK_NULL_HANDLE;
		rval.m_textureAllocation = VK_NULL_HANDLE;
	}

	Texture& Texture::operator=(Texture&& rhs)
	{
		if(this == &rhs)
		{	//Self Assignmnet guard
			return *this;
		}
		this->destroy();
		this->p_device = rhs.p_device;
		this->m_textureImg = rhs.m_textureImg;
		this->m_textureImgView = rhs.m_textureImgView;
		this->m_sampler = rhs.m_sampler;
		this->m_textureAllocation = rhs.m_textureAllocation;

		rhs.p_device = nullptr;
		rhs.m_textureImg = VK_NULL_HANDLE;
		rhs.m_textureImgView = VK_NULL_HANDLE;
		rhs.m_sampler = VK_NULL_HANDLE;
		rhs.m_textureAllocation = VK_NULL_HANDLE;

		return *this;
	}

	Texture::~Texture()
	{
		destroy();
	}

	void Texture::loadFromImage(const bs::Image& img)
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		vkCreateSampler(p_device->getDevice(), &samplerInfo, nullptr, &m_sampler);

		//Upload the img
		//Allocation Info
		VmaAllocationCreateInfo imgAllocInfo = {};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// CREATE IMAGE
		VkImageCreateInfo image{};
		image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image.imageType = VK_IMAGE_TYPE_2D;
		image.format = VK_FORMAT_R8G8B8A8_SRGB;
		image.extent.height = (u32)img.getSize().y;
		image.extent.width = (u32)img.getSize().x;
		image.extent.depth = 1;
		image.mipLevels = 1;
		image.arrayLayers = 1;
		image.samples = VK_SAMPLE_COUNT_1_BIT;
		image.flags = 0;
		image.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		image.tiling = VK_IMAGE_TILING_OPTIMAL;
		image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		const size_t sizeImg = sizeof(bs::u8vec4) * img.getSize().x * img.getSize().y;

		auto stagingbuffer = std::make_shared<bs::vk::Buffer>(p_device, bs::vk::TRANSFER_BUFFER, sizeImg, sizeof(bs::u8vec4), img.getPixelsPtr());

		vmaCreateImage(p_device->getAllocator(), &image, &imgAllocInfo, &m_textureImg, &m_textureAllocation, nullptr);
		
		auto textureImg = m_textureImg;	//Just a workaround
		p_device->submitImmediate([stagingbuffer, image, textureImg](VkCommandBuffer cmd) 
		{
			auto m_textureImg = textureImg;	//Just a workaround
			VkImageSubresourceRange range;
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.baseMipLevel = 0;
			range.levelCount = 1;
			range.baseArrayLayer = 0;
			range.layerCount = 1;

			VkImageMemoryBarrier imageBarrier_toTransfer = {};
			imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

			imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toTransfer.image = m_textureImg;
			imageBarrier_toTransfer.subresourceRange = range;

			imageBarrier_toTransfer.srcAccessMask = 0;
			imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			//barrier the image into the transfer-receive layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

			//COPY
			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = image.extent;

			//copy the buffer into the image
			vkCmdCopyBufferToImage(cmd, stagingbuffer->getAPIResource(), m_textureImg, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

			VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

			imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				
			imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			//barrier the image into the shader readable layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
		});

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_textureImg;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if(vkCreateImageView(p_device->getDevice(), &createInfo, nullptr, &m_textureImgView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image views!");
		}
	}

	void Texture::destroy()
	{
		vkDestroySampler(p_device->getDevice(), m_sampler, nullptr);
		m_textureImgView = VK_NULL_HANDLE;
		
		vkDestroyImageView(p_device->getDevice(), m_textureImgView, nullptr);
		m_sampler = VK_NULL_HANDLE;

		vmaDestroyImage(p_device->getAllocator(), m_textureImg, m_textureAllocation);
		m_textureImg = VK_NULL_HANDLE;
		m_textureAllocation = VK_NULL_HANDLE;
	}

	bs::Device* Texture::getDevice() const noexcept
	{
		return p_device;
	}
}