#include "PerspectiveCameraController.h"

#include "ApplicationEvent.h"
#include "Core/Input.h"
#include "Event.h"
#include "KeyEvent.h"

#include <GLFW/glfw3.h>

Neon::PerspectiveCameraController::PerspectiveCameraController(float aspectRatio,
															   bool rotation) noexcept
	: m_Camera(glm::radians(45.0f), aspectRatio, 0.1f, 10000.0f)
	, m_AspectRatio(aspectRatio)
	, m_Rotation(rotation)
{
	m_Camera.SetPosition({-25, 10, -50}, {-12, 2, -16});
}

void Neon::PerspectiveCameraController::OnUpdate(float ts)
{
	if (Neon::Input::IsKeyPressed(GLFW_KEY_W))
		m_Camera.Translate(m_CameraSpeed * m_Camera.GetFront() * ts);
	if (Neon::Input::IsKeyPressed(GLFW_KEY_S))
		m_Camera.Translate(-m_CameraSpeed * m_Camera.GetFront() * ts);
	if (Neon::Input::IsKeyPressed(GLFW_KEY_A))
		m_Camera.Translate(-glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) *
						   m_CameraSpeed * ts);
	if (Neon::Input::IsKeyPressed(GLFW_KEY_D))
		m_Camera.Translate(glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) *
						   m_CameraSpeed * ts);

	static float lastX = Neon::Input::GetMouseX(), lastY = Neon::Input::GetMouseY();
	float xPos = Neon::Input::GetMouseX(), yPos = Neon::Input::GetMouseY();
	if (!m_Cursor)
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

void Neon::PerspectiveCameraController::OnEvent(Neon::Event& e)
{
	Neon::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Neon::WindowResizeEvent>(
		[this](Neon::WindowResizeEvent& e) { return OnWindowResize(e); });
	dispatcher.Dispatch<Neon::KeyPressedEvent>(
		[this](Neon::KeyPressedEvent& e) { return OnKeyPress(e); });
}

bool Neon::PerspectiveCameraController::OnWindowResize(Neon::WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0) return false;
	m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
	m_Camera.SetProjection(glm::radians(45.0f), (float)e.GetWidth() / (float)e.GetHeight(), 0.1f,
						   10000.0f);
	return false;
}

bool Neon::PerspectiveCameraController::OnKeyPress(Neon::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == GLFW_KEY_ESCAPE)
	{
		m_Cursor = !m_Cursor;
		if (m_Cursor) { Neon::Input::EnableCursor(); }
		else
		{
			Neon::Input::DisableCursor();
		}
	}

	return false;
}
