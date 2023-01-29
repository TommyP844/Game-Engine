#include "Application.h"

int main(int argc, char** argv)
{
	Application* app = new Application();
	app->Init();
	app->Run();
	delete app;
	return 0;
}