# Basalt
 A rewrite of Vinegar, but using a "fiber"-based job system. It will utilize some assets from Vinegar, like decent components of its GL backend when the time comes to readd OGL support (which tbh I dont have a reason to, unless I find a good reason for interop or smth). The main features are found in the better organized code, the abstractions from APIs, and heavy multithread enabled capabilities.

# To know
 This project is built using CMake. The required dependencies are GLFW, GLM, a threads implementation on your compiler, and the Vulkan SDK.
 Do note that this CAN be compiled on a system without functional vulkan implementations, merely the SDK is needed. You obviously cannot run without it though. /shrug
 THIS CAN BE COMPILED ON BOTH LINUX AND WINDOWS WITHOUT CONFIG BESIDES INSTALLING THE BUILDING LIBS!!!

# Research
 Main things that gave me the inspiration to do this are the Naughty Dog fibers talk, Colby Hall's Newport Engine (read his amazing research doc).
 The renderer is in Vulkan for a select few reasons: Multithreading, Commandbuffers, lower CPU usage, and slightly better portability bc other backends are easily implemented after this is done. The main ones I want to have are: deko3d, DX12, OpenGL, and possibly even citro3d if there is motivation or a pull request (hint hint).

# Architecture
 The primary focus will be for parallelism through the creation of "jobs". To make a job, you pass a function (pointer) for a function that returns void and takes in a "Job" param only. Instead of the STL variadic impl for creating multithreaded work, this is a bit more hacky, but is easy enough to get used to. The ``data`` param is an array of ``void*``, which has a max length of 8. An example is shown here: 

 ```
    void* dataBufferRecord[8] = { &secondary, &m_context.m_scdetails, &rpass, &gfx, &primary, &cmdpool, &m_device.getDevice() };

	Job recordbufferSecondary = jobSystem.createJob([](Job job)
		{
			vkResetCommandPool(*reinterpret_cast<VkDevice*>(job.data[6]),
					reinterpret_cast<std::unordered_map<uint8_t, VkCommandPool>*>(job.data[5])->find(1)->second, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBuffer buffer = reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Optional

			VkCommandBufferInheritanceInfo inheritanceInfo{};
			inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritanceInfo.renderPass = *reinterpret_cast<VkRenderPass*>(job.data[2]);
			// Secondary command buffer also use the currently active framebuffer
			inheritanceInfo.framebuffer = VK_NULL_HANDLE;//reinterpret_cast<SwapChainDetails*>(job.data[1])->swapChainFramebuffers[0];

			beginInfo.pInheritanceInfo = &inheritanceInfo; // Optional

			if (vkBeginCommandBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			vkCmdBindPipeline(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), VK_PIPELINE_BIND_POINT_GRAPHICS,
							*reinterpret_cast<VkPipeline*>(job.data[3]));

			vkCmdDraw(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), 3, 1, 0, 0);

			if (vkEndCommandBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0)) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}


		}, dataBufferRecord);
```

 You see that the Job lambda uses the data from the pointers, ``reinterpret_cast<T*>(job.data[n])`` where ``T`` is type, ``n`` is index in the ``data`` array. this is how I recommend you do so, but you may know better than I of course, so do as you please. 
 The Vulkan backend is done through a CommandPool map per thread, and some other complicated stuff that I'd rather not spent an hour typing out the intracasies of. Secondary cmd buffers are recorded in renderer jobs, then combined into a singe primary cmd buffer at the end of the renderer queue secondaries building.

 # Example of Asset System
 ```cpp
#include <iostream>

void foo()
{
	vn::AssetManager<int> manager;

	manager.addAsset(5, "first int");

	std::cout << "test: " << manager.getAsset("first int") << "\n";
}
```

 ## Contributing?
 I am open to contributions, but until more of the renderer is done, the project is currently in Vulkan dream land of never ending paperwork and possibilties for more wrist cramping. The style guide is camelCase, brackets are on the next line, make as many functions thread-safe or consteval/constexpr as possible. Keep templates and modern STL shenanigans and language quirks out of this please as well. Use unordered_map and vector for most heap or map uses, and use unique or shared_ptrs instead of raw pointers as well. No using volitile obviously. Keep new library additions to a minimum as well, the only new ones I will tolerate is a networking lib, audio playback, and scripting.
 <p>Some things I don't have the time for on my plate, but should be friendly enough to PR, would be for the implementation of a high and low priority queue in the Job Scheduler (just another MPMC queue really tbh, then another check in the main thread loop). Also Vulkan cleanup stuff. I'm just way too lazy tbh, but the driver cleans 90% ish of it up anyways, but still good to do manually. This too can be jobified. </p>
