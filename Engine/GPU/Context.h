#pragma once

#include "Vulkan/VulkanHelpers.h"

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

	~Context();

private: // For members
	GLFWwindow* m_window;

	VkInstance m_instance;
	VkPhysicalDevice phyDev;
	VkDevice m_device;

	SwapChainDetails m_scdetails;
	
	VkSurfaceKHR m_surface;

	VkQueue presentationQueue;
};

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

