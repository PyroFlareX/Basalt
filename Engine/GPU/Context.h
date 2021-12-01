#pragma once

// #define BASE_CONTEXT
#ifndef BASE_CONTEXT
#include "Vulkan/Context.h"
#else

#include "../Types/Types.h"
#include "../Resources/Image.h"
#include <GLFW/glfw3.h>

namespace bs
{
	class ContextBase
	{
	public:
		ContextBase(const std::string& title, bs::vec2i size);
		~ContextBase();

		//Clear screen
		void clear();
		//Swap buffers
		void update();
		//Close the window
		void close();

		//Returns whether the window is closed or wants to close
		//Do not override
		bool isOpen() const;

		//Init the graphics API
		void initAPI();

		//Return the API handle for the window
		//Do not override
		GLFWwindow* getContext();
		
		//Set the icon of the window
		//Do not override
		void setIcon(Image& icon);

		//Set the window title name
		//Do not override
		void setTitleName(const std::string& title);
		//Get the window title
		//Do not override
		const std::string& getWindowTitle() const;

		//Returns true if references to the window handles and such need to be refreshed
		//Do not override
		bool needsRefresh() const;
		//To be used in conjunction with `needsRefresh()`, call when the refresh is completed
		//Do not override
		void setRefreshCompleted();

		//temp
		void setDeviceptr(Device* pdevice);
		
	protected:
		void initImGui();

		//The API handle
		GLFWwindow* m_window;

		//Internal use flag
		bool m_wasResized;
		//Triggered when a refresh of refs is needed externally
		bool m_needsRefresh;

		//The name of the window titlebar
		std::string m_windowname;

		//Size
		bs::vec2i m_size;

	private:
	
		void recreateSwapchain();
		
		Device* m_Device;

		VkRenderPass* rpass;

		SwapChainDetails m_scdetails;

		VkSwapchainKHR m_swapchain;

		// //true if rendering on image 2, false if on 1
		bool started = false;
		int currentFrame = 0;

		uint32_t imageIndex;
	};
}

#endif