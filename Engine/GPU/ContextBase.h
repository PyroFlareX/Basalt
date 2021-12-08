#pragma once

#include "../Types/Types.h"
#include "../Resources/Image.h"

struct GLFWwindow;

namespace bs
{
	class Device;

	class ContextBase
	{
	public:
		ContextBase(const std::string& title, const bs::vec2i& size);
		virtual ~ContextBase();

		//Init the graphics API
		virtual void initAPI();

		//Clear screen
		virtual void clear();
		//Swap buffers
		virtual void update();
		//Close the window
		virtual void close();

		//Returns whether the window is closed or wants to close
		bool isOpen() const;

		//Return the API handle for the window
		GLFWwindow* getContext();
		
		//Set the icon of the window
		void setIcon(const Image& icon);

		//Set the window title name
		void setTitleName(const std::string& title);

		//Get the window title
		const std::string& getWindowTitle() const;

		//Get the window resolution
		const bs::vec2i& getWindowSize() const;

		//Returns true if references to the window handles and such need to be refreshed
		bool needsRefresh() const;
		//To be used in conjunction with `needsRefresh()`; Call when the refresh is completed
		void setRefreshCompleted();

		//temp
		virtual void setDevicePtr(Device* p_device) = 0;

		//Temp but semi permanant
		virtual void setSwapchainStuff(void* ptr) = 0;
		
	protected:
		//Optional override, but not necessary
		//@TODO: consider having a style configurator or smth
		virtual void initImGui();

		//The API handle
		GLFWwindow* m_window;

		//The name of the window titlebar
		std::string m_window_name;

		//Size
		bs::vec2i m_size;

		//Triggered when a refresh of refs is needed externally
		bool m_needsRefresh;
		//Internal use flag
		bool m_wasResized;

	private:
		//@TODO: Move some stuff into here


	};
}
