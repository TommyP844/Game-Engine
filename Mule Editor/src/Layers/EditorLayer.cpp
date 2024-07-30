#include "EditorLayer.h"

// panels
#include "Panels/ComponentPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowser.h"
#include "Panels/SceneView.h"
#include "ThumbnailManager.h"
#include <future>

#include "Util.h"

EditorLayer::EditorLayer(const fs::path& projectPath, Mule::WeakRef<ApplicationData> appData)
	:
	ILayer(appData, "Editor Layer"),
	mAppCloseRequested(false)
{
	mEditorState = Mule::Ref<EditorState>::Make(projectPath);
	mEditorState->Device = appData->GraphicsDevice;
}

void EditorLayer::OnAttach()
{
	Mule::RenderPassDescription renderPassDesc;
	renderPassDesc.Name = "Default RenderPass";
	renderPassDesc.Context = mApplicationData->GraphicsDevice->GetMainThreadContext();
	renderPassDesc.Attachments = {
		Mule::RenderPassAttachment(Mule::TextureFormat::RGBA8, Mule::Samples::SampleCount_1)
	};
	renderPassDesc.SubPasses = {
		Mule::RenderPassSubPassDescription({0}, {0}, true)
	};
	renderPassDesc.DepthAttachment = Mule::RenderPassAttachment(Mule::TextureFormat::Depth32F, Mule::Samples::SampleCount_1);
	Mule::Ref<Mule::RenderPass> defaultRenderPass = Mule::RenderPass::Create(
		mApplicationData->GraphicsDevice,
		renderPassDesc);
	defaultRenderPass->SetHandle(Mule::DefaultRenderPassHandle);
	Mule::AssetManager::Get().InsertAsset(defaultRenderPass);

	Mule::AssetManager& manager = Mule::AssetManager::Get();
	fs::path path = "C:\\Dev\\Mule\\Resources\\Shaders\\PBRShader.shader";
	MULE_LOG_VERBOSE("\tLoading asset: {0}", path);
	Mule::SourceShaderDescription sourceDesc;

	sourceDesc.Device = mApplicationData->GraphicsDevice;
	sourceDesc.Context = mApplicationData->GraphicsDevice->GetMainThreadContext();
	sourceDesc.Path = path;
	sourceDesc.SubPassIndex = 0;
	sourceDesc.EnableBlending = false;
	sourceDesc.EnableDepthTest = true;
	sourceDesc.RenderPass = Mule::AssetManager::Get().GetAsset<Mule::RenderPass>(Mule::DefaultRenderPassHandle); // Issue

	auto shader = Mule::Shader::Create(sourceDesc);
	shader->SetHandle(Mule::DefaultShaderHandle);
	manager.InsertAsset(shader);

	ThumbnailManager::Initialize(
		mApplicationData->GraphicsDevice,
		mApplicationData->GraphicsDevice->GetMainThreadContext()
	);

	mAssetScanner = std::thread(&EditorLayer::LoadAssetsAsync, this);

	PushPanel<ComponentPanel>(PanelType::ComponentPanel, mEditorState);
	PushPanel<ContentBrowser>(PanelType::ContentBrowserPanel, mEditorState);
	PushPanel<SceneHierarchyPanel>(PanelType::SceneHierarchyPanel, mEditorState);
	PushPanel<SceneView>(PanelType::SceneEditorPanel, mEditorState);
}

void EditorLayer::OnUpdate(float dt)
{
	for (auto& [type, panel] : mPanels)
	{
		panel->OnUpdate(dt);
	}
}

void EditorLayer::OnDetach()
{
	for (auto [type, panel] : mPanels)
	{
		panel->OnDetach();
	}
	mPanels.clear();
	mEditorState->SaveState();
	mLayerAttached = false;
	mAssetScanner.join();

	ThumbnailManager::Shutdown();
}

void EditorLayer::OnEvent(Mule::Ref<Mule::Event> event)
{
	switch (event->Type())
	{
	case Mule::EventType::WindowEvent:
		{
		Mule::WeakRef<Mule::WindowEvent> windowEvent = event;
		switch (windowEvent->Type)
		{
		case Mule::WindowEventType::Close:
			mAppCloseRequested = true;
			break;
		}
		}
		break;
	}
	for (auto& [type, panel] : mPanels)
	{
		panel->OnEvent(event);
	}
}

