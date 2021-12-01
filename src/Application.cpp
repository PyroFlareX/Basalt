#include "Application.h"

#include "Camera.h"
#include "States/Menustate.h"


Application::Application()
{
	bs::asset_manager = new bs::AssetManager();

	// Loading screen
	m_states.emplace_back(std::make_unique<Menustate>(*this));
	

	// Needed for setup
	m_context = new bs::Context("Voxellium");
	m_device = new bs::Device();
	m_context->setDeviceptr(m_device);
	m_context->initAPI();

	m_renderer = new Renderer(m_device);
}

Application::~Application()
{
	delete m_renderer;
	delete bs::asset_manager;
	delete m_context;
}

Camera& Application::getCamera()
{
	return m_camera;
}

void Application::RunLoop()
{
	//Initial Utilities Setup
	bs::Clock clock;
	double t = 0;
	float dt = 0;
	int frames = 0;

	bs::vec2i winSize = bs::vec2i(bs::vk::viewportwidth, bs::vk::viewportheight);
	
//===================================================================================	

	bs::vk::createFramebuffers(m_context->rpass, m_context->m_scdetails, m_device->getDevice());

	//Framebuffer data, pass the vulkan stuff into the renderdata layout
	framebufdata[0].handle = m_context->m_scdetails.swapChainFramebuffers;
	framebufdata[0].imgView = m_context->m_scdetails.swapChainImageViews.at(0);
	framebufdata[0].size = winSize;

	//bs::vk::RenderTargetFramebuffer framebuffer(m_device, rpass, winSize);	//Originally for a render buffer to be copied to swapchain framebuffer
	//framebufdata[1] = framebuffer.getFramebufferData();
	
	//std::cout << "framebufdata handles: [size, handle] " << framebufdata[0].handle.size() << " \n";

//===================================================================================

	//Main Loop
	Input::window = m_context->getContext();
	
	//help
	Input::setupInput();

	bs::Image icon;
	icon.loadFromFile("res/papertexture.png");
	m_context->setIcon(icon);


	while(m_context->isOpen() && !m_states.empty() && !shouldClose)
	{
		dt = static_cast<float>(clock.restart());
		auto& io = ImGui::GetIO();
		auto& current = *currentState().get();

		///Main Loop, do cycle of Input, Update, Draw, Render & Swap Buffers, Handle Events
		
		/// Clear
		m_context->clear();
		m_renderer->clearQueue();
		
		/// Input
		current.input(dt);
		
		/// Update
		current.update(dt);


		current.lateUpdate(m_camera);
		m_camera.update();
		jobSystem.wait();


		/// Draw objects from gamestate
		current.render(*m_renderer);
		jobSystem.wait();

		/// Render
		m_renderer->render(m_camera);

		/// Submitting the data to the GPU and actually drawing/updating display
		m_renderer->finish(framebufdata[0], frames % framebufdata[0].handle.size());
		jobSystem.wait();
		m_context->update();

		
		/// Handle Window Events
		t += dt;
		frames++;
		if (t >= 1)
		{
			io.DeltaTime = dt;
			io.Framerate = (float)frames;
			
			//std::cout << frames << " per sec\n";

			//printf("Player Pos: X:%0.3f, Y:%0.3f, Z:%0.3f\n", m_camera.pos.x, m_camera.pos.y, m_camera.pos.z);
			//printf("Player Rot: X:%0.3f, Y:%0.3f, Z:%0.3f\n", m_camera.rot.x, m_camera.rot.y, m_camera.rot.z);
			//std::cout << dt * 1000 << " ms\n";
			t = 0;
			frames = 0;
		}


		handleEvents();
	}
	m_context->close();

	jobSystem.wait();

	m_states.clear();
}


void Application::popState()
{
	auto change = [&]()
	{
		m_states.pop_back();
	};
	m_statechanges.emplace_back(change);
	
}

std::unique_ptr<Basestate>& Application::currentState()
{
	return m_states.back();
}

void Application::handleEvents()
{
	if(m_context->resized || m_context->refresh)	//Checks if the framebuffer data needs to be updated
	{
		framebufdata[0].size = bs::vec2i(bs::vk::viewportwidth, bs::vk::viewportheight);
		framebufdata[0].handle = m_context->m_scdetails.swapChainFramebuffers;	//Since vulkan spec has this technically as a pointer, this has to be reupdated

		m_context->refresh = false;
	}

	for(const auto change : m_statechanges)
	{
		change();
	}
	m_statechanges.clear();

	//std::cout << "Frame\n";
}

