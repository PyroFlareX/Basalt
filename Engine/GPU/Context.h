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

		Device* m_Device;

		//SwapChainDetails m_scdetails;
		VkSwapchainKHR m_swapchain;

		// //true if rendering on image 2, false if on 1
		bool started = false;
		int currentFrame = 0;

		uint32_t imageIndex;
	};
}
constexpr int WIDTHA = 800;
constexpr int HEIGHTA = 600;
constexpr int MAX_FRAMES_IN_FLIGHTA = 3;

