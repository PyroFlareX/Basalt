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

namespace vn
{
	class Device;
}
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct SwapChainDetails
{
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
};

namespace vn::vk
{
	//GLOBALS
	extern VkInstance m_instance;
	extern VkSurfaceKHR m_surface;


	//Creates an instance of Vulkan
	void createInstance(std::string name);
	//Creates a renderable surface for Vulkan
	void createSurface(GLFWwindow* window);

	void pickPhysicalDevice(VkPhysicalDevice& physicalDevice);
	void createLogicalDevice(VkDevice& device, VkPhysicalDevice& physicalDevice, VkQueue& graphicsQueue, VkQueue& presentQueue);

	void createSwapChain(VkSwapchainKHR& swapchain, vn::Device& device, SwapChainDetails& swapdetails, GLFWwindow* window);
	void createImageViews(SwapChainDetails& swapdetails, VkDevice device);
	void recreateSwapChain(GLFWwindow* window, VkDevice device);

	void createRenderPass(VkDevice device, SwapChainDetails& swapdetails, VkRenderPass renderPass);
	void createGraphicsPipeline(VkRenderPass& renderPass, SwapChainDetails& swapdetails, VkPipelineLayout& pipelineLayout, VkPipeline& graphicsPipeline, VkDevice device);

	void createFramebuffers(VkRenderPass& renderPass, SwapChainDetails& swapdetails, VkDevice device);
	void createCommandPool(vn::Device& device, VkCommandPool& commandPool);
	void createCommandBuffers(VkDevice device, VkCommandPool& commandPool, SwapChainDetails& swapdetails, VkPipeline graphicsPipeline, VkRenderPass renderPass, std::vector<VkCommandBuffer>& commandBuffers);


	//Dont use
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	//Dont use
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	//Dont use
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	//Dont use
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	//Dont use
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
	//Dont use
	VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);
	
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	std::vector<const char*> getRequiredExtensions();


	// TODO IDK HOW TO WITHOUT GLOBALS
	void cleanup();


	// ALSO THIS TODO
	void cleanupSwapChain();

	








	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};
}