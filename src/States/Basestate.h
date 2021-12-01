#pragma once

#include "../Renderers/Renderer.h"

class Application;
class Camera;

class Basestate
{
public:
	Basestate(Application& app)	:	app(app)	{};

    virtual void input(float dt) = 0;
    virtual void update(float dt) = 0;
	virtual void lateUpdate(Camera& cam) = 0;

	virtual void render(Renderer& renderer) = 0;

    virtual ~Basestate() = default;
protected:
	Application& app;
};

