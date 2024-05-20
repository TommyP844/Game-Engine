
#include "Application.h"

#include "Layers/EditorLayer.h"

int main(int argc, char* argv[])
{
	Application* app = new Application("Editor");
	
	app->Setup();

	Mule::Ref<EditorLayer> editorLayer = Mule::Ref<EditorLayer>::Make("");
	app->PushLayer(editorLayer);
	app->Run();
	delete app;

	return 0;
}