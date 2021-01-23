#include "Input.h"

#include "Application.h"

bool Neon::Input::IsKeyPressed(int key)
{
	auto window = Application::Get().GetWindow().GetNativeWindow();
	auto state = glfwGetKey(window, static_cast<int32_t>(key));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Neon::Input::IsMouseButtonPressed(int button)
{
	auto window = Application::Get().GetWindow().GetNativeWindow();
	auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

std::pair<float, float> Neon::Input::GetMousePosition()
{
	auto window = Application::Get().GetWindow().GetNativeWindow();
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return {(float)xpos, (float)ypos};
}

float Neon::Input::GetMouseX()
{
	auto [x, y] = GetMousePosition();
	return x;
}

float Neon::Input::GetMouseY()
{
	auto [x, y] = GetMousePosition();
	return y;
}

void Neon::Input::EnableCursor()
{
	auto window = Application::Get().GetWindow().GetNativeWindow();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Neon::Input::DisableCursor()
{
	auto window = Application::Get().GetWindow().GetNativeWindow();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
