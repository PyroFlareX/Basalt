#include "Context.h"


namespace vn
{
	Context::Context()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(WIDTHA, HEIGHTA, "Basalt", nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
	}

	void Context::clear()
	{
//		if(!started)
//		{
			VkResult result = vkAcquireNextImageKHR(m_Device->getDevice(), m_swapchain, UINT64_MAX, vn::vk::imageAvailableSemaphores[0], VK_NULL_HANDLE, &imageIndex);
//			started = !started;
//		}
	}

	void Context::update()
	{
		glfwPollEvents();

		//static uint32_t imageIndex;
		VkResult result;

		

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		VkSemaphore signalSemaphores[] = { vn::vk::renderFinishedSemaphores[0] };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		std::cout << "IMAGE INDEX: " << imageIndex << "\n";

		result = vkQueuePresentKHR(m_Device->getPresentQueue(), &presentInfo);

		//framebuffer = !framebuffer;
	}

	void Context::close()
	{
		
	}

	void Context::initAPI()
	{
		vn::vk::createInstance("Basalt");
		vn::vk::createSurface(m_window);
		m_Device->init();
		vn::vk::createSwapChain(m_swapchain, *m_Device, m_scdetails, m_window);
		vn::vk::createImageViews(m_scdetails, m_Device->getDevice());

		//INIT SYNCH PRIMITIVES
		vn::vk::imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHTA);
		vn::vk::renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHTA);
		vn::vk::inFlightFences.resize(MAX_FRAMES_IN_FLIGHTA);
		vn::vk::imagesInFlight.resize(m_scdetails.swapChainImages.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHTA; i++) {
			if (vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &vn::vk::imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &vn::vk::renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_Device->getDevice(), &fenceInfo, nullptr, &vn::vk::inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	bool Context::isOpen()
	{
		if (glfwWindowShouldClose(m_window))
		{
			return false;
		}
		return true;
	}

	GLFWwindow* Context::getContext()
	{
		return m_window;
	}

	void Context::setDeviceptr(Device* pdevice)
	{
		m_Device = pdevice;
	}

	Context::~Context()
	{
		vkDestroyInstance(vn::vk::m_instance, nullptr);
		glfwDestroyWindow(m_window);
	}
}