void EditorLayer::OnImGuiRender(float dt)
{
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspace_flags);
	DisplayMenuBar();
	DisplayPopups();
	for (auto& [type, panel] : mPanels)
	{
		panel->OnImGuiRender();
	}
}

void EditorLayer::PopPanel(PanelType type)
{
	mPanels[type]->OnDetach();
	mPanels.erase(type);
}

void EditorLayer::PopPanelAfterUpdate(PanelType type)
{
	mPanelsToPop.insert(type);
}

void EditorLayer::DisplayMenuBar()
{
	bool openNewSceneModal = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Project...", "Crtl + Shift + N")) {}
			if (ImGui::MenuItem("Open Project", "Crtl + Shift + O")) {}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Scene"))
			{
				mNewAssetPopup = {
					PopupType::NewScene,
					NewAssetPopUp::Create("New Scene",
						mEditorState->GetAssetsPath(),
						mEditorState->GetAssetsPath() / "Scenes",
						"scene")
				};
			}
			if (ImGui::MenuItem("Material"))
			{
				mNewAssetPopup = {
					PopupType::NewMaterial,
					NewAssetPopUp::Create("New Material",
						mEditorState->GetAssetsPath(),
						mEditorState->GetAssetsPath() / "Materials",
						"material")
				};
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Panels"))
		{
			if (ImGui::MenuItem("Components", nullptr, mPanels[PanelType::ComponentPanel]->IsOpen())) mPanels[PanelType::ComponentPanel]->ToggleOpen();
			if (ImGui::MenuItem("Content Browser", nullptr, mPanels[PanelType::ContentBrowserPanel]->IsOpen())) mPanels[PanelType::ContentBrowserPanel]->ToggleOpen();
			if (ImGui::MenuItem("Scene Hierarchy", nullptr, mPanels[PanelType::SceneHierarchyPanel]->IsOpen())) mPanels[PanelType::SceneHierarchyPanel]->ToggleOpen();
			if (ImGui::MenuItem("Scene View", nullptr, mPanels[PanelType::SceneEditorPanel]->IsOpen())) mPanels[PanelType::SceneEditorPanel]->ToggleOpen();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void EditorLayer::DisplayPopups()
{
	Mule::AssetManager& manager = Mule::AssetManager::Get();

	if (mNewAssetPopup.second)
	{
		fs::path path;
		if (mNewAssetPopup.second->OnImGuiRender(path))
		{
			switch (mNewAssetPopup.first)
			{
			case PopupType::NewScene:
			{
				Mule::Ref<Mule::Scene> scene = Mule::Scene::Create(path);
				manager.InsertAsset(scene);
				mEditorState->ActiveSceneHandle = scene->Handle();
			}
				break;
			case PopupType::NewMaterial:
				break;
			default:
				break;
			}
			mNewAssetPopup = { PopupType::None, nullptr};
		}
	}


	static std::vector<fs::path> modifiedAssetPaths;
	if (mAppCloseRequested)
	{

		modifiedAssetPaths.clear();
		manager.IterateAssets([&](Mule::WeakRef<Mule::Asset> asset) {
			if (asset->Modified())
			{
				modifiedAssetPaths.push_back(asset->Filepath());
			}
			});

		if (modifiedAssetPaths.empty())
		{
			mApplicationData->CloseApplication();
		}
		else
		{
			ImGui::OpenPopup("App Close");
		}

		mAppCloseRequested = false;
	}

	int popupFlags = ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowSize({500.f, 0.f});
	if (ImGui::BeginPopupModal("App Close", 0, popupFlags))
	{
		ImGui::Text("Assets not saved");
		ImGuiChildFlags flags = ImGuiChildFlags_Border;
		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::BeginChild("App Close Assets", ImVec2(width, 200.f), flags))
		{
			fs::path pathToRemove = fs::path();
			for (auto& path : modifiedAssetPaths)
			{
				auto relativePath = path.lexically_relative(mEditorState->GetAssetsPath());
				ImGui::Text(relativePath.string().c_str());

				float buttonSize = ImGui::CalcTextSize("Ignore").x
					+ ImGui::CalcTextSize("Save").x + ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().WindowPadding.x;
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonSize);
				
				if (ImGui::Button("Ignore"))
				{
					pathToRemove = path;
				}
				ImGui::SameLine();
				if (ImGui::Button("Save"))
				{
					FileType type = TypeFromPath(path);
					switch (type)
					{
					case FileType::Unknown:
						break;
					case FileType::Directory:
						break;
					case FileType::Scene:
					{
						auto scene = manager.GetAssetByFilepath<Mule::Scene>(path);
						if (scene)
						{
							scene->Serialize(Mule::SerializationMode::Text);
							pathToRemove = path;
						}
						else
						{
							MULE_LOG_ERROR("Failed to find scene with path: {0}", path);
						}
					}
						break;
					case FileType::Texture:
						break;
					case FileType::Mesh:
						break;
					case FileType::Shader:
						break;
					case FileType::Material:
						break;
					default:
						break;
					}
				}
			}
			ImGui::EndChild();

			if (!pathToRemove.empty())
			{
				auto iter = std::find(modifiedAssetPaths.begin(), modifiedAssetPaths.end(), pathToRemove);
				if (iter != modifiedAssetPaths.end())
				{
					modifiedAssetPaths.erase(iter);
					pathToRemove = fs::path();
				}
			}
		}


		if (ImGui::Button("Back"))
		{
			modifiedAssetPaths.clear();
			ImGui::CloseCurrentPopup();
		}

		if (modifiedAssetPaths.empty())
		{
			float buttonSize = ImGui::CalcTextSize("Quit").x;
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonSize);

			if (ImGui::Button("Quit"))
			{
				mApplicationData->CloseApplication();
			}
		}
		else
		{
			float buttonSize = ImGui::CalcTextSize("Save all").x
				+ ImGui::CalcTextSize("Ignore all").x + ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().WindowPadding.x;
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonSize);

			if (ImGui::Button("Ignore all"))
			{
				modifiedAssetPaths.clear();
			}

			ImGui::SameLine();

			if (ImGui::Button("Save all"))
			{
				// TODO: Save
				modifiedAssetPaths.clear();
			}
		}
		ImGui::EndPopup();
	}
	
}

