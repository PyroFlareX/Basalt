#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <algorithm> //std::min & max
#include <vector>
#include <set>
#include <optional>


struct QueueFamilyIndicies
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Application
{
public:
	void run();

	//Init
	void initWindow();
	void initVulkan();

	//Presentation
	void createSurface();
	void createSwapchain();
	void createImageViews();
	//Swapchain Helper Functions
	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	//Create Vulkan VkInstance and set it up
	void createInstance();

	//These functions choose (a) suitable device(s) from those available
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	//Logical Device Creation
	void createLogicalDevice();
	
	//Pipeline Stuff
	void createGraphicsPipeline();
	//Pipeline Helper Functions
	VkShaderModule createShaderModule(const std::vector<char>& code);

	//Find suitable queue families
	QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice device);

	//Main Rendering Loop
	void mainLoop();

	//Destroys resources
	void cleanup();

private:
	//Context
	GLFWwindow* window;

	//Vulkan Objects
	VkInstance instance;				//Vulkan Instance
	VkPhysicalDevice physicalDevice;	//Physical Rendering Device
	VkDevice vDevice;					//Logical Device
	VkQueue graphicsQueue;				//Vulkan queue for graphics
	VkQueue presentQueue;				//Vulkan queue for presentation

	//Windowing
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;

	//Pipelines
	VkPipelineLayout pipelineLayout;

	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;
};

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};