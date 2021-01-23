#include "Particle.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "PerspectiveCamera.h"
#include "glm/gtx/norm.hpp"
#include "stb_image.h"

glm::vec3 GetRandomPoint(glm::vec2 dx, glm::vec2 dy, glm::vec2 dz)
{
	float x = (float)rand() / RAND_MAX * (dx[1] - dx[0]) + dx[0];
	float y = (float)rand() / RAND_MAX * (dy[1] - dy[0]) + dy[0];
	float z = (float)rand() / RAND_MAX * (dz[1] - dz[0]) + dz[0];
	return {x, y, z};
}

bool Particle::Update(float deltaMiliseconds)
{
	if (m_ElapsedTime >= m_LifeLength) return false;

	m_Position += m_Velocity * deltaMiliseconds / 1000.f;
	m_ElapsedTime += deltaMiliseconds;
	m_Velocity += m_GravityEffect * glm::vec3(0.f, -1.f, 0.f) * deltaMiliseconds / 1000.f;
	
	return m_ElapsedTime >= m_LifeLength;
}

ParticleSystem::ParticleSystem(const glm::vec3& position)
	: m_Position(position)
{
	glGenVertexArrays(1, &m_VertexArrayObject);
	glBindVertexArray(m_VertexArrayObject);

	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_ParticleVerticies.size() * sizeof(m_ParticleVerticies[0]),
				 m_ParticleVerticies.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_PositionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(float), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &m_ColorsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_ColorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(float), NULL, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, m_ColorsBuffer);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);

	glBindVertexArray(0);

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures/Flame_Particle.png", &texWidth, &texHeight, &texChannels,
								STBI_rgb_alpha);
	assert(pixels);
	CreateTexture(m_ParticleTexture, pixels, texWidth, texHeight);
	stbi_image_free(pixels);

	m_FreeIndices.reserve(MAX_PARTICLES);
	for (int i = 0; i < MAX_PARTICLES; i++)
		m_FreeIndices.push_back(i);
}

ParticleSystem::~ParticleSystem()
{
	glDeleteVertexArrays(1, &m_VertexArrayObject);
	glDeleteBuffers(1, &m_VertexBuffer);
	glDeleteBuffers(1, &m_PositionsBuffer);
	glDeleteBuffers(1, &m_ColorsBuffer);
}

void ParticleSystem::Render(const PerspectiveCamera& camera)
{
	if (m_FreeIndices.size() == MAX_PARTICLES) return;

	glDepthMask(false);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (!m_Particles[i].IsAlive()) continue;

		m_Particles[i].m_CameraDistance =
			glm::length2(m_Particles[i].m_Position - camera.GetPosition());
	}

	std::sort(&m_Particles[0], &m_Particles[MAX_PARTICLES]);

	uint32_t particleCount = 0;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (!m_Particles[i].IsAlive()) continue;

		m_Positions[particleCount] = m_Particles[i].m_Position;
		m_Colors[particleCount] = m_Particles[i].GetColor();
		particleCount++;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(float) * 3, m_Positions);

	glBindBuffer(GL_ARRAY_BUFFER, m_ColorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(float) * 3, m_Colors);

	glBindVertexArray(m_VertexArrayObject);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particleCount);

	glDepthMask(true);
}

void ParticleSystem::SpawnParticle(glm::vec3 initialVelocity)
{
	if (m_FreeIndices.empty()) return;

	SpawnParticle(m_FreeIndices.back(), initialVelocity);
	m_FreeIndices.pop_back();
}

void ParticleSystem::SpawnParticle(uint32_t index, glm::vec3 initialVelocity)
{
	const float delta = 0.5f;
	glm::vec2 dx = {m_Position.x - delta, m_Position.x + delta};
	glm::vec2 dy = {m_Position.y - delta, m_Position.y + delta};
	glm::vec2 dz = {m_Position.z - delta, m_Position.z + delta};

	float vx = (float)rand() / RAND_MAX * 0.5f - 0.25f;
	float vz = (float)rand() / RAND_MAX * 0.5f - 0.25f;

	float r = (float)rand() / RAND_MAX;
	float g = (float)rand() / RAND_MAX;
	float b = (float)rand() / RAND_MAX;

	m_Particles[index] = {GetRandomPoint(dx, dy, dz), 2.f * glm::normalize(initialVelocity),
						  glm::vec3{r, g, b}, 0.2f, 3000.f};
}

void ParticleSystem::Update(float deltaMiliseconds, glm::vec3 jetPosition, glm::vec3 jetVelocity)
{
	m_Position = jetPosition;

	m_ElapsedTime += deltaMiliseconds;
	if (m_ElapsedTime > m_SpawnTimer)
	{
		m_ElapsedTime -= m_SpawnTimer;
		SpawnParticle(jetVelocity);
	}

	if (m_FreeIndices.size() == MAX_PARTICLES) return;

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (m_Particles[i].Update(deltaMiliseconds)) { m_FreeIndices.push_back(i); }
	}
}

void ParticleSystem::CreateTexture(uint32_t& texture, unsigned char* pixels, GLsizei texWidth,
								   GLsizei texHeight)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				 pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
}
