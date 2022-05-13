#include "Menustate.h"

#include "ExampleScene.h"

Menustate::Menustate(Application& app) : Basestate(app)
{
	m_camtransform.pos = {0.0f, 0.0f, 0.0f};
	m_camtransform.rot = {0.0f, 0.0f, 0.0f};
	m_camtransform.scale = {0.0f, 0.0f, 0.0f};
	m_camtransform.origin = {0.0f, 0.0f, 0.0f};

	menuIndex = 1;
}

Menustate::~Menustate()
{
	app.getCamera().follow(app.getCamera());
}

void Menustate::input(float dt)
{
	ImGui::NewFrame();
	vInput = Input::getInput(dt);
	const auto& io = ImGui::GetIO();
}

void Menustate::update(float dt)
{
	constexpr auto windowflag = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

	const auto& io = ImGui::GetIO();
	ImGui::SetNextWindowSize({io.DisplaySize.x / 8.0f, io.DisplaySize.y / 16.0f});

	if(ImGui::Begin("Main Menu", nullptr, windowflag))
	{
		if(ImGui::Button("Start", ImGui::GetContentRegionAvail()))
		{
			app.pushState(std::make_unique<ExampleScene>(app));
			std::cout << "Pushed New State!\n";
			this->setInactive();
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

void Menustate::render(Renderer& renderer)
{		
	for (auto& obj : m_gameObjects)
	{
		obj.getCurrentTransform();
		renderer.drawObject(obj);
	}
}

void Menustate::onStateChange()
{

}

void Menustate::loadStateAssets()
{

}
