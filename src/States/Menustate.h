#pragma once

#include "../Application.h"

#include "../Renderers/Renderer.h"

class Menustate : public Basestate
{
public:
    Menustate(Application& app);
    ~Menustate() override;

	void createObject(bs::Transform& t, const std::string& name);
	void removeObject(const std::string& name);

    void input(float dt) override;
    void update(float dt) override;
	void lateUpdate(Camera& cam) override;
	void render(Renderer& renderer) override;
private:
	std::vector<bs::GameObject> m_gameObjects;
	
	Input::Inputs vInput;

	bs::Transform m_camtransform;
};
