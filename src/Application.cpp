#include "Application.h"

#include "States/Gamestate.h"

#include <thread>

Application::Application() : m_camera(0)
{
	pushState(std::make_unique<GameState>(*this));
	setModeVR(false);
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
	float dt = 0;
	float frames = 0.0f;

	//Refresh Window
	m_context.clear();
	m_context.update();

	vn::vec2 winSize = vn::vec2(1280.0f, 720.0f);
	

	vn::Framebuffer primary(winSize);
	
//===================================================================================
	




//===================================================================================

	//Main Loop
	Input::window = m_context.getContext();

	//m_context.getContext()->setActive(false);
	
	//std::thread renderingThread(&Application::AsyncRenderThread, this, std::ref(primary));

    while(m_context.isOpen() && !m_states.empty())
    {
		dt = clock.restart();
        ///Main Loop, do cycle of Input, Update, Draw, Render & Swap Buffers, Handle Events
        ///Input
		currentState().input();
		
        /// Update
        currentState().update(dt);
		currentState().lateUpdate(&m_camera);
		m_camera.update();

        /// Draw
		currentState().render(&m_renderer);

        /// Render
		// Async Test
		//AsyncRenderThread(primary);
		primary.bind();
		m_renderer.render(m_camera);



		primary.getTexture().bind();
		//glViewport(0, 0, 1280, 720);

		m_renderer.finish();

		m_context.update();

		glFlush();
		glFinish();

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

	//renderingThread.join();
}

void Application::AsyncRenderThread(vn::Framebuffer& fbo)
{
	//m_context.getContext()->setActive(true);
	while (m_context.isOpen())
	{
		fbo.bind();
		m_renderer.render(m_camera);

		

		fbo.getTexture().bind();
		//glViewport(0, 0, 1280, 720);

		m_renderer.finish();

		m_context.update();

		glFlush();
		glFinish();
	}
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

	sf::Event e;
	while (m_context.getContext()->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			m_context.close();
		}
	}
}
