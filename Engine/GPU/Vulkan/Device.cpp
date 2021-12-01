#include "Device.h"



namespace bs
{
	Device::Device()
	{
		
	}

	Device::~Device() 
	{
		vkDestroyCommandPool(device, m_pool, nullptr);
		vkDestroyDevice(device, nullptr);
	}

	void Device::init()
	{
		bs::vk::pickPhysicalDevice(physDevice);
		bs::vk::createLogicalDevice(device, physDevice, graphicsQueue, presentQueue);

		//initialize the memory allocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = physDevice;
		allocatorInfo.device = device;
		allocatorInfo.instance = vk::m_instance;

		vmaCreateAllocator(&allocatorInfo, &m_allocatorVMA);

		bs::vk::createCommandPool(*this, m_pool);
	}

	QueueFamilyIndices Device::getQueueFamilies()
	{
		return bs::vk::findQueueFamilies(physDevice);
	}

	SwapChainSupportDetails Device::getSwapchainDetails()
	{
		return bs::vk::querySwapChainSupport(physDevice);
	}

	VkDevice& Device::getDevice()
	{
		return device;
	}

	void Device::submitWork(std::vector<VkCommandBuffer>& cmdbuffer)
	{
		static int i = 0;

		VkResult result;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		
		VkSemaphore waitSemaphores[] = { bs::vk::imageAvailableSemaphores[i] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;


		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdbuffer[0];


		VkSemaphore signalSemaphores[] = { bs::vk::renderFinishedSemaphores[i] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		
		vkResetFences(device, 1, &bs::vk::inFlightFences[i]);
		result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, bs::vk::inFlightFences[i]);
		

		if (result != VK_SUCCESS) 
		{
			std::cout << "Queue Submission error: " << result << "\n";
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		
		vkQueueWaitIdle(graphicsQueue);
		
		if (bs::vk::inFlightFences[i] != VK_NULL_HANDLE) 
		{
			vkWaitForFences(device, 1, &bs::vk::inFlightFences[i], VK_TRUE, 500000000);
		}

		// goes from 0 to the buffer count for the swapchain buffers
		i = (i + 1) % bs::vk::NUM_SWAPCHAIN_FRAMEBUFFERS;
	}
	//NO GFX SHIT BC BREAKS AND SYNCH AND PAIN AND CRASHED DRIVERS
	void Device::submitImmediate(std::function<void(VkCommandBuffer cmd)>&& function)
	{
		VkCommandBuffer cmdbuffer;

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_pool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device, &allocInfo, &cmdbuffer) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmdbuffer, &beginInfo);

		function(cmdbuffer);

		vkEndCommandBuffer(cmdbuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 0;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdbuffer;

		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		vkQueueWaitIdle(graphicsQueue);
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkResetCommandPool(device, m_pool, 0);
	}

	VmaAllocator& Device::getAllocator()
	{
		return m_allocatorVMA;
	}

	VkQueue Device::getPresentQueue()
	{
		return presentQueue;
	}
}