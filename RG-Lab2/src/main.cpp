#include <chrono>
#include <time.h>

#include "Game.h"

int main()
{
	srand((uint32_t)time(NULL));

	Game::CreateGame("Lab 2", 2000, 1500);
	auto lastFrameTime = std::chrono::high_resolution_clock::now();
	while (Game::Get().IsRunning())
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto timeStep = currentTime - lastFrameTime;
		lastFrameTime = currentTime;
		Game::Get().Update(
			std::chrono::duration<float, std::chrono::milliseconds::period>(timeStep).count());
		Game::Get().Render();
	}
	return 0;
}
