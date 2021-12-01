#include "../Input.h"
#include "../../Util/UtilFunctions.h"
#include "../../Types/Types.h"

#ifndef _NX

#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>

namespace Input
{
	GLFWwindow* window;
	
	Inputs input = { };

	bool enableCursor = false;
	// bool updatemovement = false;

	double mX1 = 0, mY1 = 0;
	double mX2 = 0, mY2 = 0;

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto& io = ImGui::GetIO();
		/*if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
			if (!input.tilda) {
				input.tilda = true;
				io.KeysDown['`'] = true;
			}
			else {
				input.tilda = false;
				io.KeysDown['`'] = false;
			}
		}*/
		if(key >= 0 && key < 512)
		{
			if(action == GLFW_PRESS)
			{
				io.KeysDown[key] = true;
			}
			if(action == GLFW_RELEASE)
			{
				io.KeysDown[key] = false;
			}
		}

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		//input.zoom = xoffset; horizontal scrollwheel which doesnt exist lmao
		auto& io = ImGui::GetIO();
		
		io.MouseWheel = yoffset;
		io.MouseWheelH = xoffset;
	}

	void mouse_callback(GLFWwindow* window, int button, int action, int modifier) 
	{
		//std::cout << "BUTTON:" << button << std::endl; // 0 == LMB, 1 == RMB, 2 == MMB, 3 == BACKSIDEBTN, 4 == FRNTSIDEBTN
		//std::cout << "ACTION:" << action << std::endl; // 1 == PRESSED, 0 == RELEASED
		//std::cout << "MODIFIER:" << modifier << std::endl; // 0 == NONE, 1 == SHIFT, 2 == CTRL
		auto& io = ImGui::GetIO();
		
		if (button == 0) 
		{
			if (action == 1) 
			{
				if (!input.oneclick) 
				{
					input.oneclick = true;
				}
				else 
				{
					input.oneclick = false;
				}
				input.mouse1 = true;
				io.MouseDown[button] = true;
				io.MouseClicked[button] = true;
				io.MouseClickedPos[button] = ImVec2(mX1, mY1);
				
			}
			else if (action == 0) 
			{
				input.mouse1 = false;
				io.MouseDown[button] = false;
				io.MouseClicked[button] = false;
			}
		}
		if (button == 1)
		{
			if (action == 1) 
			{
				glfwGetCursorPos(window, &mX2, &mY2);

				io.MouseClicked[button] = true;
				io.MouseDown[button] = true;
			}
			else if (action == 0) 
			{
				io.MouseDown[button] = false;
				io.MouseClicked[button] = false;
			}
		}
		if (button == 2)
		{
			if (action == 1) 
			{
				glfwGetCursorPos(window, &mX2, &mY2);

				io.MouseClicked[button] = true;
				io.MouseDown[button] = true;
			}
			else if (action == 0) 
			{
				io.MouseDown[button] = false;
				io.MouseClicked[button] = false;
			}
		}
	}

	void char_callback(GLFWwindow* window, unsigned int c)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(c);
	}

	void setupInput() 
	{
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetMouseButtonCallback(window, mouse_callback);
		glfwSetKeyCallback(window, key_callback); 
		glfwSetCharCallback(window, char_callback);

		enableCursor = true;
	}

	Inputs getInput(float dt)
	{
		input.space = false;
		input.shift = false;
		input.AD = 0;
		input.WS = 0;
		input.mouseLR = 0;
		input.mouseUD = 0;
		input.escape = false;
		input.lctrl = false;

		double x = 0, y = 0;

		auto& io = ImGui::GetIO();
		io.MousePosPrev = ImVec2(mX1, mY1);
		glfwGetCursorPos(window, &mX1, &mY1);
		io.MousePos = ImVec2(mX1, mY1);
		io.MouseDelta = ImVec2(mX1 - io.MousePosPrev.x, mY1 - io.MousePosPrev.y);

		if(input.mouse1)
		{
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0] = ImVec2(mX1, mY1);
		}
		
		if(io.WantCaptureMouse)
		{
		
		}

		if(io.WantCaptureKeyboard)
		{
			return input;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			input.WS++; //LY
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			input.WS--;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			input.AD--; //LX
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			input.AD++;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			input.space = true; //up
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			input.shift = true; //down
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
		{
			input.escape = true; //pause
			// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			enableCursor = !enableCursor;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			input.lctrl = true; //accel
			io.KeyCtrl = true;
		}
		else
		{
			io.KeyCtrl = false;
		}
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		{
			io.KeyAlt = true;
		}
		else
		{
			io.KeyAlt = false;
		}

		if(!enableCursor)
		{
			glfwGetCursorPos(window, &x, &y);
			//glfwSetCursorPos(window, 0, 0);
			input.mouseUD -= y;
			input.mouseLR += x;
			
			/*input.mouseUD = -io.MouseDelta.y;
			input.mouseLR = io.MouseDelta.x;*/
			
			//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		
		return input;
	}

	void resetZoom() {
		input.zoom = 0.0f;
	}
} 

#endif // _NX