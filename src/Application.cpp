#include "Application.h"

#include "States/Gamestate.h"

#include <thread>

Application::Application() : m_camera(0)
{
	pushState(std::make_unique<GameState>(*this));
	setModeVR(false);

	// Needed for setup
	m_context.setDeviceptr(&m_device);
	m_context.initAPI();
}

void Application::setModeVR(bool haveVR) noexcept
{
	VRmode = haveVR;
}

void Application::RunLoop()
{
	//Initial Utilities Setup
	vn::Clock clock;
	float t = 0;
	double dt = 0;
	float frames = 0.0f;

	//Refresh Window
	m_context.clear();
	m_context.update();

	vn::vec2 winSize = vn::vec2(800.0f, 600.0f);
	
	
//===================================================================================
	
	/// TODO: Create 2nd half of the Vulkan wrapper, like Renderpasses, Pipelines,

	VkRenderPass rpass;
	vn::vk::createRenderPass(m_device.getDevice(), m_context.m_scdetails, rpass);

	VkPipelineLayout playout;
	VkPipeline gfx;
	vn::vk::createGraphicsPipeline(rpass, m_context.m_scdetails, playout, gfx, m_device.getDevice());

	vn::vk::createFramebuffers(rpass, m_context.m_scdetails, m_device.getDevice());

	VkCommandPool cpool;
	//VkCommandPool cpools;
	vn::vk::createCommandPool(m_device, cpool);
	//vn::vk::createCommandPool(m_device, cpools);

	std::vector<VkCommandBuffer> secondary;
	vn::vk::createSecondaryCommandBuffers(m_device.getDevice(), cpool, m_context.m_scdetails, gfx, rpass, secondary);

	std::vector<VkCommandBuffer> primary;
	vn::vk::createCommandBuffers(m_device.getDevice(), cpool, m_context.m_scdetails, gfx, rpass, primary, secondary);

	//vn::vk::RenderTargetFramebuffer framebuffer(m_device, rpass, winSize);

	void* dataBufferRecord[] = { &secondary, &m_context.m_scdetails, &rpass, &gfx, &primary };

	Job recordbufferSecondary = jobSystem.createJob([](Job job)
		{
			VkCommandBuffer buffer = reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0);
			std::cout << "RECORD 2nd COMMAND BUFFER THREAD INDEX: " << jobSystem.getThreadIndex() << std::endl;;
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Optional

			VkCommandBufferInheritanceInfo inheritanceInfo{};
			inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritanceInfo.renderPass = *reinterpret_cast<VkRenderPass*>(job.data[2]);
			// Secondary command buffer also use the currently active framebuffer
			inheritanceInfo.framebuffer = reinterpret_cast<SwapChainDetails*>(job.data[1])->swapChainFramebuffers[0];

			beginInfo.pInheritanceInfo = &inheritanceInfo; // Optional

			if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
							*reinterpret_cast<VkPipeline*>(job.data[3]));

			vkCmdDraw(buffer, 3, 1, 0, 0);

			if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}, dataBufferRecord);

	Job recordbufferPrimary = jobSystem.createJob([](Job job)
		{

			for (size_t i = 0; i < reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->size(); i++) {
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

				if (vkBeginCommandBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), &beginInfo) != VK_SUCCESS) {
					throw std::runtime_error("failed to begin recording command buffer!");
				}

				VkRenderPassBeginInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = *reinterpret_cast<VkRenderPass*>(job.data[2]);
				renderPassInfo.framebuffer = reinterpret_cast<SwapChainDetails*>(job.data[1])->swapChainFramebuffers[i];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = reinterpret_cast<SwapChainDetails*>(job.data[1])->swapChainExtent;

				VkClearValue clearColor = { 0.1f, 0.3f, 0.5f, 1.0f };
				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

				vkCmdBindPipeline(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, *reinterpret_cast<VkPipeline*>(job.data[3]));

				//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
				vkCmdExecuteCommands(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->size(), reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->data());

				vkCmdEndRenderPass(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i));

				if (vkEndCommandBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i)) != VK_SUCCESS) {
					throw std::runtime_error("failed to record command buffer!");
				}
			}
		}, dataBufferRecord);

//===================================================================================

	//Main Loop
	Input::window = m_context.getContext();


    while(m_context.isOpen() && !m_states.empty())
    {
		dt = clock.restart();
		
		
        ///Main Loop, do cycle of Input, Update, Draw, Render & Swap Buffers, Handle Events
		
		///Clear
		m_context.clear();
        
		///Input
		currentState().input();
		
        /// Update
		currentState().update(dt);
		currentState().lateUpdate(&m_camera);
		m_camera.update();

        /// Draw
		//currentState().render(&m_renderer);
		

        /// Render

		// Create Command Buffers (and then combine them into Primary Cmd Buffers)
		jobSystem.schedule(recordbufferSecondary);
		jobSystem.schedule(recordbufferPrimary);

			//vn::vk::createCommandBuffers(m_device.getDevice(), cpool, m_context.m_scdetails, gfx, rpass, primary, secondary); // Combine all the 2nd cmd buffers

		jobSystem.wait();
			// Submit Work to GPU
		m_device.submitWork(primary);

		
		//m_renderer.render(m_camera);




		//m_renderer.finish();

		m_context.update();


		vkResetCommandPool(m_device.getDevice(), cpool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        /// Handle Window Events
		t += dt;
		frames++;
		if (t >= 1)
		{
			std::cout << frames << " per sec\n";
			t = 0;
			frames = 0;
		}
		handleEvents();
    }
	m_context.close();
}



void Application::popState()
{
    m_states.pop_back();
}

Basestate& Application::currentState()
{
    return *m_states.back();
}

void Application::handleEvents()
{
	if (VRmode)
	{
		//Handle VR Events Function
	}

	//Window Events

	
}
