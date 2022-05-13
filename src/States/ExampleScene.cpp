#include "ExampleScene.h"

#include <filesystem>

ExampleScene::ExampleScene(Application& app) : Basestate(app)
{
	m_camtransform.pos = {0.0f, 0.0f, 0.0f};
	m_camtransform.rot = {0.0f, 0.0f, 0.0f};
	m_camtransform.scale = {0.0f, 0.0f, 0.0f};
	m_camtransform.origin = {0.0f, 0.0f, 0.0f};

	menuIndex = 1;

	std::cout << "Example Scene Pushed\n";

	bs::Transform t;
	t.pos.x = -0.5f;

	bs::GameObject gobj(t, "testing object");

	//bs::loadMeshFromObj("res/Models/conference.obj");
	// gobj.model_id = "conference";
	gobj.model_id = "sphere";


	auto* const p_dev = bs::asset_manager->getTextureMutable(0).getDevice();
	for(const auto& dir : std::filesystem::directory_iterator{"res\\Models"})
	{
		if(dir.is_regular_file())
		{
			if(dir.path().extension() == ".obj")
			{
				std::cout << "Adding: " << dir << '\n';
				//std::cout << dir.path().string() << std::endl;

				auto mesh = bs::loadMeshFromObj(dir.path().string());

				//std::cout << dir.path().stem().string() << "\n";

				//bs::asset_manager->addModel(bs::vk::Model(mesh, p_dev), dir.path().stem().string());
				//THERE IS AN ISSUE WITH THIS ^ CURRENTLY!!!
			}
		}
	}
	std::cout << "Finished Loading Models\n";

	m_gameObjects.emplace_back(gobj);
}

ExampleScene::~ExampleScene()
{
	app.getCamera().follow(app.getCamera());
}

void ExampleScene::input(float dt)
{
	ImGui::NewFrame();
	vInput = Input::getInput(dt);
	const auto& io = ImGui::GetIO();
}

void ExampleScene::update(float dt)
{
	constexpr auto windowflag = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

	const auto& io = ImGui::GetIO();
	ImGui::SetNextWindowSize({io.DisplaySize.x / 8, io.DisplaySize.y / 16});

	if(ImGui::Begin("Info HUD", nullptr, windowflag))
	{
		//if(ImGui::Button("Start", ImGui::GetContentRegionAvail()))
		{
			// app.pushState(std::make_unique</*New State*/>(app));
		}

		ImGui::Text("Example Scene:");
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

void ExampleScene::lateUpdate(Camera& cam)
{
	cam.follow(m_camtransform);
}

void ExampleScene::render(Renderer& renderer)
{		
	for (auto& obj : m_gameObjects)
	{
		obj.getCurrentTransform();
		renderer.drawObject(obj);
	}
}

void ExampleScene::onStateChange()
{

}

void ExampleScene::loadStateAssets()
{

}