void EditorLayer::LoadAssetsAsync()
{
	std::chrono::milliseconds sleep(1000);
	while (mLayerAttached)
	{
		if (!mAutoLoadAssets)
			continue;

		MULE_LOG_VERBOSE("Scanning directory...", "");

		Mule::AssetManager& manager = Mule::AssetManager::Get();

		for (auto entry : fs::recursive_directory_iterator(mEditorState->GetAssetsPath()))
		{
			if (entry.is_directory())
				continue;
			auto path = entry.path();
			auto asset = manager.GetAssetByFilepath<Mule::Asset>(path);
			if (asset)
				continue;

			FileType type = TypeFromPath(path);
			switch (type)
			{
			case FileType::Directory:
				break;
			case FileType::Scene:
				break;
			case FileType::Texture:
			{
				auto thread = std::thread([=]() {
					Mule::AssetManager& manager = Mule::AssetManager::Get();
					MULE_LOG_VERBOSE("\tLoading asset: {0}", path);
					auto texture = Mule::Texture::Create(
						mApplicationData->GraphicsDevice, 
						mApplicationData->GraphicsDevice->GetMainThreadContext(), 
						path);
					manager.InsertAsset(texture);
					});
				thread.detach();
			}
				break;
			case FileType::Mesh:
			{
				auto thread = std::thread([=]() {
					Mule::AssetManager& manager = Mule::AssetManager::Get();
					MULE_LOG_VERBOSE("\tLoading asset: {0}", path);
					auto model = Mule::DeserializeModel(mApplicationData->GraphicsDevice, path);
					manager.InsertAsset(model);
					});
				thread.detach();
			}
				break;
			case FileType::Shader:
			{
				auto thread = std::thread([=]() {
					Mule::AssetManager& manager = Mule::AssetManager::Get();
					MULE_LOG_VERBOSE("\tLoading asset: {0}", path);
					Mule::SourceShaderDescription sourceDesc;

					sourceDesc.Device = mApplicationData->GraphicsDevice;
					sourceDesc.Context = mApplicationData->GraphicsDevice->GetMainThreadContext();
					sourceDesc.Path = path;
					sourceDesc.SubPassIndex = 0;
					sourceDesc.EnableBlending = false;
					sourceDesc.EnableDepthTest = true;
					sourceDesc.RenderPass = nullptr; // Issue

					auto shader = Mule::Shader::Create(sourceDesc);
					manager.InsertAsset(shader);
					});
				thread.detach();
			}
				break;
			case FileType::Material:
				break;
			}

		}
		std::this_thread::sleep_for(sleep);
	}
}
