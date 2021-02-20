#pragma once

#include "../Camera.h"
#include "../../Engine/Engine.h"

class GeneralRenderer
{
public:
	GeneralRenderer();

	void addInstance(vn::GameObject& entity);
	void render(Camera& cam);

	void clearQueue();
	
	~GeneralRenderer();
private:
	std::vector<vn::GameObject> m_queue;
	
	

	vn::Image img;
};

