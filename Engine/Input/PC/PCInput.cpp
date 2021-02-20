#include "../Input.h"

#ifndef _NX

#include <GLFW/glfw3.h>
#include <iostream>

namespace Input
{
	GLFWwindow* window;

	Inputs getInput()
	{
		double x = 0, y = 0;
		Inputs input = { false, false, 0, 0, 0, 0, false, false };

		static bool enableCursor = false;
		enableCursor = false;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			input.LY++;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			input.LY--;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			input.LX--;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			input.LX++;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			input.up = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			input.down = true;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			input.pause = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			enableCursor = !enableCursor;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			input.accel = true;
		}

		if (!enableCursor)
		{
			glfwGetCursorPos(window, &x, &y);
			glfwSetCursorPos(window, 0, 0);
			input.RY += y;
			input.RX += x;
		}
		return input;
	}
}

#endif // _NX