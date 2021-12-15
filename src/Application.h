#pragma once

#include <vector>
#include <memory>

#include <GPU/GPU.h>

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
	void pushState(std::unique_ptr<Basestate> state);

	//Safe add
	void pushBackState(std::unique_ptr<Basestate> state);

	//Pop the state at the back
    void popState();

	//Handle any window or other engine events, and does whatever needs to be fixed
    void handleEvents();

	//Ask for the application to be closed
	void requestClose();

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
