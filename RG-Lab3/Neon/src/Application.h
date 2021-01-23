#pragma once

#include "ApplicationEvent.h"
#include "Core/ImGuiLayer.h"
#include "Core/LayerStack.h"
#include "Event.h"
#include "Event/ApplicationEvent.h"
#include "Event/Event.h"
#include "ImGuiLayer.h"
#include "Layer.h"
#include "Window.h"
#include "Window/Window.h"

#include <chrono>

class Application
{
public:
	explicit Application(const std::string& name) noexcept;
	~Application();
	Application(const Application& other) = delete;
	Application& operator=(const Application& other) = delete;
	Application(const Application&& other) = delete;
	Application& operator=(const Application&& other) = delete;

	void Run();
	void OnEvent(Neon::Event& e);
	void PushLayer(Neon::Layer* layer);
	void PushOverlay(Neon::Layer* layer);

	[[nodiscard]] const inline Neon::Window& GetWindow() const noexcept
	{
		return m_Window;
	}

	static const Application& Get() noexcept
	{
		return *s_Instance;
	}

private:
	bool OnWindowClose(Neon::WindowCloseEvent& e);
	bool OnWindowResize(Neon::WindowResizeEvent& e);

private:
	static Application* s_Instance;

	Neon::Window m_Window;
	bool m_Running = true;
	bool m_Minimized = false;
	Neon::ImGuiLayer* m_ImGuiLayer;
	LayerStack m_LayerStack;
	std::chrono::time_point<std::chrono::steady_clock> m_LastFrameTime =
		std::chrono::high_resolution_clock::now();
};