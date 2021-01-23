#include "Input.h"

#include "Game.h"

#include "GLFW/glfw3.h"

bool Input::s_Cursor = true;

bool Input::IsKeyPressed(int key)
{
	auto window = Game::Get().GetWindow();
	auto state = glfwGetKey(window, static_cast<int32_t>(key));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(int button)
{
	auto window = Game::Get().GetWindow();
	auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

std::pair<float, float> Input::GetMousePosition()
{
	auto window = Game::Get().GetWindow();
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return {(float)xpos, (float)ypos};
}

float Input::GetMouseX()
{
	auto [x, y] = GetMousePosition();
	return x;
}

float Input::GetMouseY()
{
	auto [x, y] = GetMousePosition();
	return y;
}

void Input::EnableCursor()
{
	auto window = Game::Get().GetWindow();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	s_Cursor = true;
}

void Input::DisableCursor()
{
	auto window = Game::Get().GetWindow();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	s_Cursor = false;
}
