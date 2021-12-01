#include "Device.h"

namespace bs
{
	Device::Device() : destroyed(false), m_resourceCleanupQueue({})
	{
		requiredDeviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			// VK_SHADER_INT_16
		};

		optionalDeviceExtensions = 
		{
			VK_NV_MESH_SHADER_EXTENSION_NAME,
		};
	}

	Device::~Device() 
	{
		destroy();
	}

	void Device::init()
	{
		int max_score = -1;
		const auto device_list = getPhysicalDevices();
		for(const auto& device : device_list)
		{
			if(max_score < device.second)
			{
				max_score = device.second;
				physDevice = device.first;
			}
		}

		createDevice();

		//initialize the memory allocator
		VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.physicalDevice = physDevice;
		allocatorInfo.device = device;
		allocatorInfo.instance = vk::m_instance;
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
		vmaCreateAllocator(&allocatorInfo, &m_allocatorVMA);

		bs::vk::createCommandPool(*this, m_pool);
	}

	void Device::destroy()
	{
		if(destroyed)
		{
			return;
		}

		for(const auto& resources : m_resourceCleanupQueue)
		{
			resources();
		}

		vmaDestroyAllocator(getAllocator());

		vkDestroyCommandPool(getDevice(), m_pool, nullptr);
		vkDestroyDevice(getDevice(), nullptr);
		destroyed = true;
	}

	VkDevice& Device::getDevice()
	{
		return device;
	}

	VkDevice Device::getDevice() const
	{
		return device;
	}

	VkPhysicalDevice Device::getPhysicalDevice() const
	{
		return physDevice; 
	}

	VmaAllocator& Device::getAllocator()
	{
		return m_allocatorVMA;
	}

	VkQueue Device::getPresentQueue() const
	{
		return presentQueue;
	}

	VkQueue Device::getGraphicsQueue() const
	{
		return graphicsQueue;
	}

	QueueFamilyIndices Device::getQueueFamilies() const
	{
		return bs::vk::findQueueFamilies(physDevice);
	}

	SwapChainSupportDetails Device::getSwapchainDetails() const
	{
		return bs::vk::querySwapChainSupport(physDevice);
	}

	void Device::submitWork(std::vector<VkCommandBuffer>& cmdbuffer)
	{
		static int i = 0;
		
		const VkSemaphore waitSemaphores[] = { bs::vk::imageAvailableSemaphores[i] };
		const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		const VkSemaphore signalSemaphores[] = { bs::vk::renderFinishedSemaphores[i] };

		const VkSubmitInfo submitInfo
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = waitSemaphores,
			.pWaitDstStageMask = waitStages,

			.commandBufferCount = 1,
			.pCommandBuffers = cmdbuffer.data(),
			
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = signalSemaphores,
		};

		vkResetFences(device, 1, &bs::vk::inFlightFences[i]);
		
		const VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, bs::vk::inFlightFences[i]);
		if (result != VK_SUCCESS) 
		{
			std::cout << "Queue Submission error: " << result << "\n";
			throw std::runtime_error("Failed to submit draw command buffer!");
		}
		
		vkQueueWaitIdle(graphicsQueue);
		
		if(bs::vk::inFlightFences[i] != VK_NULL_HANDLE) 
		{
			vkWaitForFences(device, 1, &bs::vk::inFlightFences[i], VK_TRUE, 500000000);
		}

		// goes from 0 to the buffer count for the swapchain buffers
		i = (i + 1) % bs::vk::NUM_SWAPCHAIN_FRAMEBUFFERS;
	}
	//NO GFX STUFF BC BREAKS AND SYNCH AND PAIN AND CRASHED DRIVERS
	void Device::submitImmediate(std::function<void(VkCommandBuffer cmd)>&& function)
	{
		VkCommandBuffer cmdbuffer;

		const VkCommandBufferAllocateInfo allocInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = m_pool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		if (vkAllocateCommandBuffers(device, &allocInfo, &cmdbuffer) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}

		constexpr VkCommandBufferBeginInfo beginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr,
		};

		vkBeginCommandBuffer(cmdbuffer, &beginInfo);

		function(cmdbuffer);

		vkEndCommandBuffer(cmdbuffer);

		const VkSubmitInfo submitInfo
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,

			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,

			.commandBufferCount = 1,
			.pCommandBuffers = &cmdbuffer,

			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr,
		};


		vkQueueWaitIdle(graphicsQueue);
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkResetCommandPool(device, m_pool, 0);
	}

	void Device::addCleanupCall(std::function<void()>&& function)
	{
		std::lock_guard<std::mutex> g_cleaning(m_device_lock);
		m_resourceCleanupQueue.emplace_back(function);
	}

	int Device::getScore(VkPhysicalDevice device) const
	{
		const QueueFamilyIndices indices = vk::findQueueFamilies(device);
		if(!indices.isComplete())
		{
			return -1;
		}

		//List of the extensions
		std::vector<VkExtensionProperties> availableExtensions;
		{	
			//Inside another score for idk why but I felt like it
			u32 extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
			availableExtensions.resize(extensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
		}

		//Go through the requirements for the needed extensions
		std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
		for(const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		//Check the swapchain support
		if(requiredExtensions.empty())
		{
			SwapChainSupportDetails swapChainSupport = vk::querySwapChainSupport(device);
			const bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			if(!swapChainAdequate)	//returns -1 if swapchain support is bad
			{
				return -1;
			}
		}
		else	// returns -1 if the required extensions aren't found
		{
			return -1;
		}

		//Counting the device score
		int score = 0;

		//Add score for the extensions
		const std::set<std::string> optionalExtensions(optionalDeviceExtensions.begin(), optionalDeviceExtensions.end());
		for(const auto& extension : availableExtensions)
		{
			if(optionalExtensions.contains(extension.extensionName))
			{
				std::cout << "Has " << std::string(extension.extensionName) << "\n";
				score += 1;
			}
		}

		//Now for the device properties score
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 5;
		}
		else if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			//Integrated GPU
		}
		
		//Now for the features score
		VkPhysicalDeviceFeatures2	deviceFeatures;
		deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures.pNext = nullptr;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures.features);
		vkGetPhysicalDeviceFeatures2(device, &deviceFeatures);
		
		const auto& features = deviceFeatures.features;	//For every feature thing wanted, add to the score
		if(features.multiDrawIndirect == VK_TRUE)	{	score += 1;	}	//Multidraw indirect
		if(features.drawIndirectFirstInstance == VK_TRUE)	{	score += 1;	}	//Others
		if(features.shaderInt16 == VK_TRUE)	{	score += 1;	}
		if(features.variableMultisampleRate == VK_TRUE)	{	score += 1;	}

		return score;
	}

	std::vector<std::pair<VkPhysicalDevice, int>> Device::getPhysicalDevices() const
	{
		//Get device count
		u32 deviceCount = 0;
		vkEnumeratePhysicalDevices(vk::m_instance, &deviceCount, nullptr);

		if(deviceCount == 0) 
		{
			//this might cause cases where someone has a shitty GT 530 and it doesn't work with vulkan
			//eventually we might want to have an opengl backup just in case
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}

		//Get list of physical devices
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vk::m_instance, &deviceCount, devices.data());

		//The list of devices and scores
		std::vector<std::pair<VkPhysicalDevice, int>> device_score_list;

		for(const auto& device : devices) 
		{
			const auto deviceScore = getScore(device);
			if(deviceScore < 0)
			{
				continue;
			}

			device_score_list.emplace_back(std::make_pair(device, deviceScore));
		}

		if(device_score_list.empty()) 
		{
			throw std::runtime_error("Failed to find a suitable GPU!");
		}

		return device_score_list;
	}

	void Device::createDevice()
	{
		QueueFamilyIndices indices = vk::findQueueFamilies(physDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) 
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		//Request the designated features:

		//Vulkan 1.2 features
		VkPhysicalDeviceVulkan12Features requestedFeatures = {};
		requestedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		requestedFeatures.descriptorIndexing = VK_TRUE;
		requestedFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
		requestedFeatures.runtimeDescriptorArray = VK_TRUE;
		requestedFeatures.drawIndirectCount = VK_TRUE;

		//Various other Features (vk 1.0)

		VkPhysicalDeviceFeatures2 deviceFeatures{};
		deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

		deviceFeatures.features.shaderInt16 = VK_TRUE;

		//Must add to the pNext chain
		deviceFeatures.pNext = &requestedFeatures;
		
		//Enumerate the other physical device features
		vkGetPhysicalDeviceFeatures(physDevice, &deviceFeatures.features);
		vkGetPhysicalDeviceFeatures2(physDevice, &deviceFeatures);

		



		//Defien the logical vulkan device
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

		createInfo.pEnabledFeatures = nullptr;
		createInfo.pNext = &deviceFeatures;

		if(vk::validationlayers) 
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(vk::validationLayers.size());
			createInfo.ppEnabledLayerNames = vk::validationLayers.data();
		}
		else 
		{
			createInfo.enabledLayerCount = 0;
		}

		if(vkCreateDevice(physDevice, &createInfo, nullptr, &device) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

		//Output which device is chosen:
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physDevice, &properties);
		const std::string deviceName(properties.deviceName);
		const u32 GPUID = properties.deviceID;

		std::cout << "Chose GPU #" << GPUID << "\t\t" << deviceName << std::endl;
	}
}