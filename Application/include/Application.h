

#include <stack>
#include "ILayer.h"
#include "Mule.h"

class Application
{
public:
	Application(const std::string& name);

	~Application();

	Mule::Ref<ApplicationData> GetApplicationData() const { return mApplicationData; }
	void Run();

	void PushLayer(Mule::Ref<ILayer> layer);
	void PopLayer();


	Mule::Ref<Mule::GraphicsDevice> mGraphicsDevice;
	
private:
	std::stack<Mule::Ref<ILayer>> mLayers;
	Mule::Ref<Mule::Window> mWindow;
	Mule::Ref<ApplicationData> mApplicationData;
	Mule::Ref<Mule::ImGuiMuleContext> mImGuiContext;
};