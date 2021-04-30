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
		VkPhysicalDevice& getPhysicalDevice() { return physDevice; }

		void submitWork(std::vector<VkCommandBuffer>& cmdbuffer);
		VmaAllocator& getAllocator();

		VkQueue getPresentQueue();
	private:
		VkPhysicalDevice physDevice;
		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VmaAllocator m_allocatorVMA;
	};

}