#include "Renderer.h"

Renderer::Renderer()
{
	

	
}

void Renderer::drawObject(vn::GameObject& entity)
{
	m_generalRenderer.addInstance(entity);
}

void Renderer::doCompute()
{

}


void Renderer::render(Camera& cam)
{
	//Main Pass
	
	m_generalRenderer.render(cam);
	m_computeRenderer.render();
}

void Renderer::finish()
{
	//Second Pass
	

	clearQueue();
}

void Renderer::clearQueue()
{

	m_generalRenderer.clearQueue();
}

Renderer::~Renderer()
{
    //dtor
}
