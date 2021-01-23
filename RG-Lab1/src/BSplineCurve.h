#pragma once

#include <vector>

#include <glm/glm.hpp>

class BSplineCurve
{
public:
	void OnUpdate(float ms);
	void Render();
	void RenderControlPoints();
	void RenderNormals();
	glm::mat4 GetObjectModelMatrix();

private:
	glm::vec3 GetCurvePoint(const glm::vec3& point1, const glm::vec3& point2,
							const glm::vec3& point3, const glm::vec3& point4, float t);
	glm::vec3 GetFirstDerivative(const glm::vec3& point1, const glm::vec3& point2,
								 const glm::vec3& point3, const glm::vec3& point4, float t);
	glm::vec3 GetSecondDerivative(const glm::vec3& point1, const glm::vec3& point2,
								  const glm::vec3& point3, const glm::vec3& point4, float t);

private:
	std::vector<glm::vec3> m_Points{{0, 0, 0},	{0, 10, 5},	 {10, 10, 10}, {10, 0, 15},
									{0, 0, 20}, {0, 10, 25}, {10, 10, 30}, {10, 0, 35},
									{0, 0, 40}, {0, 10, 45}, {10, 10, 50}, {10, 0, 55}};
	const glm::mat4 m0{-1, 3, -3, 1, 3, -6, 0, 4, -3, 3, 3, 1, 1, 0, 0, 0};
	const glm::mat4x3 m1{-1, 2, -1, 3, -4, 0, -3, 2, 1, 1, 0, 0};
	const glm::mat4x2 m2{-1, 1, 3, -2, -3, 1, 1, 0};

	size_t m_CurrentPoint = 0;
	float m_CurrentT = 0;
	const float m_Step = 0.01f;
	const float m_Velocity = 0.001f;
};
