#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <memory>

#include <GPU/Context.h>

#include "Renderers/Renderer.h"
#include "States/Basestate.h"
#include "Camera.h"

class Application
{
public:
    Application();
    ~Application();

	//Main loop
    void RunLoop();

	//Get the camera
	Camera& getCamera();

    //State Stuff

	void pushState(std::unique_ptr<Basestate> state)
	{
		m_states.emplace_back(std::move(state));
	}
	void pushBackState(std::unique_ptr<Basestate> state)
	{
		auto change = [&]()
		{
			auto* current = m_states.back().release();
			m_states.back() = (std::move(state));
			m_states.emplace_back(std::unique_ptr<Basestate>(current));
		};
		m_statechanges.emplace_back(change);
	}

    void popState();
    void handleEvents();
	
	void requestClose()
	{
		shouldClose = true;
	}

protected:

private:
    std::unique_ptr<Basestate>& currentState();

	bs::vk::FramebufferData m_renderFramebuffer;

	// Windowing Context
	bs::Context* m_context;
	// Device Context
	bs::Device* m_device;
	//Renderer
	Renderer* m_renderer;

	// Camera
	Camera m_camera;

	//State storage
    std::vector<std::unique_ptr<Basestate>> m_states;
	std::vector<std::function<void()>> m_statechanges;

	bool shouldClose;
};


#endif // APPLICATION_H
