#include "PerspectiveCamera.h"

Neon::PerspectiveCamera::PerspectiveCamera(float fovY, float aspect, float zNear, float zFar)
	: m_ProjectionMatrix(glm::perspective(fovY, aspect, zNear, zFar))
{
	m_ProjectionMatrix[1][1] *= -1;
	RecalculateViewMatrix();
}

void Neon::PerspectiveCamera::SetProjection(float fovY, float aspect, float zNear, float zFar)
{
	m_ProjectionMatrix = glm::perspective(fovY, aspect, zNear, zFar);
	m_ProjectionMatrix[1][1] *= -1;
}

void Neon::PerspectiveCamera::RecalculateViewMatrix()
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}
