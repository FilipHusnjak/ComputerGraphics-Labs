#pragma once

#include "Shader.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#define MAX_PARTICLES 100000

class PerspectiveCamera;

class Particle
{
public:
	struct Vertex
	{
		glm::vec3 pos;
	};

public:
	Particle() = default;
	bool Update(float deltaMiliseconds);
	glm::vec3 GetColor() const
	{
		return {1.f - m_ElapsedTime / m_LifeLength, 0.f, 0.f};
	}
	bool IsAlive() const
	{
		return m_ElapsedTime < m_LifeLength;
	}
	bool operator<(Particle& other) const
	{
		return this->m_CameraDistance > other.m_CameraDistance;
	}

public:
	glm::vec3 m_Position{};
	glm::vec3 m_Velocity{};
	glm::vec3 m_Color{};
	float m_GravityEffect{};
	float m_LifeLength{};
	float m_CameraDistance{};

	float m_ElapsedTime{};
};

class ParticleSystem
{
public:
	ParticleSystem(const glm::vec3& position);
	~ParticleSystem();

	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem& operator=(const ParticleSystem&) = delete;
	ParticleSystem(const ParticleSystem&&) = delete;
	ParticleSystem& operator=(const ParticleSystem&&) = delete;

	void Render(const PerspectiveCamera& camera);
	void Update(float deltaMiliseconds, glm::vec3 jetPosition, glm::vec3 jetVelocity);

private:
	void SpawnParticle(glm::vec3 initialVelocity);
	void SpawnParticle(uint32_t index, glm::vec3 initialVelocity);
	void CreateTexture(uint32_t& texture, unsigned char* pixels, GLsizei texWidth,
					   GLsizei texHeight);

private:
	glm::vec3 m_Position{};

	Particle m_Particles[MAX_PARTICLES];
	glm::vec3 m_Positions[MAX_PARTICLES];
	glm::vec3 m_Colors[MAX_PARTICLES];

	float m_SpawnTimer = 0.5f;
	float m_ElapsedTime = 0.f;

	std::vector<uint32_t> m_FreeIndices;

	const std::vector<float> m_ParticleVerticies = {
		-0.5f, -0.5f, 0.f, 0.f, 0.f, -0.5f, 0.5f,  0.f, 0.f, 1.f, 0.5f, 0.5f, 0.f, 1.f, 1.f,
		-0.5f, -0.5f, 0.f, 0.f, 0.f, 0.5f,	-0.5f, 0.f, 1.f, 0.f, 0.5f, 0.5f, 0.f, 1.f, 1.f};

	uint32_t m_VertexArrayObject{};
	uint32_t m_VertexBuffer{};
	uint32_t m_PositionsBuffer{};
	uint32_t m_ColorsBuffer{};
	uint32_t m_ParticleTexture{};
};
