#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <memory>
#include <iostream>

#include "../Engine/Engine.h"
#include "Renderers/Renderer.h"
#include "Camera.h"
#include "States/Basestate.h"


class Application {
public:
	Application();

	//Configuration Settings
	void setModeVR(bool haveVR) noexcept;

	void RunLoop();

	//State Stuff

	void pushState(std::unique_ptr<Basestate> state) {
		m_states.emplace_back(std::move(state));
	}

	void popState();

	void handleEvents();

	Camera &getCam() {
		return m_camera;
	}

protected:

private:
	Basestate &currentState();

	// Windowing Context
	vn::Context m_context;
	// Device Context
	vn::Device m_device;

	Camera m_camera;
	std::vector<std::unique_ptr<Basestate>> m_states;
	Renderer *m_renderer;

	bool VRmode;
};


#endif // APPLICATION_H
