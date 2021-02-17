#pragma once

#include "Vulkan/Device.h"

namespace vn
{
	class Context
	{
	public:
		Context();

		void clear();
		void update();
		void close();
		void initAPI();
		bool isOpen();

		GLFWwindow* getContext();
		void setDeviceptr(Device* pdevice);

		~Context();

		SwapChainDetails m_scdetails;
	private: // For members
		GLFWwindow* m_window;

		Device m_Device;

		//SwapChainDetails m_scdetails;
		VkSwapchainKHR m_swapchain;
	};
}
constexpr int WIDTHA = 800;
constexpr int HEIGHTA = 600;
constexpr int MAX_FRAMES_IN_FLIGHTA = 2;

