#include "Device.h"

namespace vn
{
	Device::Device()
	{
		
	}

	void Device::init()
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
		static int i = 1;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { vn::vk::imageAvailableSemaphores[0] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdbuffer;

		VkSemaphore signalSemaphores[] = { vn::vk::renderFinishedSemaphores[0] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vn::vk::inFlightFences[i - 1] != VK_NULL_HANDLE) 
		{
			vkWaitForFences(device, 1, &vn::vk::inFlightFences[i - 1], VK_TRUE, 500000000);
		}
		
		vkResetFences(device, 1, &vn::vk::inFlightFences[i - 1]);


		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, vn::vk::inFlightFences[i - 1]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		

	}
	VkQueue Device::getPresentQueue()
	{
		return presentQueue;
	}
}