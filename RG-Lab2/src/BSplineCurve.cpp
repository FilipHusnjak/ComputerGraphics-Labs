#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

#include "BSplineCurve.h"

void BSplineCurve::OnUpdate(float ms)
{
	m_CurrentT += ms * m_Velocity;
	if (m_CurrentT > 1.f)
	{
		m_CurrentT -= 1.f;
		m_CurrentPoint++;
		if (m_CurrentPoint >= m_Points.size() - 3) m_CurrentPoint = 0;
	}
}

void BSplineCurve::Render()
{
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < m_Points.size() - 3; i++)
		for (float t = 0.f; t < 1.f; t += m_Step)
			glVertex3fv(glm::value_ptr(
				GetCurvePoint(m_Points[i], m_Points[i + 1], m_Points[i + 2], m_Points[i + 3], t)));
	glEnd();
}

void BSplineCurve::RenderControlPoints()
{
	glBegin(GL_POINTS);
	for (const auto& point : m_Points)
		glVertex3fv(glm::value_ptr(point));
	glEnd();
}

void BSplineCurve::RenderNormals()
{
	glBegin(GL_LINES);
	for (size_t i = 0; i < m_Points.size() - 3; i++)
	{
		for (float t = 0.f; t < 1.f; t += m_Step)
		{
			glm::vec3 point =
				GetCurvePoint(m_Points[i], m_Points[i + 1], m_Points[i + 2], m_Points[i + 3], t);
			glm::vec3 normal = GetSecondDerivative(m_Points[i], m_Points[i + 1], m_Points[i + 2],
												   m_Points[i + 3], t);
			glVertex3fv(glm::value_ptr(point));
			glVertex3fv(glm::value_ptr(point + normal));
		}
	}
	glEnd();
}

glm::mat4 BSplineCurve::GetObjectModelMatrix()
{
	glm::mat4 model = glm::translate(
		glm::mat4(1.f),
		GetCurvePoint(m_Points[m_CurrentPoint], m_Points[m_CurrentPoint + 1],
					  m_Points[m_CurrentPoint + 2], m_Points[m_CurrentPoint + 3], m_CurrentT));
	glm::vec3 z =
		GetFirstDerivative(m_Points[m_CurrentPoint], m_Points[m_CurrentPoint + 1],
						   m_Points[m_CurrentPoint + 2], m_Points[m_CurrentPoint + 3], m_CurrentT);
	glm::vec3 y =
		GetSecondDerivative(m_Points[m_CurrentPoint], m_Points[m_CurrentPoint + 1],
							m_Points[m_CurrentPoint + 2], m_Points[m_CurrentPoint + 3], m_CurrentT);
	glm::vec3 x = glm::normalize(glm::cross(y, z));
	glm::mat4 rot(x[0], y[0], z[0], 0, x[1], y[1], z[1], 0, x[2], y[2], z[2], 0, 0, 0, 0, 1);
	return model * glm::inverse(rot);
}

glm::vec3 BSplineCurve::GetPosition()
{
	return GetCurvePoint(m_Points[m_CurrentPoint], m_Points[m_CurrentPoint + 1],
						 m_Points[m_CurrentPoint + 2], m_Points[m_CurrentPoint + 3], m_CurrentT);
}

glm::vec3 BSplineCurve::GetVelocity()
{
	return -GetFirstDerivative(m_Points[m_CurrentPoint], m_Points[m_CurrentPoint + 1],
							   m_Points[m_CurrentPoint + 2], m_Points[m_CurrentPoint + 3],
							   m_CurrentT);
}

glm::vec3 BSplineCurve::GetCurvePoint(const glm::vec3& point1, const glm::vec3& point2,
									  const glm::vec3& point3, const glm::vec3& point4, float t)
{
	glm::vec4 v = {t * t * t, t * t, t, 1};
	glm::mat3x4 r(point1[0], point2[0], point3[0], point4[0], point1[1], point2[1], point3[1],
				  point4[1], point1[2], point2[2], point3[2], point4[2]);
	return v * (1.f / 6) * m0 * r;
}

glm::vec3 BSplineCurve::GetFirstDerivative(const glm::vec3& point1, const glm::vec3& point2,
										   const glm::vec3& point3, const glm::vec3& point4,
										   float t)
{
	glm::vec3 v = {t * t, t, 1};
	glm::mat3x4 r(point1[0], point2[0], point3[0], point4[0], point1[1], point2[1], point3[1],
				  point4[1], point1[2], point2[2], point3[2], point4[2]);
	return glm::normalize(v * m1 * r);
}

glm::vec3 BSplineCurve::GetSecondDerivative(const glm::vec3& point1, const glm::vec3& point2,
											const glm::vec3& point3, const glm::vec3& point4,
											float t)
{
	glm::vec2 v = {t, 1};
	glm::mat3x4 r(point1[0], point2[0], point3[0], point4[0], point1[1], point2[1], point3[1],
				  point4[1], point1[2], point2[2], point3[2], point4[2]);
	return glm::normalize(v * m2 * r);
}
