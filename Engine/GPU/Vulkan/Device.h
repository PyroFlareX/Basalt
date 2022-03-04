#pragma once

#include <mutex>
#include <functional>

#include "VulkanHelpers.h"
#include "Texture.h"
#include "Buffer.h"

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

		//Creation Commands
		vk::Buffer createBuffer();
		vk::Texture createTexture();

		//Submit GFX work
		void submitWork(std::vector<VkCommandBuffer>& cmdbuffer);
		//Submit Data/Cmd buffer to GPU
		void submitImmediate(std::function<void(VkCommandBuffer cmd)>&& submitCmd);

		//Resource Destruction stuff, only use for lazy cleanup, manual/RAII is preferred
		void addCleanupCall(std::function<void()>&& function);

	private:
		static int getScore(VkPhysicalDevice device, const std::vector<const char*>& requiredDeviceExtensions, 
								const std::vector<const char*>& optionalDeviceExtensions);

		static std::vector<std::pair<VkPhysicalDevice, int>> getPhysicalDevices(
				const std::vector<const char*>& requiredDeviceExtensions, 
				const std::vector<const char*>& optionalDeviceExtensions);

		void createDevice();

		//Physical Device
		VkPhysicalDevice physDevice;
		//Device Handle
		VkDevice device;

		//Allocator for buffers and resources
		VmaAllocator m_allocatorVMA;

		//Command Pool for submission and any other commands needed
		VkCommandPool m_pool;

		//Fence for waiting on upload cmds
		VkFence m_uploadFence;

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
		//other thing
		u64 frame = 0;
	};
}