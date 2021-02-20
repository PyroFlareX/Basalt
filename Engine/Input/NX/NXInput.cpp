#include "../Input.h"

#ifdef NX

#include <switch.h>

Input::Inputs Input::getInput()
{
	Input::Inputs input = { false, false, 0.0f, 0.0f, 0.0f, 0.0f, false, false };

	// Scan all the inputs. This should be done once for each frame
	hidScanInput();

	// hidKeysDown returns information about which buttons have been just pressed in this frame compared to the previous one
	u64 kDown = hidKeysHeld(CONTROLLER_P1_AUTO);


	JoystickPosition pos_left, pos_right;

	//Read the joysticks' position
	hidJoystickRead(&pos_left, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);
	hidJoystickRead(&pos_right, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);

	//Set Virtual Leftstick
	input.LX = pos_left.dx;
	input.LY = pos_left.dy;

	//Set Virtual Rightstick
	input.RX = pos_right.dx;
	input.RY = pos_right.dy;

	if (kDown & KEY_B)
	{
		input.up = true;
	}
	if (kDown & KEY_ZL)
	{
		input.down = true;
	}
	if (kDown & KEY_PLUS)
	{
		input.pause = true;
	}
	if (kDown & KEY_Y)
	{
		input.accel = true;
	}

	return input;
}

#endif // NX

