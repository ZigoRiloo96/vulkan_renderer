
#include "PCH.h"

void 
InitCamera(Camera* camera)
{
	// rot
	camera->focalPoint = { 0.0f, 0.0f, 0.0f };
	camera->position = { -5, 5, 5 };
  camera->distance = glm::distance(camera->position, camera->focalPoint);
	camera->pitch = PI32 / 4.0f;
	camera->yaw = 3.0f * PI32 / 4.0f;

	UpdateCameraView(camera);
}

void
UpdateCameraView(Camera* camera)
{
	// glm::vec3 rotation = glm::eulerAngles(camera->orientation) * (180.0f / PI32);

	camera->view = glm::translate(glm::mat4(1.0f), camera->position) * glm::toMat4(camera->orientation);

	camera->view = glm::inverse(camera->view);
}

void UpdateCamera(Camera* camera, Input* input, f32 delta)
{
	float yaw = GetAxis(input, Axis::RIGHT_STICK_X);
	float pitch = GetAxis(input, Axis::RIGHT_STICK_Y);

	camera->yaw += yaw * delta * 5.0f;
	camera->pitch += pitch * delta * 5.0f;

	camera->orientation = glm::quat(glm::vec3(-camera->pitch, -camera->yaw, 0.0f));

	float x = GetAxis(input, Axis::LEFT_STICK_X);
	float y = GetAxis(input, Axis::LEFT_STICK_Y);


	float rt = GetAxis(input, Axis::RIGHT_TRIGGER);
	float lt = GetAxis(input, Axis::LEFT_TRIGGER);

	glm::vec3 forward = glm::rotate(camera->orientation, glm::vec3(0.0f, 0.0f, -1.0f));
	glm::vec3 right = glm::rotate(camera->orientation, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec3 up = glm::rotate(camera->orientation, glm::vec3(0.0f, 1.0f, 0.0f));

	camera->position -= forward * y * delta * 10.0f;
	camera->position += right * x * delta * 10.0f;

	camera->position += up * rt * delta * 10.0f;
	camera->position -= up * lt * delta * 10.0f;


	UpdateCameraView(camera);
}