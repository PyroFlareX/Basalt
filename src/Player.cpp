#include "Player.h"
#include <iostream>

Player::Player()
{
	pos = vn::vec3(50.0f, 50.0f, 50.0f);
	rot = vn::vec3(0.0f, 0.0f, 0.0f);
	velocity = vn::vec3(0.0f, 0.0f, 0.0f);
}

void Player::getInput(Input::Inputs vInput, bool VRmode)
{
	if(!VRmode)
	{
		vn::vec3 change = glm::vec3(0.0f);
		float speed = 3.0f;

		change.x += glm::cos(glm::radians(rot.y)) * speed * vInput.LX;
		change.z += glm::sin(glm::radians(rot.y)) * speed * vInput.LX;
		
		if (vInput.up)
		{
			jump();
		}
		if (vInput.down)
		{
			velocity.y-= 5;
		}

		//Backwards
		if (vInput.LY < 0)
		{
			change.x += -glm::cos(glm::radians(rot.y + 90)) * speed * vInput.LY;
			change.z += -glm::sin(glm::radians(rot.y + 90)) * speed * vInput.LY;
		}

		//Movement
		if (vInput.accel)
		{
			speed *= 3.0f;
		}

		//Forward
		if (vInput.LY > 0)
		{
			change.x += -glm::cos(glm::radians(rot.y + 90)) * speed * vInput.LY;
			change.z += -glm::sin(glm::radians(rot.y + 90)) * speed * vInput.LY;
		}

		velocity += change;

		//Rotation
		rot.x += vInput.RY * 0.05f;
		rot.y += vInput.RX * 0.05f;
	}
	else
	{
		//VR input parsing here
	}
}

void Player::jump()
{
	velocity.y += 3;
}

void Player::update(float dt)
{
	//velocity.y -= 0.7f;

	pos += velocity * dt;
	velocity = vn::vec3(0.0f);
	
	//vn::vr::getDeviceTransform(vr::k_unTrackedDeviceIndex_Hmd);
	
	//std::cout << pos.x << " " << pos.y << " " << pos.z << "\n";
	//std::cout << rot.x << " " << rot.y << " " << rot.z << "\n";
}

Player::~Player()
{

}
