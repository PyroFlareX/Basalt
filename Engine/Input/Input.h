#ifndef INPUT_H
#define INPUT_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

namespace Input {
	struct Inputs {
		bool up;
		bool down;
		int LX;
		int LY;
		int RX;
		int RY;
		bool pause;
		bool accel;
	};

	Inputs getInput();

	extern GLFWwindow *window;
};

#endif // INPUT_H
