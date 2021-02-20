#ifndef BASESTATE_H
#define BASESTATE_H

#include "../Renderers/Renderer.h"
#include "../Camera.h"

class Application;

class Basestate
{
public:
	Basestate(Application& app)	:	app(app)	{};

    virtual bool input() = 0;
    virtual void update(float dt) = 0;
	virtual void lateUpdate(Camera* cam) = 0;
	virtual void render(Renderer* renderer) = 0;

    virtual ~Basestate() = default;
protected:
	Application& app;
};


#endif // BASESTATE_H
