#include "Window.h"

#include "ApplicationEvent.h"
#include "Event/KeyEvent.h"

Neon::Window::Window(const std::string& name)
	: m_Data{name, WIDTH, HEIGHT}
{
	Init();
}

Neon::Window::~Window()
{
	Shutdown();
}

void Neon::Window::OnUpdate()
{
	glfwPollEvents();
}

void Neon::Window::ResetResized()
{
	m_Data.Resized = false;
}

void Neon::Window::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_Window = glfwCreateWindow(WIDTH, HEIGHT, m_Data.Title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, &this->m_Data);
	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		auto data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->Width = width;
		data->Height = height;
		data->Resized = true;
		Neon::WindowResizeEvent event(width, height);
		data->EventCallback(event);
	});
	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
		auto data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		Neon::WindowCloseEvent event;
		data->EventCallback(event);
	});
	glfwSetKeyCallback(m_Window,
					   [](GLFWwindow* window, int key, int scancode, int action, int mods) {
						   WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

						   switch (action)
						   {
						   case GLFW_PRESS:
						   {
							   Neon::KeyPressedEvent event(key);
							   data.EventCallback(event);
							   break;
						   }
						   case GLFW_RELEASE:
						   {
							   Neon::KeyReleasedEvent event(key);
							   data.EventCallback(event);
							   break;
						   }
						   default: break;
						   }
					   });
}

void Neon::Window::Shutdown()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}
