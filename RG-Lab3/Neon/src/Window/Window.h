#pragma once

#include "Event.h"
#include "Event/ApplicationEvent.h"
#include "Event/Event.h"

#include <GLFW/glfw3.h>

#define WIDTH 1920

#define HEIGHT 1080

namespace Neon
{
class Window
{
public:
	using EventCallbackFn = std::function<void(Event&)>;

	explicit Window(const std::string& name);
	~Window();

	static void OnUpdate();

	[[nodiscard]] inline bool Resized() const
	{
		return m_Data.Resized;
	}

	void ResetResized();

	[[nodiscard]] inline unsigned int GetWidth() const
	{
		return m_Data.Width;
	}

	[[nodiscard]] inline unsigned int GetHeight() const
	{
		return m_Data.Height;
	}

	[[nodiscard]] inline GLFWwindow* GetNativeWindow() const
	{
		return m_Window;
	}

	inline void SetEventCallback(const EventCallbackFn& callback)
	{
		m_Data.EventCallback = callback;
	}

private:
	void Init();
	void Shutdown();

private:
	GLFWwindow* m_Window = nullptr;

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height;
		EventCallbackFn EventCallback;
		bool Resized = false;
	};

	WindowData m_Data;
};
} // namespace Neon
