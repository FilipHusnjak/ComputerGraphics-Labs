#pragma once

#include <memory>
#include <string>

#include "BSplineCurve.h"
#include "Entity.h"
#include "GLFW/glfw3.h"
#include "PerspectiveCameraController.h"
#include "Shader.h"

class Game
{
public:
	static void CreateGame(const std::string& title, int width, int height)
	{
		Game* game = new Game(title, width, height);
		s_Game = std::unique_ptr<Game>(game);
	}
	static Game& Get()
	{
		assert(s_Game);
		return *s_Game;
	}

public:
	~Game();
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(const Game&& other) = delete;
	Game& operator=(const Game&& other) = delete;

	void Update(float ms);
	void Render();

	bool IsRunning() const
	{
		return !glfwWindowShouldClose(m_Window);
	}
	GLFWwindow* GetWindow()
	{
		return m_Window;
	}

private:
	Game(const std::string& title, int width, int height);

private:
	static void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void OnWindowResized(GLFWwindow* window, int width, int height);

private:
	GLFWwindow* m_Window = nullptr;

	bool m_Minimized = false;

	PerspectiveCameraController m_CameraController;

	Entity m_Object{};
	Entity m_Land{};
	Entity m_Skydome{};

	BSplineCurve m_BSpline{};

	Shader m_ObjectShader{};
	Shader m_LandShader{};
	Shader m_SkydomeShader{};
	Shader m_MarkerShader{};

private:
	static std::unique_ptr<Game> s_Game;
};
