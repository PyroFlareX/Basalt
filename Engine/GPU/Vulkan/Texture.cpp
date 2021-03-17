#include "Texture.h"

vn::vk::Texture::Texture(vn::Device* device)
{
	p_device = device;
	// Create the img

	
}

void vn::vk::Texture::loadFromImage(vn::Image& img)
{
	//Upload the img
	
	//Allocation Info
	VmaAllocationCreateInfo imgAllocInfo = {};
	imgAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	// CREATE IMAGE
	VkImageCreateInfo image{};
	image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image.imageType = VK_IMAGE_TYPE_2D;
	image.format = VK_FORMAT_B8G8R8A8_SRGB;
	image.extent.height = (int)img.getSize().y;
	image.extent.width = (int)img.getSize().x;
	image.extent.depth = 1;
	image.mipLevels = 1;
	image.arrayLayers = 1;
	image.samples = VK_SAMPLE_COUNT_1_BIT;
	image.flags = VK_IMAGE_ASPECT_COLOR_BIT;
	image.tiling = VK_IMAGE_TILING_OPTIMAL;
	image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// We will sample directly from the color attachment
	image.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vmaCreateImage(p_device->getAllocator(), &image, &imgAllocInfo, &textureImg, &textureAllocation, nullptr);
	std::cout << result << " THIS IS THE BUG BUDDY! \n";

	

	// MAP TEXTURE
	void* texture;
	vmaMapMemory(p_device->getAllocator(), textureAllocation, &texture);

	size_t numpixels = img.getSize().x * img.getSize().y;

	memcpy(texture, img.getPixelsPtr(), numpixels * sizeof(vn::u8vec4));

	vmaUnmapMemory(p_device->getAllocator(), textureAllocation);
	


	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = textureImg;
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

	if (vkCreateImageView(p_device->getDevice(), &createInfo, nullptr, &textureImgView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image views!");
	}
}
