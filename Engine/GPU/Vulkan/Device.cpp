#include "Device.h"


constexpr int MAX_FRAMES_IN_FLIGHTA = 3;

namespace vn
{
	Device::Device()
	{
		
	}

	void Device::init()
	{
		vn::vk::pickPhysicalDevice(physDevice);
		vn::vk::createLogicalDevice(device, physDevice, graphicsQueue, presentQueue);

		//initialize the memory allocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = physDevice;
		allocatorInfo.device = device;
		allocatorInfo.instance = vk::m_instance;

		vmaCreateAllocator(&allocatorInfo, &m_allocatorVMA);
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

	void Device::submitWork(std::vector<VkCommandBuffer>& cmdbuffer)
	{
		static int i = 0;

		VkResult result;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		
		VkSemaphore waitSemaphores[] = { vn::vk::imageAvailableSemaphores[i] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdbuffer[i];

		VkSemaphore signalSemaphores[] = { vn::vk::renderFinishedSemaphores[i] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		
		vkResetFences(device, 1, &vn::vk::inFlightFences[i]);

		result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, vn::vk::inFlightFences[i]);
		

		if (result != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		vkQueueWaitIdle(graphicsQueue);
		
		if (vn::vk::inFlightFences[i] != VK_NULL_HANDLE) 
		{
			vkWaitForFences(device, 1, &vn::vk::inFlightFences[i], VK_TRUE, 500000000);
		}

		// goes from 0 to the buffer count for the swapchain buffers
		i = (i + 1) % MAX_FRAMES_IN_FLIGHTA;
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