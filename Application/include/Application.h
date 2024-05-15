

#include <stack>
#include "ILayer.h"
#include "Mule.h"

class Application
{
public:
	Application(const std::string& name);
	~Application();

	void Setup();
	Mule::Ref<ApplicationData> GetApplicationData() const { return mApplicationData; }
	void Run();

	void PushLayer(Mule::Ref<ILayer> layer);
	void PopLayer();

private:
	std::stack<Mule::Ref<ILayer>> mLayers;
	Mule::Ref<Mule::Window> mWindow;
	bool mAppIsRunning;
	Mule::Ref<ApplicationData> mApplicationData;
};