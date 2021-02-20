#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <memory>
#include <iostream>

#include "Backends/Context.h"
#include "Renderers/Renderer.h"
#include "Camera.h"
#include "States/Basestate.h"


class Application
{
public:
    Application();

	//Configuration Settings
	void setModeVR(bool haveVR) noexcept;

    void RunLoop();
	void AsyncRenderThread(vn::Framebuffer& fbo);

    //State Stuff

	void pushState(std::unique_ptr<Basestate> state)
	{
		m_states.emplace_back(std::move(state));
	}

    void popState();
    void handleEvents();
	Camera& getCam() 
	{
		return m_camera;
	}
protected:

private:
    Basestate& currentState();
    Context m_context;
	Camera m_camera;
    std::vector<std::unique_ptr<Basestate>> m_states;
	Renderer m_renderer;

	bool VRmode;
};


#endif // APPLICATION_H
