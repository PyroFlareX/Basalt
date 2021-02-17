#include "Context.h"


namespace vn
{
	Context::Context()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(WIDTH, HEIGHT, "Basalt", nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
	}

	void Context::clear()
	{

	}

	void Context::update()
	{
		glfwPollEvents();

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_Device.getDevice(), m_swapchain, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 0;
		presentInfo.pWaitSemaphores = nullptr;

		VkSwapchainKHR swapChains[] = { m_swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		VkResult result = vkQueuePresentKHR(m_Device.getPresentQueue(), &presentInfo);

		/*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
			framebufferResized = false;
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}*/
	}

	void Context::close()
	{
	}

	void Context::initAPI()
	{
		vn::vk::createInstance("Basalt");
		vn::vk::createSurface(m_window);
		vn::vk::createSwapChain(m_swapchain, m_Device, m_scdetails, m_window);


	}

	bool Context::isOpen()
	{
		return false;
	}

	GLFWwindow* Context::getContext()
	{
		return m_window;
	}

	void Context::setDeviceptr(Device* pdevice)
	{
		m_Device = *pdevice;
	}

	Context::~Context()
	{
		vkDestroyInstance(vn::vk::m_instance, nullptr);
	}
}