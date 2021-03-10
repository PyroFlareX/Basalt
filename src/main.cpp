

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include "../Engine/Engine.h"

#include "Application.h"

JobSystem jobSystem;

struct Frame {
	float dt = 0.0f;

};

void print(Job job) {
	for (int i = 0; i < 10; ++i) {
		std::cout << i << std::endl;
	}
	std::cout << "Job done\n";
	return;
}

void start(Job job) {
	Application app;

	try {
		app.RunLoop();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		system("pause");
		return;
	}

	return;
}

int main() {

	Job j = jobSystem.createJob(&print);

	for (int i = 0; i < 1; ++i) {
		jobSystem.schedule(j);
	}

	//j = jobSystem.createJob(&start);

	jobSystem.schedule(j);

	jobSystem.wait();

	std::cout << "MAIN LOOP!\n";
	Application app;

	try {
		app.RunLoop();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		system("pause");
		return 1;
	}

	/*
	// Windowing Context
	vn::Context context;
	// Device Context
	vn::Device device;

	// Needed for setup
	context.setDeviceptr(&device);
	context.initAPI();

	/// TODO: Create 2nd half of the Vulkan wrapper, like Renderpasses, Pipelines,
	
	VkRenderPass rpass;
	vn::vk::createRenderPass(device.getDevice(), context.m_scdetails, rpass);

	VkPipelineLayout playout;
	VkPipeline gfx;
	vn::vk::createGraphicsPipeline(rpass, context.m_scdetails, playout, gfx, device.getDevice());
	
	vn::vk::createFramebuffers(rpass, context.m_scdetails, device.getDevice());

	VkCommandPool cpool;
	vn::vk::createCommandPool(device, cpool);

	std::vector<VkCommandBuffer> secondary;
	vn::vk::createSecondaryCommandBuffers(device.getDevice(), cpool, context.m_scdetails, gfx, rpass, secondary);

	std::vector<VkCommandBuffer> primary;
	vn::vk::createCommandBuffers(device.getDevice(), cpool, context.m_scdetails, gfx, rpass, primary, secondary);


	while (context.isOpen())
	{
		// Get Image From Swapchain and clear
		context.clear();
		vkResetCommandPool(device.getDevice(), cpool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

		// Update State


		// Create Command Buffers (and then combine them into Primary Cmd Buffers)
		vn::vk::createSecondaryCommandBuffers(device.getDevice(), cpool, context.m_scdetails, gfx, rpass, secondary);

		vn::vk::createCommandBuffers(device.getDevice(), cpool, context.m_scdetails, gfx, rpass, primary, secondary); // Combine all the 2nd cmd buffers

		// Submit Work to GPU
		device.submitWork(primary);

		// Present Image
		context.update();
	}
	*/

	return 0;
}