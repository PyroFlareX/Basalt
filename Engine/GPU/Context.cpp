#include "Context.h"



Context::Context()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Basalt", nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);

	initAPI();
}

void Context::clear()
{

}

void Context::update()
{
	
}

void Context::close()
{
}

void Context::initAPI()
{
	vn::vk::createInstance(m_instance, "Basalt");
	
}

bool Context::isOpen()
{
	return false;
}

GLFWwindow* Context::getContext()
{
	return m_window;
}

Context::~Context()
{
	vkDestroyInstance(m_instance, nullptr);
}
