//#include "Vulkan.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include "Engine/Engine.h"


JobSystem jobSystem;

void print(Job job)
{
	for (int i = 0; i < 100; ++i)
	{
		std::cout << i << std::endl;
	}
	std::cout << "Job done\n";
	return;
}


int main() {

	/*Job j = jobSystem.createJob(&print);

	for(int i = 0; i < 20; ++i)
	{
		jobSystem.schedule(j);
	}
	jobSystem.wait();*/
	
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

	std::vector<VkCommandBuffer> cbuf;
	vn::vk::createCommandBuffers(device.getDevice(), cpool, context.m_scdetails, gfx, rpass, cbuf);

	while (context.isOpen())
	{
		context.clear();

		//device.submitWork(cbuf[0]);


		context.update();

	}
	/*
	Application app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}*/

	return EXIT_SUCCESS;
}