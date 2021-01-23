#include <cassert>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "glad/glad.h"

#include "Game.h"
#include "Input.h"

std::unique_ptr<Game> Game::s_Game;

Game::Game(const std::string& title, int width, int height)
	: m_CameraController((float)width / height)
{
	auto result = glfwInit();
	assert(result);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	m_Window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	assert(m_Window);
	glfwMakeContextCurrent(m_Window);

	glfwSetWindowSizeCallback(m_Window, OnWindowResized);
	glfwSetKeyCallback(m_Window, OnKeyPressed);
	glfwSwapInterval(0);

	result = gladLoadGL();
	assert(result);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.f, 1.f, 1.f, 1.f);
	glViewport(0, 0, width, height);

	m_Object.Load("models/f16.obj", 2.f, {255, 0, 0, 255}, "");
	m_Land.Load("models/mountain.fbx", 100.f, {255, 0, 0, 255}, "textures/Normal.tga");
	m_Skydome.Load("models/dome.obj", 5000.f, {255, 0, 0, 255}, "");

	m_ObjectShader =
		std::make_shared<Shader>("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
	m_LandShader =
		std::make_shared<Shader>("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
	m_SkydomeShader = std::make_shared<Shader>("src/shaders/vertex_skydome.glsl",
											   "src/shaders/fragment_skydome.glsl");
	m_ParticleShader = std::make_shared<Shader>("src/shaders/vertex_particle.glsl",
												"src/shaders/fragment_particle.glsl");

	m_Skydome.Load("models/dome.obj", 5000.f, {255, 0, 0, 255});	

	m_ParticleSystems.push_back(std::make_shared<ParticleSystem>(glm::vec3{0.f, 0.f, 0.f}));
}

Game::~Game()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Game::Update(float deltaMiliseconds)
{
	m_CameraController.OnUpdate(deltaMiliseconds);

	m_BSpline.OnUpdate(deltaMiliseconds);

	for (auto& particleSystem : m_ParticleSystems)
	{
		particleSystem->Update(deltaMiliseconds, m_BSpline.GetPosition(), m_BSpline.GetVelocity());
	}
}

void Game::Render()
{
	if (m_Minimized) return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render sky
	m_SkydomeShader->Use();
	m_SkydomeShader->SetMat4f("model", glm::translate(glm::mat4(1.f), {0.f, -1200.f, 0.f}));
	m_SkydomeShader->SetMat4f("view", m_CameraController.GetCamera().GetViewMatrix());
	m_SkydomeShader->SetMat4f("projection", m_CameraController.GetCamera().GetProjectionMatrix());
	m_Skydome.Render();

	// Render object
	m_ObjectShader->Use();
	m_ObjectShader->SetInt("tex", 0);
	m_ObjectShader->SetMat4f("projection", m_CameraController.GetCamera().GetProjectionMatrix());
	m_ObjectShader->SetMat4f("view", m_CameraController.GetCamera().GetViewMatrix());
	m_ObjectShader->SetVec3f("cameraPos", m_CameraController.GetCamera().GetPosition());
	m_ObjectShader->SetMat4f("model", m_BSpline.GetObjectModelMatrix());
	m_Object.Render();

	// Render mountains
	m_LandShader->Use();
	m_LandShader->SetInt("tex", 0);
	m_LandShader->SetInt("normalMap", 1);
	m_LandShader->SetMat4f("projection", m_CameraController.GetCamera().GetProjectionMatrix());
	m_LandShader->SetMat4f("view", m_CameraController.GetCamera().GetViewMatrix());
	m_LandShader->SetVec3f("cameraPos", m_CameraController.GetCamera().GetPosition());
	m_LandShader->SetMat4f("model", glm::translate(glm::mat4(1.f), {0.f, -15.f, 0.f}));
	m_Land.Render();

	// Render particle systems
	m_ParticleShader->Use();
	m_ParticleShader->SetMat4f("view", m_CameraController.GetCamera().GetViewMatrix());
	m_ParticleShader->SetMat4f("projection", m_CameraController.GetCamera().GetProjectionMatrix());
	m_ParticleShader->SetMat4f("scale", glm::scale(glm::mat4(1.f), {1.f, 1.f, 1.f}));
	m_ParticleShader->SetVec3f("particleSystemCenter", m_BSpline.GetPosition());
	for (auto& particleSystem : m_ParticleSystems)
	{
		particleSystem->Render(m_CameraController.GetCamera());
	}

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Game::OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (Input::Cursor()) { Input::DisableCursor(); }
		else
		{
			Input::EnableCursor();
		}
	}
}

void Game::OnWindowResized(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0) return;
	Get().m_CameraController.OnWindowResized(width, height);
	glViewport(0, 0, width, height);
}
