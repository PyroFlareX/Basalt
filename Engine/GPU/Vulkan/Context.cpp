#include "Context.h"

#include <imgui.h>
#include "VulkanHelpers.h"
#include <GLFW/glfw3.h>


namespace bs
{
	VulkanContext::VulkanContext(const std::string& title, const bs::vec2i& window_resolution)	:	ContextBase(title, window_resolution), currentFrame(0)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow( m_size.x, m_size.y, m_window_name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
	}

	VulkanContext::~VulkanContext()
	{
		//Local objects destruction
		//Destroy Depth Image
		vmaDestroyImage(p_device->getAllocator(), m_depthImg, m_depthImageAllocation);
		vkDestroyImageView(p_device->getDevice(), m_depthImgView, nullptr);
		//Destroy Render Pass
		vkDestroyRenderPass(p_device->getDevice(), m_renderpass, nullptr);

		///Beginning full destruction

		//Destroy Sync Primitives
		for (auto& fence : bs::vk::inFlightFences)
		{
			vkDestroyFence(p_device->getDevice(), fence, nullptr);
		}
		for (auto& semaphore : bs::vk::renderFinishedSemaphores) 
		{
			vkDestroySemaphore(p_device->getDevice(), semaphore, nullptr);
		}
		for (auto& semaphore : bs::vk::imageAvailableSemaphores) 
		{
			vkDestroySemaphore(p_device->getDevice(), semaphore, nullptr);
		}

		//Destroy swapchain framebuffers & image views
		for (int i = 0; i < m_scdetails.swapChainFramebuffers.size(); ++i) 
		{
			vkDestroyFramebuffer(p_device->getDevice(), m_scdetails.swapChainFramebuffers[i], nullptr);
		}

		for (int i = 0; i < m_scdetails.swapChainImageViews.size(); ++i)
		{
			vkDestroyImageView(p_device->getDevice(), m_scdetails.swapChainImageViews[i], nullptr);
		}
		//Destroy swapchain and render surface
		vkDestroySwapchainKHR(p_device->getDevice(), m_swapchain, nullptr);
		vkDestroySurfaceKHR(bs::vk::m_instance, bs::vk::m_surface, nullptr);

		p_device->destroy();
		
		//glfwDestroyWindow(m_window);
		//glfwTerminate();
		
		vkDestroyInstance(bs::vk::m_instance, nullptr);
	}

	void VulkanContext::clear()
	{
		ContextBase::clear();

		// Acquire the INDEX into the swapchain for the next image
		VkResult result = vkAcquireNextImageKHR(p_device->getDevice(), m_swapchain, UINT64_MAX, bs::vk::imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if(result == VK_ERROR_OUT_OF_DATE_KHR) 
		{
			recreateSwapchain();
			m_needsRefresh = true;
		} 
		else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image!");
		}
	}

