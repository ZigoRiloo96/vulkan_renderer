#pragma once

struct Input;

struct 
Camera
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::quat orientation;

	glm::vec3 focalPoint;
	glm::vec3 position;

	float distance;
	float pitch;
	float yaw;
};

void
InitCamera(Camera* camera);

void
UpdateCamera(Camera* camera, Input* input, f32 delta);

void
UpdateCameraView(Camera* camera);