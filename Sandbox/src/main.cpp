
#include "Application.h"

#include "SandboxLayer.h"

int main() 
{
	Application* application = new Application("v1.0");
	application->Setup();

	auto appData = application->GetApplicationData();
	Mule::Ref<SandboxLayer> layer = Mule::Ref<SandboxLayer>::Make(appData, "Sandbox");
	application->PushLayer(layer);


	application->Run();
	delete application;
	return 0;
}