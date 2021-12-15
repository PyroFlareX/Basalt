#pragma once

#include <vector>
#include <memory>

#include <GPU/GPU.h>

#include "States/Basestate.h"
#include "Camera.h"

#include <stack>

class Application
{
public:
    Application();
    ~Application();

	using GameState = std::unique_ptr<Basestate>;

	//Main loop
    void RunLoop();

	//Get the camera
	Camera& getCamera();

	//Handles any window or engine events, and does whatever needs to be fixed
    void handleEvents();
	//Ask for the application to be closed
	void requestClose();

	///State Stuff
	//Push a state to the back of the queue
	void pushState(GameState state);

	//Pop the state at the back
    void popState();

	//Get a reference to the top state; useful for having two states active at once
    GameState& currentState();
private:

	//Get unique ownership of the current state
	GameState getCurrentState();
	//Add the current state back into the stack
	void returnState(GameState state);

	//State storage
    std::stack<GameState> m_states;

	///Other engine data
	// Windowing Context
	bs::ContextBase* m_context;
	bs::vk::FramebufferData m_renderFramebuffer;

	// Device Context
	bs::Device* m_device;
	//Renderer
	Renderer* m_renderer;

	// Camera
	Camera m_camera;

	bool shouldClose;
};
