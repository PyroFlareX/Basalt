#pragma once

#include "../Engine/Engine.h"

class Player : public vn::Transform {
public:
	Player();

	void getInput(Input::Inputs inputs, bool VRmode);

	void jump();

	void update(float dt);

	~Player();

private:

	bool isFlying;
	vn::vec3 velocity;
};

