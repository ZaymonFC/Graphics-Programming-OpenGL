#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(const glm::vec3 position, const glm::vec3 up, const float yaw, const float pitch)
{
	Front = glm::vec3(0.0f, 0.0f, 1.0f);
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;

	MovementSpeed = SPEED;
	MouseSensitivity = SENSITIVITY;

	UpdateCameraState();
}

auto Camera::GetViewMatrix() const -> glm::mat4
{
	return glm::lookAt(Position, Position + Front, Up);
}

auto Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) -> void
{
	const auto velocity = MovementSpeed * deltaTime;

	if (direction == FORWARD)
	{
		Position += Front * velocity;
	}
	if (direction == BACKWARD)
	{
		Position -= Front * velocity;
	}
	if (direction == LEFT)
	{
		Position -= Right * velocity;
	}
	if (direction == RIGHT)
	{
		Position += Right * velocity;
	}
}

auto Camera::ProcessMouseLook(float xOffset, float yOffset, GLboolean constrainPitch) -> void
{
	yOffset *= MouseSensitivity;
	xOffset *= MouseSensitivity;

	Yaw += xOffset;
	Pitch += yOffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
		{
			Pitch = 89.0f;
		}
		if (Pitch < -89.0f)
		{
			Pitch = -89.0f;
		}
	}

	UpdateCameraState();
}

auto Camera::UpdateCameraState() -> void
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	// Normalise the vector to prevent speed changes relative to pitch
	Front = glm::normalize(front);

	// Recalculate the camera Right and Up Vectors
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}


