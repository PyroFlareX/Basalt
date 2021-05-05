#include "Texture.h"

vn::vk::Texture::Texture(vn::Device* device)
{
	p_device = device;
	// Create the img

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	vkCreateSampler(device->getDevice(), &samplerInfo, nullptr, &sampler);
}

void vn::vk::Texture::loadFromImage(vn::Image& img)
{
	//Upload the img
	
	//Allocation Info
	VmaAllocationCreateInfo imgAllocInfo = {};
	imgAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	//imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


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
	image.flags = 0;
	image.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	image.tiling = VK_IMAGE_TILING_LINEAR;//VK_IMAGE_TILING_OPTIMAL;
	image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateImage(p_device->getDevice(), &image, nullptr, &textureImg);
	std::cout << result << " THIS IS THE BUG BUDDY! \n";

	VkDeviceSize offset = 0;
	void* texture = nullptr;
	size_t sizeImg = sizeof(vn::u8vec4) * (int)img.getSize().x * (int)img.getSize().y;
	VkDeviceSize sizeDev = sizeImg;

	VkMemoryAllocateInfo meminfo{};
	meminfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	

	VkMemoryRequirements memreqs;
	vkGetImageMemoryRequirements(p_device->getDevice(), textureImg, &memreqs);

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(p_device->getPhysicalDevice(), &memProperties);

	uint32_t index = 0;
	meminfo.memoryTypeIndex = 2;
	// @TODO: FINISH THE MEM STUFF
	for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		if((memreqs.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)))// == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) 
		{
			index = i;
			meminfo.memoryTypeIndex = i;
			std::cout << i << " This is i\n";
			break;
		}
	}

	meminfo.allocationSize = memreqs.size;//sizeDev;
	

	result = vkAllocateMemory(p_device->getDevice(), &meminfo, nullptr, &deviceMem);
	std::cout << result << " THIS IS THE BUG BUDDYAlloc! \n";


	result = vkBindImageMemory(p_device->getDevice(), textureImg, deviceMem, 0);
	std::cout << result << " THIS IS THE BUG BUDDYBIND! \n";

	

	result = vkMapMemory(p_device->getDevice(), deviceMem, offset, sizeDev, 0, &texture);
	std::cout << result << " THIS IS THE BUG BUDDYMAP! \n";

	memcpy(texture, img.getPixelsPtr(), sizeImg);

	vkUnmapMemory(p_device->getDevice(), deviceMem);
	


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