	void VulkanContext::update()
	{
		ContextBase::update();
		// Waits to present until the "render finished" semaphore (signal) is "signaled" (when the render is done, the semaphore is triggered by)
		VkSemaphore signalSemaphore = bs::vk::renderFinishedSemaphores[currentFrame];

		const VkPresentInfoKHR presentInfo
		{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,

			//Wait Semaphores
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &signalSemaphore,

			//Pass Swapchain
			.swapchainCount = 1,
			.pSwapchains = &m_swapchain,
			//Image Index
			.pImageIndices = &imageIndex,

			.pResults = nullptr,
		};

		// Submit Image that just finished rendering to the presentation surface
		const VkResult result = vkQueuePresentKHR(p_device->getPresentQueue(), &presentInfo);

		if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
		{
			recreateSwapchain();
			m_needsRefresh = true;
		} 
		else if(result != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

		// Increase Frame Index
		currentFrame = (currentFrame + 1) % bs::vk::NUM_SWAPCHAIN_FRAMEBUFFERS;
	}

	void VulkanContext::close()
	{
		ContextBase::close();
	}

	void VulkanContext::initAPI()
	{
		ContextBase::initAPI();

		bs::vk::createInstance("Bathsalts");
		bs::vk::createSurface(m_window);
		p_device->init();

		createContextRenderpass();
		createSwapchain();

		//INIT SYNCH PRIMITIVES
		bs::vk::imageAvailableSemaphores.resize(bs::vk::NUM_SWAPCHAIN_FRAMEBUFFERS);
		bs::vk::renderFinishedSemaphores.resize(bs::vk::NUM_SWAPCHAIN_FRAMEBUFFERS);
		bs::vk::inFlightFences.resize(bs::vk::NUM_SWAPCHAIN_FRAMEBUFFERS);
		bs::vk::imagesInFlight.resize(m_scdetails.swapChainImages.size(), VK_NULL_HANDLE);

		constexpr VkSemaphoreCreateInfo semaphoreInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		constexpr VkFenceCreateInfo fenceInfo
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};

		for(auto i = 0; i < bs::vk::NUM_SWAPCHAIN_FRAMEBUFFERS; ++i) 
		{
			if(	vkCreateSemaphore(p_device->getDevice(), &semaphoreInfo, nullptr, &bs::vk::imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(p_device->getDevice(), &semaphoreInfo, nullptr, &bs::vk::renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(p_device->getDevice(), &fenceInfo, nullptr, &bs::vk::inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create synchronization objects for a frame!");
			}
		}
	}

	void VulkanContext::setDevicePtr(Device* device_ptr)
	{
		p_device = device_ptr;
	}

	VkRenderPass VulkanContext::getGenericRenderpass() const
	{
		return m_renderpass;
	}

	void VulkanContext::initImGui()
	{
		//Start IMGUI init
		IMGUI_CHECKVERSION();
		auto* ctximgui = ImGui::CreateContext();
		ImGui::SetCurrentContext(ctximgui);

		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2(static_cast<float>(bs::vk::viewportwidth), static_cast<float>(bs::vk::viewportheight));
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;

		//Setup style
		ImGui::StyleColorsDark();
	}

	void VulkanContext::createSwapchain()
	{
		//Create Swapchain
		bs::vk::createSwapChain(m_swapchain, *p_device, m_scdetails, m_window);

		//Create imageviews
		bs::vk::createImageViews(m_scdetails, p_device->getDevice());

		createDepthbuffer();

		//Create framebuffer
		bs::vk::createFramebuffersWithDepth(getGenericRenderpass(), m_scdetails, p_device->getDevice(), m_depthImgView);
	}
		
	void VulkanContext::createContextRenderpass()
	{
		//Attachment to draw colors
		constexpr VkAttachmentDescription colorAttachment
		{
			.flags = 0,
			.format = VK_FORMAT_B8G8R8A8_SRGB,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};
		
		//Attachment to have depth
		constexpr auto depthFormat = VK_FORMAT_D32_SFLOAT;
		constexpr VkAttachmentDescription depthAttachment
		{
			.flags = 0,
			.format = depthFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};
		
		//Array to hold the 2
		const VkAttachmentDescription attachmentDescriptions[2] = {colorAttachment, depthAttachment};

		//Ref to color
		const VkAttachmentReference colorAttachmentRef
		{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		//Ref to depth
		const VkAttachmentReference depthAttachmentRef
		{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 2;
		renderPassInfo.pAttachments = &attachmentDescriptions[0];
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkRenderPass renderpass;
		if (vkCreateRenderPass(p_device->getDevice(), &renderPassInfo, nullptr, &renderpass) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create render pass!");
		}

		m_renderpass = renderpass;
	}

	void VulkanContext::recreateSwapchain()
	{
		bs::vec2i size{0, 0};
		glfwGetFramebufferSize(m_window, &size.x, &size.y);
		while (size.x == 0 || size.y == 0) 
		{
			glfwGetFramebufferSize(m_window, &size.x, &size.y);
			glfwWaitEvents();
		}
		
		bs::vk::viewportwidth = size.x;
		bs::vk::viewportheight = size.y;
		m_size = size;
		ImGui::GetIO().DisplaySize = { static_cast<float>(m_size.x), static_cast<float>(m_size.y) };

		vkDeviceWaitIdle(p_device->getDevice());

		//Cleanup swapchain + stuff
		for(auto i = 0; i < m_scdetails.swapChainFramebuffers.size(); ++i) 
		{
			vkDestroyFramebuffer(p_device->getDevice(), m_scdetails.swapChainFramebuffers[i], nullptr);
		}

		for(auto i = 0; i < m_scdetails.swapChainImageViews.size(); ++i)
		{
			vkDestroyImageView(p_device->getDevice(), m_scdetails.swapChainImageViews[i], nullptr);
		}

		//Clean depth buffer
		vmaDestroyImage(p_device->getAllocator(), m_depthImg, m_depthImageAllocation);
		vkDestroyImageView(p_device->getDevice(), m_depthImgView, nullptr);
		
		vkDestroySwapchainKHR(p_device->getDevice(), m_swapchain, nullptr);

		//Recreate swapchain + stuff
		bs::vk::createSwapChain(m_swapchain, *p_device, m_scdetails, m_window);
		bs::vk::createImageViews(m_scdetails, p_device->getDevice());
		createDepthbuffer();

		bs::vk::createFramebuffersWithDepth(getGenericRenderpass(), m_scdetails, p_device->getDevice(), m_depthImgView);

		m_wasResized = false;
	}

	void VulkanContext::createDepthbuffer()
	{
		VmaAllocationCreateInfo imgAllocInfo = {};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkImageCreateInfo depthImage{};
		depthImage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthImage.imageType = VK_IMAGE_TYPE_2D;
		depthImage.format = VK_FORMAT_D32_SFLOAT;
		depthImage.extent.width = bs::vk::viewportwidth;
		depthImage.extent.height = bs::vk::viewportheight;
		depthImage.extent.depth = 1;
		depthImage.mipLevels = 1;
		depthImage.arrayLayers = 1;
		depthImage.samples = VK_SAMPLE_COUNT_1_BIT;
		depthImage.tiling = VK_IMAGE_TILING_OPTIMAL;
		// We will sample directly from the depth attachment
		depthImage.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		vmaCreateImage(p_device->getAllocator(), &depthImage, &imgAllocInfo, &m_depthImg, &m_depthImageAllocation, nullptr);

		// CREATE DEPTH VIEW
		VkImageViewCreateInfo depthViewInfo{};
		depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthViewInfo.image = m_depthImg;
		depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthViewInfo.format = VK_FORMAT_D32_SFLOAT;
		depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		depthViewInfo.subresourceRange.baseMipLevel = 0;
		depthViewInfo.subresourceRange.levelCount = 1;
		depthViewInfo.subresourceRange.baseArrayLayer = 0;
		depthViewInfo.subresourceRange.layerCount = 1;

		if(vkCreateImageView(p_device->getDevice(), &depthViewInfo, nullptr, &m_depthImgView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create depth image views!");
		}
	}

	void resizeCallback(GLFWwindow* window, int width, int height)
	{
		auto context = static_cast<VulkanContext*>(glfwGetWindowUserPointer(window));
		context->m_wasResized = true;
	}
}