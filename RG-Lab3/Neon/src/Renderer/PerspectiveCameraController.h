#pragma once

#include "ApplicationEvent.h"
#include "Event.h"
#include "Event/ApplicationEvent.h"
#include "Event/Event.h"
#include "Event/KeyEvent.h"
#include "KeyEvent.h"
#include "PerspectiveCamera.h"

namespace Neon
{
class PerspectiveCameraController
{
public:
	explicit PerspectiveCameraController(float aspectRatio, bool rotation = false) noexcept;

	void OnUpdate(float ts);
	void OnEvent(Event& e);

	PerspectiveCamera& GetCamera()
	{
		return m_Camera;
	}

	[[nodiscard]] const PerspectiveCamera& GetCamera() const
	{
		return m_Camera;
	}

	[[nodiscard]] float GetZoomLevel() const
	{
		return m_ZoomLevel;
	}

	void SetZoomLevel(float level)
	{
		m_ZoomLevel = level;
	}

private:
	bool OnWindowResize(WindowResizeEvent& e);
	bool OnKeyPress(KeyPressedEvent& e);

private:
	PerspectiveCamera m_Camera;
	float m_AspectRatio;
	bool m_Rotation = false;
	float m_CameraSpeed = 0.02f;
	float m_ZoomLevel = 1.0f;
	bool m_Cursor = true;
};
} // namespace Neon
