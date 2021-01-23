#include "Application.h"

int main()
{
	auto* app = new Application("Neon");
	app->Run();
	delete app;
	return 0;
}