#pragma once

#include "../ContextBase.h"

//Include all the Vulkan stuff
#include "Device.h"
#include "Framebuffer.h"
#include "VModel.h"
#include "Texture.h"


namespace bs
{
	class VulkanContext	:	public ContextBase
	{
	public:
		VulkanContext(const std::string& title, const bs::vec2i& window_resolution);
		~VulkanContext();
		
		//Init API, it is vulkan for this
		void initAPI();

		//Clear screen
		void clear();
		//Swap buffers
		void update();
		//Close window 
		void close();

		//Temp
		// @TODO: Make something along the lines of "createWindowFromDevice" or something
		void setDevicePtr(Device* device_ptr) override;

		//Temp but semi permanant
		void setSwapchainStuff(void* ptr) override
		{
			auto& framebuf = *reinterpret_cast<vk::FramebufferData*>(ptr);
		
			framebuf.handle =	m_scdetails.swapChainFramebuffers;
			framebuf.imgView =	m_scdetails.swapChainImageViews.at(0);
			framebuf.size = 	getWindowSize();
		}

		VkRenderPass getGenericRenderpass() const;

	private:
		//Inits ImGui
		void initImGui();

		//Initializes the swapchain
		void createSwapchain();
		//Initializes the depthbuffer
		void createDepthbuffer();

		//Destroys and creates the swapchain
		void recreateSwapchain();
		
		//Create the basic generic renderpass
		// @TODO: Think about and maybe try to implement the new DYNAMIC_RENDERING extension
		void createContextRenderpass();

		//Pointer to the device
		Device* p_device;

		//Swapchain things
		VkSwapchainKHR m_swapchain;

		//Details
		SwapChainDetails m_scdetails;

		//For depth stuff
		VkImage m_depthImg;
		VkImageView m_depthImgView;
		VmaAllocation m_depthImageAllocation;
		//Renderpass
		VkRenderPass m_renderpass;

		//For the current index of frame in swapchain or something
		u32 currentFrame;

		//For Vulkan getting image index
		u32 imageIndex;

		friend void resizeCallback(GLFWwindow*, int, int);
	};

	void resizeCallback(GLFWwindow* window, int width, int height);
}