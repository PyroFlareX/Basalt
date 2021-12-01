#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VMA/vk_mem_alloc.h"

#include "../../Resources/Mesh.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <algorithm> //std::min & max
#include <vector>
#include <cstdint>
#include <set>
#include <optional>

namespace bs
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

struct VertexInputDescription 
{
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct PushConstantsStruct
{
	bs::vec4 textureids;
};

struct PushConstantsIMGUI
{
	bs::vec2 scale;
	bs::vec2 translate;
	bs::vec4 textureID;
};


struct SwapChainDetails
{
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
};



namespace bs::vk
{
	//GLOBALS
	extern short NUM_SWAPCHAIN_FRAMEBUFFERS;

	//For Window Resizing
	
	extern int viewportwidth;
	extern int viewportheight;

	extern VkInstance m_instance;
	extern VkSurfaceKHR m_surface;
	extern bool validationlayers;
	extern VkDebugUtilsMessengerEXT debugMessenger;

	//GLOBALS FOR SYNCRONIZATION
	extern std::vector<VkSemaphore> imageAvailableSemaphores;
	extern std::vector<VkSemaphore> renderFinishedSemaphores;
	extern std::vector<VkFence> inFlightFences;
	extern std::vector<VkFence> imagesInFlight;
	
	class RenderTargetFramebuffer;

	//Creates an instance of Vulkan
	void createInstance(const std::string& name);
	//Creates a renderable surface for Vulkan
	void createSurface(GLFWwindow* window);

	void pickPhysicalDevice(VkPhysicalDevice& physicalDevice);
	void createLogicalDevice(VkDevice& device, VkPhysicalDevice& physicalDevice, VkQueue& graphicsQueue, VkQueue& presentQueue);

	void createSwapChain(VkSwapchainKHR& swapchain, bs::Device& device, SwapChainDetails& swapdetails, GLFWwindow* window);
	void createImageViews(SwapChainDetails& swapdetails, VkDevice device);
	void recreateSwapChain(GLFWwindow* window, VkDevice device);

	void createRenderPass(VkDevice& device, SwapChainDetails& Swapdetails, VkRenderPass& renderPass);
	void createGraphicsPipeline(VkRenderPass& renderPass,/* SwapChainDetails& swapdetails, */VkPipelineLayout& pipelineLayout, VkPipeline& graphicsPipeline, VkDevice device);

	void createUIPipeline(bs::Device& device, VkPipeline& pipeline, VkRenderPass& rpass, VkPipelineLayout& playout, VkDescriptorSetLayout& dlayout);

	void createPipelineDefault(bs::Device& device, VkPipeline& pipeline, VkRenderPass& rpass, VkPipelineLayout& playout, VkDescriptorSetLayout& dlayout);
	void createPipeline(bs::Device& device, VkPipeline& pipeline, VkRenderPass& rpass, VkPipelineLayout& playout, VkDescriptorSetLayout& dlayout, const std::string& vertpath, const std::string& fragpath);

	void createFramebuffers(VkRenderPass& renderPass, SwapChainDetails& swapdetails, VkDevice device);
	void createCommandPool(bs::Device& device, VkCommandPool& commandPool);

	VertexInputDescription getVertexDescription();
	VertexInputDescription getVertexDescriptionImGUI();



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

	//VALIDATION LAYER STUFF
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool checkValidationLayerSupport();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT * pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);


	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};
}