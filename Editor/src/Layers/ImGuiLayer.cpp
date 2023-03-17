#include "ImGuiLayer.h"

#include "Mule/Mule.h"

#include "imgui.h"

#include "ImGuiPanels/SceneHierarchyPanel.h"
#include "ImGuiPanels/ComponentPanel.h"
#include "ImGuiPanels/MaterialPanel.h"
#include "ImGuiPanels/AssetManagerPanel.h"
#include "ImGuiPanels/ContentBrowserPanel.h"
#include "ImGuiPanels/SceneViewPanel.h"

#include "../ImGuiGlobals.h"

#define FILE_BUFFER_SIZE 64
#define DIR_BUFFER_SIZE 512

static char filenameBuffer[FILE_BUFFER_SIZE] = { 0 };
static char dirBuffer[DIR_BUFFER_SIZE] = { 0 };

void ImGuiLayer::OnAttach()
{
	memcpy(dirBuffer, mAppData.AssetDir.u8string().data(), mAppData.AssetDir.u8string().size());
	// default panels
	PushPanel(Mule::MakeRef<ScenePanel>(mAppData));
	PushPanel(Mule::MakeRef<ComponentPanel>(mAppData));
	PushPanel(Mule::MakeRef<ContentBrowserPanel>(mAppData, "C:\\Dev\\Mule Engine\\Resource"));
	PushPanel(Mule::MakeRef<SceneViewPanel>(mAppData));
}

void ImGuiLayer::OnUpdate()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize); 
	ImGui::SetNextWindowViewport(viewport->ID);
	
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	bool openNewMaterialPopup = false;
	bool openNewScenePopup = false;

	if (ImGui::Begin("MainWindow", 0, window_flags))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("Open");
				ImGui::MenuItem("Save");
				ImGui::MenuItem("Save As...", "Crtl + s");

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Material Editor"))
				{
					PushPanel(Mule::MakeRef<MaterialPanel>(mAppData));
				}
				if (ImGui::MenuItem("Asset Manager"))
				{
					PushPanel(Mule::MakeRef<AssetManagerPanel>(mAppData, "C:\\Dev\\Mule Engine\\Resource"));
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Material"))
				{
					openNewMaterialPopup = true;
				}
				if (ImGui::MenuItem("Scene"))
				{
					openNewScenePopup = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (openNewMaterialPopup)	ImGui::OpenPopup("New Material");
		if (openNewScenePopup) ImGui::OpenPopup("New Scene");
		
		if (NewFileExplorer(mAppData, "New Material", filenameBuffer, FILE_BUFFER_SIZE, dirBuffer, DIR_BUFFER_SIZE))
		{
			std::string dir = dirBuffer;
			std::string filename = filenameBuffer;
			PushPanel(Mule::MakeRef<MaterialPanel>(mAppData, dir + std::string("\\") + filename + ".material"));
			memset(filenameBuffer, 0, FILE_BUFFER_SIZE);
			memset(dirBuffer, 0, DIR_BUFFER_SIZE);
			memcpy(dirBuffer, mAppData.AssetDir.u8string().data(), mAppData.AssetDir.u8string().size());
		}

		if (NewFileExplorer(mAppData, "New Scene", filenameBuffer, FILE_BUFFER_SIZE, dirBuffer, DIR_BUFFER_SIZE))
		{
			std::string dir = dirBuffer;
			std::string filename = filenameBuffer;
			std::string path = dir + std::string("\\") + filename + ".scene";
			Mule::Ref<Mule::Scene> scene = Mule::Scene::Create(filename);
			Mule::Scene::Serialize(path, scene);
			Mule::AssetManager& manager = Mule::AssetManager::Get();
			manager.TrackAsset(filename, scene, path, Mule::Asset_Scene);
			mAppData.ActiveScene = scene;
			PushPanel(Mule::MakeRef<SceneViewPanel>(mAppData));
			memset(filenameBuffer, 0, FILE_BUFFER_SIZE);
			memset(dirBuffer, 0, DIR_BUFFER_SIZE);
			memcpy(dirBuffer, mAppData.AssetDir.u8string().data(), mAppData.AssetDir.u8string().size());
		}


		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
		}
		
		for (Mule::Ref<IPanel> panel : mPanels)
		{
			panel->OnUpdate();
		}
	}
	ImGui::End();
}

void ImGuiLayer::OnDetach()
{
	for (Mule::Ref<IPanel> panel : mPanels)
	{
		panel->OnDetach();
	}
}

void ImGuiLayer::PushPanel(Mule::Ref<IPanel> panel)
{
	panel->OnAttach();
	mPanels.push_back(panel);
}

void ImGuiLayer::RemovePanel(Mule::Ref<IPanel> panel)
{
	auto iter = std::find(mPanels.begin(), mPanels.end(), panel);
	if (iter != mPanels.end())
	{
		panel->OnDetach();
		mPanels.erase(iter);
		return;
	}
	LOG_ERR(L"Could not find panel: {0}", panel->GetName());
}
