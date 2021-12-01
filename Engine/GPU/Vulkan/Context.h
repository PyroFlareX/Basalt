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
		~Context();
		
		void setIcon(Image& icon);

		//Clear screen
		void clear();
		//Swap buffers
		void update();
		//Close window 
		void close();
		//Init api, vulkan here
		void initAPI();

		//Checks if the window is open
		bool isOpen() const;

		GLFWwindow* getContext();

		void setDeviceptr(Device* p_device);

		VkRenderPass getGenericRenderpass() const;

		SwapChainDetails m_scdetails;

		bool resized = false;	//For Callback / internal use
		bool refresh = false;	//To let other stuff know to refresh its references to the render framebuffer
	private:
		///Internal Convenience functions

		//Inits imgui
		void initGui();

		void createSwapchain();
		
		void createContextRenderpass();
		
		void recreateSwapchain();
		void createDepthbuffer();

		///Window Vars
		//Window Handle
		GLFWwindow* m_window;
		//Device ptr
		Device* m_device;
		//Window title
		const std::string m_windowname;
		//Dimensions
		bs::vec2i m_size;

		///Vulkan Private Stuff

		//Swapchain things
		VkSwapchainKHR m_swapchain;
		//For depth stuff
		VkImage m_depthImg;
		VkImageView m_depthImgView;
		VmaAllocation m_depthImageAllocation;
		//Renderpass
		VkRenderPass m_renderpass;

		///Other Members

		//For the current index of frame in swapchain or something
		int currentFrame;

		//For Vulkan getting image index
		uint32_t imageIndex;
	};


	void resizeCallback(GLFWwindow* window, int width, int height);
}