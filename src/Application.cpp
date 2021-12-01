#include "Application.h"

#include "Camera.h"
#include "States/Menustate.h"


Application::Application()	:	shouldClose(false)
{
	bs::asset_manager = new bs::AssetManager();

	// Loading screen
	m_states.emplace_back(std::make_unique<Menustate>(*this));
	
	// Needed for setup
	m_context = new bs::Context("Voxellium");
	m_device = new bs::Device();
	m_context->setDeviceptr(m_device);
	m_context->initAPI();

	m_renderer = new Renderer(m_device, m_context->getGenericRenderpass());
}

Application::~Application()
{
	delete m_renderer;
	delete bs::asset_manager;
	delete m_context;
	delete m_device;
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

	const bs::vec2i winSize = bs::vec2i(bs::vk::viewportwidth, bs::vk::viewportheight);

	//Setting icon for the window
	bs::Image icon;
	icon.loadFromFile("res/papertexture2.png");
	m_context->setIcon(icon);
	
//===================================================================================	
		
	//The handle to the swap chain image to render to
	m_renderFramebuffer.handle = m_context->m_scdetails.swapChainFramebuffers;
	m_renderFramebuffer.imgView = m_context->m_scdetails.swapChainImageViews.at(0);
	m_renderFramebuffer.size = winSize;

//===================================================================================

	//Main Loop
	Input::window = m_context->getContext();
	Input::setupInput();

	auto& jobSystem = bs::getJobSystem();

	//Main loop running
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
		jobSystem.wait(0);


		/// Draw objects from gamestate
		current.render(*m_renderer);
		jobSystem.wait(0);

		/// Render
		m_renderer->render(m_camera);

		/// Submitting the data to the GPU and actually drawing/updating display
		m_renderer->finish(m_renderFramebuffer, frames % m_renderFramebuffer.handle.size());
		jobSystem.wait(0);
		m_context->update();
		
		/// Handle Window Events
		t += dt;
		frames++;
		if (t >= 1.0)
		{
			//std::cout << frames << " per sec\n";
			//std::cout << dt * 1000 << " ms\n";
			//printf("Player Pos: X:%0.3f, Y:%0.3f, Z:%0.3f\n", m_camera.pos.x, m_camera.pos.y, m_camera.pos.z);
			//printf("Player Rot: X:%0.3f, Y:%0.3f, Z:%0.3f\n", m_camera.rot.x, m_camera.rot.y, m_camera.rot.z);

			io.DeltaTime = dt;
			io.Framerate = (float)frames;
			t = 0;
			frames = 0;
		}

		//Handle the window and state events
		handleEvents();
	}
	m_context->close();

	jobSystem.wait(0);

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
		m_renderFramebuffer.size = bs::vec2i(bs::vk::viewportwidth, bs::vk::viewportheight);
		m_renderFramebuffer.handle = m_context->m_scdetails.swapChainFramebuffers;	//Bc the handle changed, this must be changed

		m_context->refresh = false;
	}

	for(const auto change : m_statechanges)
	{
		change();
	}
	m_statechanges.clear();
}
