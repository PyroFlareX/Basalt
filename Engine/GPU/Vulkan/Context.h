#pragma once

#include "Device.h"
#include "Framebuffer.h"
#include "VModel.h"
#include "Texture.h"

namespace bs
{
	class Context
	{
	public:
		Context(const std::string& title);
		void setIcon(Image& icon);

		void clear(); //Clear screen
		void update(); //Swap buffers
		void close(); //Close window
		void initAPI(); //Init api, vulkan here
		bool isOpen();

		GLFWwindow* getContext();
		void setDeviceptr(Device* pdevice);

		~Context();

		SwapChainDetails m_scdetails;

		bool resized = false;	//For Callback / internal use
		bool refresh = false;	//To let other stuff know to refresh its references to the render framebuffer
		VkRenderPass rpass;
	private:
		void recreateSwapchain();

		GLFWwindow* m_window;
		Device* m_Device;

		VkSwapchainKHR m_swapchain;

		// //true if rendering on image 2, false if on 1
		bool started = false;
		int currentFrame = 0;

		uint32_t imageIndex;
		const std::string m_windowname;
	};


	void resizeCallback(GLFWwindow* window, int width, int height);
}