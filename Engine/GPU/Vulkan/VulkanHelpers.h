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
	void createInstance(VkInstance& instance, std::string name);

	void createSurface(VkInstance& instance, GLFWwindow* window, VkSurfaceKHR& surface);

	void pickPhysicalDevice(VkInstance& instance, VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface);
	void createLogicalDevice(VkDevice& device, VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentQueue);

	void createSwapChain(VkSwapchainKHR& swapchain, VkSurfaceKHR surface, VkPhysicalDevice& physicalDevice, VkDevice& device, SwapChainDetails& swapdetails, GLFWwindow* window);
	void createImageViews(SwapChainDetails& swapdetails, VkDevice device);
	void recreateSwapChain(GLFWwindow* window, VkDevice device);

	void createRenderPass(VkDevice device, SwapChainDetails& swapdetails, VkRenderPass renderPass);
	void createGraphicsPipeline(VkRenderPass& renderPass, SwapChainDetails& swapdetails, VkPipelineLayout& pipelineLayout, VkPipeline& graphicsPipeline, VkDevice device);

	void createFramebuffers(VkRenderPass& renderPass, SwapChainDetails& swapdetails, VkDevice device);
	void createCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool& commandPool);
	void createCommandBuffers(VkDevice device, VkCommandPool& commandPool, SwapChainDetails& swapdetails, VkPipeline graphicsPipeline, VkRenderPass renderPass, std::vector<VkCommandBuffer>& commandBuffers)
	{
		commandBuffers.resize(swapdetails.swapChainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for (size_t i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = swapdetails.swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapdetails.swapChainExtent;

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
	VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);
	
	// Screw it just gonna pass in the surface although it should technically not be necessary, but whatever
	bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	std::vector<const char*> getRequiredExtensions();

	void cleanup();

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