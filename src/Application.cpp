#include "Application.h"

#include "Camera.h"
#include "States/Gamestate.h"

#include <unordered_map>

Application::Application() : m_camera(0)
{
	pushState(std::make_unique<GameState>(*this));
	setModeVR(false);

	// Needed for setup
	m_context.setDeviceptr(&m_device);
	m_context.initAPI();

	m_renderer = new Renderer(&m_device);
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
	//m_context.clear();
	//m_context.update();

	vn::vec2 winSize = vn::vec2(800.0f, 600.0f);
	
	
//===================================================================================

	VkRenderPass rpass;
	vn::vk::createRenderPass(m_device.getDevice(), m_context.m_scdetails, rpass);
	

	vn::vk::createFramebuffers(rpass, m_context.m_scdetails, m_device.getDevice());

	vn::vk::FramebufferData framebufdata{};
	framebufdata.handle = m_context.m_scdetails.swapChainFramebuffers;
	framebufdata.imgView = m_context.m_scdetails.swapChainImageViews.at(0);
	framebufdata.size = winSize;


//===================================================================================

	//Main Loop
	Input::window = m_context.getContext();

	//jobSystem.schedule(recordbufferPrimary);
	jobSystem.wait();

    while(m_context.isOpen() && !m_states.empty())
    {
		dt = clock.restart();
		
		
        ///Main Loop, do cycle of Input, Update, Draw, Render & Swap Buffers, Handle Events
		
		///Clear
		m_context.clear();
		m_renderer->clearQueue();
        
		///Input
		currentState().input();
		
        /// Update
		currentState().update(dt);
		currentState().lateUpdate(&m_camera);
		m_camera.update();
		jobSystem.wait();


        /// Draw
		currentState().render(m_renderer);
		jobSystem.wait();

        /// Render
		m_renderer->render(m_camera);



		m_renderer->finish(framebufdata);
		jobSystem.wait();
		m_context.update();
		

		std::cout << dt * 1000 << " ms\n";
        /// Handle Window Events
		t += dt;
		frames++;
		if (t >= 1)
		{
			std::cout << frames << " per sec\n";
			std::cout << dt * 1000 << " ms\n";
			t = 0;
			frames = 0;

			std::cout << "CAMERA POS XZ: " << this->getCam().pos.x << " " << this->getCam().pos.z << "\n";
			std::cout << "CAMERA ROT XY: " << this->getCam().rot.x << " " << this->getCam().rot.y << "\n";
		}
		handleEvents();
    }
	m_context.close();

	delete m_renderer;
	
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
