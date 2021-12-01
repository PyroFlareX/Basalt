#include "Context.h"

#ifdef BASE_CONTEXT
#include <imgui.h>

namespace bs
{
	ContextBase::ContextBase(const std::string& title, bs::vec2i size)	:	m_windowname(title), m_size(size)
	{
		glfwInit();

		// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(m_size.x, m_size.y, m_windowname.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
	}

	ContextBase::~ContextBase()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void ContextBase::clear()
	{
		glfwPollEvents();
	}
	
	void ContextBase::update()
	{

	}
	
	void ContextBase::close()
	{

	}

	bool ContextBase::isOpen() const
	{
		if(glfwWindowShouldClose(m_window))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void ContextBase::initAPI()
	{
		initImGui();
	}

	GLFWwindow* ContextBase::getContext()
	{
		return m_window;
	}
	
	void ContextBase::setIcon(Image& icon)
	{
		const GLFWimage img
		{
			.width = icon.getSize().x,
			.height = icon.getSize().y,
			.pixels = (unsigned char*)icon.getPixelsPtr()
		};

		glfwSetWindowIcon(getContext(), 1, &img);	
	}
	
	void ContextBase::setTitleName(const std::string& title)
	{
		m_windowname = title;
		glfwSetWindowTitle(m_window, m_windowname.c_str());
	}
	
	const std::string& ContextBase::getWindowTitle() const
	{
		return m_windowname;
	}

	bool ContextBase::needsRefresh() const
	{
		return m_needsRefresh;
	}
	
	void ContextBase::setRefreshCompleted()
	{
		m_needsRefresh = false;
	}

	void ContextBase::initImGui()
	{
		//Start IMGUI init
		IMGUI_CHECKVERSION();
		auto* ctximgui = ImGui::CreateContext();
		ImGui::SetCurrentContext(ctximgui);

		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2(static_cast<float>(m_size.x), static_cast<float>(m_size.y));
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;

		//Setup style
		ImGui::StyleColorsDark();
	}
}

#endif