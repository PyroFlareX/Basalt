#pragma once

#include "VulkanHelpers.h"

namespace vn
{

	class Device
	{
	public:
		Device();

		QueueFamilyIndices getQueueFamilies();
		SwapChainSupportDetails getSwapchainDetails();
		VkDevice& getDevice();

		void submitWork(VkCommandBuffer& cmdbuffer);

		VkQueue getPresentQueue();
	private:
		VkPhysicalDevice physDevice;
		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;
	};

}