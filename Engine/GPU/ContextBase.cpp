#include "ContextBase.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace bs
{
	ContextBase::ContextBase(const std::string& title, const bs::vec2i& size)	:	m_window_name(title), m_size(size)
	{
		glfwInit();

		m_window = nullptr;
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
		assert(getContext != nullptr);
		return m_window;
	}
	
	void ContextBase::setIcon(const Image& icon)
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
		m_window_name = title;
		glfwSetWindowTitle(m_window, m_window_name.c_str());
	}
	
	const std::string& ContextBase::getWindowTitle() const
	{
		return m_window_name;
	}

	const bs::vec2i& ContextBase::getWindowSize() const
	{
		return m_size;
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
		//Start ImGui init
		IMGUI_CHECKVERSION();
		auto* ctximgui = ImGui::CreateContext();
		ImGui::SetCurrentContext(ctximgui);

		auto& io = ImGui::GetIO();

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
