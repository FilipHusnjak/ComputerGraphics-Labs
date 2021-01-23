#pragma once

#include "PerspectiveCamera.h"

class PerspectiveCameraController
{
public:
	PerspectiveCameraController(float aspectRatio);

	void OnUpdate(float ms);

	PerspectiveCamera& GetCamera()
	{
		return m_Camera;
	}

	const PerspectiveCamera& GetCamera() const
	{
		return m_Camera;
	}

	float GetZoomLevel() const
	{
		return m_ZoomLevel;
	}

	void SetZoomLevel(float level)
	{
		m_ZoomLevel = level;
	}

	void OnWindowResized(int width, int height);

private:
	PerspectiveCamera m_Camera;
	float m_AspectRatio;
	float m_CameraSpeed = 0.01f;
	float m_ZoomLevel = 1.0f;
};
