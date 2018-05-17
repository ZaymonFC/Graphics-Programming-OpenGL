#pragma once
#include <glm/mat4x4.hpp>
#include <glad/glad.h>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Camera Defaults
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
//const float ZOOM = 45.0f;

class Camera
{

public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// Euler Angles
	float Yaw;
	float Pitch;

	// Camera settings
	float MovementSpeed;
	float MouseSensitivity;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	
	auto GetViewMatrix() const -> glm::mat4;
	auto ProcessKeyboard(Camera_Movement direction, float deltaTime) -> void;
	auto ProcessMouseLook(float xOffset, float yOffset, GLboolean constrainPitch = true) -> void;

private:
	auto UpdateCameraState() -> void;

};

