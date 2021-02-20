#pragma once

#include "VulkanHelpers.h"

namespace vn
{

	class Device
	{
	public:
		Device();

		void init();

		QueueFamilyIndices getQueueFamilies();
		SwapChainSupportDetails getSwapchainDetails();
		VkDevice& getDevice();

		void submitWork(std::vector<VkCommandBuffer>& cmdbuffer);

		VkQueue getPresentQueue();
	private:
		VkPhysicalDevice physDevice;
		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;
	};

}