#include "PerspectiveCameraController.h"

#include "Input.h"

#include "GLFW/glfw3.h"

PerspectiveCameraController::PerspectiveCameraController(float aspectRatio)
	: m_Camera(glm::radians(45.0f), aspectRatio, 0.1f, 10000.0f)
	, m_AspectRatio(aspectRatio)
{
	m_Camera.SetPosition({-10, 5, -10}, {0, 5, 5});
}

void PerspectiveCameraController::OnUpdate(float ms)
{
	if (Input::IsKeyPressed(GLFW_KEY_W))
		m_Camera.Translate(m_CameraSpeed * m_Camera.GetFront() * ms);
	if (Input::IsKeyPressed(GLFW_KEY_S))
		m_Camera.Translate(-m_CameraSpeed * m_Camera.GetFront() * ms);
	if (Input::IsKeyPressed(GLFW_KEY_A))
		m_Camera.Translate(-glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) *
						   m_CameraSpeed * ms);
	if (Input::IsKeyPressed(GLFW_KEY_D))
		m_Camera.Translate(glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) *
						   m_CameraSpeed * ms);

	static float lastX = Input::GetMouseX(), lastY = Input::GetMouseY();
	float xPos = Input::GetMouseX(), yPos = Input::GetMouseY();

	if (!Input::Cursor())
	{
		float yaw = lastX - xPos;
		float pitch = lastY - yPos;

		float sensitivity = 0.1f;
		yaw *= sensitivity;
		pitch *= sensitivity;

		m_Camera.Rotate(yaw, pitch);
	}

	lastX = xPos;
	lastY = yPos;
}

void PerspectiveCameraController::OnWindowResized(int width, int height)
{
	assert(width && height);
	m_AspectRatio = static_cast<float>(width) / height;
	m_Camera.SetProjection(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 10000.0f);
}
