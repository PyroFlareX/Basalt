#pragma once

#include "VulkanHelpers.h"

#include <mutex>

namespace bs
{
	class Device
	{
	public:
		Device();
		~Device();

		//Init device
		void init();
		//destroy
		void destroy();

		VkDevice& getDevice();

		VkDevice getDevice() const;
		VkPhysicalDevice getPhysicalDevice() const;

		//Get VMA Allocator
		VmaAllocator& getAllocator();

		//Get queues
		VkQueue getPresentQueue() const;
		VkQueue getGraphicsQueue() const;

		//Other stuff
		QueueFamilyIndices getQueueFamilies() const;
		SwapChainSupportDetails getSwapchainDetails() const;

		//Submit GFX work
		void submitWork(std::vector<VkCommandBuffer>& cmdbuffer);
		//Submit Data/Cmd buffer to GPU
		void submitImmediate(std::function<void(VkCommandBuffer cmd)>&& function);

		//Resource Destruction stuff, only use for lazy cleanup, manual/RAII is preferred
		void addCleanupCall(std::function<void()>&& function);

	private:
		int getScore(VkPhysicalDevice device) const;

		std::vector<std::pair<VkPhysicalDevice, int>> getPhysicalDevices() const;

		void createDevice();

		//Physical Device
		VkPhysicalDevice physDevice;
		//Device Handle
		VkDevice device;

		//Allocator for buffers and resources
		VmaAllocator m_allocatorVMA;

		//Command Pool for submission and any other commands needed
		VkCommandPool m_pool;

		//Queues
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		//Extensions
		std::vector<const char*> requiredDeviceExtensions;
		std::vector<const char*> optionalDeviceExtensions;

		//Destruction queue
		std::vector<std::function<void()>> m_resourceCleanupQueue;

		//Mutex for if exclusive access is needed
		std::mutex m_device_lock;

		//Flag to show if the device was destroyed already
		bool destroyed;
	};

}