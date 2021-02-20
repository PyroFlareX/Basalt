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

	vn::vec2 winSize = vn::vec2(1280.0f, 720.0f);
	
	
//===================================================================================
	
	/// TODO: Create 2nd half of the Vulkan wrapper, like Renderpasses, Pipelines,

	VkRenderPass rpass;
	vn::vk::createRenderPass(m_device.getDevice(), m_context.m_scdetails, rpass);

	VkPipelineLayout playout;
	VkPipeline gfx;
	vn::vk::createGraphicsPipeline(rpass, m_context.m_scdetails, playout, gfx, m_device.getDevice());

	vn::vk::createFramebuffers(rpass, m_context.m_scdetails, m_device.getDevice());

	VkCommandPool cpool;
	vn::vk::createCommandPool(m_device, cpool);

	std::vector<VkCommandBuffer> secondary;
	vn::vk::createSecondaryCommandBuffers(m_device.getDevice(), cpool, m_context.m_scdetails, gfx, rpass, secondary);

	std::vector<VkCommandBuffer> primary;
	vn::vk::createCommandBuffers(m_device.getDevice(), cpool, m_context.m_scdetails, gfx, rpass, primary, secondary);



//===================================================================================

	//Main Loop
	Input::window = m_context.getContext();


    while(m_context.isOpen() && !m_states.empty())
    {
		dt = clock.restart();
		
		
        ///Main Loop, do cycle of Input, Update, Draw, Render & Swap Buffers, Handle Events
		
		///Clear
		m_context.clear();
		vkResetCommandPool(m_device.getDevice(), cpool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        
		///Input
		//currentState().input();
		
        /// Update
       // currentState().update(dt);
		//currentState().lateUpdate(&m_camera);
		//m_camera.update();

        /// Draw
		//currentState().render(&m_renderer);

        /// Render

			// Create Command Buffers (and then combine them into Primary Cmd Buffers)
			vn::vk::createSecondaryCommandBuffers(m_device.getDevice(), cpool, m_context.m_scdetails, gfx, rpass, secondary);

			vn::vk::createCommandBuffers(m_device.getDevice(), cpool, m_context.m_scdetails, gfx, rpass, primary, secondary); // Combine all the 2nd cmd buffers

			// Submit Work to GPU
			m_device.submitWork(primary);

		
		//m_renderer.render(m_camera);




		//m_renderer.finish();

		m_context.update();

		

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
