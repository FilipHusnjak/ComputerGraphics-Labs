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

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glViewport(0, 0, width, height);
	glLineWidth(3);
	glPointSize(10);

	m_Object.Load("models/f16.obj", 2.f, {255, 0, 0, 255}, "");
	m_Land.Load("models/mountain.fbx", 100.f, {255, 0, 0, 255}, "textures/Normal.tga");
	m_Skydome.Load("models/dome.obj", 5000.f, {255, 0, 0, 255}, "");

	m_ObjectShader.Load("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
	m_LandShader.Load("src/shaders/vertex.glsl", "src/shaders/fragment_land.glsl");
	m_SkydomeShader.Load("src/shaders/vertex_skydome.glsl", "src/shaders/fragment_skydome.glsl");
	m_MarkerShader.Load("src/shaders/vertex_marker.glsl", "src/shaders/fragment_marker.glsl");
}

Game::~Game()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Game::Update(float ms)
{
	m_CameraController.OnUpdate(ms);
	m_BSpline.OnUpdate(ms);
}

void Game::Render()
{
	if (m_Minimized) return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render object
	m_ObjectShader.Use();
	m_ObjectShader.SetInt("tex", 0);
	m_ObjectShader.SetMat4f("projection", m_CameraController.GetCamera().GetProjectionMatrix());
	m_ObjectShader.SetMat4f("view", m_CameraController.GetCamera().GetViewMatrix());
	m_ObjectShader.SetVec3f("cameraPos", m_CameraController.GetCamera().GetPosition());
	m_ObjectShader.SetMat4f("model", m_BSpline.GetObjectModelMatrix());
	m_Object.Render();

	// Render mountains
	m_LandShader.Use();
	m_LandShader.SetInt("tex", 0);
	m_LandShader.SetInt("normalMap", 1);
	m_LandShader.SetMat4f("projection", m_CameraController.GetCamera().GetProjectionMatrix());
	m_LandShader.SetMat4f("view", m_CameraController.GetCamera().GetViewMatrix());
	m_LandShader.SetVec3f("cameraPos", m_CameraController.GetCamera().GetPosition());
	m_LandShader.SetMat4f("model", glm::translate(glm::mat4(1.f), {0.f, -15.f, 0.f}));
	m_Land.Render();

	m_MarkerShader.Use();
	m_MarkerShader.SetMat4f("projection", m_CameraController.GetCamera().GetProjectionMatrix());
	m_MarkerShader.SetMat4f("view", m_CameraController.GetCamera().GetViewMatrix());
	m_MarkerShader.SetMat4f("model", glm::mat4(1.f));

	// Render curve
	m_MarkerShader.SetVec3f("color", {0.f, 1.f, 0.f});
	m_BSpline.Render();

	// Render curve normals
	m_MarkerShader.SetVec3f("color", {0.f, 0.f, 1.f});
	m_BSpline.RenderNormals();

	// Render control points
	m_MarkerShader.SetVec3f("color", {1.f, 1.f, 0.f});
	m_BSpline.RenderControlPoints();

	// Render sky
	m_SkydomeShader.Use();
	m_SkydomeShader.SetMat4f("model", glm::translate(glm::mat4(1.f), {0.f, -1200.f, 0.f}));
	m_SkydomeShader.SetMat4f("view", m_CameraController.GetCamera().GetViewMatrix());
	m_SkydomeShader.SetMat4f("projection", m_CameraController.GetCamera().GetProjectionMatrix());
	m_Skydome.Render();

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
