#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <memory>

#include <GPU/GPU.h>

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

	//Push a state to the back of the queue
	void pushState(std::unique_ptr<Basestate> state)
	{
		m_states.emplace_back(std::move(state));
	}

	//Safe add
	void pushBackState(std::unique_ptr<Basestate> state)
	{
		auto change = [&]()
		{
			m_states.emplace_back(std::move(state));
			//Swap the back two
			if(m_states.size() >= 2)
			{
				auto& secondToLast = m_states.at(m_states.size() - 2);
				auto& last = m_states.back();

				secondToLast.swap(last);
			}
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
	bs::ContextBase* m_context;
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
