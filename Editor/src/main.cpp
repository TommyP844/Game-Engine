

#include "Mule/Mule.h"

#include <iostream>

int main()
{



	Mule::Internal::ThreadPool::Init();
	Mule::Ref<Mule::Scene> s = Mule::Scene::DeSerializeAsync("C:\\Dev\\Mule Engine\\Resource\\Scene.scene");
	

	Mule::Internal::ThreadPool::Shutdown();

	return 0;
}