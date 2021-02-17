#include "Device.h"

namespace vn
{
	Device::Device()
	{
		vn::vk::pickPhysicalDevice(physDevice);
		vn::vk::createLogicalDevice(device, physDevice, graphicsQueue, presentQueue);
	}

	QueueFamilyIndices Device::getQueueFamilies()
	{
		return vn::vk::findQueueFamilies(physDevice);
	}

	SwapChainSupportDetails Device::getSwapchainDetails()
	{
		return vn::vk::querySwapChainSupport(physDevice);
	}

	VkDevice& Device::getDevice()
	{
		return device;
	}

	void Device::submitWork(VkCommandBuffer& cmdbuffer)
	{
		//vkQueueSubmit(graphicsQueue, )
	}
	VkQueue Device::getPresentQueue()
	{
		return presentQueue;
	}
}