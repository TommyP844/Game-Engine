#include "Application.h"

#include "Layers/ImGuiLayer.h"
#include "Layers/SandboxLayer.h"

#include "imgui.h"



void Application::Init()
{
	mAppData.AssetDir = "..\\Resource";
	Mule::AssetManager::Init(mAppData.AssetDir);
	Mule::Window::Init();
	mWindow = Mule::Window::Create(1600, 900, "Mule");

	mAppData.Window = mWindow;

	Mule::Renderer::Init(mWindow);
	mWindow->InitEventSystemWithImGui(); // must be called after imgui is setup

	mAppData.EditorTextures["FolderIcon"] = Mule::Texture::Create("../Resource/Textures/folder.png");
	mAppData.EditorTextures["FileIcon"] = Mule::Texture::Create("../Resource/Textures/file.png");
	mAppData.EditorTextures["MissingImage"] = Mule::Texture::Create("../Resource/Textures/missing.png");

	PushLayer(Mule::MakeRef<ImGuiLayer>(mAppData));
	PushLayer(Mule::MakeRef<SandBoxLayer>(mAppData));


	// Temporary for testing
	// TODO: future load all textures and materials from previews, then only load assets that are in the active scene(s)
	Mule::AssetManager& manager = Mule::AssetManager::Get();
	for (auto dir : fs::recursive_directory_iterator("../Resource"))
	{
		if (!dir.is_directory())
		{
			Mule::Ref<void> asset = nullptr;
			Mule::AssetType type;
			std::string ext = dir.path().extension().u8string();
			if (ext == ".material")
			{
				asset = Mule::DeSerializeMaterial(dir.path());
				type = Mule::Asset_Material;
			}
			if (ext == ".scene")
			{
				Mule::Ref<Mule::Scene> scene = Mule::Scene::DeSerialize(dir.path());
				scene->InitRenderer(400, 400);
				asset = scene;
				type = Mule::Asset_Scene;
			}
			if (ext == ".png" || ext == ".tga" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
			{
				asset = Mule::Texture::Create(dir.path());
				type = Mule::Asset_Texture;
			}
			if (ext == ".shader")
			{
				asset = Mule::Shader::Create(dir.path());
				type = Mule::Asset_Shader;
			}
			if (ext == ".obj")
			{
				asset = Mule::MeshLoader::LoadMesh(dir.path());
				type = Mule::Asset_MeshCollection;
			}

			if (asset)
			{				
				std::string name = dir.path().filename().u8string();
				name = name.substr(0, name.find_last_of("."));
				if (name.empty())
				{
					LOG_WARN(L"Invalid asset name: {0}", dir.path().u8string());
				}
				else
				{
					manager.TrackAsset(name, asset, dir.path(), type);
				}
			}
		}
	}

}

void Application::Run()
{	
	//Mule::Ref<Mule::Scene> scene = Mule::Scene::Create();
	//Mule::Entity entity = scene->CreateEntity();
	//auto& c = entity.AddComponent<Mule::MeshCollectionComponent>();
	//c.Meshes = Mule::AssetManager::Get().GetAsset<Mule::MeshCollection>("spider");
	//entity.AddComponent<Mule::MeshRendererComponent>();
	//auto size = mWindow->GetSize();
	while (mWindow->IsOpen())
	{
		mWindow->PollEvents();
		//scene->OnEditorRender(size.x, size.y);
		
		Mule::Renderer::NewImGuiFrame();
		
		for (auto layer : mLayers)
		{
			layer->OnUpdate();
		}
		
		Mule::Renderer::EndImGuiFrame();
		
		Mule::Renderer::EndFrame();
	}

	while (mLayers.size() > 0)
	{
		PopLayer();
	}

	//Mule::Internal::ThreadPool::Shutdown();
	Mule::AssetManager::Shutdown();
	Mule::Renderer::Shutdown();
	Mule::Window::Shutdown();
}

void Application::PushLayer(Mule::Ref<ILayer> layer)
{
	layer->OnAttach();
	mLayers.push_back(layer);
}

void Application::PopLayer()
{
	mLayers.back()->OnDetach();
	mLayers.pop_back();
}
