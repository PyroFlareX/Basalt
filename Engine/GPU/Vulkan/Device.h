#pragma once

#include "VulkanHelpers.h"


namespace bs
{


	class Device
	{
	public:
		Device();
		~Device();

		//Init device
		void init();

		QueueFamilyIndices getQueueFamilies();
		SwapChainSupportDetails getSwapchainDetails();
		VkDevice& getDevice();
		VkPhysicalDevice& getPhysicalDevice() { return physDevice; }

		//Submit GFX work
		void submitWork(std::vector<VkCommandBuffer>& cmdbuffer);
		//Submit Data/Cmd buffer to GPU
		void submitImmediate(std::function<void(VkCommandBuffer cmd)>&& function);

		//Get VMA Allocator
		VmaAllocator& getAllocator();

		VkQueue getPresentQueue();

		VkQueue graphicsQueue;
		VkQueue presentQueue;

	private:
		VkPhysicalDevice physDevice;
		VkDevice device;

		VmaAllocator m_allocatorVMA;

		VkCommandPool m_pool;
	};

}