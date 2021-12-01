#pragma once

#include <Types/Types.h>

class Camera : public bs::Transform
{
public:
    Camera();
	~Camera() = default;

	//Get the view matrix of the camera
	bs::mat4 getViewMatrix() const;
	//Get the projection matrix for the scene
	bs::mat4 getProjMatrix() const;

	//Capture a transform to follow
	void follow(bs::Transform& entity);
	//Update the transform of the camera from the followed transform
	void update();
	
private:
	float lerp;
	
	//Mode 0 is default, 1 is Left Eye, 2 is Right Eye	//Or now ig for other stuff THE BOBSTER does
	int mode;
	//Projection matrix
	bs::mat4 proj;
	//Pointer to the followed entity
	bs::Transform* entityPos;
};

