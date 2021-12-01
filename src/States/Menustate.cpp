#include "Menustate.h"

#include "Worldstate.h"

Menustate::Menustate(Application& app) : Basestate(app)
{
	m_camtransform.pos = {0.0f, 0.0f, 0.0f};
	m_camtransform.rot = {0.0f, 0.0f, 0.0f};
	m_camtransform.scale = {0.0f, 0.0f, 0.0f};
	m_camtransform.origin = {0.0f, 0.0f, 0.0f};
}

Menustate::~Menustate()
{
	app.getCamera().follow(app.getCamera());
}

void Menustate::input(float dt)
{
	ImGui::NewFrame();
	vInput = Input::getInput(dt);
	auto& io = ImGui::GetIO();

}

void Menustate::update(float dt)
{
	static uint8_t menuIndex = 1;
	constexpr auto windowflag = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

	if(ImGui::Begin("Main Menu", nullptr, windowflag))
	{
		//ImGui::Window

		if(ImGui::Button("Start"))
		{
			app.pushState(std::make_unique<Worldstate>(Worldstate(app)));
		}
	}

	ImGui::End();

	switch(menuIndex)
	{
		case 1:
		{
			
			break;
		}

		case 2:
		{
			
			break;
		}
	}
}

void Menustate::lateUpdate(Camera& cam)
{
	cam.follow(m_camtransform);
}

//called in Application.cpp loop
void Menustate::render(Renderer& renderer)
{		
	for (auto& obj : m_gameObjects)
	{
		obj.getCurrentTransform();
		renderer.drawObject(obj);
	}
}

