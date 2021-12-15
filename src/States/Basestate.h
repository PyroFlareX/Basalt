#pragma once

#include "../Renderers/Renderer.h"

class Application;
class Camera;

class Basestate
{
public:
	Basestate(Application& app)	:	app(app), m_active(true)	{};
    virtual ~Basestate() = default;

    virtual void input(float dt) = 0;
    virtual void update(float dt) = 0;
	virtual void lateUpdate(Camera& cam) = 0;

	virtual void render(Renderer& renderer) = 0;

	bool isActive() const
	{
		return m_active;
	}

protected:
	Application& app;

	void setInactive()
	{
		m_active = true;
	}

private:
	bool m_active;
};

