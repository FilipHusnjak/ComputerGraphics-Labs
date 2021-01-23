#include "Renderer/VulkanRenderer.h"

#include "Application.h"

#include "ApplicationEvent.h"
#include "Event.h"
#include "ImGuiLayer.h"
#include "Layer.h"
#include "Sandbox3D.h"

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name) noexcept
	: m_Window(name)
{
	assert(s_Instance == nullptr);
	s_Instance = this;
	m_Window.SetEventCallback([this](Neon::Event& e) { OnEvent(e); });
	m_ImGuiLayer = new Neon::ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
	Neon::VulkanRenderer::Init(&m_Window);
	PushLayer(new Sandbox3D());
}

Application::~Application()
{
	Neon::VulkanRenderer::Shutdown();
}

void Application::OnEvent(Neon::Event& e)
{
	Neon::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Neon::WindowCloseEvent>(
		[this](Neon::WindowCloseEvent& e) { return OnWindowClose(e); });
	dispatcher.Dispatch<Neon::WindowResizeEvent>(
		[this](Neon::WindowResizeEvent& e) { return OnWindowResize(e); });

	for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
	{
		(*it)->OnEvent(e);
		if (e.Handled) break;
	}
}

void Application::PushLayer(Neon::Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Neon::Layer* layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

void Application::Run()
{
	while (m_Running)
	{
		auto time = std::chrono::high_resolution_clock::now();
		auto timeStep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		m_Window.OnUpdate();

		Neon::VulkanRenderer::Begin();
		if (!m_Minimized)
		{
			for (Neon::Layer* layer : m_LayerStack)
				layer->OnUpdate(
					std::chrono::duration<float, std::chrono::milliseconds::period>(timeStep)
						.count());
			Neon::ImGuiLayer::Begin();
			{
				for (Neon::Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			Neon::ImGuiLayer::End();
		}
		Neon::VulkanRenderer::End();
	}
}

bool Application::OnWindowClose(Neon::WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(Neon::WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimized = true;
		return false;
	}
	m_Minimized = false;
	return false;
}
