#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <algorithm> //std::min & max
#include <vector>
#include <cstdint>
#include <set>
#include <optional>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

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

	VkInstance instance;
	VkSurfaceKHR surface;

	bool enableValidationLayers;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const std::vector<const char*> validationLayers = 
	{
		"VK_LAYER_KHRONOS_validation"
	};


private: // For Functions
	
};

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

