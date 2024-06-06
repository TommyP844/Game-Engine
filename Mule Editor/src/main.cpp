
#include "Application.h"

#include "Layers/EditorLayer.h"

#include "EditorState.h"

int main(int argc, char* argv[])
{
	Application* app = new Application("Editor");
	app->PushLayer(
		Mule::Ref<EditorLayer>::Make(
			"C:\\Mule\\Projects\\Sample Project",
			app->GetApplicationData()
	));
	app->Run();
	delete app;
	return 0;
}