#ifndef INPUT_H
#define INPUT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Input
{
	struct Inputs
	{
		int AD;
		int WS;
		float mouseLR;
		float mouseUD;
		bool mouse1;
		bool mouseMiddle;
		bool mouse2;
		bool space;
		bool shift;
		bool escape;
		bool lctrl;
		bool tilda;
		double zoom = 0.0f;

		//probably bot implement
		bool oneclick; // this is to not select provinces over and over again after the first click
	};

	void setupInput();
	Inputs getInput(float dt);

	void resetZoom();

	extern GLFWwindow* window;
};

#endif // INPUT_H
