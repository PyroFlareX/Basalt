#include "GeneralRenderer.h"


GeneralRenderer::GeneralRenderer()
{
	img.loadFromFile("res/container.jpg");

	

}

void GeneralRenderer::addInstance(vn::GameObject& entity)
{
	m_queue.push_back(entity);
}

void GeneralRenderer::render(Camera& cam)
{
	
}

void GeneralRenderer::clearQueue()
{
	m_queue.clear();
}

GeneralRenderer::~GeneralRenderer()
{

}
